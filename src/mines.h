#ifndef MINES_H
#define MINES_H

#include <string>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <unordered_set>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "basescreen.h"

using namespace std;

// Constants
const int CELL_SIZE = 32;
const int GRID_WIDTH = 10;
const int GRID_HEIGHT = 10;
const int TOTAL_MINES = 10;
const int BUTTON_WIDTH = 52;
const int BUTTON_HEIGHT = 44;
const int TIMER_BAR_HEIGHT = 40;
const int LEFT_PANEL_WIDTH = 150;
const int RIGHT_PANEL_WIDTH = 200;
const SDL_Color DARK_BROWN = {101, 67, 33, 255};
const SDL_Color LIGHT_BROWN = {222, 184, 135, 255};
const SDL_Color BLACK = {0, 0, 0};
const SDL_Color WHITE = {255, 255, 255};


enum CellState { HIDDEN, REVEALED, FLAGGED };
enum CellContent { EMPTY, MINE, NUMBER };
enum ToolMode { SHOVEL, FLAG };

// Cell Class
class Cell {
	public:
		CellState state;
		CellContent content;
		int adjacentMines;

		Cell() : state(HIDDEN), content(EMPTY), adjacentMines(0) {}
};


class Mines : public BaseScreen {
	private:

		const int SCREEN_WIDTH = 900;
		const int SCREEN_HEIGHT = 500;
//const int BG_SCROLL_SPEED = 2;

		const float G_MOVEMENT_OFFSET = 0.5;
		const int FRAME_DELAY = 100; // Delay in milliseconds for frame animation
		SDL_Renderer* renderer;
		GameState* game;
		SDL_Texture* bg1;
		int bg1X, bg1Y, gx;
	public:
		void load(GameState* game);
		void handleEvents(SDL_Event event);
		void render();
		void renderText(TTF_Font* font, const char* text,
		                int x, int y, SDL_Color color);
		void cleanUp();
};


// Minesweeper Class
class Minesweeper : public BaseScreen {
	private:
		const int WINDOW_WIDTH = 1000;
		const int WINDOW_HEIGHT = 500;
		std::vector<std::vector<Cell>> grid;
		std::unordered_set<int> flaggedCells;
		bool isGameOver;
		int revealedCells;
		int currentFlags, currentScore;
		ToolMode currentTool;
		Uint32 startTime;
		Uint32 elapsedTime;
		bool isPaused, isSoundOn, quitGame;
		const int TIMER_LIMIT = 99;
		int maxFlags;



	public:
		Minesweeper() : isGameOver(false), revealedCells(0), currentFlags(0), currentTool(SHOVEL), elapsedTime(0), isPaused(false), isSoundOn(true) {
			grid.resize(GRID_WIDTH, std::vector<Cell>(GRID_HEIGHT));
		}

		bool isPause() {
			return isPaused;
		}
		void initGame();
		void placeMines();
		void calculateAdjacentMines();
		void revealCell(int x, int y);
		void revealAdjacentCells(int x, int y);
		void flagCell(int x, int y);
		void renderPauseScreen(SDL_Renderer* renderer, SDL_Texture* playButtonTexture,  SDL_Texture* quitButtonTexture);
		void renderGrid(SDL_Renderer* renderer, SDL_Texture* hiddenTexture, SDL_Texture* revealedTexture, SDL_Texture* flagTexture, SDL_Texture* mineTexture, TTF_Font* font);
		void renderUI(SDL_Renderer* renderer, TTF_Font* font, SDL_Texture* shovelButton, SDL_Texture* flagButton,SDL_Texture* pauseButtonTexture, SDL_Texture* soundButtonTexture, SDL_Texture* soundOffButtonTexture, Mix_Chunk* clickSound, SDL_Texture* playButtonTexture,  SDL_Texture* quitButtonTexture);
		void handleInput(SDL_Event& e, SDL_Rect& shovelButtonRect, SDL_Rect& flagButtonRect, SDL_Rect& pauseButtonRect,  SDL_Rect& soundButtonRect, Mix_Chunk* clickSound, SDL_Texture* playButtonTexture, SDL_Texture* quitButtonTexture);
		void updateTimer();
		bool isFinished() const {
			return isGameOver;
		}
		int getElapsedTime() const {
			return elapsedTime;
		}
		void handlePauseInput(SDL_Event& e, SDL_Texture* playButtonTexture, SDL_Texture* quitButtonTexture);
		void renderText(const std::string& text, int x, int y, SDL_Color color, TTF_Font* font, SDL_Renderer* renderer);
		void toggleFlag(int x, int y);

		void handleEvents(SDL_Event event); //-keymouse keypress
		void cleanUp(); //-clean free surfaces
		void load(GameState* game); //-load the textures, font
		void render(); //-screen draw


};

#endif