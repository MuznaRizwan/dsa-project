#include "mines.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <unordered_set>
#include <vector>

void Minesweeper::updateTimer() {
	if (isPaused || win || loose) return;
	elapsedTime = (SDL_GetTicks() - startTime) / 1000;
}

// void Minesweeper::renderText(TTF_Font* font, const char* text, int x, int y, SDL_Color color)
// {
// 	SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
// 	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
// 	SDL_Rect destRect = { x, y, surface->w, surface->h };
// 	SDL_RenderCopy(renderer, texture, nullptr, &destRect);
// 	SDL_FreeSurface(surface);
// 	SDL_DestroyTexture(texture);
// }

void Minesweeper::initGame() {
	srand(static_cast<unsigned>(time(0)));
	revealedCells = 0;
	currentFlags = 0;
	//clickSound = Mix_LoadWAV("click.wav");
	maxFlags = TOTAL_MINES;
	// isGameOver = false;
	currentScore = 0;
	elapsedTime = 0;

	// Initialize the grid
	grid.resize(GRID_WIDTH, std::vector<Cell>(GRID_HEIGHT));
	for (int x = 0; x < GRID_WIDTH; ++x) {
		for (int y = 0; y < GRID_HEIGHT; ++y) {
			grid[x][y] = Cell();
		}
	}

	placeMines();
	calculateAdjacentMines();
	startTime = SDL_GetTicks(); // Reset the timer
}

void Minesweeper::handlePauseInput(SDL_Event& e, SDL_Texture* playButtonTexture, SDL_Texture* quitButtonTexture) {
	if (e.type == SDL_MOUSEBUTTONDOWN) {
		int x = e.button.x;
		int y = e.button.y;

		SDL_Rect playButtonRect = { 300, 200, BUTTON_WIDTH, BUTTON_HEIGHT };
		SDL_Rect quitButtonRect = { 500, 300, BUTTON_WIDTH, BUTTON_HEIGHT };

		if (x >= playButtonRect.x && x <= playButtonRect.x + BUTTON_WIDTH &&
			y >= playButtonRect.y && y <= playButtonRect.y + BUTTON_HEIGHT) {
			isPaused = false; // Resume the game
		} else if (x >= quitButtonRect.x && x <= quitButtonRect.x + BUTTON_WIDTH &&
			y >= quitButtonRect.y && y <= quitButtonRect.y + BUTTON_HEIGHT) {
			// isGameOver = true; // End the game
			running = false;
		}
	}
}

// void Minesweeper::toggleFlag(int x, int y) {
// 	int idx = x * GRID_HEIGHT + y;
// 	if (grid[x][y].state == HIDDEN && flaggedCells.size() < static_cast<size_t>(maxFlags)) {
// 		grid[x][y].state = FLAGGED;
// 		flaggedCells.insert(idx);
// 	}
// 	else if (grid[x][y].state == FLAGGED) {
// 		grid[x][y].state = HIDDEN;
// 		flaggedCells.erase(idx);
// 	}
// }

void Minesweeper::placeMines() {
	int placedMines = 0;
	while (placedMines < TOTAL_MINES) {
		int x = rand() % GRID_WIDTH;
		int y = rand() % GRID_HEIGHT;
		if (grid[x][y].content != MINE) {
			grid[x][y].content = MINE;
			++placedMines;
		}
	}
}

void Minesweeper::calculateAdjacentMines() {
	for (int x = 0; x < GRID_WIDTH; ++x) {
		for (int y = 0; y < GRID_HEIGHT; ++y) {
			if (grid[x][y].content == MINE) continue;
			for (int dx = -1; dx <= 1; ++dx) {
				for (int dy = -1; dy <= 1; ++dy) {
					int nx = x + dx, ny = y + dy;
					if (nx >= 0 && nx < GRID_WIDTH && ny >= 0 && ny < GRID_HEIGHT && grid[nx][ny].content == MINE) {
						grid[x][y].adjacentMines++;
					}
				}
			}
			if (grid[x][y].adjacentMines > 0) {
				grid[x][y].content = NUMBER;
			}
		}
	}
}

void Minesweeper::revealCell(int x, int y, SDL_Renderer* renderer, SDL_Texture* screenTexture, SDL_Texture* orangeTexture, SDL_Texture* headTexture, SDL_Texture* backgroundTexture, TTF_Font* font) {
	Cell& cell = grid[x][y];
    if (win || loose || cell.state != HIDDEN) return;

	cell.state = REVEALED;
	revealedCells++;
	currentScore++;

	if (revealedCells == SAFE_CELLS) {
		win = true;
	}
	if (cell.content == MINE) {
		loose = true;
		return;
	}

	if (cell.content == EMPTY) {
        revealAdjacentCells(x, y, renderer,screenTexture, orangeTexture, headTexture, backgroundTexture, font);
	}
}

void Minesweeper::revealAdjacentCells(int x, int y, SDL_Renderer* renderer,SDL_Texture* screenTexture, SDL_Texture* orangeTexture, SDL_Texture* headTexture, SDL_Texture* backgroundTexture, TTF_Font* font) {
	for (int dx = -1; dx <= 1; ++dx) {
		for (int dy = -1; dy <= 1; ++dy) {
			if (dx == 0 && dy == 0) continue;
			int nx = x + dx, ny = y + dy;
			if (nx >= 0 && nx < GRID_WIDTH && ny >= 0 && ny < GRID_HEIGHT) {
                revealCell(nx, ny, renderer,screenTexture, orangeTexture, headTexture,backgroundTexture, font);
			}
		}
	}
}

void Minesweeper::flagCell(int x, int y) {
    if (win || loose) return;

    int idx = x * GRID_HEIGHT + y;
	Cell& cell = grid[x][y];
	// if (isGameOver || cell.state == REVEALED) return;

    // Add flag
    if (cell.state == HIDDEN && flaggedCells.size() < static_cast<size_t>(maxFlags)) {
		cell.state = FLAGGED;
        flaggedCells.insert(idx);
		currentFlags++;
	}
    // Remove flag
	else if (cell.state == FLAGGED) {
		cell.state = HIDDEN;
        flaggedCells.erase(idx);
		currentFlags--;
	}
}

void Minesweeper::handleInput(SDL_Event& e, SDL_Rect& shovelButtonRect, SDL_Rect& flagButtonRect, SDL_Rect& pauseButtonRect,
                              SDL_Rect& soundButtonRect, Mix_Chunk* clickSound, SDL_Texture* playButtonTexture, 
                              SDL_Texture* quitButtonTexture, SDL_Renderer* renderer, SDL_Texture* screenTexture, SDL_Texture* orangeTexture, SDL_Texture* headTexture, 
                              SDL_Texture* backgroundTexture, TTF_Font* font) {
//    if (win) {
//       renderBackground( renderer, backgroundTexture, screenTexture, orangeTexture, headTexture, font); // Show the game-over overlay
//        return; // Disable further input
//    } else if (loose) {
//       renderBackground( renderer, backgroundTexture, screenTexture, orangeTexture, headTexture, font);
//	}

    if (isPaused) {
        handlePauseInput(e, playButtonTexture, quitButtonTexture);
        return;
    }

    if (e.type == SDL_MOUSEBUTTONDOWN) {
        int x = e.button.x;
        int y = e.button.y;

        // Tool selection
        if (x >= shovelButtonRect.x && x <= shovelButtonRect.x + BUTTON_WIDTH &&
            y >= shovelButtonRect.y && y <= shovelButtonRect.y + BUTTON_HEIGHT) {
            currentTool = SHOVEL;
        } else if (x >= flagButtonRect.x && x <= flagButtonRect.x + BUTTON_WIDTH &&
                   y >= flagButtonRect.y && y <= flagButtonRect.y + BUTTON_HEIGHT) {
            if (!win && !loose) // Allow flag selection only if game is not over
                currentTool = FLAG;
        } else if (x >= soundButtonRect.x && x <= soundButtonRect.x + BUTTON_WIDTH &&
                   y >= soundButtonRect.y && y <= soundButtonRect.y + BUTTON_HEIGHT) {
            isSoundOn = !isSoundOn;
            return;
        } else if (x >= pauseButtonRect.x && x <= pauseButtonRect.x + BUTTON_WIDTH &&
                   y >= pauseButtonRect.y && y <= pauseButtonRect.y + BUTTON_HEIGHT) {
            isPaused = !isPaused;
            return;
        }

        // Grid interaction
        int gridXOffset = (WINDOW_WIDTH - GRID_WIDTH * CELL_SIZE) / 2;
        int gridYOffset = (WINDOW_HEIGHT - GRID_HEIGHT * CELL_SIZE) / 2;
        x = (x - gridXOffset) / CELL_SIZE;
        y = (y - gridYOffset) / CELL_SIZE;

        if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
            if (currentTool == SHOVEL) {
                revealCell(x, y, renderer,screenTexture, orangeTexture, headTexture, backgroundTexture, font);
                if (clickSound && isSoundOn) Mix_PlayChannel(-1, clickSound, 0);
            } else if (currentTool == FLAG) {
                flagCell(x, y);
            }
        }
    }
}

void Minesweeper::renderPauseScreen(SDL_Renderer* renderer, SDL_Texture* playButtonTexture, SDL_Texture* quitButtonTexture) {
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
	SDL_Rect overlayRect = { 0, 0, 1000, 500 };
	SDL_RenderFillRect(renderer, &overlayRect);

	SDL_Rect playButtonRect = { 300, 200, BUTTON_WIDTH, BUTTON_HEIGHT };
	SDL_Rect quitButtonRect = { 300, 300, BUTTON_WIDTH, BUTTON_HEIGHT };

	SDL_RenderCopy(renderer, playButtonTexture, nullptr, &playButtonRect);
	SDL_RenderCopy(renderer, quitButtonTexture, nullptr, &quitButtonRect);
}

void Minesweeper::renderGrid(SDL_Renderer* renderer, SDL_Texture* hiddenTexture, SDL_Texture* revealedTexture, SDL_Texture* flagTexture, SDL_Texture* mineTexture, TTF_Font* font) {

	int gridXOffset = (WINDOW_WIDTH - GRID_WIDTH * CELL_SIZE) / 2;
	int gridYOffset = (WINDOW_HEIGHT - GRID_HEIGHT * CELL_SIZE) / 2;

	for (int x = 0; x < GRID_WIDTH; ++x) {
		for (int y = 0; y < GRID_HEIGHT; ++y) {
			Cell& cell = grid[x][y];
			SDL_Rect cellRect = { gridXOffset + x * CELL_SIZE, gridYOffset + y * CELL_SIZE, CELL_SIZE, CELL_SIZE };
			if (cell.state == HIDDEN) {
				SDL_RenderCopy(renderer, hiddenTexture, nullptr, &cellRect);
			} else if (cell.state == FLAGGED) {
				SDL_RenderCopy(renderer, flagTexture, nullptr, &cellRect);
			} else if (cell.state == REVEALED) {
				SDL_RenderCopy(renderer, revealedTexture, nullptr, &cellRect);
				if (cell.content == MINE) {
					SDL_RenderCopy(renderer, mineTexture, nullptr, &cellRect);
				} else if (cell.content == NUMBER) {
					//SDL_Color textColor = {0, 0, 0};
					renderText(std::to_string(grid[x][y].adjacentMines), cellRect.x + 10, cellRect.y + 5, BLACK, font, renderer);
				}
			}
		}
	}
}

void Minesweeper::renderText(const std::string& text, int x, int y, SDL_Color color, TTF_Font* font, SDL_Renderer* renderer) {
	SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_Rect textRect = { x, y, surface->w, surface->h };
	SDL_RenderCopy(renderer, texture, nullptr, &textRect);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}

void Minesweeper::renderUI(SDL_Renderer* renderer, TTF_Font* font, SDL_Texture* shovelButton, SDL_Texture* flagButton, 
SDL_Texture* pauseButtonTexture, SDL_Texture* soundButtonTexture, SDL_Texture* soundOffButtonTexture, Mix_Chunk* clickSound, 
SDL_Texture* playButtonTexture, SDL_Texture* quitButtonTexture, SDL_Texture* screenTexture, SDL_Texture* orangeTexture, SDL_Texture* headTexture, SDL_Texture* backgroundTexture) {

	int remainingTime = TIMER_LIMIT - (elapsedTime);
	if (remainingTime <= 0) {
		renderText("Time's over!", 300, 20, BLACK, font, renderer);
		loose = true;
		return;
	}
	renderText("Time: " + std::to_string(remainingTime) + "s", 10, 10, BLACK, font, renderer);
	renderText("Flags: " + std::to_string(currentFlags), 10, 40, BLACK, font, renderer);
	renderText("Score: " + std::to_string(currentScore), 10, 70, BLACK, font, renderer);
	if (currentFlags >= maxFlags) {
		renderText("FLAG LIMIT EXCEEDED!", 200, 5, BLACK, font, renderer);
	}

	SDL_Rect shovelButtonRect = { 10, 110, BUTTON_WIDTH, BUTTON_HEIGHT };
	SDL_Rect flagButtonRect = { 70, 110, BUTTON_WIDTH, BUTTON_HEIGHT };
	SDL_Rect pauseButtonRect = { 10, 300, BUTTON_WIDTH, BUTTON_HEIGHT };
	SDL_Rect soundButtonRect = { 10, 200, BUTTON_WIDTH, BUTTON_HEIGHT };

	SDL_RenderCopy(renderer, shovelButton, nullptr, &shovelButtonRect);
	SDL_RenderCopy(renderer, flagButton, nullptr, &flagButtonRect);
	SDL_RenderCopy(renderer, pauseButtonTexture, nullptr, &pauseButtonRect);
	SDL_RenderCopy(renderer, isSoundOn ? soundButtonTexture : soundOffButtonTexture, nullptr, &soundButtonRect);

	SDL_SetRenderDrawColor(renderer, DARK_BROWN.r, DARK_BROWN.g, DARK_BROWN.b, DARK_BROWN.a); // brown
	if (currentTool == SHOVEL) {
		SDL_RenderDrawRect(renderer, &shovelButtonRect);
	} else if (currentTool == FLAG) {
		SDL_RenderDrawRect(renderer, &flagButtonRect);
	}

	if (isPaused) {
		renderPauseScreen(renderer, playButtonTexture, quitButtonTexture);    
		//SDL_SetRenderDrawColor(renderer,BLACK); // Reset to black
	} else if(win){
		std::cout << "win\n";
		renderBackground(renderer, backgroundTexture,screenTexture, orangeTexture, headTexture, font);
//		return;
	} else if (loose) {
		std::cout << "loose\n";
		renderBackground(renderer, backgroundTexture,screenTexture, orangeTexture, headTexture, font);
	}
}

//void Minesweeper::handleInput(SDL_Event& e, SDL_Rect& shovelButtonRect, SDL_Rect& flagButtonRect, SDL_Rect& pauseButtonRect,
//	SDL_Rect& soundButtonRect, Mix_Chunk* clickSound, SDL_Texture* playButtonTexture, SDL_Texture* quitButtonTexture) {
//	if (isPause()) {
//		handlePauseInput(e, playButtonTexture, quitButtonTexture);
//		return;
//	}
//	else {
//
//		if (e.type == SDL_MOUSEBUTTONDOWN) {
//			int x = e.button.x;
//			int y = e.button.y;
//
//			if (x >= shovelButtonRect.x && x <= shovelButtonRect.x + BUTTON_WIDTH &&
//				y >= shovelButtonRect.y && y <= shovelButtonRect.y + BUTTON_HEIGHT) {
//				currentTool = SHOVEL;
//			}
//			else if (x >= flagButtonRect.x && x <= flagButtonRect.x + BUTTON_WIDTH &&
//				y >= flagButtonRect.y && y <= flagButtonRect.y + BUTTON_HEIGHT) {
//				currentTool = FLAG;
//			}
//			else if (x >= soundButtonRect.x && x <= soundButtonRect.x + BUTTON_WIDTH &&
//				y >= soundButtonRect.y && y <= soundButtonRect.y + BUTTON_HEIGHT) {
//				isSoundOn = !isSoundOn;
//				return;
//			}
//			else if (x >= pauseButtonRect.x && x <= pauseButtonRect.x + BUTTON_WIDTH &&
//				y >= pauseButtonRect.y && y <= pauseButtonRect.y + BUTTON_HEIGHT) {
//				isPaused = !isPaused;
//				isPause(); // Toggle pause state
//				return;
//			}
//			int gridXOffset = (WINDOW_WIDTH - GRID_WIDTH * CELL_SIZE) / 2;
//			int gridYOffset = (WINDOW_HEIGHT - GRID_HEIGHT * CELL_SIZE) / 2;
//			x = (x - gridXOffset) / CELL_SIZE;
//			y = (y - gridYOffset) / CELL_SIZE;
//
//			if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
//				if (currentTool == SHOVEL) {
//					revealCell(x, y);
//					if (clickSound && isSoundOn) Mix_PlayChannel(-1, clickSound, 0);
//				}
//				else if (currentTool == FLAG) {
//					flagCell(x, y);
//					// toggleFlag(x, y);
//				}
//			}
//		}
//	}
//}

// Main function
void Minesweeper::load(GameState* game) {
	this->game = game;
	this->renderer = game->renderer;
	const int WINDOW_WIDTH = 1000;
	const int WINDOW_HEIGHT = 500;
	//	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	//	IMG_Init(IMG_INIT_PNG);
	//	TTF_Init();
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

	//	SDL_Window* window = SDL_CreateWindow("Minesweeper", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	//	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	hiddenTexture = IMG_LoadTexture(renderer, "assets/images/hidden.png");
	revealedTexture = IMG_LoadTexture(renderer, "assets/images/revealed.png");
	flagTexture = IMG_LoadTexture(renderer, "assets/images/flag_button.png");
	mineTexture = IMG_LoadTexture(renderer, "assets/images/mine.png");
	shovelButton = IMG_LoadTexture(renderer, "assets/images/shovel_button.png");
	flagButton = IMG_LoadTexture(renderer, "assets/images/flag_button.png");
	pauseButtonTexture = IMG_LoadTexture(renderer, "assets/images/pause.png");
	playButtonTexture = IMG_LoadTexture(renderer, "assets/images/play_button.png");
	quitButtonTexture = IMG_LoadTexture(renderer, "assets/images/quit_button.png");
	soundButtonTexture = IMG_LoadTexture(renderer, "assets/images/sound_button.png");
	soundOffButtonTexture = IMG_LoadTexture(renderer, "assets/images/volumeoff.png");
    backgroundTexture = IMG_LoadTexture(renderer, "assets/images/minesweeper.png");
    screenTexture = IMG_LoadTexture(renderer, "assets/images/pinkbg.png");
    orangeTexture = IMG_LoadTexture(renderer, "assets/images/lightorangebg.png");
    headTexture = IMG_LoadTexture(renderer, "assets/images/heading.png");
	if (!screenTexture || !orangeTexture || !headTexture) {
	    std::cerr << "Failed to load overlay: " << IMG_GetError() << std::endl;
	    return; // -1;
	}

	if (!backgroundTexture) {
	    std::cerr << "Failed to load background image: " << IMG_GetError() << std::endl;
	    return; // -1;
	}

	font = TTF_OpenFont("assets/font/textN.ttf", 24);
    font2 = TTF_OpenFont("assets/images/arial.ttf", 28);
	clickSound = Mix_LoadWAV("assets/audio/click.wav");
	//	Minesweeper minesweeperGame;
	//	minesweeperGame.initGame();

	initGame();

}

void Minesweeper::renderBackground(SDL_Renderer* renderer, SDL_Texture* backgroundTexture,SDL_Texture* screenTexture,
		SDL_Texture* orangeTexture, SDL_Texture* headTexture, TTF_Font* font ){
	if(win){
		SDL_RenderCopy(renderer, screenTexture, nullptr, nullptr);
	    SDL_RenderCopy(renderer, orangeTexture, nullptr, nullptr);

	    SDL_Rect headTextureRect = {300, 50,300, 50};
	    SDL_RenderCopy(renderer, headTexture, nullptr, &headTextureRect);
	    renderText("Score: " + std::to_string(currentScore), 300, 150, BLACK, font, renderer);
	} else if (loose) {
		SDL_RenderCopy(renderer, screenTexture, nullptr, nullptr);
	    SDL_RenderCopy(renderer, orangeTexture, nullptr, nullptr);
	    renderText("YOU LOST!", 200, 150, BLACK, font, renderer);
	    renderText("Score: " + std::to_string(currentScore), 300, 150, BLACK, font, renderer);
	} else {
    	SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
	}
}

void Minesweeper::handleEvents(SDL_Event e) {
	//bool running = true;

	//	while (running) {
	//		SDL_Event e;
	//		while (SDL_PollEvent(&e)) {
	if (e.type == SDL_QUIT) running = false;
	handleInput(e, shovelButtonRect, flagButtonRect, pauseButtonRect, soundButtonRect, clickSound, playButtonTexture, quitButtonTexture,renderer,screenTexture, orangeTexture, headTexture,backgroundTexture, font);
	//		}
}
void Minesweeper::render() {
	updateTimer();

	SDL_SetRenderDrawColor(renderer, 196, 164, 132, 255);
	SDL_RenderClear(renderer);
    renderBackground(renderer, backgroundTexture, screenTexture, orangeTexture, headTexture, font2);
	//	renderOverlay(renderer, overlayTexture);

    renderGrid(renderer, hiddenTexture, revealedTexture, flagTexture, mineTexture, font);
    renderUI(renderer, font, shovelButton, flagButton, pauseButtonTexture,soundButtonTexture,soundOffButtonTexture,clickSound, playButtonTexture, quitButtonTexture,screenTexture, orangeTexture, headTexture, backgroundTexture);

	SDL_RenderPresent(renderer);
}

void Minesweeper::cleanUp() {
	SDL_DestroyTexture(hiddenTexture);
	SDL_DestroyTexture(revealedTexture);
	SDL_DestroyTexture(flagTexture);
	SDL_DestroyTexture(mineTexture);
	SDL_DestroyTexture(shovelButton);
	SDL_DestroyTexture(flagButton);
	SDL_DestroyTexture(playButtonTexture);
	SDL_DestroyTexture(quitButtonTexture);
	SDL_DestroyTexture(pauseButtonTexture);
	SDL_DestroyTexture(soundButtonTexture);
	SDL_DestroyTexture(soundOffButtonTexture);
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(headTexture);
	SDL_DestroyTexture(orangeTexture);
	SDL_DestroyTexture(screenTexture);

//    SDL_DestroyRenderer(renderer);
//    SDL_DestroyWindow(window);
//    TTF_CloseFont(font);
	Mix_FreeChunk(clickSound);

//	Mix_CloseAudio();
	//	TTF_Quit();
	//	IMG_Quit();
	//	SDL_Quit();

	//	return 0;
}



