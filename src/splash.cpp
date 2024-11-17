#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include "splash.h"

using namespace std;
void Splash::load(	SDL_Renderer* renderer) {

// Function prototypes
	SDL_Surface* surface;
	surface = IMG_Load("splash.png");

	if (!surface) {
		printf("Failed to load image %s: %s\n", "splash.png", IMG_GetError());
	}
	SDL_Texture* splashScreenTexture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	// Initialize SDL_mixer
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	Mix_Music* music = Mix_LoadMUS("background_music.mp3"); // Replace with your music file
	Mix_PlayMusic(music, -1); // Loop the music

	// Load background images
	SDL_Texture* bg1 = IMG_LoadTexture(renderer, "splash.png"); // Replace with your image file
	//SDL_Texture* bg2 = IMG_LoadTexture(renderer, "background1.png");

	TTF_Font* font = TTF_OpenFont("arial.ttf", 24);
	if (!font) {
		printf("Font loading failed: %s\n", TTF_GetError());
		return;
	}
}

void Splash::handleEvents(SDL_Event event) {
//    girlAnimation.frames.push_back(IMG_LoadTexture(renderer, "girlchar.png"));
//    girlAnimation.frames.push_back(IMG_LoadTexture(renderer, "girlchar.png"));

	// Button states
	bool running = true;
	bool resume = false;


	int bg1Y = 0, bg2Y = -SCREEN_HEIGHT; // Starting positions for scrolling backgrounds
	float gx=0.0;
	if (event.type == SDL_QUIT) {
		running = false;
	} else if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_ESCAPE) {
			running = false;
		} else if (event.key.keysym.sym == SDLK_r) { // Press 'R' to resume
			resume = true;
		}
	}

}

void Splash::render() {
	SDL_RenderClear(renderer);
	SDL_Texture* splashScreenTexture;
	// Render backgrounds
	SDL_Rect bgRect1 = { 0, bg1Y, SCREEN_WIDTH, SCREEN_HEIGHT };
//        SDL_Rect bgRect2 = { 0, bg2Y, SCREEN_WIDTH, SCREEN_HEIGHT };
	SDL_RenderCopy(renderer, bg1, NULL, &bgRect1);
	//SDL_RenderCopy(renderer, bg2, NULL, &bgRect2);


	// Render text
	SDL_Color white = { 255, 255, 255 };
	renderText(font, "Game Over", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 50, white);
	renderText(font, "Press 'R' to Resume or ESC to Quit", SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2, white);


	SDL_Color textColor = {255, 255, 255, 255};

	SDL_Rect fullRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

	SDL_RenderCopy(renderer, splashScreenTexture,
	               NULL, &fullRect);
	// Render back button
	SDL_SetRenderDrawColor(renderer, 40, 40, 50, 255);
	SDL_Rect backButton = {20, 20, 100, 40};
	DrawRoundedRect(renderer, backButton.x, backButton.y,
	                backButton.w, backButton.h);
	renderText(renderer,font, "Back",
	           35, 25, textColor);
	SDL_RenderPresent(renderer);


}
void Splash::renderText(TTF_Font* font, const char* text,
                        int x, int y, SDL_Color color) {
	SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_Rect rect = {x, y, surface->w, surface->h};
	SDL_RenderCopy(renderer, texture, NULL, &rect);

	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}
	void Splash::cleanUp() {

		// Clean up resources
		SDL_DestroyTexture(bg1);
//    SDL_DestroyTexture(bg2);
//	Mix_FreeMusic(music);
		Mix_CloseAudio();
//	return 0;
	}
