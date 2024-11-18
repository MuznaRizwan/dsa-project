#ifndef WIN_H
#define WIN_H

#include <string>
#include "basescreen.h"
#include "animation.h"

using namespace std;

class Win : public BaseScreen {
	private:

		const int SCREEN_WIDTH = 900;
		const int SCREEN_HEIGHT = 500;
//const int BG_SCROLL_SPEED = 2;

		const float G_MOVEMENT_OFFSET = 0.5;
		const int FRAME_DELAY = 100; // Delay in milliseconds for frame animation
		int highScore=0, gameTime=0, candyCollected=0;
		SDL_Renderer* renderer;
		GameState* game;
		SDL_Texture* bg1;
		int bg1X, bg1Y, gx;
		Animation girlAnimation;


	public:
		void load(GameState* game);
		void handleEvents();
		void render();
		void renderText(TTF_Font* font, const char* text,
		                int x, int y, SDL_Color color);
		void cleanUp();
};

#endif