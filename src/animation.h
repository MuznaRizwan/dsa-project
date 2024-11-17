#ifndef ANIMATION_H
#define ANIMATION_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include<string>

using namespace std;

class Animation {

	public:
		string background="animation.png";
		string asset1="playb.png";
		string asset2="creditsb.png";
		vector<SDL_Texture*> frames = {};
		int currentFrame;
		Uint32 lastFrameTime;
		const int FRAME_DELAY = 100; // Delay in milliseconds for frame animation

		Animation();

		void update();
		void render(SDL_Renderer* renderer, int x, int y);
		void cleanUp();
};
#endif