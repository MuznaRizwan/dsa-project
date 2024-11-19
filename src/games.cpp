#ifndef __ANDROID__
#define SDL_MAIN_HANDLED
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <math.h>
#include "maze.h"
#include "zombie.h"
#include "mines.h"
#include "menu.h"
#include "basescreen.h"
#include "splash.h"
#include "loose.h"
#include "win.h"
#include "credits.h"
#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 500
#define GAME_COUNT 3

using namespace std;

// Helper function to draw rounded rectangle
void DrawRoundedRect(SDL_Renderer* renderer, int x, int y, int w, int h) {
	// Draw the middle rectangle
	SDL_Rect rect = {x, y, w, h};
	SDL_RenderFillRect(renderer, &rect);
}

bool initGame(GameState* game) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		printf("SDL initialization failed: %s\n", SDL_GetError());
		return false;
	}

	if (TTF_Init() < 0) {
		printf("TTF initialization failed: %s\n", TTF_GetError());
		return false;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		printf("SDL_mixer initialization failed: %s\n", Mix_GetError());
		return false;
	}

	game->window = SDL_CreateWindow("Tiny Triad",
	                                SDL_WINDOWPOS_UNDEFINED,
	                                SDL_WINDOWPOS_UNDEFINED,
	                                WINDOW_WIDTH,
	                                WINDOW_HEIGHT,
	                                SDL_WINDOW_SHOWN);

	game->renderer = SDL_CreateRenderer(game->window, -1,
	                                    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	// Load font
	game->font = TTF_OpenFont("assets/font/arial.ttf", 24);
	if (!game->font) {
		printf("Font loading failed: %s\n", TTF_GetError());
		return false;
	}

	// Initialize games array
	const char* gameNames[GAME_COUNT] = {"Mines Sweeper", "Sugar Rush", "Zombies World"};
	const char* gameImages[GAME_COUNT] = {"assets/images/icon1.png", "assets/images/icon2.png", "assets/images/icon3.png"};

	for (int i = 0; i < GAME_COUNT; i++) {
		SDL_Surface* surface = IMG_Load(gameImages[i]);
		if (!surface) {
			printf("Failed to load image %s: %s\n", gameImages[i], IMG_GetError());
			return false;
		}
//		game->games[i].texture = SDL_CreateTextureFromSurface(game->renderer, surface);
//		game->games[i].name = gameNames[i];
		SDL_FreeSurface(surface);
	}

	SDL_Surface* surface;
//	surface = IMG_Load("splash.png");

//	if (!surface) {
//		printf("Failed to load image %s: %s\n", "splash.png", IMG_GetError());
//		return false;
//	}
//	game->splashScreenTexture = SDL_CreateTextureFromSurface(game->renderer, surface);
//	SDL_FreeSurface(surface);

	surface = IMG_Load("assets/images/menu.png");
	if (!surface) {
		printf("Failed to load image %s: %s\n", "assets/images/menu.png", IMG_GetError());
		return false;
	}
	game->menuScreenTexture = SDL_CreateTextureFromSurface(game->renderer, surface);
	SDL_FreeSurface(surface);


	surface = IMG_Load("assets/images/overlay_logos.png");
	if (!surface) {
		printf("Failed to load image %s: %s\n", "assets/images/overlay_logos.png", IMG_GetError());
		return false;
	}
	game->menuScreenTexture = SDL_CreateTextureFromSurface(game->renderer, surface);
	SDL_FreeSurface(surface);

//	surface = IMG_Load("maze.png");
//	if (!surface) {
//		printf("Failed to load image %s: %s\n", "maze.png", IMG_GetError());
//		return false;
//	}
//	game->mazeScreenTexture = SDL_CreateTextureFromSurface(game->renderer, surface);
//	SDL_FreeSurface(surface);

	surface = IMG_Load("assets/images/minesweeper.png");
	if (!surface) {
		printf("Failed to load image %s: %s\n", "assets/images/minesweeper.png", IMG_GetError());
		return false;
	}
	game->minesweeperScreenTexture = SDL_CreateTextureFromSurface(game->renderer, surface);
	SDL_FreeSurface(surface);

//	surface = IMG_Load("zombie.png");
//	if (!surface) {
//		printf("Failed to load image %s: %s\n", "zombie.png", IMG_GetError());
//		return false;
//	}
//	game->zombieScreenTexture = SDL_CreateTextureFromSurface(game->renderer, surface);
//	SDL_FreeSurface(surface);

//	surface = IMG_Load("loose.png");
//	if (!surface) {
//		printf("Failed to load image %s: %s\n", "loose.png", IMG_GetError());
//		return false;
//	}
//	game->looseScreenTexture = SDL_CreateTextureFromSurface(game->renderer, surface);
//	SDL_FreeSurface(surface);

//	surface = IMG_Load("win.png");
//	if (!surface) {
//		printf("Failed to load image %s: %s\n", "win.png", IMG_GetError());
//		return false;
//	}

	surface = IMG_Load("assets/images/credits.png");
	if (!surface) {
		printf("Failed to load image %s: %s\n", "assets/images/credits.png", IMG_GetError());
		return false;
	}
	game->creditsScreenTexture = SDL_CreateTextureFromSurface(game->renderer, surface);
	SDL_FreeSurface(surface);


	// Load audio
	game->bgMusic = Mix_LoadMUS("assets/audio/background_music.mp3");
	game->hoverSound = Mix_LoadWAV("assets/audio/hover.wav");
	game->selectSound = Mix_LoadWAV("assets/audio/select.wav");

	game->previousScreen = SPLASH_SCREEN;
	game->currentScreen = MENU_SCREEN;
	game->selectedGame = 0;
	game->hoveredGame = -1;

	// Mix_PlayMusic(game->bgMusic, -1);

	return true;
}

void cleanupGame(GameState* game) {
//	for (int i = 0; i < GAME_COUNT; i++) {
//		SDL_DestroyTexture(game->games[i].texture);
//	}
	TTF_CloseFont(game->font);
	Mix_FreeMusic(game->bgMusic);
	Mix_FreeChunk(game->hoverSound);
	Mix_FreeChunk(game->selectSound);
	SDL_DestroyRenderer(game->renderer);
	SDL_DestroyWindow(game->window);
	TTF_Quit();
	Mix_CloseAudio();
	SDL_Quit();
}

void handleEvents(GameState* game, bool* quit) {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if(game->cScreen!=nullptr && game->currentScreen==game->previousScreen) {
			game->cScreen->handleEvents(/*game,quit,*/event);
		}
		if (event.type == SDL_QUIT) {
			*quit = true;
		} /*else if (event.type == SDL_MOUSEMOTION && game->currentScreen == MENU_SCREEN) {
			int mouseX, mouseY;
			SDL_GetMouseState(&mouseX, &mouseY);

			// Check if mouse is over any game MENU
			int oldHovered = game->hoveredGame;
			game->hoveredGame = -1;


			for (int i = 0; i < GAME_COUNT; i++) {
				SDL_Rect gameRect = (i == 0) ? SDL_Rect{305, 345, 192, 196} :
				                    (i == 1) ? SDL_Rect{548, 345, 192, 196} :
				                    SDL_Rect{738, 345, 192, 196};

				if (mouseX >= gameRect.x && mouseX <= gameRect.x + gameRect.w &&
				        mouseY >= gameRect.y && mouseY <= gameRect.y + gameRect.h) {
					game->hoveredGame = i;
					if (oldHovered != i) {
						Mix_PlayChannel(-1, game->hoverSound, 0);
					}
				}
			}


//			break;

		} else if (event.type == SDL_MOUSEBUTTONDOWN) {
			if (game->currentScreen == MENU_SCREEN && game->hoveredGame != -1) {

				game->selectedGame = game->hoveredGame;
				game->currentScreen = static_cast<GameScreen>(game->selectedGame);
				Mix_PlayChannel(-1, game->selectSound, 0);
			}
//            else if (game->currentScreen == DETAIL_SCREEN) {
//                // Check if back button was clicked
//                int mouseX, mouseY;
//                SDL_GetMouseState(&mouseX, &mouseY);
//                SDL_Rect backButton = {20, 20, 100, 40};
//
//                if (mouseX >= backButton.x && mouseX <= backButton.x + backButton.w &&
//                    mouseY >= backButton.y && mouseY <= backButton.y + backButton.h) {
//                    game->currentScreen = MENU_SCREEN;
//                    Mix_PlayChannel(-1, game->selectSound, 0);
//                }
//            }
			else if (game->currentScreen == CREDITS_SCREEN) {
				// Check if back button was clicked
				int mouseX, mouseY;
				SDL_GetMouseState(&mouseX, &mouseY);
				SDL_Rect backButton = {20, 20, 100, 40};

				if (mouseX >= backButton.x && mouseX <= backButton.x + backButton.w &&
				        mouseY >= backButton.y && mouseY <= backButton.y + backButton.h) {
					game->currentScreen = MENU_SCREEN;
					Mix_PlayChannel(-1, game->selectSound, 0);
				}
			}
		} else 	} else if(game->currentScreen == MENU_SCREEN) {
				switch (event.key.keysym.sym) {
					case SDLK_UP:
						game->hoveredGame = (game->hoveredGame - 1 + GAME_COUNT) % GAME_COUNT;
						Mix_PlayChannel(-1, game->hoverSound, 0);
						break;
					case SDLK_DOWN:
						game->hoveredGame = (game->hoveredGame + 1) % GAME_COUNT;
						Mix_PlayChannel(-1, game->hoverSound, 0);
						break;
					case SDLK_RETURN:
						if (game->hoveredGame != -1) {
							game->selectedGame = game->hoveredGame;
							game->currentScreen = static_cast<GameScreen>(game->selectedGame);

							Mix_PlayChannel(-1, game->selectSound, 0);
						}
						break;
					case SDLK_ESCAPE:
//                    if (game->currentScreen == DETAIL_SCREEN)
					{
						game->currentScreen = MENU_SCREEN;
						Mix_PlayChannel(-1, game->selectSound, 0);
					}
					break;
				}
			} else if(game->currentScreen == MAZE_SCREEN) {
				if (event.key.keysym.sym == SDLK_RETURN) {
					game->currentScreen = MENU_SCREEN;
				}
			} else if(game->currentScreen == MINESWEEPER_SCREEN) {
				if (event.key.keysym.sym == SDLK_RETURN) {
					game->currentScreen = MENU_SCREEN;
				}
			} else if(game->currentScreen == ZOMBIE_SCREEN) {
				if (event.key.keysym.sym == SDLK_RETURN) {
					game->currentScreen = MENU_SCREEN;
				}
			} else if(game->currentScreen == WIN_SCREEN) {
				if (event.key.keysym.sym == SDLK_RETURN) {
					game->currentScreen = LOOSE_SCREEN;
				}
			} else if(game->currentScreen == LOOSE_SCREEN) {
				if (event.key.keysym.sym == SDLK_RETURN) {
					game->currentScreen = MENU_SCREEN;
				}
			} else if(game->currentScreen == CREDITS_SCREEN) {
				if (event.key.keysym.sym == SDLK_RETURN) {
					game->currentScreen = WIN_SCREEN;
				}

			}
		}*/

	}
}

void renderText(SDL_Renderer* renderer, TTF_Font* font, const char* text,
                int x, int y, SDL_Color color) {
	SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_Rect rect = {x, y, surface->w, surface->h};
	SDL_RenderCopy(renderer, texture, NULL, &rect);

	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}

void renderMain(GameState* game) {
	SDL_SetRenderDrawColor(game->renderer, 20, 20, 30, 255);
	SDL_RenderClear(game->renderer);
	if(game->cScreen!=nullptr && game->currentScreen==game->previousScreen) {
		game->cScreen->render();
	}
	if(game->currentScreen!=game->previousScreen) {
		switch (game->currentScreen) {
			case MAZE_SCREEN:
				 game->cScreen = new MazeScreen();
				break;
			case MINESWEEPER_SCREEN:
				 game->cScreen = new Minesweeper();
				break;
			case ZOMBIE_SCREEN:
				 game->cScreen = new ZombieScreen();
				break;
			case SPLASH_SCREEN:
				game->cScreen = new Splash();
				break;
			case MENU_SCREEN:
				game->cScreen = new Menu();
				break;
			case LOOSE_SCREEN:
				game->cScreen = new Loose();
				break;
			case WIN_SCREEN:
				game->cScreen = new Win();
				break;
			case CREDITS_SCREEN:
				game->cScreen = new Credits();
				break;
		}
		game->cScreen->load(game);
		game->previousScreen=game->currentScreen;
	}
	/*	} else if (game->currentScreen == MENU_SCREEN) {
			// Render game MENUs as rounded rectangles on the left
			for (int i = 0; i < GAME_COUNT; i++) {
				SDL_Rect gameRect = {306, 370, 400, 200};

				// Draw highlight for hovered/selected game
				if (i == game->hoveredGame) {
					SDL_SetRenderDrawColor(game->renderer, 60, 60, 80, 255);
					DrawRoundedRect(game->renderer, gameRect.x - 5, gameRect.y - 5,
					                gameRect.w + 10, gameRect.h + 10);
				}

				// Draw game thumbnail
				SDL_SetRenderDrawColor(game->renderer, 40, 40, 50, 255);
				DrawRoundedRect(game->renderer, gameRect.x, gameRect.y,
				                gameRect.w, gameRect.h);
				SDL_RenderCopy(game->renderer, game->games[i].texture, NULL, &gameRect);
			}

			// Show selected game name on the right
			if (game->hoveredGame != -1) {
				SDL_Color textColor = {255, 255, 255, 255};
				renderText(game->renderer, game->font, game->games[game->hoveredGame].name,
				           500, WINDOW_HEIGHT/2, textColor);

			}
			SDL_RenderPresent(game->renderer);
		}
	//    else if (game->currentScreen == DETAIL_SCREEN) {
	//        // Render full-screen game image
	//        SDL_Rect fullRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
	//        SDL_RenderCopy(game->renderer, game->games[game->selectedGame].texture,
	//                      NULL, &fullRect);
	//
	//        // Render game name in bottom right
	//        SDL_Color textColor = {255, 255, 255, 255};
	//        renderText(game->renderer, game->font, game->games[game->selectedGame].name,
	//                  WINDOW_WIDTH - 300, WINDOW_HEIGHT - 50, textColor);
	//
	//        // Render back button
	//        SDL_SetRenderDrawColor(game->renderer, 40, 40, 50, 255);
	//        SDL_Rect backButton = {20, 20, 100, 40};
	//        DrawRoundedRect(game->renderer, backButton.x, backButton.y,
	//                       backButton.w, backButton.h, 10);
	//        renderText(game->renderer, game->font, "Back",
	//                  35, 25, textColor);
	//    }
		else if (game->currentScreen == CREDITS_SCREEN) {
			// Render full-screen game image
			SDL_Color textColor = {255, 255, 255, 255};

			SDL_Rect fullRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
			SDL_RenderCopy(game->renderer, game->creditsScreenTexture,
			               NULL, &fullRect);
			// Render back button
			SDL_SetRenderDrawColor(game->renderer, 40, 40, 50, 255);
			SDL_Rect backButton = {20, 20, 100, 40};
			DrawRoundedRect(game->renderer, backButton.x, backButton.y,
			                backButton.w, backButton.h);
			renderText(game->renderer, game->font, "Back",
			           35, 25, textColor);
			SDL_RenderPresent(game->renderer);
		} else if (game->currentScreen == LOOSE_SCREEN) {
			// Render full-screen game image
	//		SDL_Color textColor = {255, 255, 255, 255};
	//
	//		SDL_Rect fullRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
	//		SDL_RenderCopy(game->renderer, game->looseScreenTexture,
	//		               NULL, &fullRect);
	//		// Render back button
	//		SDL_SetRenderDrawColor(game->renderer, 40, 40, 50, 255);
	//		SDL_Rect backButton = {20, 20, 100, 40};
	//		DrawRoundedRect(game->renderer, backButton.x, backButton.y,
	//		                backButton.w, backButton.h);
	//		renderText(game->renderer, game->font, "Back",
	//		           35, 25, textColor);
	//		SDL_RenderPresent(game->renderer);
			game->looseScreen->render();
		} else if (game->currentScreen == WIN_SCREEN) {
			// Render full-screen game image
	//		SDL_Color textColor = {255, 255, 255, 255};
	//
	//		SDL_Rect fullRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
	//		SDL_RenderCopy(game->renderer, game->winScreenTexture,
	//		               NULL, &fullRect);
	//		// Render back button
	//		SDL_SetRenderDrawColor(game->renderer, 40, 40, 50, 255);
	//		SDL_Rect backButton = {20, 20, 100, 40};
	//		DrawRoundedRect(game->renderer, backButton.x, backButton.y,
	//		                backButton.w, backButton.h);
	//		renderText(game->renderer, game->font, "Back",
	//		           35, 25, textColor);
	//		SDL_RenderPresent(game->renderer);
			game->winScreen->render();
		}

	//    switch (game->currentScreen) {
	//        case MENU_SCREEN: {
	//            // Render background and buttons for the MENU screen
	////            SDL_RenderCopy(game->renderer, game->backgrounds[MENU_SCREEN], NULL, NULL);
	//
	//            // Render MENU options (could be car images or game buttons)
	//            SDL_Rect option1 = {50, 100, 700, 100};
	//            SDL_Rect option2 = {50, 250, 700, 100};
	//
	//            SDL_SetRenderDrawColor(game->renderer, 100, 100, 200, 255);
	//            SDL_RenderFillRect(game->renderer, &option1);  // Button for Zombie Sweeper
	//            SDL_RenderFillRect(game->renderer, &option2);  // Button for Maze Game
	//            break;
	//        }

		else if(game->currentScreen == ZOMBIE_SCREEN) {
			// Call the zombie game function or code here
			zombieMain();
			game->currentScreen = MENU_SCREEN;
		}


		else if(game->currentScreen == MAZE_SCREEN) {
			// Call the maze game function or code here
			mazeMain();
			game->currentScreen = MENU_SCREEN;
	//	} else if(game->currentScreen == MINESWEEPER_SCREEN) {
	//		// Call the maze game function or code here
	//		mineSweeperMain();
	//		game->currentScreen = MENU_SCREEN;
		} else {
			// Call the maze game function or code here
			game->currentScreen = MENU_SCREEN;
		}*/
}


int main(int argc, char* argv[]) {
	GameState game;
	bool quit = false;

	if (!initGame(&game)) {
		return 1;
	}

	while (!quit) {
		SDL_PumpEvents(); // Update the event state
		handleEvents(&game, &quit);
		renderMain(&game);
	}

	cleanupGame(&game);
	return 0;
}
