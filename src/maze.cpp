#include "basescreen.h"
#include "maze.h"
#include <algorithm>
#include <iostream>
#include <map>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <set>
#include <stack>
#include <string>
#include <vector>

void MazeScreen::load(GameState* game) {
	this->game = game;
	this->renderer = game->renderer;

	isRunning = true;
	isPaused = false;
	pausedTime=0;
	pauseButtonRect = {SCREEN_WIDTH - 60, 10, 50, 50};
	overlayRect = {0, 0, 1000, 500};
	playButtonRect = {475, 225, 80, 80};
	quitButtonRect = {575, 225, 80, 80};
	soundButtonRect = {375, 225, 80, 80};
	win = false;
	lose = false;
	winBannerRect = {250, 50, 500, 100};
	timerRect = {80, 285, 80, 80};
	winBannerRect = {250, 50, 500, 100};
	scoreRect = {80, 185, 80, 80};
	mazeGraph = levelGraphs[0];
	gameDuration = levelTimers[0];
	mazeGraph.findShortestPath({startX, startY}, mazeGraph.lastNode);


	//			if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
	//				std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
	//				return false;
	//			}
	//
	//			window = SDL_CreateWindow("Maze Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	//			if (!window) {
	//				std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
	//				return false;
	//			}
	//
	//			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	//			if (!renderer) {
	//				std::cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
	//				return false;
	//			}

	int currentLevel = 1;
	SDL_Texture* levelBackgrounds[3];
	Graph levelGraphs[3];
	Maze* maze;
	Maze* levelMazes[3];

	levelBackgrounds[0] = loadTexture("assets/images/pinkbg.png");
	levelBackgrounds[1] = loadTexture("assets/images/skybluebg.png");
	levelBackgrounds[2] = loadTexture("assets/images/darkbluebg.png");

	floorTexture = loadTexture("assets/images/floor.png");
	roofTexture = loadTexture("assets/images/roof.png");
	backgroundTexture2 = loadTexture("assets/images/lightorangebg.png");
	backgroundTexture3 = loadTexture("assets/images/orangebg3.png");
	backgroundTexture4 = loadTexture("assets/images/curvebg.png");
	playerTexture1 = loadTexture("assets/images/pl1.png");
	playerTexture2 = loadTexture("assets/images/pl4.png");
	sunTexture = loadTexture("assets/images/sun.png");
	moonTexture = loadTexture("assets/images/moon.png");
	lollipopTexture = loadTexture("assets/images/lollipop.png");
	pauseButtonTexture = loadTexture("assets/images/pause.png");
	playButtonTexture = loadTexture("assets/images/play.png");
	quitButtonTexture = loadTexture("assets/images/quit.png");
	soundOnButtonTexture = loadTexture("assets/images/sound_on.png");
	soundOffButtonTexture = loadTexture("assets/images/volumeoff.png"); //need to vhnage
	cellTexture1 = loadTexture("assets/images/cell.png");
	cellTexture2 = loadTexture("assets/images/cell2.png");
	lastNodeTexture = loadTexture("assets/images/cell3.png");
	scoreTexture = loadTexture("assets/images/trophy.png");
	timerTexture = loadTexture("assets/images/timer.png");
	replayButtonTexture = loadTexture("assets/images/replay.png"); //need to chnge
	winBannerTexture = loadTexture("assets/images/heading.png");

	backgroundTexture = loadTexture("assets/images/bg1.png");
	cellTexture = loadTexture("assets/images/cell.png");

	font = TTF_OpenFont("assets/font/arial.ttf", 28);
	if (!font) {
		cout << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << endl;
		return;
	}

	if (!cellTexture1 || !cellTexture2 || !lastNodeTexture || !playerTexture1 || !playerTexture2
	        || !backgroundTexture  || !backgroundTexture2
	        || !backgroundTexture3 || !backgroundTexture4 || !sunTexture || !moonTexture || !lollipopTexture || !floorTexture
	        || !roofTexture || !pauseButtonTexture || !playButtonTexture || !quitButtonTexture || !soundOnButtonTexture
	        || !soundOffButtonTexture || !scoreTexture || !timerTexture || !winBannerTexture) {
		return;
	}

	// Define graphs for each level
	defineL1Graph();
	defineL2Graph();
	defineL3Graph();
	// Create maze objects for each level
	levelMazes[0] = new Maze(&levelGraphs[0]);
	levelMazes[1] = new Maze(&levelGraphs[1]);
	levelMazes[2] = new Maze(&levelGraphs[2]);

	// Set the initial maze and background
	maze = levelMazes[0];
	backgroundTexture = levelBackgrounds[0];
	gameDuration = levelTimers[0];
	remainingTime = levelTimers[0];  // Initialize with level 1 timer

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		std::cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
		return; // false;
	}
	moveSound = Mix_LoadWAV("assets/audio/move.wav");
	if (!moveSound) {
		std::cout << "Failed to load movement sound! SDL_mixer Error: " << Mix_GetError() << std::endl;
		return; // false;
	}

	return; // true;
}

void MazeScreen::loadNextLevel() {
	if (currentLevel < 3) {
		currentLevel++;
		maze = levelMazes[currentLevel - 1];  // Point to the next level's maze
		mazeGraph = levelGraphs[currentLevel - 1];  // Update the graph
		backgroundTexture = levelBackgrounds[currentLevel - 1];  // Update background
		gameDuration = levelTimers[currentLevel - 1];
//	        remainingTime = levelTimers[currentLevel - 1];  // Reset timer
		startTime = SDL_GetTicks();
		player = Player(startX, startY);  // Reset player
		cameraX = 0;
		cameraY = 0;
		player.resetMoves();
		optimalPathFollowed = false;
		win = false;
	}
}

// void MazeScreen::replayLevel() {
// 	maze = levelMazes[currentLevel - 1];
// 	mazeGraph = levelGraphs[currentLevel - 1];
// 	backgroundTexture = levelBackgrounds[currentLevel - 1];
// 	gameDuration = levelTimers[currentLevel - 1];
// //	    remainingTime = levelTimers[currentLevel - 1];  // Reset timer
// 	player = Player(startX, startY);
// 	cameraX = 0;
// 	cameraY = 0;
// 	player.resetMoves();
// 	score = 0;
// 	lose = false;
// }

void MazeScreen::run() {
	startTime = SDL_GetTicks();
	while (isRunning) {
		SDL_Event event;
		handleEvents(event);
		render();
		if (win || lose) {
			player.resetMoves();
		}
	}
}



void MazeScreen::cleanUp() {
	for (int i = 0; i < 3; ++i) {
		delete levelMazes[i];
	}
	SDL_DestroyTexture(backgroundTexture);
	SDL_DestroyTexture(backgroundTexture2);
	SDL_DestroyTexture(backgroundTexture3);
	SDL_DestroyTexture(backgroundTexture4);
	SDL_DestroyTexture(floorTexture);
	SDL_DestroyTexture(roofTexture);
	SDL_DestroyTexture(cellTexture1);
	SDL_DestroyTexture(cellTexture2);
	SDL_DestroyTexture(lastNodeTexture);
	SDL_DestroyTexture(playerTexture1);
	SDL_DestroyTexture(playerTexture2);
	SDL_DestroyTexture(cellTexture);
//	SDL_DestroyTexture(playerTexture);
	SDL_DestroyTexture(sunTexture);
	SDL_DestroyTexture(moonTexture);
	SDL_DestroyTexture(lollipopTexture);
	SDL_DestroyTexture(pauseButtonTexture);
	SDL_DestroyTexture(playButtonTexture);
	SDL_DestroyTexture(quitButtonTexture);
	SDL_DestroyTexture(soundOnButtonTexture);
	SDL_DestroyTexture(soundOffButtonTexture);
	SDL_DestroyTexture(scoreTexture);
	SDL_DestroyTexture(timerTexture);
	// SDL_DestroyTexture(replayButtonTexture);
	SDL_DestroyTexture(winBannerTexture);
	Mix_FreeChunk(moveSound);
	// TTF_CloseFont(font);
	// TTF_Quit();
	//			SDL_DestroyRenderer(renderer);
	//			SDL_DestroyWindow(window);
	//			Mix_Quit();
	//			SDL_Quit();
}

SDL_Texture* MazeScreen::loadTexture(const std::string& path) {
	SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
	if (!texture) {
		std::cout << "Failed to load texture: " << path << " SDL_Error: " << SDL_GetError() << std::endl;
	}
	return texture;
}

void MazeScreen::checkLevelCompletion() {
	if (mazeGraph.isLastNode(player.getCurrentPosition())) {
		cout << "Player reached the last node! You win!" << endl;
		win = true;
		time = remainingTime;

		mazeGraph.findShortestPath({startX, startY}, mazeGraph.lastNode);

		if (player.moves == mazeGraph.optimalPath) {
			cout << "Player followed the most efficient path!" << endl;
			optimalPathFollowed = true;
			score += 10 * player.moves.size();
			cout << "BONUS SCORE: " << score << endl;
		} else {
			cout << "Player did not follow the most efficient path!" << endl;
			for (size_t i = 0; i < player.moves.size() && i < mazeGraph.optimalPath.size(); ++i) {
				if (player.moves[i] == mazeGraph.optimalPath[i]) {
					score += 10; // Increment for each correct move
				}
			}
		}
		score += remainingTime * 10;
		cout << "TOTAL SCORE: " << score << endl;
	}
}

//void MazeScreen::defineGraph() {
//	mazeGraph.addEdge({ 3, 8 }, { 3, 7 });
//	mazeGraph.addEdge({ 3, 7 }, { 3, 6 });
//	mazeGraph.addEdge({ 3, 6 }, { 3, 5 });
//	mazeGraph.addEdge({ 3, 5 }, { 3, 4 });
//	//        mazeGraph.addEdge({5, 6}, {5, 5});
//	//        mazeGraph.addEdge({5, 5}, {5, 4});
//	//        mazeGraph.addEdge({5, 4}, {5, 3});
//	//        mazeGraph.addEdge({5, 3}, {5, 2});
//	//        mazeGraph.addEdge({5, 2}, {5, 1});
//	//        mazeGraph.addEdge({5, 1}, {5, 0});
//	//        mazeGraph.addEdge({4, 6}, {3, 6});
//	//        mazeGraph.addEdge({5, 4}, {6, 4});
//	//        mazeGraph.addEdge({6, 4}, {7, 4});
//	//        mazeGraph.addEdge({3, 6}, {3, 5});
//	//        mazeGraph.addEdge({3, 5}, {3, 4});
//	//        mazeGraph.addEdge({3, 4}, {3, 3});
//	//        mazeGraph.addEdge({3, 3}, {3, 2});
//}

void MazeScreen::defineL1Graph() {
	levelGraphs[0].addEdge({startX, startY}, {startX, startY-1}, 2, 2);
	levelGraphs[0].addEdge({startX, startY}, {startX, startY+1}, 2, 2);
	levelGraphs[0].addEdge({startX, startY-1}, {startX, startY-2}, 2, 2); //(5,2) (5,1)
	levelGraphs[0].addEdge({startX, startY-2}, {startX, startY-3}, 2, 2);
	levelGraphs[0].addEdge({startX, startY-2}, {startX-1, startY-2}, 2, 1);
	levelGraphs[0].addEdge({startX, startY-1}, {startX+1, startY-1}, 2, 1); //(5,2) (6,2)
	levelGraphs[0].addEdge({startX, startY-3}, {startX+1, startY-3}, 2, 1);
	levelGraphs[0].addEdge({startX+1, startY-3}, {startX+2, startY-3}, 1, 1);
	levelGraphs[0].addEdge({startX+2, startY-3}, {startX+3, startY-3}, 1, 2); //(7,0) (8,0)
	levelGraphs[0].addEdge({startX+2, startY-3}, {startX+2, startY-4}, 1, 2);
	levelGraphs[0].addEdge({startX+2, startY-4}, {startX+2, startY-5}, 2, 2);
	levelGraphs[0].addEdge({startX+2, startY-5}, {startX+3, startY-5}, 2, 1); //(7,-2) (8,-2)
	levelGraphs[0].addEdge({startX+3, startY-5}, {startX+4, startY-5}, 1, 1);
	levelGraphs[0].addEdge({startX+4, startY-5}, {startX+5, startY-5}, 1, 1);
	levelGraphs[0].addEdge({startX+5, startY-5}, {startX+6, startY-5}, 1, 2); //(10,-2) (11,-2)
	levelGraphs[0].addEdge({startX+6, startY-5}, {startX+6, startY-4}, 1, 2);
	levelGraphs[0].addEdge({startX+6, startY-4}, {startX+7, startY-4}, 1, 2);
	levelGraphs[0].addEdge({startX+7, startY-4}, {startX+7, startY-3}, 1, 2);
	levelGraphs[0].addEdge({startX+6, startY-5}, {startX+6, startY-4}, 2, 2);
	levelGraphs[0].addEdge({startX+3, startY-3}, {startX+3, startY-2}, 2, 2);
	levelGraphs[0].addEdge({startX+3, startY-2}, {startX+4, startY-2}, 2, 2);
	levelGraphs[0].addEdge({startX+4, startY-2}, {startX+4, startY-1}, 2, 2);
	levelGraphs[0].addEdge({startX+4, startY-1}, {startX+5, startY-1}, 2, 1);
	levelGraphs[0].addEdge({startX+5, startY-1}, {startX+6, startY-1}, 1, 1);
	levelGraphs[0].addEdge({startX+6, startY-1}, {startX+7, startY-1}, 1, 2); //(11,2) (12,2)
	levelGraphs[0].addEdge({startX+7, startY-1}, {startX+8, startY-1}, 1, 2); //(12,2) (13, 2) win node
	levelGraphs[0].addEdge({startX+7, startY-1}, {startX+7, startY}, 2, 2); //(12,2) (12,3)

	levelGraphs[0].setLastNode({13, 2});
}

void MazeScreen::defineL2Graph() {
	levelGraphs[1].addEdge({startX, startY}, {startX, startY-1}, 2, 2); //(5,3) (5,2)
	levelGraphs[1].addEdge({startX, startY-1}, {startX, startY-2}, 2, 2); //(5,2) (5,1)
	levelGraphs[1].addEdge({startX, startY-1}, {startX+1, startY-1}, 2, 1); //(5,2) (6,2)
	levelGraphs[1].addEdge({startX, startY-2}, {startX, startY-3}, 2, 2);
	levelGraphs[1].addEdge({startX, startY-2}, {startX-1, startY-2}, 2, 1);
	levelGraphs[1].addEdge({startX, startY-3}, {startX+1, startY-3}, 2, 2);
	levelGraphs[1].addEdge({startX+1, startY-3}, {startX+2, startY-3}, 2, 1);
	levelGraphs[1].addEdge({startX+1, startY-3}, {startX+1, startY-4}, 2, 2);
	levelGraphs[1].addEdge({startX+1, startY-4}, {startX+1, startY-5}, 2, 2);
	levelGraphs[1].addEdge({startX+1, startY-5}, {startX, startY-5}, 2, 1);
	levelGraphs[1].addEdge({startX, startY-5}, {startX-1, startY-5}, 1, 1);
	levelGraphs[1].addEdge({startX+2, startY-3}, {startX+3, startY-3}, 1, 2);
	levelGraphs[1].addEdge({startX+3, startY-3}, {startX+3, startY-4}, 2, 2); // (8,0) (8,-1)
	levelGraphs[1].addEdge({startX+3, startY-4}, {startX+4, startY-4}, 2, 2);
	levelGraphs[1].addEdge({startX+4, startY-4}, {startX+4, startY-5}, 2, 2);
	levelGraphs[1].addEdge({startX+4, startY-5}, {startX+5, startY-5}, 2, 1);
	levelGraphs[1].addEdge({startX+5, startY-5}, {startX+6, startY-5}, 1, 2);
	levelGraphs[1].addEdge({startX+6, startY-5}, {startX+7, startY-5}, 2, 1); // (11,-2) (12,-2)
	levelGraphs[1].addEdge({startX+6, startY-5}, {startX+6, startY-4}, 2, 2);
	levelGraphs[1].addEdge({startX+6, startY-4}, {startX+6, startY-3}, 2, 2);
	levelGraphs[1].addEdge({startX+6, startY-3}, {startX+6, startY-2}, 2, 2);
	levelGraphs[1].addEdge({startX+6, startY-2}, {startX+6, startY-1}, 2, 2); // (11,1) (11,2)
	levelGraphs[1].addEdge({startX+6, startY-1}, {startX+7, startY-1}, 2, 1);
	levelGraphs[1].addEdge({startX+6, startY-3}, {startX+7, startY-3}, 2, 1);
	levelGraphs[1].addEdge({startX+7, startY-3}, {startX+8, startY-3}, 1, 1);
	levelGraphs[1].addEdge({startX+8, startY-3}, {startX+9, startY-3}, 1, 2);
	levelGraphs[1].addEdge({startX+9, startY-3}, {startX+9, startY-4}, 2, 2); // (14,0) (14,-1)

	levelGraphs[1].setLastNode({14, -1});
}

void MazeScreen::defineL3Graph() {
	levelGraphs[2].addEdge({startX, startY}, {startX, startY-1}, 2, 2);
	levelGraphs[2].addEdge({startX, startY-1}, {startX, startY-2}, 2, 2); //(5,2) (5,1)
	levelGraphs[2].addEdge({startX, startY-2}, {startX, startY-3}, 2, 2); //(5,1) (5,0)
	levelGraphs[2].addEdge({startX, startY-2}, {startX-1, startY-2}, 2, 1);
	levelGraphs[2].addEdge({startX, startY-1}, {startX+1, startY-1}, 2, 1); //(5,2) (6,2)
	levelGraphs[2].addEdge({startX, startY-3}, {startX+1, startY-3}, 2, 2);
	levelGraphs[2].addEdge({startX+1, startY-3}, {startX+1, startY-4}, 2, 2);
	levelGraphs[2].addEdge({startX+1, startY-3}, {startX+2, startY-3}, 2, 1);
	levelGraphs[2].addEdge({startX+2, startY-3}, {startX+3, startY-3}, 1, 2);
	levelGraphs[2].addEdge({startX+3, startY-3}, {startX+3, startY-4}, 2, 2);
	levelGraphs[2].addEdge({startX+3, startY-3}, {startX+3, startY-2}, 2, 2);
	levelGraphs[2].addEdge({startX+3, startY-2}, {startX+4, startY-2}, 2, 2);
	levelGraphs[2].addEdge({startX+4, startY-2}, {startX+4, startY-1}, 2, 2);
	levelGraphs[2].addEdge({startX+3, startY-3}, {startX+3, startY-4}, 2, 2);
	levelGraphs[2].addEdge({startX+3, startY-4}, {startX+4, startY-4}, 2, 1);
	levelGraphs[2].addEdge({startX+4, startY-4}, {startX+5, startY-4}, 1, 1);
	levelGraphs[2].addEdge({startX+5, startY-4}, {startX+6, startY-4}, 1, 2);
	levelGraphs[2].addEdge({startX+6, startY-4}, {startX+6, startY-3}, 2, 2); //(11,-1) (11,0)
	levelGraphs[2].addEdge({startX+6, startY-3}, {startX+6, startY-2}, 2, 2);
	levelGraphs[2].addEdge({startX+6, startY-2}, {startX+6, startY-1}, 2, 2);
	levelGraphs[2].addEdge({startX+6, startY-1}, {startX+7, startY-1}, 2, 1);
	levelGraphs[2].addEdge({startX+6, startY-3}, {startX+7, startY-3}, 2, 1);
	levelGraphs[2].addEdge({startX+7, startY-3}, {startX+8, startY-3}, 1, 1);
	levelGraphs[2].addEdge({startX+8, startY-3}, {startX+9, startY-3}, 1, 2); //(13,0) (14,0)
	levelGraphs[2].addEdge({startX+9, startY-3}, {startX+10, startY-3}, 2, 2); //(14.0) (15,0)
	levelGraphs[2].addEdge({startX+9, startY-3}, {startX+9, startY-4}, 2, 2);
	levelGraphs[2].addEdge({startX+10, startY-3}, {startX+10, startY-2}, 2, 2);
	levelGraphs[2].addEdge({startX+10, startY-2}, {startX+10, startY-1}, 2, 2); //(15,1) (15,2)
	levelGraphs[2].addEdge({startX+10, startY-1}, {startX+11, startY-1}, 2, 1);
	levelGraphs[2].addEdge({startX+11, startY-1}, {startX+12, startY-1}, 1, 1);
	levelGraphs[2].addEdge({startX+12, startY-1}, {startX+13, startY-1}, 1, 1);

	levelGraphs[2].setLastNode({18, 2});
}


void MazeScreen::handleEvents(SDL_Event event) {
	int dx, dy;
	if (event.type == SDL_QUIT) {
		isRunning = false;
	}
	if (event.type == SDL_MOUSEBUTTONDOWN) {
		SDL_GetMouseState(&dx, &dy);
		SDL_Point mousePoint = { dx, dy };

		if (isPaused) {
			// if (SDL_PointInRect(&mousePoint, &playButtonRect)) {
			// 	isPaused = false;
			// 	// Update pausedTime with the current paused duration
			// 	pausedTime += SDL_GetTicks() - pauseStartTime;
			// } else 
			if (SDL_PointInRect(&mousePoint, &quitButtonRect)) {
				isRunning = false;
			} else if (SDL_PointInRect(&mousePoint, &soundButtonRect)) {
				isSoundOn = !isSoundOn;
			}
		} else if (win) {
			if (SDL_PointInRect(&mousePoint, &playButtonRect) && currentLevel < 3) {
				loadNextLevel();
			} else if (SDL_PointInRect(&mousePoint, &quitButtonRect)) {
				isRunning = false;
			}
		} else if (lose) {
// 			if (SDL_PointInRect(&mousePoint, &replayButtonRect)) {
// //	            		isRunning = true;
// //	            		lose = false;
// 				replayLevel();
// 				cout << "replay\n";
// 			} else 
			if (SDL_PointInRect(&mousePoint, &quitButtonRect)) {
				isRunning = false;
			}
		} else {
			if (SDL_PointInRect(&mousePoint, &pauseButtonRect)) {
				isPaused = true;
				pauseStartTime = SDL_GetTicks();
			}
		}
	}

	if (event.type == SDL_KEYDOWN && !isPaused && !win && !lose) {
		switch (event.key.keysym.sym) {
			case SDLK_UP:
				dy = -1;
				break;
			case SDLK_DOWN:
				dy = 1;
				break;
			case SDLK_LEFT:
				dx = -1;
				break;
			case SDLK_RIGHT:
				dx = 1;
				break;
		}

		if (dx != 0 || dy != 0) {
			player.move(dx, dy, mazeGraph, moveSound);
			checkLevelCompletion();
		}
	}

}

void MazeScreen::render() {
	// Clear the screen
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(renderer);

	if (!isPaused) {
		bg2X1 -= bg2Speed;
		bg2X2 -= bg2Speed;
		if (bg2X1 <= -SCREEN_WIDTH) bg2X1 = bg2X2 + SCREEN_WIDTH;
		if (bg2X2 <= -SCREEN_WIDTH) bg2X2 = bg2X1 + SCREEN_WIDTH;

		bg3X1 -= bg3Speed;
		bg3X2 -= bg3Speed;
		if (bg3X1 <= -SCREEN_WIDTH) bg3X1 = bg3X2 + SCREEN_WIDTH;
		if (bg3X2 <= -SCREEN_WIDTH) bg3X2 = bg3X1 + SCREEN_WIDTH;

		bg4X1 -= bg4Speed;
		bg4X2 -= bg4Speed;
		if (bg4X1 <= -SCREEN_WIDTH) bg4X1 = bg4X2 + SCREEN_WIDTH;
		if (bg4X2 <= -SCREEN_WIDTH) bg4X2 = bg4X1 + SCREEN_WIDTH;
	}

	// Render the first background (static)
	SDL_Rect backgroundRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	SDL_RenderCopy(renderer, backgroundTexture, nullptr, &backgroundRect);

	SDL_Rect sunRect = { 600, 50, 180, 150 };  // Fixed position
	SDL_RenderCopy(renderer, currentLevel == 1 ? sunTexture : moonTexture, nullptr, &sunRect);

	//CURVE
	SDL_Rect bg4Rect1 = { bg4X1, SCREEN_HEIGHT / 3, SCREEN_WIDTH, 130 };
	SDL_Rect bg4Rect2 = { bg4X2, SCREEN_HEIGHT / 3, SCREEN_WIDTH, 130 };
	SDL_RenderCopy(renderer, backgroundTexture4, nullptr, &bg4Rect1);
	SDL_RenderCopy(renderer, backgroundTexture4, nullptr, &bg4Rect2);

	// Render the second background (scrolling) LIGHT ORANGE
	SDL_Rect bg2Rect1 = { bg2X1, SCREEN_HEIGHT / 4, SCREEN_WIDTH, 300 };
	SDL_Rect bg2Rect2 = { bg2X2, SCREEN_HEIGHT / 4, SCREEN_WIDTH, 300 };
	SDL_RenderCopy(renderer, backgroundTexture2, nullptr, &bg2Rect1);
	SDL_RenderCopy(renderer, backgroundTexture2, nullptr, &bg2Rect2);

	// Render the third background (scrolling) ORANGE
	SDL_Rect bg3Rect1 = { bg3X1, SCREEN_HEIGHT / 4, SCREEN_WIDTH, 350 };
	SDL_Rect bg3Rect2 = { bg3X2, SCREEN_HEIGHT / 4, SCREEN_WIDTH, 350 };
	SDL_RenderCopy(renderer, backgroundTexture3, nullptr, &bg3Rect1);
	SDL_RenderCopy(renderer, backgroundTexture3, nullptr, &bg3Rect2);

	SDL_Rect lollipopRect = { 20, 345, 50, 75 };  // Fixed position
	SDL_RenderCopy(renderer, lollipopTexture, nullptr, &lollipopRect);

	maze->render(renderer, cellTexture1, cellTexture2, lastNodeTexture, cameraX, cameraY);
	player.render(renderer, playerTexture1, playerTexture2);

	SDL_Rect floorRect = {0, 420, SCREEN_WIDTH, 80};
	SDL_RenderCopy(renderer, floorTexture, nullptr, &floorRect);

	SDL_Rect roofRect = {0, 0, SCREEN_WIDTH, 30};
	SDL_RenderCopy(renderer, roofTexture, nullptr, &roofRect);

	SDL_RenderCopy(renderer, pauseButtonTexture, nullptr, &pauseButtonRect);

	elapsedTime = (SDL_GetTicks() - startTime - pausedTime) / 1000;
	remainingTime = gameDuration - elapsedTime;
	if (remainingTime <= 0 && !win) {
		remainingTime = 0;
		lose = true;
	}
	// Render maze and player
	if (win) {
		SDL_RenderCopy(renderer, backgroundTexture, nullptr, &overlayRect);
		SDL_RenderCopy(renderer, winBannerTexture, nullptr, &winBannerRect);
		SDL_RenderCopy(renderer, scoreTexture, nullptr, &scoreRect);
		renderText("Score: " + to_string(score), 180, 210);
		renderText("Timer: " + to_string(time), 180, 300);
		SDL_RenderCopy(renderer, timerTexture, nullptr, &timerRect);

		if (currentLevel < 3) {
			SDL_RenderCopy(renderer, playButtonTexture, nullptr, &playButtonRect);
		}
		SDL_RenderCopy(renderer, quitButtonTexture, nullptr, &quitButtonRect);
		// Present the rendered content
		if (optimalPathFollowed) {
			renderText("You followed the most efficient path!", 250, 400);
		} else {
			renderText("You did not follow the most efficient path!", 230, 400);
		}
	}

	else if (lose) {
		SDL_RenderCopy(renderer, backgroundTexture, nullptr, &overlayRect);
		SDL_RenderCopy(renderer, quitButtonTexture, nullptr, &quitButtonRect);
		// SDL_RenderCopy(renderer, replayButtonTexture, nullptr, &replayButtonRect);
		renderText("You Lose!", 400, 150);
	}

	else if (isPaused) {
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
		SDL_Rect overlayRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
		SDL_RenderFillRect(renderer, &overlayRect);

		SDL_RenderCopy(renderer, playButtonTexture, nullptr, &playButtonRect);
		SDL_RenderCopy(renderer, quitButtonTexture, nullptr, &quitButtonRect);
		SDL_RenderCopy(renderer, isSoundOn ? soundOnButtonTexture : soundOffButtonTexture, nullptr, &soundButtonRect);
	} else {
		renderText("Time: " + to_string(remainingTime), 10, 10);
	}
	SDL_RenderPresent(renderer);
}

void MazeScreen::renderText(const string& text, int x, int y) {
	SDL_Color textColor = {255, 255, 255, 255};
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
	SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_Rect renderQuad = {x, y, textSurface->w, textSurface->h};
	SDL_RenderCopy(renderer, textTexture, nullptr, &renderQuad);
	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(textTexture);
}

