#include "animation.h"
#include "loose.h"
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <vector>

using namespace std;

void Loose::load(GameState* game) {
	this->game = game;
	this->renderer = game->renderer;
	// Initialize SDL_mixer
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	Mix_Music* music = Mix_LoadMUS("assets/audio/background_music.mp3"); // Replace with your music file
	Mix_PlayMusic(music, -1); // Loop the music

	// Load background images
	SDL_Texture* bg1 = IMG_LoadTexture(renderer, "assets/images/loose/loose.png"); // Replace with your image file
	//SDL_Texture* bg2 = IMG_LoadTexture(renderer, "assets/images/background1.png");

	// Load girl animation frames
	girlAnimation.frames.push_back(IMG_LoadTexture(renderer, "assets/images/girlanimationf1.png")); // Replace with your images
	girlAnimation.frames.push_back(IMG_LoadTexture(renderer, "assets/images/girlanimationf2.png")); // Replace with your images
	girlAnimation.frames.push_back(IMG_LoadTexture(renderer, "assets/images/girlanimationf3.png")); // Replace with your images
	girlAnimation.frames.push_back(IMG_LoadTexture(renderer, "assets/images/girlanimationf4.png")); // Replace with your images

	font = TTF_OpenFont("assets/font/arial.ttf", 24);
	if (!font) {
		printf("Font loading failed: %s\n", TTF_GetError());
		return;
	}
}

void Loose::handleEvents(SDL_Event event) {
	//    girlAnimation.frames.push_back(IMG_LoadTexture(renderer, "assets/images/girlchar.png"));
	//    girlAnimation.frames.push_back(IMG_LoadTexture(renderer, "assets/images/girlchar.png"));

		// Button states
	bool running = true;
	bool resume = false;

	int bg1Y = 0, bg2Y = -SCREEN_HEIGHT; // Starting positions for scrolling backgrounds
	float gx = 0.0;
	if (event.type == SDL_QUIT) {
		running = false;
	}
	else if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_ESCAPE) {
			running = false;
		}
		else if (event.key.keysym.sym == SDLK_r) { // Press 'R' to resume
			resume = true;
		}

		// Update background positions
//        bg1Y += BG_SCROLL_SPEED;
		//bg2Y += BG_SCROLL_SPEED;
		// Update background positions
		gx += G_MOVEMENT_OFFSET;

		// Reset background positions
//        if (bg1Y >= SCREEN_HEIGHT) bg1Y = -SCREEN_HEIGHT;
		//if (bg2Y >= SCREEN_HEIGHT) bg2Y = -SCREEN_HEIGHT;
		if (gx >= SCREEN_WIDTH) {
			gx = 0;
		}
	}
}

void Loose::render() {

	//	girlAnimation.update();

		// Clear screen
	SDL_RenderClear(renderer);

	// Render backgrounds
	SDL_Rect bgRect1 = { 0, bg1Y, SCREEN_WIDTH, SCREEN_HEIGHT };
	//        SDL_Rect bgRect2 = { 0, bg2Y, SCREEN_WIDTH, SCREEN_HEIGHT };
	SDL_RenderCopy(renderer, bg1, NULL, &bgRect1);
	//SDL_RenderCopy(renderer, bg2, NULL, &bgRect2);

	// Render girl animation
//	girlAnimation.render(renderer, gx /*(SCREEN_WIDTH - 100) / 2*/, SCREEN_HEIGHT - 200);

	// Render text
	SDL_Color white = { 255, 255, 255 };
	renderText(font, "Game Over", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 50, white);
	renderText(font, "Press 'R' to Resume or ESC to Quit", SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2, white);

	// Present the rendered frame
	SDL_RenderPresent(renderer);
}

void Loose::renderText(TTF_Font* font, const char* text,
	int x, int y, SDL_Color color) {
	SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_Rect rect = { x, y, surface->w, surface->h };
	SDL_RenderCopy(renderer, texture, NULL, &rect);

	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}


void Loose::cleanUp() {

	// Clean up resources
//	girlAnimation.cleanUp();
	SDL_DestroyTexture(bg1);
	//    SDL_DestroyTexture(bg2);
	//	Mix_FreeMusic(music);
	Mix_CloseAudio();
	//	SDL_DestroyRenderer(renderer);
	//	SDL_DestroyWindow(window);
	//	SDL_Quit();
	//	return 0;
}
