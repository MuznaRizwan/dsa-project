#ifndef MINES_H
#define MINES_H
#include <string>
#include "basescreen.h"
#include <SDL2/SDL_ttf.h>
using namespace std;

class Mines : public BaseScreen {
	private:

		const int SCREEN_WIDTH = 900;
		const int SCREEN_HEIGHT = 500;
//const int BG_SCROLL_SPEED = 2;

		const float G_MOVEMENT_OFFSET = 0.5;
		const int FRAME_DELAY = 100; // Delay in milliseconds for frame animation
		SDL_Renderer* renderer;
		SDL_Texture* bg1;
		int bg1X, bg1Y, gx;
	public:
		void load(SDL_Renderer* renderer);
		void handleEvents(SDL_Event event);
		void render();
		void renderText(TTF_Font* font, const char* text,
		                int x, int y, SDL_Color color);
		void cleanUp();
};

#endif