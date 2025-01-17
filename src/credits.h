#ifndef CREDITS_H
#define CREDITS_H

#include "basescreen.h"
// #include "creditscroll.h"
#include <string>
#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <queue>
#include <algorithm>
#include <thread>

using namespace std;

class Credits : public BaseScreen {
private:

	const int SCREEN_WIDTH = 900;
	const int SCREEN_HEIGHT = 500;
	//const int BG_SCROLL_SPEED = 2;

	const float G_MOVEMENT_OFFSET = 0.5;
	const int FRAME_DELAY = 100; // Delay in milliseconds for frame animation
	int highScore = 0, gameTime = 0, candyCollected = 0;
	SDL_Renderer* renderer = nullptr;
	GameState* game = nullptr;
	SDL_Texture* bg1 = nullptr;
	TTF_Font* font = nullptr;
	Mix_Music* music = nullptr;
	int bg1X = 0, bg1Y = 0, gx = 0;
//	RoleQueue roleQueue;
//    CreditScroll credits;

public:
	void load(GameState* game);
	void handleEvents(SDL_Event event);
	void render();
	void renderText(TTF_Font* font, const char* text,
		int x, int y, SDL_Color color);
	void cleanUp();
};

#endif