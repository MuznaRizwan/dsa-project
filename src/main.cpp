#ifndef __ANDROID__
#define SDL_MAIN_HANDLED
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <string>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

// C:\SDL2\x86_64-w64-mingw32\lib
// C:\SDL2_devel\SDL2-2.28.5\x86_64-w64-mingw32\lib

enum GameScreen {
    SELECTION_SCREEN,
    DETAILS_SCREEN,
    LOADING_SCREEN
};

struct GameState {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* backgrounds[3];  // Textures for different screens
    Mix_Music* bgMusic;
    Mix_Chunk* clickSound;
    GameScreen currentScreen;
    int selectedOption;
};

bool initGame(GameState* game) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        return false;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer initialization failed: %s\n", Mix_GetError());
        return false;
    }

    // Create window
    game->window = SDL_CreateWindow("Game Selection",
                                  SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED,
                                  WINDOW_WIDTH,
                                  WINDOW_HEIGHT,
                                  SDL_WINDOW_SHOWN);
    if (!game->window) {
        printf("Window creation failed: %s\n", SDL_GetError());
        return false;
    }

    // Create renderer
    game->renderer = SDL_CreateRenderer(game->window, -1,
                                      SDL_RENDERER_ACCELERATED);
    if (!game->renderer) {
        printf("Renderer creation failed: %s\n", SDL_GetError());
        return false;
    }

    // Load textures
    // Note: Replace paths with actual image files
    SDL_Surface* tempSurface = IMG_Load("selection_bg.png");
    game->backgrounds[0] = SDL_CreateTextureFromSurface(game->renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    tempSurface = IMG_Load("details_bg.png");
    game->backgrounds[1] = SDL_CreateTextureFromSurface(game->renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    tempSurface = IMG_Load("loading_bg.png");
    game->backgrounds[2] = SDL_CreateTextureFromSurface(game->renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    // Load music and sound effects
    game->bgMusic = Mix_LoadMUS("background_music.mp3");
    game->clickSound = Mix_LoadWAV("click.wav");

    game->currentScreen = SELECTION_SCREEN;
    game->selectedOption = 0;

    // Start playing background music
    Mix_PlayMusic(game->bgMusic, -1);  // -1 for infinite loop

    return true;
}

void cleanupGame(GameState* game) {
    for (int i = 0; i < 3; i++) {
        SDL_DestroyTexture(game->backgrounds[i]);
    }
    Mix_FreeMusic(game->bgMusic);
    Mix_FreeChunk(game->clickSound);
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);
    Mix_CloseAudio();
    SDL_Quit();
}

void handleEvents(GameState* game, bool* quit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *quit = true;
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            Mix_PlayChannel(-1, game->clickSound, 0);

            switch (game->currentScreen) {
                case SELECTION_SCREEN:
                    // Check which option was clicked
                    if (mouseY > 100 && mouseY < 200) game->selectedOption = 0;
                    else if (mouseY > 250 && mouseY < 350) game->selectedOption = 1;
                    else if (mouseY > 400 && mouseY < 500) game->selectedOption = 2;
                    game->currentScreen = DETAILS_SCREEN;
                    break;

                case DETAILS_SCREEN:
                    game->currentScreen = LOADING_SCREEN;
                    break;

                case LOADING_SCREEN:
                    // Could transition to actual game here
                    game->currentScreen = SELECTION_SCREEN;
                    break;
            }
        }
    }
}

void render(GameState* game) {
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
    SDL_RenderClear(game->renderer);

    // Render background based on current screen
    SDL_RenderCopy(game->renderer,
                   game->backgrounds[game->currentScreen],
                   NULL, NULL);

    // Render additional elements based on current screen
    switch (game->currentScreen) {
        case SELECTION_SCREEN: {
            // Render selection options (could be car images)
            SDL_Rect option1 = {50, 100, 700, 100};
            SDL_Rect option2 = {50, 250, 700, 100};
            SDL_Rect option3 = {50, 400, 700, 100};

            SDL_SetRenderDrawColor(game->renderer, 100, 100, 200, 255);
            SDL_RenderFillRect(game->renderer, &option1);
            SDL_RenderFillRect(game->renderer, &option2);
            SDL_RenderFillRect(game->renderer, &option3);
			break;
		}

        case DETAILS_SCREEN: {
            // Render details for selected option
            SDL_Rect detailsBox = {100, 100, 600, 400};
            SDL_SetRenderDrawColor(game->renderer, 150, 150, 150, 255);
            SDL_RenderFillRect(game->renderer, &detailsBox);
			break;
		}

        case LOADING_SCREEN: {
            // Render loading animation
            static int loadingAnim = 0;
            SDL_Rect loadingBar = {200, 280, loadingAnim, 40};
            SDL_SetRenderDrawColor(game->renderer, 0, 255, 0, 255);
            SDL_RenderFillRect(game->renderer, &loadingBar);
            loadingAnim = (loadingAnim + 2) % 400;
			break;
		}
    }

    SDL_RenderPresent(game->renderer);
}

int main(int argc, char* argv[]) {
    GameState game;
    bool quit = false;

    if (!initGame(&game)) {
        return 1;
    }

    // Main game loop
    while (!quit) {
        handleEvents(&game, &quit);
        render(&game);
        SDL_Delay(16);  // Cap at roughly 60 FPS
    }

    cleanupGame(&game);
    return 0;
}