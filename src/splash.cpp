#include "splash.h"
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <vector>

using namespace std;
void Splash::load(GameState* game) {
	this->game = game;
	this->renderer = game->renderer;
	// Function prototypes
//	SDL_Surface* surface;
//	surface = IMG_Load("assets/images/splash.png");
//
//	if (!surface) {
//		printf("Failed to load image %s: %s\n", "assets/images/splash.png", IMG_GetError());
//	}
//	SDL_Texture* splashScreenTexture = SDL_CreateTextureFromSurface(renderer, surface);
//	SDL_FreeSurface(surface);

	// Initialize SDL_mixer
//	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
//	Mix_Music* music = Mix_LoadMUS("assets/audio/background_music.mp3"); // Replace with your music file
//	Mix_PlayMusic(music, -1); // Loop the music

	// Load background images
	bg1 = IMG_LoadTexture(renderer, "assets/images/splash/splashbg.png"); // Replace with your image file
	//SDL_Texture* bg2 = IMG_LoadTexture(renderer, "assets/images/background1.png");

	font = TTF_OpenFont("assets/font/arial.ttf", 96);
	if (!font) {
		printf("Font loading failed: %s\n", TTF_GetError());
		return;
	}
}

void Splash::handleEvents(SDL_Event event) {
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
	}

}

void Splash::render() {
	SDL_RenderClear(renderer);
	// Render backgrounds
//	SDL_Rect bgRect1 = { 0, bg1Y, SCREEN_WIDTH, SCREEN_HEIGHT };
//	//        SDL_Rect bgRect2 = { 0, bg2Y, SCREEN_WIDTH, SCREEN_HEIGHT };
//	SDL_RenderCopy(renderer, bg1, NULL, &bgRect1);
	//SDL_RenderCopy(renderer, bg2, NULL, &bgRect2);


	// Render text
//	SDL_Color white = { 255, 255, 255 };
//	renderText(font, "Game Over", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 50, white);
//	renderText(font, "Press 'R' to Resume or ESC to Quit", SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2, white);

	SDL_Rect fullRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	SDL_RenderCopy(renderer, bg1, NULL, &fullRect);

	int TEXT_WIDTH = 460;
	int TEXT_HEIGHT = 90;

	SDL_Color black = { 0, 0, 0 };
	renderText(font, "Tiny Triads", SCREEN_WIDTH / 2 - /*(SDL_GetTicks() - startTime)*/ TEXT_WIDTH / 2, SCREEN_HEIGHT / 2 - TEXT_HEIGHT / 2, black);

	SDL_Color white = { 255, 255, 255 };
	renderText(font, "Tiny Triads", SCREEN_WIDTH / 2 - /*(SDL_GetTicks() - startTime)*/ TEXT_WIDTH / 2 - 2, SCREEN_HEIGHT / 2 - TEXT_HEIGHT / 2 - 2, white);

//	SDL_Color textColor = { 255, 255, 255, 255 };

	// Render back button
//	SDL_SetRenderDrawColor(renderer, 40, 40, 50, 255);
//	SDL_Rect backButton = { 20, 20, 100, 40 };
//	DrawRoundedRect(backButton.x, backButton.y, backButton.w, backButton.h);
//	renderText(font, "Back", 35, 25, textColor);
	SDL_RenderPresent(renderer);

	SDL_Delay(2 * 1000);
	game->currentScreen = CREDITS_SCREEN;

}
// Helper function to draw rounded rectangle
void Splash::DrawRoundedRect(int x, int y, int w, int h) {
	// Draw the middle rectangle
	SDL_Rect rect = { x, y, w, h };
	SDL_RenderFillRect(renderer, &rect);
}

void Splash::renderText(TTF_Font* font, const char* text, int x, int y, SDL_Color color) {
	SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_Rect rect = { x, y, surface->w, surface->h };
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
