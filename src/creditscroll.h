#ifndef CREDITSCROLL_H
#define CREDITSCROLL_H

#include <string>
#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <queue>
#include <algorithm>
#include <thread>

using namespace std;

// Screen dimensions
const int CSCREEN_WIDTH = 900;
const int CSCREEN_HEIGHT = 500;

// Structure to hold the role and associated names
struct Credit {
    std::string role;
    std::vector<std::string> names;

    Credit(std::string r, std::vector<std::string> n) : role(r), names(n) {}
};

class CreditScroll {
public:
//    CreditScroll();
//    ~CreditScroll();
    bool init(SDL_Renderer* renderer);
    void run();
    void cleanUp();

private:
//    SDL_Window* window;
    SDL_Renderer* renderer = nullptr;
    TTF_Font* font = nullptr;
    SDL_Color textColor = {128,128,128,128};
    std::queue<Credit> creditQueue;
    int scrollSpeed = 2; // Pixels per frame

    void renderCredits(const Credit& credit, int offset);
    void loadCredits();
};

#endif