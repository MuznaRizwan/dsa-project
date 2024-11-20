#include "zombie.h"
#include <cmath>
#include <cstdio>  // For debugging
#include <cstdlib>
#include <ctime>
#include <list>
#include <queue>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <string>

void ZombieScreen::load(GameState* game) {
	this->game = game;
	this->renderer = game->renderer;
	srand(static_cast<unsigned int>(time(0)));

	font = TTF_OpenFont("assets/font/path_to_font.ttf", 28);
	if (!font) {
		printf("Failed to load font! TTF_Error: %s", TTF_GetError());
		return; // -1;
	}
	// Initialize the textures
	playButtonTexture = loadTexture("assets/images/play.png");
	pauseButtonTexture = loadTexture("assets/images/pause.png");
	quitButtonTexture = loadTexture("assets/images/quit.png");
	soundButtonTexture = loadTexture("assets/images/sound.png");
	musicButtonTexture = loadTexture("assets/images/music.png");
	blackTexture = loadTexture("assets/images/black_screen.png");


	if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
		printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		return; // -1;
	}

	spawnZombiesForWave();
}

void ZombieScreen::render() {
	//
	//	while (!quit) {
	Uint32 currentTime = SDL_GetTicks();

	renderGame();

	if (zombieGameState != PAUSED) {
		if (currentTime - lastBulletDropSpawnTime >= BULLET_DROP_RESPAWN_TIME) {
			spawnBulletDrop();
			lastBulletDropSpawnTime = currentTime;
		}
	}

	//		handleInput();  // Centralized input handling

	if (zombieGameState == PLAYING) {
		if (waitingForNextLevel && currentTime - levelTransitionStartTime >= LEVEL_TRANSITION_DELAY) {
			nextLevel();
			waitingForNextLevel = false;
		}

		if (!waitingForNextLevel) {
			if (currentTime - lastMovementTime > MOVEMENT_INTERVAL) {
				moveBullets();
				moveBossBullets();
				lastMovementTime = currentTime;
			}

			moveZombies();
			if (boss) moveBoss();
			checkCollisions();

			if (zombies.empty() && !bossWave) {
				nextWave();
			}
		}
	}
	else if (zombieGameState == PAUSED) {
		renderPauseScreen();
		//			handleInput();
	}
	else if (zombieGameState == GAME_OVER) {
		gameOver();
	}
	//	}
}

void ZombieScreen::renderText(TTF_Font* font, const char* text, int x, int y, SDL_Color color)
{
	SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_Rect destRect = { x, y, surface->w, surface->h };
	SDL_RenderCopy(renderer, texture, nullptr, &destRect);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
}

void ZombieScreen::cleanUp() {
	//TTF_CloseFont(font);
//	SDL_DestroyRenderer(renderer);
//	SDL_DestroyWindow(window);
//	TTF_Quit();
//	SDL_Quit();
//	return 0;
}

void ZombieScreen::handleEvents(SDL_Event e) {
	//	SDL_Event e;
	//	while (SDL_PollEvent(&e) != 0) {
	if (e.type == SDL_QUIT) {
		quit = true;
	}

	if (e.type == SDL_KEYDOWN) {
		switch (e.key.keysym.sym) {
		case SDLK_p:
			if (zombieGameState == PLAYING)
				zombieGameState = PAUSED;
			else {
				zombieGameState = PLAYING;
			}
			break;
		case SDLK_q:
			quit = true;
			break;
		case SDLK_SPACE:
			if (zombieGameState == PLAYING) {
				if (playerBulletCount > 0 && bulletCooldown == 0) {
					bullets.push_back(Bullet(playerX + PLAYER_WIDTH, playerY + PLAYER_HEIGHT / 2, 10, 0, 0));
					bulletCooldown = BULLET_COOLDOWN_TIME;
					playerBulletCount--; // Decrease bullet count
				}
			}
			break;
		case SDLK_UP:
		case SDLK_DOWN:
			if (zombieGameState == PLAYING) {
				movePlayer();
				break;
			}
		}
	}

	if (e.type == SDL_MOUSEBUTTONDOWN) {
		int x, y;
		SDL_GetMouseState(&x, &y);
		SDL_Point mousePoint = { x, y };
		if (SDL_PointInRect(&mousePoint, &playRect)) {
			zombieGameState = PLAYING;
		}
		else	if (SDL_PointInRect(&mousePoint, &pauseRect)) {
			zombieGameState = PAUSED;
		}
		else	if (SDL_PointInRect(&mousePoint, &musicRect)) {

		}
		else	if (SDL_PointInRect(&mousePoint, &soundRect)) {

		}
		else	if (SDL_PointInRect(&mousePoint, &quitRect)) {
			zombieGameState = GAME_OVER;
		}
	}
	//	}

	if (bulletCooldown > 0) bulletCooldown--;
}

void ZombieScreen::movePlayer() {
	const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
	int moveAmount = PLAYER_MOVE_SPEED;

	if (currentKeyStates[SDL_SCANCODE_UP]) playerY = std::max(0, playerY - moveAmount);
	if (currentKeyStates[SDL_SCANCODE_DOWN]) playerY = std::min(GAME_HEIGHT - PLAYER_HEIGHT, playerY + moveAmount);
}

void ZombieScreen::moveBullets() {
	for (auto it = bullets.begin(); it != bullets.end();) {
		it->x += it->speedX;
		if (it->x > WINDOW_WIDTH) {
			it = bullets.erase(it);
		}
		else {
			++it;
		}
	}
}

void ZombieScreen::moveZombies() {

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

void ZombieScreen::moveBoss() {
	Uint32 currentTime = SDL_GetTicks();
	if (currentTime >= boss->lastMoveTime + boss->moveInterval) { // Check if it's time for the boss to move
		int dx = playerX - boss->x;
		int dy = playerY - boss->y;
		float distance = sqrt(dx * dx + dy * dy);
		if (distance > 0) {
			int moveX = static_cast<int>((dx / distance) * BOSS_MOVE_SPEED); // Calculate the movement on X-axis
			int moveY = static_cast<int>((dy / distance) * BOSS_MOVE_SPEED); // Calculate the movement on Y-axis

			// Update boss's position
			boss->x += moveX;
			boss->y += moveY;
			boss->y = std::max(0, std::min(GAME_HEIGHT - BOSS_HEIGHT, boss->y)); // Prevent moving outside the screen
		}
		boss->lastMoveTime = currentTime; // Update the time of the last move
	}

	bossShoot(); // Boss shooting remains as before
}

void ZombieScreen::bossShoot() {
	Uint32 currentTime = SDL_GetTicks();
	if (currentTime - boss->lastShotTime >= BOSS_SHOOT_INTERVAL) {
		int dx = playerX - boss->x;
		int dy = playerY - boss->y;
		float distance = sqrt(dx * dx + dy * dy);

		int bulletSpeedX = static_cast<int>((dx / distance) * 8);
		int bulletSpeedY = static_cast<int>((dy / distance) * 8);

		bossBullets.push_back(Bullet(boss->x, boss->y, bulletSpeedX, bulletSpeedY, 0));
		boss->lastShotTime = currentTime;
	}
}

void ZombieScreen::moveBossBullets() {
	for (auto it = bossBullets.begin(); it != bossBullets.end();) {
		it->x += it->speedX;
		it->y += it->speedY;
		if (it->x > WINDOW_WIDTH || it->y > GAME_HEIGHT || it->y < 0) {
			it = bossBullets.erase(it);
		}
		else {
			++it;
		}
	}
}

// Handle bullet and zombie/boss collisions
void ZombieScreen::checkCollisions() {
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
			}
			else {
				updatedZombies.push(z); // No collision, keep zombie in the queue
			}
		}

		// Restore zombies back to the priority queue
		zombies = updatedZombies;

		// Remove the bullet if it hit a zombie
		if (hit) {
			bulletIt = bullets.erase(bulletIt);
		}
		else {
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
				zombieGameState = WINNING; // Win the game if the boss is defeated
			}
			bulletIt = bullets.erase(bulletIt); // Remove bullet after hit
		}
		else {
			++bulletIt;
		}
	}

	// Check collision with bullet drops
	for (auto it = bulletDrops.begin(); it != bulletDrops.end();) {
		SDL_Rect drop = *it;
		SDL_Rect playerRect = { playerX, playerY, PLAYER_WIDTH, PLAYER_HEIGHT };

		// If player collides with a bullet drop
		if (SDL_HasIntersection(&playerRect, &drop)) {
			playerBulletCount = std::min(playerBulletCount + 5, maxBullets); // Increase bullets, up to max
			it = bulletDrops.erase(it); // Remove bullet drop after collection
		}
		else {
			++it;
		}
	}

}

// Render the game screen
void ZombieScreen::renderGame() {

	{

		SDL_SetRenderDrawColor(renderer, 0, 100, 100, 100);
		SDL_RenderClear(renderer);
		SDL_Rect screenRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };

		// Render player
		SDL_Rect playerRect = { playerX, playerY, PLAYER_WIDTH, PLAYER_HEIGHT };
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
		SDL_RenderFillRect(renderer, &playerRect);

		// Render zombies
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

		std::priority_queue<Zombie> tempZombies = zombies;
		while (!tempZombies.empty()) {
			Zombie z = tempZombies.top();
			tempZombies.pop();
			SDL_Rect zombieRect = { z.x, z.y, ZOMBIE_WIDTH, ZOMBIE_HEIGHT };
			SDL_RenderFillRect(renderer, &zombieRect);
		}

		// Render boss if present
		if (boss) {
			SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255); // Purple color for the boss
			SDL_Rect bossRect = { boss->x, boss->y, BOSS_WIDTH, BOSS_HEIGHT };
			SDL_RenderFillRect(renderer, &bossRect);
		}

		// Render player bullets
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		for (const Bullet& bullet : bullets) {
			SDL_Rect bulletRect = { bullet.x, bullet.y, 10, 10 };
			SDL_RenderFillRect(renderer, &bulletRect);
		}

		// Render boss bullets
		SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow color for boss bullets
		for (const Bullet& bullet : bossBullets) {
			SDL_Rect bulletRect = { bullet.x, bullet.y, 10, 10 };
			SDL_RenderFillRect(renderer, &bulletRect);
		}

		// Render level number
		SDL_Color textColor = { 255, 255, 255, 255 }; // White color
		std::string levelText = "Level: " + std::to_string(currentLevel);
		SDL_Surface* levelSurface = TTF_RenderText_Solid(font, levelText.c_str(), textColor);
		SDL_Texture* levelTexture = SDL_CreateTextureFromSurface(renderer, levelSurface);

		SDL_Rect levelRect = { 10, 450, 0, 0 }; // Position at the top-left of the screen
		SDL_QueryTexture(levelTexture, nullptr, nullptr, &levelRect.w, &levelRect.h);
		SDL_RenderCopy(renderer, levelTexture, nullptr, &levelRect);

		// Render wave number below the level number
		std::string waveText = "Wave: " + std::to_string(currentWave);
		SDL_Surface* waveSurface = TTF_RenderText_Solid(font, waveText.c_str(), textColor);
		SDL_Texture* waveTexture = SDL_CreateTextureFromSurface(renderer, waveSurface);

		SDL_Rect waveRect = { 150, 450, 0, 0 }; // Position below the level text
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

		SDL_Rect bulletRect = { WINDOW_WIDTH - 150, 450, 0, 0 }; // Top-right of the screen
		SDL_QueryTexture(bulletTexture, nullptr, nullptr, &bulletRect.w, &bulletRect.h);
		SDL_RenderCopy(renderer, bulletTexture, nullptr, &bulletRect);

		if (zombieGameState == PAUSED) {
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
			SDL_RenderFillRect(renderer, &screenRect);
			SDL_Delay(1);
		}

		if (zombieGameState != PAUSED) {
			pauseRect = { 425, startY, buttonWidth, buttonHeight };
			SDL_RenderCopy(renderer, pauseButtonTexture, NULL, &pauseRect);
		}
		SDL_FreeSurface(bulletSurface);
		SDL_DestroyTexture(bulletTexture);

		SDL_RenderPresent(renderer);
	}
}

// Spawn zombies for the current wave
void ZombieScreen::spawnZombiesForWave() {
	int zombiesAmount = currentWave == 1 ? 5 : 10;

	for (int i = 0; i < zombiesAmount; ++i) {
		Uint32 moveInterval;
		ZombieType type;

		if (currentLevel == 1) {
			type = SLOW;
			moveInterval = 80;  // Slow zombies have a move interval of 100
		}
		else if (currentLevel == 2) {
			type = FAST;
			moveInterval = 30;  // Fast zombies have a move interval of 50
		}
		else if (currentLevel == 3 && !bossWave) {
			// Mix of fast and slow zombies in level 3, except for the boss wave
			type = (rand() % 2 == 0) ? SLOW : FAST;
			moveInterval = (type == SLOW) ? 80 : 30;
		}
		else {
			bossWave = true;
			spawnBoss();
			continue;
		}

		int x1 = WINDOW_WIDTH / 2 + 100;
		int x2 = WINDOW_WIDTH + ZOMBIE_WIDTH * 2;
		SDL_Delay(5);

		zombies.push(Zombie(x1 + rand() % x2, rand() % (GAME_HEIGHT - ZOMBIE_HEIGHT), moveInterval, type));
	}

}
// Spawn the boss for the final wave of level 3
void ZombieScreen::spawnBoss() {
	int spawnX = WINDOW_WIDTH - BOSS_WIDTH; // Right side of the screen
	int spawnY = rand() % (GAME_HEIGHT - BOSS_HEIGHT);
	Uint32 moveInterval = 1000; // Slow movement interval for the boss
	boss = new Boss(spawnX, spawnY, BOSS_HEALTH, moveInterval);
}

void ZombieScreen::spawnBulletDrop() {
	int dropY = rand() % (GAME_HEIGHT - 20); // Random y position within screen bounds
	SDL_Rect drop = { playerX, dropY, 20, 20 }; // Bullet drop size
	bulletDrops.push_back(drop);
}

void ZombieScreen::nextWave() {
	currentWave++;
	if ((currentLevel < 3 && currentWave > totalWavesInLevel) || (currentLevel == 3 && currentWave > 3)) {
		waitingForNextLevel = true;
		levelTransitionStartTime = SDL_GetTicks();
	}
	else if (currentLevel == 3 && currentWave == 3) {
		bossWave = true;
		spawnBoss(); // Spawn the boss in the final wave of level 3
	}
	else {
		spawnZombiesForWave();
	}
}

// Move to the next level
void ZombieScreen::nextLevel() {
	currentLevel++;
	currentWave = 1;

	if (currentLevel == 3) {
		totalWavesInLevel = 3;  // Level 3 has 3 waves
	}

	if (currentLevel > 3) {
		zombieGameState = WINNING;
	}
	else {
		spawnZombiesForWave();
	}
}

// Handle game over logic
void ZombieScreen::gameOver() {
	zombieGameState = GAME_OVER;
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Black background
	SDL_RenderClear(renderer);

	SDL_Color white = { 255, 255, 255 };
	SDL_Surface* gameOverSurface = TTF_RenderText_Solid(font, "Game Over! Press R to Restart or Q to Quit", white);
	SDL_Texture* gameOverTexture = SDL_CreateTextureFromSurface(renderer, gameOverSurface);
	int textW, textH;
	SDL_QueryTexture(gameOverTexture, NULL, NULL, &textW, &textH);
	SDL_Rect textRect = { WINDOW_WIDTH / 2 - textW / 2, GAME_HEIGHT / 2 - textH / 2, textW, textH };
	SDL_RenderCopy(renderer, gameOverTexture, NULL, &textRect);

	SDL_RenderPresent(renderer);

	SDL_FreeSurface(gameOverSurface);
	SDL_DestroyTexture(gameOverTexture);
}

// Render the pause screen
void ZombieScreen::renderPauseScreen() {
	playRect = { startX, startY, buttonWidth, buttonHeight };
	SDL_RenderCopy(renderer, playButtonTexture, NULL, &playRect);

	quitRect = { startX + buttonWidth + 10, startY, buttonWidth, buttonHeight };
	SDL_RenderCopy(renderer, quitButtonTexture, NULL, &quitRect);

	soundRect = { startX + 2 * buttonWidth + 20, startY, buttonWidth, buttonHeight };
	SDL_RenderCopy(renderer, soundButtonTexture, NULL, &soundRect);

	musicRect = { startX + 3 * buttonWidth + 30, startY, buttonWidth, buttonHeight };
	SDL_RenderCopy(renderer, musicButtonTexture, NULL, &musicRect);

	SDL_RenderPresent(renderer);

}

SDL_Texture* ZombieScreen::loadTexture(const char* path) const {
	SDL_Texture* newTexture = nullptr;
	SDL_Surface* loadedSurface = IMG_Load(path);
	if (!loadedSurface) {
		printf("Unable to load image! SDL_image Error: %s", IMG_GetError());
	}
	else {
		newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);  // Create texture from surface
		SDL_FreeSurface(loadedSurface);  // Free the loaded surface as it's no longer needed
		if (!newTexture) {
			printf("Unable to create texture from surface! SDL_Error: %s", SDL_GetError());
		}
	}
	return newTexture;
}
