#ifndef MAZE_H
#define MAZE_H
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

#include "basescreen.h"

// Constants for window size and tile size
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 500;
const int TILE_SIZE = 100;

//int cameraX = 0;
//int cameraY = 0;
//
//bool isSoundOn = true;

// Custom graph structure for the irregular maze
class Graph {
	public:
		std::map<std::pair<int, int>, std::vector<std::pair<int, int>>> adjacencyList;

		map<pair<int, int>, int> cellTypes;
		pair<int, int> lastNode;
		vector<pair<int, int>> optimalPath;

		void addEdge(std::pair<int, int> u, std::pair<int, int> v, int typeU = 1, int typeV = 1) {
			adjacencyList[u].push_back(v);
			adjacencyList[v].push_back(u);
			cellTypes[u] = typeU;
			cellTypes[v] = typeV;
		}

		void setLastNode(pair<int, int> node) {
			lastNode = node;
		}

		bool isLastNode(pair<int, int> node) const {
			return node == lastNode;
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

		bool dfs(pair<int, int> current, pair<int, int> target, set<pair<int, int>>& visited, vector<pair<int, int>>& path) {
			if (current == target) {
				path.push_back(current);
				return true;
			}

			visited.insert(current);

			for (auto& neighbor : adjacencyList[current]) {
				if (visited.find(neighbor) == visited.end()) {
					if (dfs(neighbor, target, visited, path)) {
						path.push_back(current);
						return true;
					}
				}
			}

			return false;
		}

		void findShortestPath(pair<int, int> start, pair<int, int> end) {
			set<pair<int, int>> visited;
			vector<pair<int, int>> path;
			optimalPath.clear();

			if (dfs(start, end, visited, path)) {
				reverse(path.begin(), path.end());
				optimalPath = path;
			}
		}

};

class Maze {
	public:
		Maze(const Graph* graph) : graph(graph) {}

		void render(SDL_Renderer* renderer, SDL_Texture* cellTexture1, SDL_Texture* cellTexture2, SDL_Texture* lastNodeTexture, int cameraX, int cameraY) const {
			for (const auto& node : graph->adjacencyList) {
				int x = node.first.first * TILE_SIZE - cameraX;
				int y = node.first.second * TILE_SIZE - cameraY;
				if (x + TILE_SIZE >= 0 && x <= SCREEN_WIDTH && y + TILE_SIZE >= 50 && y <= SCREEN_HEIGHT - 50) {
					SDL_Texture* currentTexture;
					if (graph->isLastNode(node.first)) {
						currentTexture = lastNodeTexture;
					} else {
						currentTexture = (graph->cellTypes.at(node.first) == 1) ? cellTexture1 : cellTexture2;
					}

					SDL_Rect tileRect = { x, y, TILE_SIZE, TILE_SIZE };
					SDL_RenderCopy(renderer, currentTexture, nullptr, &tileRect);
				}
			}
		}

	private:
		const Graph* graph;
};

class Player {
	public:
		std::pair<int, int> currentPos;
		vector<pair<int, int>> moves;

		Player(int startX, int startY) : currentPos({ startX, startY }), animationFrame(0) {
			moves.push_back(currentPos);
		}

		void move(int dx, int dy, Graph& graph, Mix_Chunk* moveSound) {
			std::pair<int, int> newPos = { currentPos.first + dx, currentPos.second + dy };

			if (graph.areConnected(currentPos, newPos)) {
				currentPos = newPos;
				moves.push_back(currentPos);
				animationFrame = (animationFrame + 1) % 2;
				std::cout << "Player moved to: (" << currentPos.first << ", " << currentPos.second << ")" << std::endl;
				if (isSoundOn) Mix_PlayChannel(-1, moveSound, 0);  // Play movement sound

				cameraX = currentPos.first * TILE_SIZE - SCREEN_WIDTH / 2;
				cameraY = currentPos.second * TILE_SIZE - SCREEN_HEIGHT / 2;

			}
		}

		void resetMoves() {
			moves.clear();
			moves.push_back(currentPos);
		}

		void render(SDL_Renderer* renderer, SDL_Texture* playerTexture1, SDL_Texture* playerTexture2) const {
			int playerRenderX = currentPos.first * TILE_SIZE - cameraX;
			int playerRenderY = currentPos.second * TILE_SIZE - cameraY;
			SDL_Rect playerRect = { playerRenderX, playerRenderY, TILE_SIZE, TILE_SIZE };

			SDL_Texture* currentTexture = (animationFrame == 0) ? playerTexture1 : playerTexture2;
			SDL_RenderCopy(renderer, currentTexture, nullptr, &playerRect);
		}

		pair<int, int> getCurrentPosition() const {
			return currentPos;
		}

	private:
		int animationFrame;
		int cameraX = 0;
		int cameraY = 0;

		bool isSoundOn = true;
};

class MazeScreen : public BaseScreen {
	private:
		float bg2X1 = 0, bg3X1 = 0, bg4X1 = 0;
		float bg2X2 = SCREEN_WIDTH, bg3X2 = SCREEN_WIDTH, bg4X2 = SCREEN_WIDTH;
		const float bg2Speed = 0.25;
		const float bg3Speed = 0.3;
		const float bg4Speed = 0.2;
		const int startX = 5;
		const int startY = 3;
		int gameDuration;
		int score = 0;
		int level = 1;
		const int levelTimers[3] = {50, 40, 30};

		int mazeWidth = 20;
		int mazeHeight = 20;

		bool optimalPathFollowed = false;
		int remainingTime;
		int time = 0;

		GameState* game = nullptr;
		SDL_Renderer* renderer = nullptr;
		SDL_Texture* backgroundTexture = nullptr;
		SDL_Texture* backgroundTexture2 = nullptr;
		SDL_Texture* backgroundTexture3 = nullptr;
		SDL_Texture* backgroundTexture4 = nullptr;
		SDL_Texture* floorTexture = nullptr;
		SDL_Texture* roofTexture = nullptr;
		SDL_Texture* cellTexture = nullptr;
		SDL_Texture* playerTexture1 = nullptr;
		SDL_Texture* playerTexture2 = nullptr;
		SDL_Texture* sunTexture = nullptr;
		SDL_Texture* moonTexture = nullptr;
		SDL_Texture* lollipopTexture = nullptr;
		SDL_Texture* pauseButtonTexture = nullptr;
		SDL_Texture* playButtonTexture = nullptr;
		SDL_Texture* quitButtonTexture = nullptr;
		SDL_Texture* soundOnButtonTexture = nullptr;
		SDL_Texture* soundOffButtonTexture = nullptr;
		SDL_Texture* cellTexture1 = nullptr;
		SDL_Texture* cellTexture2 = nullptr;
		SDL_Texture* lastNodeTexture = nullptr;
		SDL_Texture* scoreTexture = nullptr;
		SDL_Texture* timerTexture = nullptr;
//		SDL_Texture* replayButtonTexture = nullptr;
		SDL_Texture* winBannerTexture = nullptr;

		SDL_Rect pauseButtonRect = { 0, 0, 0, 0};
		SDL_Rect playButtonRect = { 0, 0, 0, 0};
		SDL_Rect quitButtonRect = { 0, 0, 0, 0};
		SDL_Rect soundButtonRect = { 0, 0, 0, 0};
		SDL_Rect overlayRect = { 0, 0, 0, 0};
		SDL_Rect scoreRect = { 0, 0, 0, 0};
		SDL_Rect timerRect = {80, 285, 80, 80};
//		SDL_Rect replayButtonRect = { 0, 0, 0, 0};
		SDL_Rect winBannerRect = {250, 50, 500, 100};
		Mix_Chunk* moveSound = nullptr;
		TTF_Font* font = nullptr;

		Uint32 startTime;
		Uint32 elapsedTime;
		Uint32 pausedTime;
		Uint32 pauseStartTime;

		Graph mazeGraph;
		Player player;

		bool isRunning;
		bool win;
		bool loose;
		bool isPaused;

		int currentLevel = 1;
		SDL_Texture* levelBackgrounds[3];
		Graph levelGraphs[3];
		Maze* maze;
		Maze* levelMazes[3];
		int cameraX = 0;
		int cameraY = 0;

		bool isSoundOn = true;

	public:
		MazeScreen() : renderer(nullptr), player(3, 8), isRunning(true) {}
		void handleEvents(SDL_Event event); //-keymouse keypress
		void cleanUp(); //-clean free surfaces
		void load(GameState* game); //-load the textures, font
		void render(); //-screen draw
		SDL_Texture* loadTexture(const std::string& path);
		void defineL1Graph();
		void defineL2Graph();
		void defineL3Graph();
		void loadNextLevel();
		void replayLevel();
		void run();
		void checkLevelCompletion();
		void renderText(const string& text, int x, int y);
};
#endif
