#ifndef UTILS_H
#define UTILS_H

#include <string>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_mixer.h"

class Utils {
public:
    static void renderText(const std::string& message, int x, int y, SDL_Color color, TTF_Font* font, SDL_Renderer* renderer);
    static Mix_Chunk* loadSound(const std::string& path);
};

#endif