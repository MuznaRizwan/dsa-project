#include "animation.h"
#include "credits.h"
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <string>
#include <thread>  // For using sleep_for

using namespace std;

void Credits::load(GameState* game) {
	this->game = game;
	this->renderer = game->renderer;

//    font = TTF_OpenFont("assets/font/arial.ttf", 24);
	// Initialize SDL_mixer
//	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
//	music = Mix_LoadMUS("assets/audio/background_music.mp3"); // Replace with your music file
//	Mix_PlayMusic(music, -1); // Loop the music

	// Load background images
	bg1 = IMG_LoadTexture(renderer, "assets/images/credits/credits.png"); // Replace with your image file
	//SDL_Texture* bg2 = IMG_LoadTexture(renderer, "assets/images/background1.png");

//	credits.init(renderer);
//	credits.run();
}

void Credits::handleEvents(SDL_Event event) {
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

void Credits::render() {

//		// Clear screen
//	SDL_RenderClear(renderer);
//
//	// Render backgrounds
//	SDL_Rect bgRect1 = { 0, bg1Y, SCREEN_WIDTH, SCREEN_HEIGHT };
//	//        SDL_Rect bgRect2 = { 0, bg2Y, SCREEN_WIDTH, SCREEN_HEIGHT };
//	SDL_RenderCopy(renderer, bg1, NULL, &bgRect1);
//	//SDL_RenderCopy(renderer, bg2, NULL, &bgRect2);
//
//	// Render girl animation
////	girlAnimation.render(renderer, gx /*(SCREEN_WIDTH - 100) / 2*/, SCREEN_HEIGHT - 200);
//
//	// Render text
//	SDL_Color white = { 255, 255, 255 };
//	TTF_Font* font = TTF_OpenFont("assets/font/arial.ttf", 24);
//	if (!font) {
//		printf("Font loading failed: %s\n", TTF_GetError());
//		return;
//	}
//	renderText(font, "Press 'R' to Resume or ESC to Quit", SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2, white);
//
//	// Present the rendered frame
//	SDL_RenderPresent(renderer);

    // Create a circular queue and add roles
//    RoleQueue roleQueue(9);  // Queue with capacity for 9 roles
//
//    // Create roles and add names
//    Role producer("Producer");
//    producer.addPerson("Mashal Zehra (GA-005)");
//    roleQueue.push(&producer);
//
//    Role technicalDirector("Technical Director");
//    technicalDirector.addPerson("Aasiya Asadullah (GA-001)");
//    roleQueue.push(&technicalDirector);
//
//    Role programmers("Programmers");
//    programmers.addPerson("Aasiya Asadullah (GA-001)");
//    programmers.addPerson("Rafia Gull (GA-002)");
//    programmers.addPerson("Mashal Zehra (GA-005)");
//    programmers.addPerson("Muzna Rizwan Zubairy (GA-007)");
//    programmers.sortPeople();
//    roleQueue.push(&programmers);
//
//    Role artDirector("Art Director");
//    artDirector.addPerson("Muzna Rizwan Zubairy (GA-007)");
//    roleQueue.push(&artDirector);
//
//    Role gameDesigner("Game Designer");
//    gameDesigner.addPerson("Muzna Rizwan Zubairy (GA-007)");
//    roleQueue.push(&gameDesigner);
//
//    Role gameArtist("Game Artist");
//    gameArtist.addPerson("Muzna Rizwan Zubairy (GA-007)");
//    roleQueue.push(&gameArtist);
//
//    Role tester("Tester");
//    tester.addPerson("Aasiya Asadullah (GA-001)");
//    roleQueue.push(&tester);
//
//    Role composer("Composer");
//    composer.addPerson("Rafia Gull (GA-002)");
//    roleQueue.push(&composer);
//
//    Role soundDesigner("Sound Designer");
//    soundDesigner.addPerson("Rafia Gull (GA-002)");
//    roleQueue.push(&soundDesigner);
//
//    // Start scrolling
//    int offsetY = 0;
//    bool running = true;
//    while (running)
//{
//        SDL_RenderClear(renderer);
//
//        // Render and display roles one by one
//        Role* currentRole = roleQueue.pop();
//        if (currentRole) {
//            currentRole->display(renderer, font, offsetY);
//            roleQueue.push(currentRole);  // Push the role back to the queue after display
//        }
//
//        // Handle input and update offsetY for scrolling
////        SDL_Event e;
////        while (SDL_PollEvent(&e)) {
////            if (e.type == SDL_QUIT) {
////                running = false;
////            }
////        }
//
//        offsetY -= 2;  // Scroll speed
//
//        // If all roles have been shown, introduce a slight pause before starting again
//        if (roleQueue.isEmpty()) {
//            std::this_thread::sleep_for(std::chrono::seconds(2));  // Pause for 2 seconds
////             Push the roles back into the queue to restart
//            roleQueue.push(&producer);
//            roleQueue.push(&programmers);
//        }
//
//        SDL_RenderPresent(renderer);
//        SDL_Delay(16);  // Delay to control frame rate
//    }

//	credits.run();

	SDL_Rect fullRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	SDL_RenderCopy(renderer, bg1, NULL, &fullRect);

	SDL_RenderPresent(renderer);

	SDL_Delay(2 * 1000);
	game->currentScreen = MENU_SCREEN;
}

void Credits::renderText(TTF_Font* font, const char* text,
	int x, int y, SDL_Color color) {
	SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_Rect rect = { x, y, surface->w, surface->h };
	SDL_RenderCopy(renderer, texture, NULL, &rect);

	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}


void Credits::cleanUp() {

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

  // Clean up
    TTF_CloseFont(font);
//    SDL_DestroyRenderer(renderer);
//    SDL_DestroyWindow(window);
//    TTF_Quit();
//    SDL_Quit();
}
