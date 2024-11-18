#ifndef SPLASH_H
#define SPLASH_H
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include "basescreen.h"

using namespace std;

class Splash : public BaseScreen {
	private:

		const int SCREEN_WIDTH = 900;
		const int SCREEN_HEIGHT = 500;
//const int BG_SCROLL_SPEED = 2;

		TTF_Font* font;
		SDL_Renderer* renderer;
		SDL_Texture* bg1;
		int bg1X, bg1Y, gx;

	public:

		string background="assets/images/splash.png";
		string asset1="assets/images/playb.png";
		string asset2="assets/images/creditsb.png";
		void load(SDL_Renderer* renderer);
		void handleEvents(SDL_Event event);
		void render();
		void renderText(TTF_Font* font, const char* text, int x, int y, SDL_Color color);
		void DrawRoundedRect( int x, int y, int w, int h);
		void cleanUp();
};

#endif
