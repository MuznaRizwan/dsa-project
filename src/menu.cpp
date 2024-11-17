#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "menu.h"
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>

using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Class to represent a button
class Button {
	public:
		SDL_Rect rect;//SDL_Rect=data type and rect is a variable for drawing rectangle later in the program
		string label;//variable of type string which stores the name of assets
		SDL_Color color;//stores color of rectangle of type SDL_Color

		Button(int x, int y, int width, int height,  const char* text, const string& label,  SDL_Color color)
		: label(label), color(color) { //Button is a parametrized constructor with 7 parameters being passed and a member initialization list which initialises label and color
			rect = { x, y, width, height };//rect is a structure of the constructor that has 4 variables which cannot be initialised in the member nitialisation list
		}
		Button() = default;//default constructor

		void render(SDL_Renderer* renderer, TTF_Font* font) {//render is a function of type void
			// Draw button background
			SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);//renderDrawColor draws renderer
			SDL_RenderFillRect(renderer, &rect);//color fill

			// Render label
			SDL_Color textColor = { 255, 255, 255 }; // White text
			rendererText(renderer, font, label, rect.x + (rect.w - getTextWidth(font, label)) / 2, rect.y + (rect.h - 20) / 2, textColor);
		}
		bool isClicked(int x, int y) const;//function of type bool
		bool isClicked(int mouseX, int mouseY) {//function of
			return mouseX >= rect.x && mouseX <= rect.x + rect.w && mouseY >= rect.y && mouseY <= rect.y + rect.h;
		}
};
void Menu::renderText(TTF_Font* font, const char* text, SDL_Color color, int x, int y) {
	SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_Rect destRect = { x, y, surface->w, surface->h };
	SDL_RenderCopy(renderer, texture, nullptr, &destRect);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}

// Function prototypes
//bool init(SDL_Window*& window, SDL_Renderer*& renderer);
//void renderText(SDL_Renderer* renderer, TTF_Font* font, const string& text, int x, int y, SDL_Color color);
//int getTextWidth(TTF_Font* font, const string& text);
//void close(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* font);
//void handleButtonClicks(const vector<Button>& buttons, int mouseX, int mouseY);

void Menu::load(SDL_Renderer* renderer) {
	this->renderer = renderer;

	// Load font
	TTF_Font* font = TTF_OpenFont("arial.ttf", 28); // Replace with your font file
	if (!font) {
		cerr << "Failed to load font: " << TTF_GetError() << endl;
		close(window, renderer, font);
		return -1;
	}

	SDL_Color buttonColor = { 255, 0, 0, 255 }; // Red
	vector<Button> buttons;
	buttons.emplace_back(SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 25, 100, 50, "Play", buttonColor);
	buttons.emplace_back(SCREEN_WIDTH - 100, 10, 80, 30, "Credits", { 0, 255, 0, 255 }); // Green
	buttons.emplace_back(SCREEN_WIDTH - 60, 50, 80, 30, "Menu", { 0, 0, 255, 255 }); // Blue
}
void Menu::handleEvent(SDL_Event event) {
	bool running = true;
	if (event.type == SDL_QUIT) {
		running = false;
	} else if (event.type == SDL_MOUSEBUTTONDOWN) {
		int mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);
		handleButtonClicks(buttons, mouseX, mouseY);
	}
}
void Menu::render() {
	// Clear the screen
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
	SDL_RenderClear(renderer);

	// Render buttons
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


void Menu::handleButtonClicks(const vector<Button>& buttons, int mouseX, int mouseY) {
	for (const auto& button : buttons) {
		if (button.isClicked(mouseX, mouseY)) {
			if (button.label == "Play") {
				cout << "Play button clicked!" << endl;
				// Implement game start logic here
			} else if (button.label == "Credits") {
				cout << "Credits button clicked!" << endl;
				// Implement credits display logic here
			} else if (button.label == "Menu") {
				cout << "Menu button clicked!" << endl;
				// Implement menu logic here
			}
		}
	}
}

void Menu::cleanUp() {
}