#ifndef MINES_H
#define MINES_H

#include "basescreen.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <unordered_set>
#include <vector>

using namespace std;

// Constants
const int CELL_SIZE = 32;
const int GRID_WIDTH = 10;
const int GRID_HEIGHT = 10;
const int TOTAL_MINES = 10;
const int SAFE_CELLS = 90;
const int BUTTON_WIDTH = 52;
const int BUTTON_HEIGHT = 44;
const int TIMER_BAR_HEIGHT = 40;
const int LEFT_PANEL_WIDTH = 150;
const int RIGHT_PANEL_WIDTH = 200;
const SDL_Color DARK_BROWN = { 101, 67, 33, 255 };
const SDL_Color LIGHT_BROWN = { 222, 184, 135, 255 };
const SDL_Color BLACK = { 0, 0, 0 };
const SDL_Color WHITE = { 255, 255, 255 };

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

// Minesweeper Class
class Minesweeper : public BaseScreen {
private:
	const int SCREEN_WIDTH = 900;
	const int SCREEN_HEIGHT = 500;
	//const int BG_SCROLL_SPEED = 2;

	bool running = true;
	bool win = false;
	bool loose = false;
	const float G_MOVEMENT_OFFSET = 0.5;
	const int FRAME_DELAY = 100; // Delay in milliseconds for frame animation
	SDL_Renderer* renderer = nullptr;
	GameState* game = nullptr;
	SDL_Texture* bg1 = nullptr;
	int bg1X = 0, bg1Y = 0, gx = 0;

	const int WINDOW_WIDTH = 1000;
	const int WINDOW_HEIGHT = 500;
	std::vector<std::vector<Cell>> grid;
	std::unordered_set<int> flaggedCells;
	bool isGameOver;
	int revealedCells;
	int currentFlags, currentScore = 0;
	ToolMode currentTool;
	Uint32 startTime = 0;
	Uint32 elapsedTime = 0;
	bool isPaused, isSoundOn, quitGame = false;
	const int TIMER_LIMIT = 99;
	int maxFlags = 0;

	SDL_Texture* hiddenTexture = nullptr;
	SDL_Texture* revealedTexture = nullptr;
	SDL_Texture* flagTexture = nullptr;
	SDL_Texture* mineTexture = nullptr;
	SDL_Texture* shovelButton = nullptr;
	SDL_Texture* flagButton = nullptr;
	SDL_Texture* pauseButtonTexture = nullptr;
	SDL_Texture* playButtonTexture = nullptr;
	SDL_Texture* quitButtonTexture = nullptr;
	SDL_Texture* soundButtonTexture = nullptr;
	SDL_Texture* soundOffButtonTexture = nullptr;
	SDL_Texture* backgroundTexture = nullptr;
    SDL_Texture* screenTexture = nullptr;
    SDL_Texture* orangeTexture = nullptr;
    SDL_Texture* headTexture = nullptr;

	Mix_Chunk* clickSound = nullptr;
	TTF_Font* font = nullptr;
    TTF_Font* font2 = nullptr;

	SDL_Rect shovelButtonRect = { 10, 110, BUTTON_WIDTH, BUTTON_HEIGHT };
	SDL_Rect flagButtonRect = { 70, 110, BUTTON_WIDTH, BUTTON_HEIGHT };
	SDL_Rect pauseButtonRect = { 10, 300, BUTTON_WIDTH, BUTTON_HEIGHT };
	SDL_Rect soundButtonRect = { 10, 200, BUTTON_WIDTH, BUTTON_HEIGHT };

public:
	Minesweeper() : isGameOver(false), revealedCells(0), currentFlags(0), currentTool(SHOVEL), elapsedTime(0), isPaused(false), isSoundOn(true) {
		grid.resize(GRID_WIDTH, std::vector<Cell>(GRID_HEIGHT));
	}

	void renderText(TTF_Font* font, const char* text,
		int x, int y, SDL_Color color);
	bool isPause() {
		return isPaused;
	}
	void initGame();
	void placeMines();
	void calculateAdjacentMines();
    void revealCell(int x, int y, SDL_Renderer* renderer,SDL_Texture* screenTexture, SDL_Texture* orangeTexture, SDL_Texture* headTexture, SDL_Texture* backgroundTexture, TTF_Font* font);
    void revealAdjacentCells(int x, int y, SDL_Renderer* renderer, SDL_Texture* screenTexture, SDL_Texture* orangeTexture, SDL_Texture* headTexture, SDL_Texture* backgroundTexture, TTF_Font* font);
	void flagCell(int x, int y);
	void renderPauseScreen(SDL_Renderer* renderer, SDL_Texture* playButtonTexture, SDL_Texture* quitButtonTexture);
	void renderGrid(SDL_Renderer* renderer, SDL_Texture* hiddenTexture, SDL_Texture* revealedTexture, SDL_Texture* flagTexture, SDL_Texture* mineTexture, TTF_Font* font);
	void renderUI(SDL_Renderer* renderer, TTF_Font* font, SDL_Texture* shovelButton, SDL_Texture* flagButton, SDL_Texture* pauseButtonTexture, SDL_Texture* soundButtonTexture, SDL_Texture* soundOffButtonTexture, Mix_Chunk* clickSound, SDL_Texture* playButtonTexture, SDL_Texture* quitButtonTexture, SDL_Texture* screenTexture, SDL_Texture* orangeTexture, SDL_Texture* headTexture, SDL_Texture* backgroundTexture);
	void handleInput(SDL_Event& e, SDL_Rect& shovelButtonRect, SDL_Rect& flagButtonRect, SDL_Rect& pauseButtonRect,
	SDL_Rect& soundButtonRect, Mix_Chunk* clickSound, SDL_Texture* playButtonTexture, SDL_Texture* quitButtonTexture,
	SDL_Renderer* renderer,SDL_Texture* screenTexture, SDL_Texture* orangeTexture, SDL_Texture* headTexture,
	SDL_Texture* backgroundTexture, TTF_Font* font);
	void updateTimer();
	// bool isFinished() const {
	// 	return isGameOver;
	// }
	// int getElapsedTime() const {
	// 	return elapsedTime;
	// }
	void handlePauseInput(SDL_Event& e, SDL_Texture* playButtonTexture, SDL_Texture* quitButtonTexture);
	void renderText(const std::string& text, int x, int y, SDL_Color color, TTF_Font* font, SDL_Renderer* renderer);
	void toggleFlag(int x, int y);
    void renderBackground(SDL_Renderer* renderer, SDL_Texture* backgroundTexture,SDL_Texture* screenTexture, SDL_Texture* orangeTexture, SDL_Texture* headTexture, TTF_Font* font);

	void handleEvents(SDL_Event event); //-keymouse keypress
	void cleanUp(); //-clean free surfaces
	void load(GameState* game); //-load the textures, font
	void render(); //-screen draw


};

#endif