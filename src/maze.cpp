#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mixer.h"
#include <iostream>
#include <vector>
#include <stack>
#include <map>

// Constants for window size and tile size
const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 500;
const int TILE_SIZE = 50;
float bg2X1 = 0, bg3X1 = 0, bg4X1 = 0;
float bg2X2 = SCREEN_WIDTH, bg3X2 = SCREEN_WIDTH, bg4X2 = SCREEN_WIDTH;
float bg2Speed = 0.25;
float bg3Speed = 0.3;
float bg4Speed = 0.2;

// Custom graph structure for the irregular maze
class Graph {
	public:
		std::map<std::pair<int, int>, std::vector<std::pair<int, int>>> adjacencyList;

		void addEdge(std::pair<int, int> u, std::pair<int, int> v) {
			adjacencyList[u].push_back(v);
			adjacencyList[v].push_back(u);
		}

		bool areConnected(std::pair<int, int> u, std::pair<int, int> v) const {
			for (auto neighbor : adjacencyList.at(u)) {
				if (neighbor == v) return true;
			}
			return false;
		}

		const std::vector<std::pair<int, int>>& getNeighbors(std::pair<int, int> node) const {
			return adjacencyList.at(node);
		}
};

class Maze {
	public:
		Maze(const Graph& graph) : graph(graph) {}

		void render(SDL_Renderer* renderer, SDL_Texture* cellTexture) const {
			for (const auto& node : graph.adjacencyList) {
				int x = node.first.first;
				int y = node.first.second;
				SDL_Rect tileRect = { x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE + 5, TILE_SIZE };
				SDL_RenderCopy(renderer, cellTexture, nullptr, &tileRect);
			}
		}

	private:
		const Graph& graph;
};

class Player {
	public:
		Player(int startX, int startY) : currentPos({startX, startY}) {}

		void move(int dx, int dy, const Graph& graph, Mix_Chunk* moveSound) {
			std::pair<int, int> newPos = {currentPos.first + dx, currentPos.second + dy};

			if (graph.areConnected(currentPos, newPos)) {
				currentPos = newPos;
				std::cout << "Player moved to: (" << currentPos.first << ", " << currentPos.second << ")" << std::endl;
				Mix_PlayChannel(-1, moveSound, 0);  // Play movement sound
			}
		}

		void render(SDL_Renderer* renderer, SDL_Texture* playerTexture) const {
			SDL_Rect playerRect = { currentPos.first * TILE_SIZE, currentPos.second * TILE_SIZE, TILE_SIZE + 5, TILE_SIZE };
			SDL_RenderCopy(renderer, playerTexture, nullptr, &playerRect);
		}

	private:
		std::pair<int, int> currentPos;
};

class Game {
	public:
		Game(): window(nullptr), renderer(nullptr), player(3, 8), isRunning(true) {}

		bool init() {
			if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
				std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
				return false;
			}

			window = SDL_CreateWindow("Maze Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
			if (!window) {
				std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
				return false;
			}

			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
			if (!renderer) {
				std::cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
				return false;
			}

			backgroundTexture = loadTexture("assets/images/bg1.png");
			floorTexture = loadTexture("assets/images/floor.png");
			roofTexture = loadTexture("assets/images/bg1t.png");
			backgroundTexture2 = loadTexture("assets/images/bg2t.png");  // Additional background 1
			backgroundTexture3 = loadTexture("assets/images/bg2.png");  // Additional background 2
			backgroundTexture4 = loadTexture("assets/images/gummy-bear.png");
			cellTexture = loadTexture("assets/images/cell.png");
			playerTexture = loadTexture("assets/images/flag3.png");
			sunTexture = loadTexture("assets/images/flag.png");  // Sun texture
			lollipopTexture = loadTexture("assets/images/bomb.png");  // Lollipop texture

			if (!cellTexture || !playerTexture || !backgroundTexture || !backgroundTexture2 || !backgroundTexture3 || !backgroundTexture4 || !sunTexture || !lollipopTexture || !floorTexture || !roofTexture) {
				return false;
			}

			if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
				std::cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
				return false;
			}
			moveSound = Mix_LoadWAV("assets/audio/move.wav");
			if (!moveSound) {
				std::cout << "Failed to load movement sound! SDL_mixer Error: " << Mix_GetError() << std::endl;
				return false;
			}

			defineGraph();

			return true;
		}

		void run() {
			while (isRunning) {
				handleEvents();
				render();
			}
		}

		void cleanUp() {
			SDL_DestroyTexture(backgroundTexture);
			SDL_DestroyTexture(backgroundTexture2);
			SDL_DestroyTexture(backgroundTexture3);
			SDL_DestroyTexture(backgroundTexture4);
			SDL_DestroyTexture(floorTexture);
			SDL_DestroyTexture(roofTexture);
			SDL_DestroyTexture(cellTexture);
			SDL_DestroyTexture(playerTexture);
			SDL_DestroyTexture(sunTexture);
			SDL_DestroyTexture(lollipopTexture);
			Mix_FreeChunk(moveSound);
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			Mix_Quit();
			SDL_Quit();
		}

	private:
		SDL_Window* window;
		SDL_Renderer* renderer;
		SDL_Texture* backgroundTexture;
		SDL_Texture* backgroundTexture2;
		SDL_Texture* backgroundTexture3;
		SDL_Texture* backgroundTexture4;
		SDL_Texture* floorTexture;
		SDL_Texture* roofTexture;
		SDL_Texture* cellTexture;
		SDL_Texture* playerTexture;
		SDL_Texture* sunTexture;
		SDL_Texture* lollipopTexture;
		Mix_Chunk* moveSound;
		Graph mazeGraph;
		Maze maze = Maze(mazeGraph);
		Player player;
		bool isRunning;

		SDL_Texture* loadTexture(const std::string& path) {
			SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
			if (!texture) {
				std::cout << "Failed to load texture: " << path << " SDL_Error: " << SDL_GetError() << std::endl;
			}
			return texture;
		}

		void defineGraph() {
			mazeGraph.addEdge({3, 8}, {3, 7});
			mazeGraph.addEdge({3, 7}, {3, 6});
			mazeGraph.addEdge({3, 6}, {3, 5});
			mazeGraph.addEdge({3, 5}, {3, 4});
//        mazeGraph.addEdge({5, 6}, {5, 5});
//        mazeGraph.addEdge({5, 5}, {5, 4});
//        mazeGraph.addEdge({5, 4}, {5, 3});
//        mazeGraph.addEdge({5, 3}, {5, 2});
//        mazeGraph.addEdge({5, 2}, {5, 1});
//        mazeGraph.addEdge({5, 1}, {5, 0});
//        mazeGraph.addEdge({4, 6}, {3, 6});
//        mazeGraph.addEdge({5, 4}, {6, 4});
//        mazeGraph.addEdge({6, 4}, {7, 4});
//        mazeGraph.addEdge({3, 6}, {3, 5});
//        mazeGraph.addEdge({3, 5}, {3, 4});
//        mazeGraph.addEdge({3, 4}, {3, 3});
//        mazeGraph.addEdge({3, 3}, {3, 2});
		}

		void handleEvents() {
			SDL_Event event;
			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT) {
					isRunning = false;
				}
				if (event.type == SDL_KEYDOWN) {
					switch (event.key.keysym.sym) {
						case SDLK_UP:
							player.move(0, -1, mazeGraph, moveSound);
							break;
						case SDLK_DOWN:
							player.move(0, 1, mazeGraph, moveSound);
							break;
						case SDLK_LEFT:
							player.move(-1, 0, mazeGraph, moveSound);
							break;
						case SDLK_RIGHT:
							player.move(1, 0, mazeGraph, moveSound);
							break;
						case SDLK_q:
							isRunning = false;
							break;
					}
				}
			}
		}

		void render() {
			// Clear the screen
			SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
			SDL_RenderClear(renderer);

			// Scroll second background
			bg2X1 -= bg2Speed;
			bg2X2 -= bg2Speed;

			// If background 2 moves off screen, loop it
			if (bg2X1 <= -SCREEN_WIDTH) {
				bg2X1 = bg2X2 + SCREEN_WIDTH;
			}

			if (bg2X2 <= -SCREEN_WIDTH) {
				bg2X2 = bg2X1 + SCREEN_WIDTH;
			}

			// Scroll third background
			bg3X1 -= bg3Speed;
			bg3X2 -= bg3Speed;

			// If background 3 moves off screen, loop it
			if (bg3X1 <= -SCREEN_WIDTH) {
				bg3X1 = bg3X2 + SCREEN_WIDTH;
			}

			if (bg3X2 <= -SCREEN_WIDTH) {
				bg3X2 = bg3X1 + SCREEN_WIDTH;
			}

			// Scroll third background
			bg4X1 -= bg4Speed;
			bg4X2 -= bg4Speed;

			// If background 4 moves off screen, loop it
			if (bg4X1 <= -SCREEN_WIDTH) {
				bg4X1 = bg4X2 + SCREEN_WIDTH;
			}

			if (bg4X2 <= -SCREEN_WIDTH) {
				bg4X2 = bg4X1 + SCREEN_WIDTH;
			}

			// Render the first background (static)
			SDL_Rect backgroundRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
			SDL_RenderCopy(renderer, backgroundTexture, nullptr, &backgroundRect);

			SDL_Rect sunRect = { 600, 50, 180, 150 };  // Fixed position
			SDL_RenderCopy(renderer, sunTexture, nullptr, &sunRect);

			//CURVE
			SDL_Rect bg4Rect1 = { bg4X1, SCREEN_HEIGHT/3, SCREEN_WIDTH, 130 };
			SDL_Rect bg4Rect2 = { bg4X2, SCREEN_HEIGHT/3, SCREEN_WIDTH, 130 };
			SDL_RenderCopy(renderer, backgroundTexture4, nullptr, &bg4Rect1);
			SDL_RenderCopy(renderer, backgroundTexture4, nullptr, &bg4Rect2);

			// Render the second background (scrolling) LIGHT ORANGE
			SDL_Rect bg2Rect1 = { bg2X1, SCREEN_HEIGHT/4, SCREEN_WIDTH, 300 };
			SDL_Rect bg2Rect2 = { bg2X2, SCREEN_HEIGHT/4, SCREEN_WIDTH, 300 };
			SDL_RenderCopy(renderer, backgroundTexture2, nullptr, &bg2Rect1);
			SDL_RenderCopy(renderer, backgroundTexture2, nullptr, &bg2Rect2);

			// Render the third background (scrolling) ORANGE
			SDL_Rect bg3Rect1 = { bg3X1, SCREEN_HEIGHT/4, SCREEN_WIDTH, 350 };
			SDL_Rect bg3Rect2 = { bg3X2, SCREEN_HEIGHT/4, SCREEN_WIDTH, 350 };
			SDL_RenderCopy(renderer, backgroundTexture3, nullptr, &bg3Rect1);
			SDL_RenderCopy(renderer, backgroundTexture3, nullptr, &bg3Rect2);

			SDL_Rect floorRect = { 0, 400, SCREEN_WIDTH, 100 };
			SDL_RenderCopy(renderer, floorTexture, nullptr, &floorRect);

			SDL_Rect roofRect = { 0, 0, SCREEN_WIDTH, 50 };
			SDL_RenderCopy(renderer, roofTexture, nullptr, &roofRect);

			// Render sun and lollipop
			SDL_Rect lollipopRect = { 20, 345, 50, 75 };  // Fixed position
			SDL_RenderCopy(renderer, lollipopTexture, nullptr, &lollipopRect);

			// Render maze and player
			maze.render(renderer, cellTexture);
			player.render(renderer, playerTexture);

			// Present the rendered content
			SDL_RenderPresent(renderer);
		}

};

int mazeMain() {
	Game game;

	if (!game.init()) {
		std::cout << "Failed to initialize game!" << std::endl;
		return -1;
	}

	game.run();
	game.cleanUp();

	return 0;
}
