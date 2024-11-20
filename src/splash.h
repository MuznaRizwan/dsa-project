#ifndef SPLASH_H
#define SPLASH_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <string>

#include "basescreen.h"

using namespace std;

class Splash : public BaseScreen {
private:

	const int SCREEN_WIDTH = 900;
	const int SCREEN_HEIGHT = 500;
	//const int BG_SCROLL_SPEED = 2;

	TTF_Font* font = nullptr;
	SDL_Renderer* renderer = nullptr;
	GameState* game = nullptr;
	SDL_Texture* bg1 = nullptr;
	int bg1X = 0, bg1Y = 0, gx = 0;

public:

	string background = "assets/images/splash.png";
	string asset1 = "assets/images/playb.png";
	string asset2 = "assets/images/creditsb.png";
	void load(GameState* game);
	void handleEvents(SDL_Event event);
	void render();
	void renderText(TTF_Font* font, const char* text, int x, int y, SDL_Color color);
	void DrawRoundedRect(int x, int y, int w, int h);
	void cleanUp();
};

#endif
