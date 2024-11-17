#ifndef MENU_H
#define MENU_H
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <SDL2/SDL_ttf.h>
#include "basescreen.h"
#include "button.h"

using namespace std;

class Menu : public BaseScreen {
	private:

		const int SCREEN_WIDTH = 900;
		const int SCREEN_HEIGHT = 500;
//const int BG_SCROLL_SPEED = 2;

		const float G_MOVEMENT_OFFSET = 0.5;
		const int FRAME_DELAY = 100; // Delay in milliseconds for frame animation
		SDL_Renderer* renderer;
		SDL_Texture* bg1;
		int bg1X, bg1Y, gx;
		// const vector<Button>& buttons;
//		vector<Button>& buttons;
//		vector<shared_ptr<Button>> buttons;
		Button buttons[3] = { Button(), Button(), Button() };

//		std::array<Button, 3> buttons;
		TTF_Font* font;

	public:
		void load(SDL_Renderer* renderer) override ;
		void handleEvents(SDL_Event event) override ;
		void render() override ;
		void renderText(TTF_Font* font, const string& text, int x, int y, SDL_Color color);
		void cleanUp() override ;
//		void handleButtonClicks(const vector<Button>& buttons, int mouseX, int mouseY);
//		void handleButtonClicks(vector<Button>& buttons, int mouseX, int mouseY);
		void handleButtonClicks(/*Button buttons[],*/ int mouseX, int mouseY);
		int getTextWidth(TTF_Font* font, const char* text);
};

#endif