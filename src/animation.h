#ifndef ANIMATION_H
#define ANIMATION_H
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include<string>
#include <vector>

using namespace std;

class Animation {

public:
	string background = "assets/images/animation.png";
	string asset1 = "assets/images/playb.png";
	string asset2 = "assets/images/creditsb.png";
	vector<SDL_Texture*> frames = {};
	Uint32 currentFrame = 0;
	Uint32 lastFrameTime = 0;
	const Uint32 FRAME_DELAY = 100; // Delay in milliseconds for frame animation

	Animation();

	void load();
	void update();
	void render(SDL_Renderer* renderer, int x, int y);
	void cleanUp();
};
#endif
