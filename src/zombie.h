#ifndef ZOMBIE_H
#define ZOMBIE_H

#include "globals.h"
#include <list>
#include <queue>
#include <SDL2/SDL_ttf.h>

const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 500;
const int GAME_HEIGHT = 450;
const int PLAYER_WIDTH = 50;
const int PLAYER_HEIGHT = 50;
const int ZOMBIE_WIDTH = 40;
const int ZOMBIE_HEIGHT = 40;
const int BOSS_WIDTH = 80;
const int BOSS_HEIGHT = 80;
const int PLAYER_MOVE_SPEED = 5;
const int BULLET_COOLDOWN_TIME = 100;
const int MOVEMENT_INTERVAL = 10;
const int BOSS_MOVE_SPEED = 3;
const int BOSS_SHOOT_INTERVAL = 1000;
const int BOSS_HEALTH = 10;
const int BOSS_MOVE_COOLDOWN = 200;
const Uint32 LEVEL_TRANSITION_DELAY = 2000;
const int BULLET_DROP_RESPAWN_TIME = 3000; // 3 seconds
const int buttonWidth = 50;
const int buttonHeight = 50;
const int startX = WINDOW_WIDTH / 2 - 115;
const int startY = GAME_HEIGHT;

enum ZombieGameState { PLAYING, PAUSED, GAME_OVER, WINNING };
enum ZombieType { SLOW, FAST };  // Define zombie types

struct Bullet {
	int x, y;
	int speedX, speedY;

	Bullet(int _x, int _y, int _speedX, int _speedY)
		: x(_x), y(_y), speedX(_speedX), speedY(_speedY) {}
};

struct Boss {
	int x, y, health;
	Uint32 lastShotTime;
	Uint32 lastMoveTime; // Tracks the cooldown between movements
	Uint32 moveInterval; // Tracks when to change direction
	bool movingDown; // Track the current movement direction (true = down, false = up)

	Boss(int _x, int _y, int _health, Uint32 _moveInterval)
		: x(_x), y(_y), health(_health), moveInterval(_moveInterval), lastShotTime(SDL_GetTicks()),
		lastMoveTime(SDL_GetTicks()), movingDown(true) {}
};

class Zombie {
public:
	//		SDL_Renderer* renderer;
	//		TTF_Font* font;
	//
	//		SDL_Texture* playButtonTexture = nullptr;
	//		SDL_Texture* pauseButtonTexture = nullptr;
	//		SDL_Texture* quitButtonTexture = nullptr;
	//		SDL_Texture* soundButtonTexture = nullptr;
	//		SDL_Texture* musicButtonTexture = nullptr;
	//		SDL_Texture* blackTexture = nullptr;
	//
	//		SDL_Rect playRect;
	//		SDL_Rect pauseRect;
	//		SDL_Rect quitRect;
	//		SDL_Rect musicRect;
	//		SDL_Rect soundRect;
	//
	//		std::list<Bullet> bullets;
	//		std::list<Bullet> bossBullets;
	//		std::priority_queue<Zombie> zombies;  // Priority queue for zombie movement
	//		std::list<SDL_Rect> bulletDrops;
	//		Boss* boss = nullptr;
	//
	//		Uint32 levelTransitionStartTime = 0;
	//		bool waitingForNextLevel = false;
	//		ZombieGameState zombieGameState = PLAYING;
	//		int playerX = 50, playerY = GAME_HEIGHT / 2, playerHealth = 5;
	//		int bulletCooldown = 0;
	//		bool quit = false;
	//		Uint32 lastMovementTime = 0;
	//		int currentLevel = 1;
	//		int currentWave = 1;
	//		int totalWavesInLevel = 2;
	//		bool bossWave = false;
	//		int playerBulletCount = 10; // Initial bullet count
	//		int maxBullets = 20; // Maximum bullets player can have
	//		Uint32 lastBulletDropSpawnTime = 0; // To control bullet drop spawn interval
	//
	int x, y;
	Uint32 nextMoveTime;
	Uint32 moveInterval;
	ZombieType type;  // Add a type to distinguish between fast and slow zombies

	Zombie(int _x, int _y, Uint32 _moveInterval, ZombieType _type)
		: x(_x), y(_y), moveInterval(_moveInterval), type(_type) {
		nextMoveTime = SDL_GetTicks();
	};

	bool operator<(const Zombie& other) const {
		return nextMoveTime > other.nextMoveTime;
	};

	//		void renderText(TTF_Font* font, const char* text,
	//		                int x, int y, SDL_Color color);
	//
	//		void movePlayer();
	//		void moveZombies();
	//		void moveBullets();
	//		void moveBoss();
	//		void moveBossBullets();
	//		void checkCollisions();
	//		void renderGame();
	//		void spawnZombiesForWave();
	//		void spawnBoss();
	//		void spawnBulletDrop();
	//		void nextWave();
	//		void nextLevel();
	//		void gameOver();
	//		void renderPauseScreen();
	//		void bossShoot();
	//		SDL_Texture* loadTexture(const char* path);
	//		bool isWithinRect(int x, int y, SDL_Rect rect);

};

class ZombieScreen : public BaseScreen {
public:
	GameState* game = nullptr;
	SDL_Renderer* renderer = nullptr;
	TTF_Font* font = nullptr;

	SDL_Texture* playButtonTexture = nullptr;
	SDL_Texture* pauseButtonTexture = nullptr;
	SDL_Texture* quitButtonTexture = nullptr;
	SDL_Texture* soundButtonTexture = nullptr;
	SDL_Texture* musicButtonTexture = nullptr;
	SDL_Texture* blackTexture = nullptr;

	SDL_Rect playRect = { 0,0,1,1 };
	SDL_Rect pauseRect = { 0,0,1,1 };
	SDL_Rect quitRect = { 0,0,1,1 };
	SDL_Rect musicRect = { 0,0,1,1 };
	SDL_Rect soundRect = { 0,0,1,1 };

	std::list<Bullet> bullets;
	std::list<Bullet> bossBullets;
	std::priority_queue<Zombie> zombies;  // Priority queue for zombie movement
	std::list<SDL_Rect> bulletDrops;
	Boss* boss = nullptr;

	Uint32 levelTransitionStartTime = 0;
	bool waitingForNextLevel = false;
	ZombieGameState zombieGameState = PLAYING;
	int playerX = 50, playerY = GAME_HEIGHT / 2, playerHealth = 5;
	int bulletCooldown = 0;
	bool quit = false;
	Uint32 lastMovementTime = 0;
	int currentLevel = 1;
	int currentWave = 1;
	int totalWavesInLevel = 2;
	bool bossWave = false;
	int playerBulletCount = 10; // Initial bullet count
	int maxBullets = 20; // Maximum bullets player can have
	Uint32 lastBulletDropSpawnTime = 0; // To control bullet drop spawn interval

	int x = 0, y = 0;
	Uint32 nextMoveTime = 0;
	Uint32 moveInterval = 0;
	ZombieType type = SLOW;  // Add a type to distinguish between fast and slow zombies

	void load(GameState* game);
	void handleEvents(SDL_Event event);
	void render();
	void renderText(TTF_Font* font, const char* text,
		int x, int y, SDL_Color color);
	void cleanUp();

	SDL_Texture* loadTexture(const char* path) const;
	void spawnZombiesForWave();
	void renderGame();
	void spawnBulletDrop();
	void nextLevel();
	void moveBullets();
	void moveBossBullets();
	void moveZombies();
	void checkCollisions();
	void nextWave();
	void moveBoss();
	void renderPauseScreen();
	void gameOver();
	void gameWin();
	void spawnBoss();
	void movePlayer();
	void bossShoot();
};
#endif
