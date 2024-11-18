#include "animation.h"
//#include <string>

Animation::Animation() : currentFrame(0), lastFrameTime(0) {
}

void Animation::update() {
	Uint32 currentTime = SDL_GetTicks();
	if (currentTime - lastFrameTime >= FRAME_DELAY) {
		currentFrame = (currentFrame + 1) % frames.size();
		lastFrameTime = currentTime;
	}
}
void Animation::load() {

// Function prototypes
	SDL_Surface* surface;
	surface = IMG_Load("assets/images/girlanimation.png");

	if (!surface) {
		printf("Failed to load image %s: %s\n", "assets/images/girlanimatio.png", IMG_GetError());
	}
	//game->splashScreenTexture = SDL_CreateTextureFromSurface(game->renderer, surface);
	SDL_FreeSurface(surface);

}

void Animation::render(SDL_Renderer* renderer, int x, int y) {
	SDL_RenderCopy(renderer, frames[currentFrame], NULL, new SDL_Rect{ x, y, 100, 150 });
}

void Animation::cleanUp() {
	for (auto& frame : frames) {
		SDL_DestroyTexture(frame);
	}
	frames.clear();
}




