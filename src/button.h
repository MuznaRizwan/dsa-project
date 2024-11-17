#ifndef BUTTON_H
#define BUTTON_H
#include <string>
#include <vector>
#include <SDL2/SDL_ttf.h>

using namespace std;

// Class to represent a button
class Button {
	public:
		SDL_Rect rect;//SDL_Rect=data type and rect is a variable for drawing rectangle later in the program
		string label;//variable of type string which stores the name of assets
		SDL_Color color;//stores color of rectangle of type SDL_Color

		Button(int x, int y, int width, int height, /*const char* text,*/ const string& label,  SDL_Color color)
			: label(label), color(color) { //Button is a parametrized constructor with 7 parameters being passed and a member initialization list which initialises label and color
			rect = { x, y, width, height };//rect is a structure of the constructor that has 4 variables which cannot be initialised in the member nitialisation list
		}
		// Copy constructor
		Button(const Button& other) {
			//cout << "Copy constructor called for: " << other.label << endl;
			label = other.label;  // Copy the label from the other Button
			rect = other.rect;
			color = other.color;
		}
		Button() = default;//default constructor
		bool isClicked(int x, int y);//function of type bool
		int getTextWidth(TTF_Font* font, const string& text);
		void renderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color, int x, int y);
		int getTextWidth(TTF_Font* font, const char* text);
		void render(SDL_Renderer* renderer, TTF_Font* font);
};

#endif