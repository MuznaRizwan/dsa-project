#ifndef GLOBALS_H
#define GLOBALS_H

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include "basescreen.h"
#include "enums.h"

using namespace std;

class BaseScreen;

enum GameScreen {
	MAZE_SCREEN,
	MINESWEEPER_SCREEN,
	ZOMBIE_SCREEN,

	SPLASH_SCREEN,
	MENU_SCREEN,

	LOOSE_SCREEN,
	WIN_SCREEN,
	CREDITS_SCREEN
};

//struct Game {
//	SDL_Texture* texture;
//	const char* name;
//};

//variables of images
//load texture making
//load mein load krwana he
//load render screen condition lagegi 5ou screen render krde
struct GameState {
	SDL_Window* window=nullptr;
	SDL_Renderer* renderer=nullptr;
	TTF_Font* font=nullptr;
	Mix_Music* bgMusic=nullptr;
	Mix_Chunk* hoverSound = nullptr;
	Mix_Chunk* selectSound = nullptr;
//	Game games[GAME_COUNT];
	GameScreen currentScreen;
	int selectedGame=0;
	GameScreen previousScreen;
	int hoveredGame=-1;
	//string backgrounds[8] = {"assets/images/maze.png","assets/images/minesweeper.png","assets/images/zombie.png","assets/images/menu.png","assets/images/loose.png","assets/images/win.png","assets/images/credits.png"};
	//SDL_Texture* creditsScreenTexture;
	//SDL_Texture* menuScreenTexture;
//	SDL_Texture* looseScreenTexture;
//	SDL_Texture* winScreenTexture;
//
//	SDL_Texture* zombieScreenTexture;
//	SDL_Texture* mazeScreenTexture;
	//SDL_Texture* minesweeperScreenTexture;
	BaseScreen* cScreen=nullptr;
	//Loose* looseScreen = new ;
};

struct Globals {
	const int SCREEN_WIDTH = 800;
	const int SCREEN_HEIGHT = 500;
	bool screenLoaded = false;
	bool soundOn = true;
	Screens currentScreen = SPLASH;
};

#endif