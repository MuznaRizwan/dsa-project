#include "button.h"

bool Button::isClicked(int x, int y) {//function of
	if (x >= 0 && x <= 900 && y >= 0 && y <= 500) {
		return x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h;
	} else {
		return 0;
	}
}

void Button::renderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, SDL_Color color, int x, int y) {
	SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_Rect destRect = { x, y, surface->w, surface->h };
	SDL_RenderCopy(renderer, texture, nullptr, &destRect);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}

int Button::getTextWidth(TTF_Font* font, const char* text) {
	int width;
	TTF_SizeText(font, text, &width, nullptr);
	return width;
}

void Button::render(SDL_Renderer* renderer, TTF_Font* font) {//render is a function of type void
	// Draw button background
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);//renderDrawColor draws renderer
	SDL_RenderFillRect(renderer, &rect);//color fill

	// Render label
	SDL_Color textColor = { 255, 255, 255 }; // White text
	renderText(renderer, font, label.c_str(), textColor, rect.x + (rect.w - getTextWidth(font, label.c_str())) / 2, rect.y + (rect.h - 20) / 2);
}

