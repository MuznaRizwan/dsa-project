#ifndef LOOSE_H
#define LOOSE_H
#include "animation.h"
#include "basescreen.h"
#include <string>

using namespace std;

class Loose : public BaseScreen {
private:

	const int SCREEN_WIDTH = 900;
	const int SCREEN_HEIGHT = 500;
	//const int BG_SCROLL_SPEED = 2;

	const float G_MOVEMENT_OFFSET = 0.5;
	const int FRAME_DELAY = 100; // Delay in milliseconds for frame animation
	int highScore = 0, gameTime = 0, candyCollected = 0;
	TTF_Font* font = nullptr;
	SDL_Renderer* renderer = nullptr;
	GameState* game = nullptr;
	SDL_Texture* bg1 = nullptr;
	int bg1X = 0, bg1Y = 0, gx = 0;
	Animation girlAnimation;

public:
	void load(GameState* game);
	void handleEvents(SDL_Event event);
	void render();
	void renderText(TTF_Font* font, const char* text,
		int x, int y, SDL_Color color);
	void cleanUp();
};

#endif