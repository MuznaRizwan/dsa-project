#ifndef MAZE_H
#define MAZE_H
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mixer.h"
#include <iostream>
#include <vector>
#include <stack>
#include <map>

#include "basescreen.h"

// Constants for window size and tile size
const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 500;
const int TILE_SIZE = 50;

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

		int mazeMain();

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

class MazeScreen : public BaseScreen {
	private:
		float bg2X1 = 0, bg3X1 = 0, bg4X1 = 0;
		float bg2X2 = SCREEN_WIDTH, bg3X2 = SCREEN_WIDTH, bg4X2 = SCREEN_WIDTH;
		float bg2Speed = 0.25;
		float bg3Speed = 0.3;
		float bg4Speed = 0.2;
		SDL_Renderer* renderer;
		GameState* game;
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

	public:
		MazeScreen(): renderer(nullptr), player(3, 8), isRunning(true) {}
		void handleEvents(SDL_Event event); //-keymouse keypress
		void cleanUp(); //-clean free surfaces
		void load(GameState* game); //-load the textures, font
		void render(); //-screen draw
		SDL_Texture* loadTexture(const std::string& path);
		void defineGraph();
};
#endif
