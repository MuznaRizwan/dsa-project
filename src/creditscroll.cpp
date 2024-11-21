#include "creditscroll.h"
//CreditScroll::CreditScroll() : /*window(nullptr),*/ renderer(nullptr), font(nullptr), scrollSpeed(2) {
//}

//CreditScroll::~CreditScroll() {
//    cleanUp();
//}

bool CreditScroll::init(SDL_Renderer* renderer) {
//    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
//        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
//        return false;
//    }
//
//    window = SDL_CreateWindow("Scrolling Credits", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
//                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
//    if (!window) {
//        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
//        return false;
//    }
//
//    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
//    if (!renderer) {
//        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
//        return false;
//    }
//
//    if (TTF_Init() == -1) {
//        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
//        return false;
//    }
	this->renderer = renderer;
    font = TTF_OpenFont("assets/font/arial.ttf", 24);
    if (!font) {
        std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }

    textColor = {255, 255, 255, 255};  // White text
    loadCredits();

    return true;
}

void CreditScroll::loadCredits() {
    // Load roles and names here (this can be dynamically added as well)
    creditQueue.push(Credit("Producer", {"John Doe", "Alice Smith", "Robert Brown"}));
    creditQueue.push(Credit("Director", {"James White"}));
    creditQueue.push(Credit("Lead Developer", {"Emma Green"}));
    creditQueue.push(Credit("Sound Engineer", {"Daniel Black", "Chris Grey"}));
    creditQueue.push(Credit("Artist", {"Mia Blue", "Liam Red"}));
}

void CreditScroll::renderCredits(const Credit& credit, int offset) {
    // Render the role (right-aligned to the left of the horizontal center)
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, credit.role.c_str(), textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect;
    textRect.x = CSCREEN_WIDTH / 2 - textSurface->w - 10; // 10 pixels padding from left
    textRect.y = offset;
    textRect.w = textSurface->w;
    textRect.h = textSurface->h;
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);

    // Render the names (left-aligned to the right of the horizontal center)
    int nameOffset = offset + textSurface->h + 10; // 10 pixels space after role

    for (const auto& name : credit.names) {
        textSurface = TTF_RenderText_Blended(font, name.c_str(), textColor);
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect nameRect;
        nameRect.x = CSCREEN_WIDTH / 2 + 10; // 10 pixels padding from center
        nameRect.y = nameOffset;
        nameRect.w = textSurface->w;
        nameRect.h = textSurface->h;
        SDL_RenderCopy(renderer, textTexture, NULL, &nameRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);

        nameOffset += textSurface->h + 5; // Add space between names
    }
}

void CreditScroll::run() {
    bool quit = false;
    SDL_Event e;
    int offset = CSCREEN_HEIGHT;

    while (!quit) {
        // Handle events
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
        SDL_RenderClear(renderer);

        // Render all credits with scrolling effect
        if (!creditQueue.empty()) {
            Credit credit = creditQueue.front();
            creditQueue.pop();  // Remove current credit

            // Sort names alphabetically
            std::sort(credit.names.begin(), credit.names.end());

            // Render the credit and names
            renderCredits(credit, offset);
            offset -= scrollSpeed;

            if (offset < -300) {
                // Add delay between scrolls (e.g., 1-2 seconds pause)
                std::this_thread::sleep_for(std::chrono::seconds(1));
                offset = CSCREEN_HEIGHT;  // Reset the scroll position
                creditQueue.push(credit); // Add the credit back for looping
            }
        }

        // Present the renderer
        SDL_RenderPresent(renderer);

        // Delay to control the frame rate
        SDL_Delay(16);  // ~60 FPS
    }
}

void CreditScroll::cleanUp() {
    TTF_CloseFont(font);
    font = nullptr;

//    SDL_DestroyRenderer(renderer);
//    SDL_DestroyWindow(window);
//    renderer = nullptr;
//    window = nullptr;

//    TTF_Quit();
//    SDL_Quit();
}

