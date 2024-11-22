#ifndef MENU_H
#define MENU_H
#include "basescreen.h"
#include "button.h"
#include <iostream>
#include <memory>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>

using namespace std;

class Menu : public BaseScreen {
private:

	const int SCREEN_WIDTH = 900;
	const int SCREEN_HEIGHT = 500;
	//const int BG_SCROLL_SPEED = 2;

	const float G_MOVEMENT_OFFSET = 0.5;
	const int FRAME_DELAY = 100; // Delay in milliseconds for frame animation
	SDL_Renderer* renderer = nullptr;
	GameState* game = nullptr;
	SDL_Texture* bg1 = nullptr;
	SDL_Texture* btn1a = nullptr;
	SDL_Texture* btn2a = nullptr;
	SDL_Texture* btn3a = nullptr;
	SDL_Texture* btn1l = nullptr;
	SDL_Texture* btn2l = nullptr;
	SDL_Texture* btn3l = nullptr;
	SDL_Texture* btn1u = nullptr;
	SDL_Texture* btn2u = nullptr;
	SDL_Texture* btn3u = nullptr;
	int bg1X = 0, bg1Y = 0, gx = 0;
	// const vector<Button>& buttons;
//		vector<Button>& buttons;
//		vector<shared_ptr<Button>> buttons;
	string bLabels[6] = { /*"Play","Credits","Menu",*/ "Maze","Minesweeper","Zombie" };
	Button buttons[6] = { Button(), Button(), Button() /*,Button(), Button(), Button()*/ };

	//		std::array<Button, 3> buttons;
	TTF_Font* font = nullptr;

public:
	void load(GameState* game) override;
	void handleEvents(SDL_Event event) override;
	void render() override;
	void renderText(TTF_Font* font, const string& text, int x, int y, SDL_Color color);
	void cleanUp() override;
	//		void handleButtonClicks(const vector<Button>& buttons, int mouseX, int mouseY);
	//		void handleButtonClicks(vector<Button>& buttons, int mouseX, int mouseY);
	void handleButtonClicks(/*Button buttons[],*/ int mouseX, int mouseY);
	int getTextWidth(TTF_Font* font, const char* text);
};

#endif