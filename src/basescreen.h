#ifndef BASE_H
#define BASE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string>
#include "globals.h"

struct GameState;

class BaseScreen {
	public:
		virtual void handleEvents(SDL_Event event) = 0; //-keymouse keypress
		virtual void cleanUp() = 0; //-clean free surfaces
		virtual void load(GameState* game) = 0; //-load the textures, font
		virtual void render() = 0; //-screen draw
};
#endif