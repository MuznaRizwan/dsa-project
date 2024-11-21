#include "menu.h"
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

using namespace std;

// Function prototypes
//bool init(SDL_Window*& window, SDL_Renderer*& renderer);
//void renderText(SDL_Renderer* renderer, TTF_Font* font, const string& text, int x, int y, SDL_Color color);
//int getTextWidth(TTF_Font* font, const string& text);
//void close(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* font);
//void handleButtonClicks(const vector<Button>& buttons, int mouseX, int mouseY);

void Menu::load(GameState* game) {
	this->game = game;
	this->renderer = game->renderer;

	// Load font
	font = TTF_OpenFont("assets/font/path_to_font.ttf", 28); // Replace with your font file
	if (!font) {
		cerr << "Failed to load font: " << TTF_GetError() << endl;
	}

	SDL_Color buttonColor = { 255, 0, 0, 255 }; // Red

	//	Button b(0,0,100,30,"Hello World",(SDL_Color){ 0, 255, 0, 255 });
	//	buttons.push_back(b);
	//	buttons.push_back(make_shared<Button>(0,0,100,30,"Hello World",(SDL_Color){ 0, 255, 0, 255 }));
	//	buttons[0] = b;
	//	buttons[1] = b;
	//	buttons[2] = b;
		/*{			}
		Button(0,  0,100,50,"B-001",(SDL_Color){ 0, 255, 0, 255 }),
		Button(0,100,100,50,"B-002",(SDL_Color){ 0, 255, 0, 255 }),
		Button(0,200,100,50,"B-003",(SDL_Color){ 0, 255, 0, 255 })
		};*/

		//	buttons.emplace_back(b);
		//	buttons[0] = b;
		//	buttons.emplace_back(SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 25, 100, 50, "Play", buttonColor);
		//	buttons.emplace_back(SCREEN_WIDTH - 100, 10, 80, 30, "Credits", (SDL_Color){ 0, 255, 0, 255 }); // Green
		//	buttons.emplace_back(SCREEN_WIDTH - 60, 50, 80, 30, "Menu", (SDL_Color){ 0, 0, 255, 255 }); // Blue

	SDL_Color c = { 0, 255, 0, 255 };

	Button b(0, 0, 90, 50, "Game-0", c);

	int i = 0;
	string bLabels[6] = { "Play","Credits","Menu","Maze","Minesweeper","Zombie" };
	for (auto& button : buttons) {
		b.rect.x += 100;
		b.label = bLabels[i++];
		button = Button(b);
	}
}

void Menu::handleEvents(SDL_Event event) {
	bool running = true;
	if (event.type == SDL_QUIT) {
		running = false;
	}
	else if (event.type == SDL_MOUSEBUTTONDOWN) {
		int mouseX = 0, mouseY = 0;
		SDL_GetMouseState(&mouseX, &mouseY);
		handleButtonClicks(/*buttons,*/ mouseX, mouseY);
	}
}

void Menu::render() {
	// Clear the screen
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
	SDL_RenderClear(renderer);

	// Render buttons
//	for (auto& button : buttons) {
//		button.render(renderer, font);
//	}

//	Button b(0,0,100,30,"Hello World",(SDL_Color){ 0, 255, 0, 255 });
//	b.render(renderer, font);

	for (auto& button : buttons) {
		button.render(renderer, font);
	}

	// Present the rendered frame
	SDL_RenderPresent(renderer);
}

// Clean up and close
void Menu::renderText(TTF_Font* font, const string& text, int x, int y, SDL_Color color) {
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
	SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

	int textWidth = textSurface->w;
	int textHeight = textSurface->h;

	SDL_Rect textRect = { x, y, textWidth, textHeight };
	SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(textTexture);
}

int Menu::getTextWidth(TTF_Font* font, const char* text) {
	int width;
	TTF_SizeText(font, text, &width, nullptr);
	return width;
}

//void Menu::handleButtonClicks(const vector<Button>& buttons, int mouseX, int mouseY) {
//void Menu::handleButtonClicks(vector<Button>& buttons, int mouseX, int mouseY) {
void Menu::handleButtonClicks(/*Button buttons[],*/ int mouseX, int mouseY) {
	//	for (const auto& button : buttons) {
	for (auto& button : buttons) {
		if (button.isClicked(mouseX, mouseY)) {
			cout << button.label << " button clicked!" << endl;
			if (button.label == "Play") {
				cout << "Play" << "Play button clicked!" << endl;
				// HERE YOU HAVE TO CHANGE / SET game->currentScreen = ...
				// Implement game start logic here
			}
			else if (button.label == "Credits") {
				cout << "Credits button clicked!" << endl;
				game->currentScreen = CREDITS_SCREEN;
				// Implement credits display logic here
			}
			else if (button.label == "Menu") {
				cout << "Menu button clicked!" << endl;
				// Implement menu logic here
			}
			else if (button.label == "Maze") {
				cout << "Maze button clicked!" << endl;
				// Implement menu logic here
				game->currentScreen = MAZE_SCREEN;
			}
			else if (button.label == "Minesweeper") {
				cout << "Minesweeper button clicked!" << endl;
				// Implement menu logic here
				game->currentScreen = MINESWEEPER_SCREEN;
			}
			else if (button.label == "Zombie") {
				cout << "Zombie button clicked!" << endl;
				// Implement menu logic here
				game->currentScreen = ZOMBIE_SCREEN;
			}
		}
	}
}

void Menu::cleanUp() {
}