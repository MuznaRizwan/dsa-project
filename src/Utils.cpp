#include <iostream>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include "Utils.h"

// Utility class for various helper functions
void Utils::renderText(const std::string& message, int x, int y, SDL_Color color, TTF_Font* font, SDL_Renderer* renderer)
{
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, message.c_str(), color);
	if (textSurface == NULL)
	{
		std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
	}
	else
	{
		SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
		if (textTexture == NULL)
		{
			std::cerr << "Unable to create texture from rendered text! SDL_Error: " << SDL_GetError() << std::endl;
		}
		else
		{
			SDL_Rect renderQuad = { x, y, textSurface->w, textSurface->h };
			SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);
			SDL_DestroyTexture(textTexture);
		}
		SDL_FreeSurface(textSurface);
	}
}

// Manager class for handling SDL sound loading
Mix_Chunk* Utils::loadSound(const std::string& path)
{
	Mix_Chunk* sound = Mix_LoadWAV(path.c_str());
	if (sound == NULL)
	{
		std::cerr << "Failed to load sound effect! SDL_mixer Error: " << Mix_GetError() << std::endl;
	}
	return sound;
}