#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>
#include <list>
#include <cstdio>  // For debugging

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 500;
const int PLAYER_WIDTH = 50;
const int PLAYER_HEIGHT = 50;
const int ZOMBIE_WIDTH = 40;
const int ZOMBIE_HEIGHT = 40;
const int BOSS_WIDTH = 80;
const int BOSS_HEIGHT = 80;
const int PLAYER_MOVE_SPEED = 5;
const int BULLET_COOLDOWN_TIME = 500;
const int MOVEMENT_INTERVAL = 16;
const int BOSS_MOVE_SPEED = 3;
const int BOSS_SHOOT_INTERVAL = 1000;
const int BOSS_HEALTH = 10;
const int BOSS_MOVE_COOLDOWN = 200;
const Uint32 LEVEL_TRANSITION_DELAY = 2000;

enum GameState { PLAYING, PAUSED, GAME_OVER, WIN };
enum ZombieType { SLOW, FAST };  // Define zombie types

struct Bullet {
    int x, y, strength;
    int speedX, speedY;

    Bullet(int _x, int _y, int _speedX, int _speedY)
        : x(_x), y(_y), speedX(_speedX), speedY(_speedY) {}
};

struct Zombie {
    int x, y;
    Uint32 nextMoveTime;
    Uint32 moveInterval;
    ZombieType type;  // Add a type to distinguish between fast and slow zombies

    Zombie(int _x, int _y, Uint32 _moveInterval, ZombieType _type)
        : x(_x), y(_y), moveInterval(_moveInterval), type(_type) {
        nextMoveTime = SDL_GetTicks();
    }

    bool operator<(const Zombie& other) const {
        return nextMoveTime > other.nextMoveTime;
    }
};

struct Boss {
    int x, y, health;
    Uint32 lastShotTime;
    Uint32 lastMoveTime; // Tracks the cooldown between movements
    Uint32 lastDirectionChangeTime; // Tracks when to change direction
    bool movingDown; // Track the current movement direction (true = down, false = up)

    Boss(int _x, int _y, int _health)
        : x(_x), y(_y), health(_health), lastShotTime(SDL_GetTicks()),
          lastMoveTime(SDL_GetTicks()), lastDirectionChangeTime(SDL_GetTicks()),
          movingDown(true) {}
};

SDL_Renderer* renderer;
TTF_Font* font;

std::list<Bullet> bullets;
std::list<Bullet> bossBullets;
std::priority_queue<Zombie> zombies;  // Priority queue for zombie movement
Boss* boss = nullptr;

Uint32 levelTransitionStartTime = 0;
bool waitingForNextLevel = false;
GameState gameState = PLAYING;
int playerX = 50, playerY = SCREEN_HEIGHT / 2, playerHealth = 5;
int bulletCooldown = 0;
bool isPaused = false;
Uint32 lastMovementTime = 0;
int currentLevel = 1;
int currentWave = 1;
int totalWavesInLevel = 2;
bool bossWave = false;
int playerBulletCount = 10; // Initial bullet count
int maxBullets = 20; // Maximum bullets player can have
std::list<SDL_Rect> bulletDrops; // List to store bullet drop positions
const int BULLET_DROP_RESPAWN_TIME = 3000; // 3 seconds
Uint32 lastBulletDropSpawnTime = 0; // To control bullet drop spawn interval

void handleInput();
void movePlayer();
void moveZombies();
void moveBullets();
void moveBoss();
void moveBossBullets();
void checkCollisions();
void renderGame();
void spawnZombiesForWave();
void spawnBoss();
void nextWave();
void nextLevel();
void gameOver();
void renderPauseScreen();
void bossShoot();
void spawnBulletDrop();

int zombieMain() {
    srand(static_cast<unsigned int>(time(0)));

    if (SDL_Init(SDL_INIT_VIDEO) < 0) return -1;
    if (TTF_Init() == -1) return -1;

    SDL_Window* window = SDL_CreateWindow("Zombie Shooter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    font = TTF_OpenFont("arial.ttf", 28);
    if (!font) {
        printf("Failed to load font! TTF_Error: %s", TTF_GetError());
        return -1;
    }

    spawnZombiesForWave();

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        Uint32 currentTime = SDL_GetTicks();

    // Spawn a bullet drop every few seconds
    if (currentTime - lastBulletDropSpawnTime >= BULLET_DROP_RESPAWN_TIME) {
        spawnBulletDrop();
        lastBulletDropSpawnTime = currentTime;
    }

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) quit = true;

            // Check for SDL_KEYDOWN event for toggling pause with 'P'
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_p) {
                    // Toggle gameState between PLAYING and PAUSED
                    gameState = (gameState == PLAYING) ? PAUSED : PLAYING;
                } else if (e.key.keysym.sym == SDLK_q) {
                    quit = true;
                }
            }
        }

        if (gameState == PLAYING) {
            handleInput();
            Uint32 currentTime = SDL_GetTicks();

            if (waitingForNextLevel && currentTime - levelTransitionStartTime >= LEVEL_TRANSITION_DELAY) {
                nextLevel();
                waitingForNextLevel = false;
            }

            if (!waitingForNextLevel) {
                if (currentTime - lastMovementTime > MOVEMENT_INTERVAL) {
                    movePlayer();
                    moveBullets();
                    moveBossBullets();
                    lastMovementTime = currentTime;
                }

                moveZombies();
                if (boss) moveBoss();
                checkCollisions();
                renderGame();

                if (zombies.empty() && !bossWave) {
                    nextWave();
                }
            }
        } else if (gameState == PAUSED) {
            renderPauseScreen();
        } else if (gameState == GAME_OVER) {
            gameOver();
        }
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}

void handleInput() {
    const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

    if (currentKeyStates[SDL_SCANCODE_SPACE] && playerBulletCount > 0) {
        if (bulletCooldown == 0) {
            bullets.push_back(Bullet(playerX + PLAYER_WIDTH, playerY + PLAYER_HEIGHT / 2, 10, 0));
            bulletCooldown = BULLET_COOLDOWN_TIME;
            playerBulletCount--; // Decrease bullet count
        }
    }

    if (bulletCooldown > 0) bulletCooldown--;
}

void spawnBulletDrop() {
    int dropY = rand() % (SCREEN_HEIGHT - 20); // Random x position within screen bounds
    SDL_Rect drop = {playerX, dropY, 20, 20}; // Bullet drop size
    bulletDrops.push_back(drop);
}

void movePlayer() {
    const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
    int moveAmount = PLAYER_MOVE_SPEED;

    if (currentKeyStates[SDL_SCANCODE_UP]) playerY = std::max(0, playerY - moveAmount);
    if (currentKeyStates[SDL_SCANCODE_DOWN]) playerY = std::min(SCREEN_HEIGHT - PLAYER_HEIGHT, playerY + moveAmount);
}

void moveBullets() {
    for (auto it = bullets.begin(); it != bullets.end();) {
        it->x += it->speedX;
        if (it->x > SCREEN_WIDTH) {
            it = bullets.erase(it);
        } else {
            ++it;
        }
    }
}

void moveZombies() {
    Uint32 currentTime = SDL_GetTicks();
    std::priority_queue<Zombie> updatedZombies;

    while (!zombies.empty()) {
        Zombie z = zombies.top();
        zombies.pop();

        if (currentTime >= z.nextMoveTime) {
            int dx = playerX - z.x;
            int dy = playerY - z.y;
            float distance = sqrt(dx * dx + dy * dy);
            if (distance > 0) {
                int moveX = static_cast<int>((dx / distance) * 5);
                int moveY = static_cast<int>((dy / distance) * 5);
                z.x += moveX;
                z.y += moveY;
            }
            z.nextMoveTime = currentTime + z.moveInterval;
        }

        updatedZombies.push(z);
    }

    zombies = updatedZombies;
}

void moveBoss() {
    Uint32 currentTime = SDL_GetTicks();

    // Set a random movement duration between 500 and 2000 milliseconds
    int BOSS_MOVE_DURATION = 500 + (rand() % 1500);  // Random interval between 500 and 2000 ms

    // Change direction if movement duration has passed
    if (currentTime - boss->lastDirectionChangeTime >= BOSS_MOVE_DURATION) {
        boss->movingDown = !boss->movingDown; // Toggle direction
        boss->lastDirectionChangeTime = currentTime; // Reset direction change timer
    }

    // Only move if cooldown period has passed
    if (currentTime - boss->lastMoveTime >= BOSS_MOVE_COOLDOWN) {
        if (boss->movingDown) {
            boss->y += BOSS_MOVE_SPEED;
            if (boss->y > SCREEN_HEIGHT - BOSS_HEIGHT) boss->y = SCREEN_HEIGHT - BOSS_HEIGHT;
        } else {
            boss->y -= BOSS_MOVE_SPEED;
            if (boss->y < 0) boss->y = 0;
        }
        boss->lastMoveTime = currentTime; // Update last move time
    }

    bossShoot(); // Boss shooting remains as before
}


void bossShoot() {
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - boss->lastShotTime >= BOSS_SHOOT_INTERVAL) {
        int dx = playerX - boss->x;
        int dy = playerY - boss->y;
        float distance = sqrt(dx * dx + dy * dy);

        int bulletSpeedX = static_cast<int>((dx / distance) * 8);
        int bulletSpeedY = static_cast<int>((dy / distance) * 8);

        bossBullets.push_back(Bullet(boss->x, boss->y, bulletSpeedX, bulletSpeedY));
        boss->lastShotTime = currentTime;
    }
}

void moveBossBullets() {
    for (auto it = bossBullets.begin(); it != bossBullets.end();) {
        it->x += it->speedX;
        it->y += it->speedY;
        if (it->x > SCREEN_WIDTH || it->y > SCREEN_HEIGHT || it->y < 0) {
            it = bossBullets.erase(it);
        } else {
            ++it;
        }
    }
}

// Handle bullet and zombie/boss collisions
void checkCollisions() {
    // Check collisions between player bullets and zombies
    for (auto bulletIt = bullets.begin(); bulletIt != bullets.end();) {
        bool hit = false;
        std::priority_queue<Zombie> updatedZombies;

        // Process zombies individually to check for collisions
        while (!zombies.empty()) {
            Zombie z = zombies.top();
            zombies.pop();

            // Check if the bullet collides with this zombie
            if (!hit && bulletIt->x >= z.x && bulletIt->x <= z.x + ZOMBIE_WIDTH &&
                bulletIt->y >= z.y && bulletIt->y <= z.y + ZOMBIE_HEIGHT) {
                hit = true; // Mark bullet as hitting a zombie
            } else {
                updatedZombies.push(z); // No collision, keep zombie in the queue
            }
        }

        // Restore zombies back to the priority queue
        zombies = updatedZombies;

        // Remove the bullet if it hit a zombie
        if (hit) {
            bulletIt = bullets.erase(bulletIt);
        } else {
            ++bulletIt;
        }
    }

    // Check collisions between player bullets and the boss
    for (auto bulletIt = bullets.begin(); bulletIt != bullets.end();) {
        if (boss && bulletIt->x >= boss->x && bulletIt->x <= boss->x + BOSS_WIDTH &&
            bulletIt->y >= boss->y && bulletIt->y <= boss->y + BOSS_HEIGHT) {

            boss->health -= 1;
            if (boss->health <= 0) {
                delete boss; // Remove boss if health is 0 or below
                boss = nullptr;
                gameState = WIN; // Win the game if the boss is defeated
            }
            bulletIt = bullets.erase(bulletIt); // Remove bullet after hit
        } else {
            ++bulletIt;
        }
    }

// Check collision with bullet drops
    for (auto it = bulletDrops.begin(); it != bulletDrops.end();) {
        SDL_Rect drop = *it;
        SDL_Rect playerRect = {playerX, playerY, PLAYER_WIDTH, PLAYER_HEIGHT};

        // If player collides with a bullet drop
        if (SDL_HasIntersection(&playerRect, &drop)) {
            playerBulletCount = std::min(playerBulletCount + 5, maxBullets); // Increase bullets, up to max
            it = bulletDrops.erase(it); // Remove bullet drop after collection
        } else {
            ++it;
        }
    }
}

// Render the game screen
void renderGame() {
    // Clear the screen with a background color
    SDL_SetRenderDrawColor(renderer, 0, 100, 100, 100);
    SDL_RenderClear(renderer);

    // Render player
    SDL_Rect playerRect = {playerX, playerY, PLAYER_WIDTH, PLAYER_HEIGHT};
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &playerRect);

    // Render zombies
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    std::priority_queue<Zombie> tempZombies = zombies;
    while (!tempZombies.empty()) {
        Zombie z = tempZombies.top();
        tempZombies.pop();
        SDL_Rect zombieRect = {z.x, z.y, ZOMBIE_WIDTH, ZOMBIE_HEIGHT};
        SDL_RenderFillRect(renderer, &zombieRect);
    }

    // Render boss if present
    if (boss) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255); // Purple color for the boss
        SDL_Rect bossRect = {boss->x, boss->y, BOSS_WIDTH, BOSS_HEIGHT};
        SDL_RenderFillRect(renderer, &bossRect);
    }

    // Render player bullets
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (const Bullet& bullet : bullets) {
        SDL_Rect bulletRect = {bullet.x, bullet.y, 10, 10};
        SDL_RenderFillRect(renderer, &bulletRect);
    }

    // Render boss bullets
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow color for boss bullets
    for (const Bullet& bullet : bossBullets) {
        SDL_Rect bulletRect = {bullet.x, bullet.y, 10, 10};
        SDL_RenderFillRect(renderer, &bulletRect);
    }

    // Render level number
    SDL_Color textColor = {255, 255, 255, 255}; // White color
    std::string levelText = "Level: " + std::to_string(currentLevel);
    SDL_Surface* levelSurface = TTF_RenderText_Solid(font, levelText.c_str(), textColor);
    SDL_Texture* levelTexture = SDL_CreateTextureFromSurface(renderer, levelSurface);

    SDL_Rect levelRect = {10, 10, 0, 0}; // Position at the top-left of the screen
    SDL_QueryTexture(levelTexture, nullptr, nullptr, &levelRect.w, &levelRect.h);
    SDL_RenderCopy(renderer, levelTexture, nullptr, &levelRect);

    // Render wave number below the level number
    std::string waveText = "Wave: " + std::to_string(currentWave);
    SDL_Surface* waveSurface = TTF_RenderText_Solid(font, waveText.c_str(), textColor);
    SDL_Texture* waveTexture = SDL_CreateTextureFromSurface(renderer, waveSurface);

    SDL_Rect waveRect = {10, levelRect.y + levelRect.h + 5, 0, 0}; // Position below the level text
    SDL_QueryTexture(waveTexture, nullptr, nullptr, &waveRect.w, &waveRect.h);
    SDL_RenderCopy(renderer, waveTexture, nullptr, &waveRect);

    // Clean up level and wave textures and surfaces
    SDL_FreeSurface(levelSurface);
    SDL_DestroyTexture(levelTexture);
    SDL_FreeSurface(waveSurface);
    SDL_DestroyTexture(waveTexture);

    // Render bullet drops
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue color for bullet drops
    for (const SDL_Rect& drop : bulletDrops) {
        SDL_RenderFillRect(renderer, &drop);
    }

    // Render bullet count
    std::string bulletCountText = "Bullets: " + std::to_string(playerBulletCount);
    SDL_Surface* bulletSurface = TTF_RenderText_Solid(font, bulletCountText.c_str(), textColor);
    SDL_Texture* bulletTexture = SDL_CreateTextureFromSurface(renderer, bulletSurface);

    SDL_Rect bulletRect = {SCREEN_WIDTH - 150, 10, 0, 0}; // Top-right of the screen
    SDL_QueryTexture(bulletTexture, nullptr, nullptr, &bulletRect.w, &bulletRect.h);
    SDL_RenderCopy(renderer, bulletTexture, nullptr, &bulletRect);

    SDL_FreeSurface(bulletSurface);
    SDL_DestroyTexture(bulletTexture);

    SDL_RenderPresent(renderer);
}

// Spawn zombies for the current wave
void spawnZombiesForWave() {
    int spawnX = SCREEN_WIDTH + ZOMBIE_WIDTH;  // Start just off the right side of the screen
	int zombiesAmount = currentWave == 1? 5 : 10;

    for (int i = 0; i < zombiesAmount; ++i) {
        Uint32 moveInterval;
        ZombieType type;

        if (currentLevel == 1) {
            type = SLOW;
            moveInterval = 80;  // Slow zombies have a move interval of 100
        } else if (currentLevel == 2) {
            type = FAST;
            moveInterval = 30;  // Fast zombies have a move interval of 50
        } else if (currentLevel == 3 && !bossWave) {
            // Mix of fast and slow zombies in level 3, except for the boss wave
            type = (rand() % 2 == 0) ? SLOW : FAST;
            moveInterval = (type == SLOW) ? 80 : 30;
        } else {
            bossWave = true;
            spawnBoss();
            continue;
        }

        zombies.push(Zombie(spawnX, rand() % (SCREEN_HEIGHT - ZOMBIE_HEIGHT), moveInterval, type));
    }
}

// Spawn the boss for the final wave of level 3
void spawnBoss() {
    int spawnX = SCREEN_WIDTH - BOSS_WIDTH; // Right side of the screen
    int spawnY = rand() % (SCREEN_HEIGHT - BOSS_HEIGHT);
    boss = new Boss(spawnX, spawnY, BOSS_HEALTH);
}

void nextWave() {
    currentWave++;
    if ((currentLevel < 3 && currentWave > totalWavesInLevel) || (currentLevel == 3 && currentWave > 3)) {
        waitingForNextLevel = true;
        levelTransitionStartTime = SDL_GetTicks();
    } else if (currentLevel == 3 && currentWave == 3) {
        bossWave = true;
        spawnBoss(); // Spawn the boss in the final wave of level 3
    } else {
        spawnZombiesForWave();
    }
}

// Move to the next level
void nextLevel() {
    currentLevel++;
    currentWave = 1;

    if (currentLevel == 3) {
        totalWavesInLevel = 3;  // Level 3 has 3 waves
    }

    if (currentLevel > 3) {
        gameState = WIN;
    } else {
        spawnZombiesForWave();
    }
}

// Handle game over logic
void gameOver() {
    gameState = GAME_OVER;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Black background
    SDL_RenderClear(renderer);

    SDL_Color white = {255, 255, 255};
    SDL_Surface* gameOverSurface = TTF_RenderText_Solid(font, "Game Over! Press R to Restart or Q to Quit", white);
    SDL_Texture* gameOverTexture = SDL_CreateTextureFromSurface(renderer, gameOverSurface);
    int textW, textH;
    SDL_QueryTexture(gameOverTexture, NULL, NULL, &textW, &textH);
    SDL_Rect textRect = {SCREEN_WIDTH / 2 - textW / 2, SCREEN_HEIGHT / 2 - textH / 2, textW, textH};
    SDL_RenderCopy(renderer, gameOverTexture, NULL, &textRect);

    SDL_RenderPresent(renderer);

    SDL_FreeSurface(gameOverSurface);
    SDL_DestroyTexture(gameOverTexture);
}

// Render the pause screen
void renderPauseScreen() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Black background
    SDL_RenderClear(renderer);

    SDL_Color white = {255, 255, 255};
    SDL_Surface* pauseSurface = TTF_RenderText_Solid(font, "Paused - Press P to Resume", white);
    SDL_Texture* pauseTexture = SDL_CreateTextureFromSurface(renderer, pauseSurface);
    int textW, textH;
    SDL_QueryTexture(pauseTexture, NULL, NULL, &textW, &textH);
    SDL_Rect textRect = {SCREEN_WIDTH / 2 - textW / 2, SCREEN_HEIGHT / 2 - textH / 2, textW, textH};
    SDL_RenderCopy(renderer, pauseTexture, NULL, &textRect);

    SDL_RenderPresent(renderer);

    SDL_FreeSurface(pauseSurface);
    SDL_DestroyTexture(pauseTexture);
}






