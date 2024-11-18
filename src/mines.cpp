#define SDL_MAIN_HANDLED

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>

const int CELL_SIZE = 32;
const int GRID_WIDTH = 10;
const int GRID_HEIGHT = 10;
const int TOTAL_MINES = 10;
const int BUTTON_WIDTH = 64;
const int BUTTON_HEIGHT = 32;

enum CellState { HIDDEN, REVEALED, FLAGGED };
enum CellContent { EMPTY, MINE, NUMBER };
enum ToolMode { SHOVEL, FLAG };

struct Cell {
	CellState state;
	CellContent content;
	int adjacentMines;
};

std::vector<std::vector<Cell>> grid(GRID_WIDTH, std::vector<Cell>(GRID_HEIGHT));
bool isGameOver = false;
int revealedCells = 0;
ToolMode currentTool = SHOVEL;

// Function prototypes
void initGame();
void placeMines();
void calculateAdjacentMines();
void renderGrid(SDL_Renderer* renderer, SDL_Texture* hiddenTexture, SDL_Texture* revealedTexture, SDL_Texture* flagTexture, SDL_Texture* mineTexture, SDL_Texture* shovelButton, SDL_Texture* flagButton, TTF_Font* font);
void handleInput(SDL_Event& e, SDL_Rect& shovelButtonRect, SDL_Rect& flagButtonRect, Mix_Chunk* clickSound);
bool revealCell(int x, int y);
void flagCell(int x, int y);
void renderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y, SDL_Color color);

int minemain(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);  // Initialize SDL_mixer

	SDL_Window* window = SDL_CreateWindow("Minesweeper", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, GRID_WIDTH * CELL_SIZE, GRID_HEIGHT * CELL_SIZE + BUTTON_HEIGHT, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	// Load images
	SDL_Texture* hiddenTexture = IMG_LoadTexture(renderer, "assets/images/hidden.png");
	SDL_Texture* revealedTexture = IMG_LoadTexture(renderer, "assets/images/revealed.png");
	SDL_Texture* flagTexture = IMG_LoadTexture(renderer, "assets/images/flag.png");
	SDL_Texture* mineTexture = IMG_LoadTexture(renderer, "assets/images/mine.png");
	SDL_Texture* shovelButton = IMG_LoadTexture(renderer, "assets/images/shovel_button.png");
	SDL_Texture* flagButton = IMG_LoadTexture(renderer, "assets/images/flag_button.png");

	// Load font for rendering text
	TTF_Font* font = TTF_OpenFont("assets/font/Tiny5.ttf", 24);  // Adjust font size if necessary
	SDL_Color textColor = {0, 0, 0}; // Black color for score

	// Load sound
	Mix_Chunk* clickSound = Mix_LoadWAV("assets/audio/move.wav");

	initGame();

	SDL_Rect shovelButtonRect = {0, GRID_HEIGHT * CELL_SIZE, BUTTON_WIDTH, BUTTON_HEIGHT};
	SDL_Rect flagButtonRect = {BUTTON_WIDTH, GRID_HEIGHT * CELL_SIZE, BUTTON_WIDTH, BUTTON_HEIGHT};

	bool running = true;
	while (running) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				running = false;
			}
			handleInput(e, shovelButtonRect, flagButtonRect, clickSound);
		}

		SDL_RenderClear(renderer);
		renderGrid(renderer, hiddenTexture, revealedTexture, flagTexture, mineTexture, shovelButton, flagButton, font);
		SDL_RenderPresent(renderer);
	}

	// Clean up resources
	Mix_FreeChunk(clickSound);
	TTF_CloseFont(font);
	SDL_DestroyTexture(hiddenTexture);
	SDL_DestroyTexture(revealedTexture);
	SDL_DestroyTexture(flagTexture);
	SDL_DestroyTexture(mineTexture);
	SDL_DestroyTexture(shovelButton);
	SDL_DestroyTexture(flagButton);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	Mix_CloseAudio();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
	return 0;
}

// Initialize the game grid and place mines
void initGame() {
	srand(time(0));
	revealedCells = 0;
	for (int x = 0; x < GRID_WIDTH; ++x) {
		for (int y = 0; y < GRID_HEIGHT; ++y) {
			grid[x][y].state = HIDDEN;
			grid[x][y].content = EMPTY;
			grid[x][y].adjacentMines = 0;
		}
	}
	placeMines();
	calculateAdjacentMines();
}

// Randomly place mines in the grid
void placeMines() {
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

// Calculate the number of adjacent mines for each cell
void calculateAdjacentMines() {
	for (int x = 0; x < GRID_WIDTH; ++x) {
		for (int y = 0; y < GRID_HEIGHT; ++y) {
			if (grid[x][y].content == MINE) continue;
			int mineCount = 0;
			for (int dx = -1; dx <= 1; ++dx) {
				for (int dy = -1; dy <= 1; ++dy) {
					int nx = x + dx;
					int ny = y + dy;
					if (nx >= 0 && nx < GRID_WIDTH && ny >= 0 && ny < GRID_HEIGHT && grid[nx][ny].content == MINE) {
						++mineCount;
					}
				}
			}
			grid[x][y].adjacentMines = mineCount;
			if (mineCount > 0) grid[x][y].content = NUMBER;
		}
	}
}

// Render the game grid and buttons
void renderGrid(SDL_Renderer* renderer, SDL_Texture* hiddenTexture, SDL_Texture* revealedTexture, SDL_Texture* flagTexture, SDL_Texture* mineTexture, SDL_Texture* shovelButton, SDL_Texture* flagButton, TTF_Font* font) {
	for (int x = 0; x < GRID_WIDTH; ++x) {
		for (int y = 0; y < GRID_HEIGHT; ++y) {
			SDL_Rect dst = {x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE};

			// Render based on the cell state
			if (grid[x][y].state == HIDDEN) {
				SDL_RenderCopy(renderer, hiddenTexture, nullptr, &dst);
			} else if (grid[x][y].state == FLAGGED) {
				SDL_RenderCopy(renderer, flagTexture, nullptr, &dst);
			} else if (grid[x][y].state == REVEALED) {
				if (grid[x][y].content == MINE) {
					SDL_RenderCopy(renderer, mineTexture, nullptr, &dst);
				} else {
					SDL_RenderCopy(renderer, revealedTexture, nullptr, &dst);
					if (grid[x][y].content == NUMBER) {
						std::string number = std::to_string(grid[x][y].adjacentMines);
						renderText(renderer, font, number.c_str(), x * CELL_SIZE + 8, y * CELL_SIZE + 4, {0, 0, 0});
					}
				}
			}
		}
	}

	// Render tool buttons
	SDL_Rect shovelButtonRect = {0, GRID_HEIGHT * CELL_SIZE, BUTTON_WIDTH, BUTTON_HEIGHT};
	SDL_Rect flagButtonRect = {BUTTON_WIDTH, GRID_HEIGHT * CELL_SIZE, BUTTON_WIDTH, BUTTON_HEIGHT};
	SDL_RenderCopy(renderer, shovelButton, nullptr, &shovelButtonRect);
	SDL_RenderCopy(renderer, flagButton, nullptr, &flagButtonRect);

	// Render score
	std::string scoreText = "Score: " + std::to_string(revealedCells);
	renderText(renderer, font, scoreText.c_str(), 10, 10, {0, 0, 0});
}

// Handle mouse input for switching tools, revealing, or flagging cells
void handleInput(SDL_Event& e, SDL_Rect& shovelButtonRect, SDL_Rect& flagButtonRect, Mix_Chunk* clickSound) {
	if (e.type == SDL_MOUSEBUTTONDOWN && !isGameOver) {
		int x = e.button.x / CELL_SIZE;
		int y = e.button.y / CELL_SIZE;

		// Check if shovel or flag button is clicked
		if (e.button.y >= GRID_HEIGHT * CELL_SIZE && e.button.y <= GRID_HEIGHT * CELL_SIZE + BUTTON_HEIGHT) {
			if (e.button.x >= shovelButtonRect.x && e.button.x <= shovelButtonRect.x + BUTTON_WIDTH) {
				currentTool = SHOVEL;
			} else if (e.button.x >= flagButtonRect.x && e.button.x <= flagButtonRect.x + BUTTON_WIDTH) {
				currentTool = FLAG;
			}
		} else if (x < GRID_WIDTH && y < GRID_HEIGHT) {
			// Use the current tool on the grid
			if (currentTool == SHOVEL) {
				if (!revealCell(x, y)) {
					isGameOver = true; // Game over if a mine is revealed
				} else {
					Mix_PlayChannel(-1, clickSound, 0); // Play reveal sound
				}
			} else if (currentTool == FLAG) {
				flagCell(x, y);
			}
		}
	}
}

// Reveal a cell and return false if a mine was revealed
bool revealCell(int x, int y) {
	if (grid[x][y].state == HIDDEN) {
		grid[x][y].state = REVEALED;
		if (grid[x][y].content == MINE) {
			return false; // Mine revealed
		}
		revealedCells++;
	}
	return true;
}

// Flag or unflag a cell
void flagCell(int x, int y) {
	if (grid[x][y].state == HIDDEN) {
		grid[x][y].state = FLAGGED;
	} else if (grid[x][y].state == FLAGGED) {
		grid[x][y].state = HIDDEN;
	}
}

// Render text (numbers, score) on cells using SDL_ttf
void renderText2(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y, SDL_Color color) {
	SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_Rect dst = {x, y, surface->w, surface->h};
	SDL_RenderCopy(renderer, texture, nullptr, &dst);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}
