#include "iGraphics.h"
#include "SDL.h"
#include "SDL_mixer.h"
#include <fstream>
#include <iostream>
#include<string>

using namespace std;

#define screenwidth 1000
#define screenheight 750
#define MAX_LASERS 100
#define asteroidnumber 7
#define asteroidwidth 60
#define asteroidheight 60
#define MAX_EXPLOSIONS 10
#define button_width 265
#define button_height 55
#define laser_width 35
#define laser_height 40
#define spaceship_width 100
#define spaceship_height 100
#define enemyspaceship_width 200
#define enemyspaceship_height 100
#define MAX_LIVES 5
#define healthnumber 1
#define healthwidth 55
#define healthheight 50
#define MAX_ENEMY_BULLETS 100
#define easyenemyshipnumber 1
#define mediumenemyshipnumber 2
#define hardenemyshipnumber 3
#define enemylaserwidth 35
#define enemylaserheight 40
#define bossheight 256
#define bosswidth 500

void drawHomePage();
void drawStartPage();
void drawAboutPage();
void drawInstructionPage();
void drawAHighScorePage();
void drawGameOverPage();
void drawWinnerPage();
void startButtonClickHandler();
void aboutButtonClickHandler();
void exitButtonClickHandler();
void instructionButtonClickHandler();
void highScoreButtonClickHandler();
void backButtonClickHandler();
void homeButtonClickHandler();
void easyButtonClickHandler();
void mediumButtonClickHandler();
void hardButtonClickHandler();
void soundClickHandler();
void musicClickHandler();
void drawDifficultyPage();

int home_img;
int credits_img;
int instruction_img;
int highscore_img;
int gameover_img;
int winner_img;
int difficulty_img;
int enemySpaceshipImg;
int nameBoxImg;
int back_button;

enum pages { home, about, instruction, highscore, difficulty, easy, medium, hard, gameover, winner };

pages currentPage = home;

struct Button {
	int x, y;
	int width, height;
	unsigned int normalImageID;
	unsigned int hoverImageID;
	bool isHovering;
};
const int numButtons = 5;
Button homebuttons[5];
const int numGameOverButtons = 4;
Button gameOverButtons[4];
const int numDifficulty = 3;
Button difficultyButtons[3];

struct Option {
	int x, y;
	int width, height;
	unsigned int normalImageID;
	unsigned int clickedImageID;
	bool isClicked;
};
const int numOptions = 2;
Option options[2];

int lastEnemyLaserTime = 0;
int enemyLaserDelay = 100;

struct Laser {
	float x, y;
	bool active;
	float speed;
};
Laser enemyLasers[MAX_ENEMY_BULLETS];
Laser bossLasers[MAX_ENEMY_BULLETS];

struct enemy {
	double x, y;
	bool show;
	double dx, dy;
	bool movingRight;
	int life;
	bool horizontal;
};
enemy bossEnemy = { 0.0, 0.0, false, 0.1, 0.1, false, 120, false };
enemy asteroid[asteroidnumber];
enemy healthitem[healthnumber];
enemy easyEnemyShip[easyenemyshipnumber] = { 0, 0, false, 3, 3, false, 4, false };
enemy mediumEnemyShip[mediumenemyshipnumber] = { 0, 0, false, 3, 3, false, 6, false };
enemy hardEnemyShip[hardenemyshipnumber] = { 0, 0, false, 3, 3, false, 8, false };

struct Explosion {
	float x, y;
	bool active;
	float time;
};

struct Life {
	float x, y;
	bool active;
};

struct HighScoreEntry {
	string playerName;
	int score;
};
HighScoreEntry highScoreEntry;
bool newHighScore = false;
char playerNameInput[100];
int playerNameIndex = 0;

int img;
int a;
int b;
int c;
int explosionImage;
int lifeImg;
int healthimg;
int enemylaser;
int bossimg;
int bosslaser;
int bossLifeImg[7];
float y = 0.0;
int playLives = 5;
bool musicOn = true;
bool soundOn = true;
bool gameOverSoundPlayed = false;
bool winnerSoundPlayed = false;
Laser lasers[MAX_LASERS];
Explosion explosions[MAX_EXPLOSIONS];
Life lives[MAX_LIVES];

float spaceship_x = 350;
float spaceship_y = 50;

float explosionDuration = 1.0;
int playerLives = MAX_LIVES;
int playerScore = 0;
int highScore = 0;
int currentEnemyShipIndex = 0;

void initializeLasers();
void initializeAsteroids();
void initializeHealthItem();
void initializeExplosions();
void enemy_spawn();
void renderExplosions();
void renderLives();
void renderScore();
void triggerExplosion(float x, float y);
bool checkCollision(float x1, float y1, float width1, float height1, float x2, float y2, float width2, float height2);
void iDraw();
void updateBackground();
void updateLasers();
void updateAsteroids();
void updateHealthItem();
void autoFireLasers();
void autoFireBossEnemyLasers();
void updateLives();
void updateBossEnemy();
void checkLifeCollision();
void saveHighScore();
void loadHighScore();

bool initSDL() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		return false;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		return false;
	}
	Mix_AllocateChannels(2);
	return true;
}

Mix_Music* backgroundMusic = NULL;

void playBackgroundMusic() {
	if (musicOn) {
		backgroundMusic = Mix_LoadMUS("8-bit-arcade-138828.wav");
		Mix_PlayMusic(backgroundMusic, -1);
	}
}
void stopBackgroundMusic() {
	Mix_HaltMusic();
	Mix_FreeMusic(backgroundMusic);
	backgroundMusic = NULL;
}

Mix_Chunk* explosionSound = NULL;
Mix_Chunk* gameOverSound = NULL;
Mix_Chunk* winnerSound = NULL;
Mix_Chunk* buttonPressSound = NULL;

void loadSoundEffects() {
	explosionSound = Mix_LoadWAV("./soundeffects/destroy.wav");
	gameOverSound = Mix_LoadWAV("./soundeffects/game_over.wav");
	winnerSound = Mix_LoadWAV("./soundeffects/winner.wav");
	buttonPressSound = Mix_LoadWAV("./soundeffects/click.wav");
}
void playExplosionSound() {
	if (soundOn) {
		Mix_PlayChannel(-1, explosionSound, 0);
	}
}
void playButtonPressSound() {
	if (soundOn) {
		Mix_PlayChannel(0, buttonPressSound, 0);
	}
}
void playGameOverSound() {
	if (soundOn) {
		Mix_PlayChannel(1, gameOverSound, 0);
	}
}
void playWinnerSound() {
	if (soundOn) {
		Mix_PlayChannel(1, winnerSound, 0);
	}
}
void drawDifficultyButtons() {
	for (int i = 0; i < numDifficulty; i++) {
		if (difficultyButtons[i].isHovering) {
			iShowImage(difficultyButtons[i].x, difficultyButtons[i].y, difficultyButtons[i].width, difficultyButtons[i].height, difficultyButtons[i].hoverImageID);
		}
		else {
			iShowImage(difficultyButtons[i].x, difficultyButtons[i].y, difficultyButtons[i].width, difficultyButtons[i].height, difficultyButtons[i].normalImageID);
		}
	}
}
void drawButtons() {
	for (int i = 0; i < numButtons; i++) {
		if (homebuttons[i].isHovering) {
			iShowImage(homebuttons[i].x, homebuttons[i].y, homebuttons[i].width, homebuttons[i].height, homebuttons[i].hoverImageID);
		}
		else {
			iShowImage(homebuttons[i].x, homebuttons[i].y, homebuttons[i].width, homebuttons[i].height, homebuttons[i].normalImageID);
		}
	}
}
void drawGameOverButtons() {
	for (int i = 0; i < numGameOverButtons; i++) {
		if (gameOverButtons[i].isHovering) {
			iShowImage(gameOverButtons[i].x, gameOverButtons[i].y, gameOverButtons[i].width, gameOverButtons[i].height, gameOverButtons[i].hoverImageID);
		}
		else {
			iShowImage(gameOverButtons[i].x, gameOverButtons[i].y, gameOverButtons[i].width, gameOverButtons[i].height, gameOverButtons[i].normalImageID);
		}
	}
}
void drawOptions() {
	for (int i = 0; i < numOptions; i++) {
		if (options[i].isClicked) {
			iShowImage(options[i].x, options[i].y, options[i].width, options[i].height, options[i].clickedImageID);
		}
		else {
			iShowImage(options[i].x, options[i].y, options[i].width, options[i].height, options[i].normalImageID);
		}
	}
}
void initializeLasers() {
	for (int i = 0; i < MAX_LASERS; ++i) {
		lasers[i].active = false;
	}
}
void initializeEnemyLasers() {
	for (int i = 0; i < MAX_ENEMY_BULLETS; ++i) {
		enemyLasers[i].active = false;
		enemyLasers[i].x = -100;
		enemyLasers[i].y = -100;
		enemyLasers[i].speed = 0.4;
	}
}
void initializeBossLaser() {
	for (int i = 0; i < MAX_ENEMY_BULLETS; ++i) {
		bossLasers[i].active = false;
		bossLasers[i].speed = 0.4;
	}
}
void initializeAsteroids() {
	for (int i = 0; i < asteroidnumber; ++i) {
		asteroid[i].x = rand() % (screenwidth - asteroidwidth);
		asteroid[i].y = screenheight + rand() % 100;
		asteroid[i].show = true;
	}
}
void initializeHealthItem(){
	for (int i = 0; i < healthnumber; ++i) {
		healthitem[i].x = rand() % (screenwidth - healthwidth);
		healthitem[i].y = screenheight + rand() % 100;
		healthitem[i].show = true;
	}
}
void initializeEasyEnemySpaceship() {
	for (int i = 0; i < easyenemyshipnumber; i++) {
		easyEnemyShip[i].show = false;
	}
}
void initializeMediumEnemySpaceship() {
	for (int i = 0; i < mediumenemyshipnumber; i++) {
		mediumEnemyShip[i].show = false;
	}
}
void initializeHardEnemySpaceship() {
	for (int i = 0; i < hardenemyshipnumber; i++) {
		hardEnemyShip[i].show = false;
	}
}
void easyEnemyAutoFireLasers() {
	int currentTime = clock();
	if (easyEnemyShip[currentEnemyShipIndex].show && currentTime - lastEnemyLaserTime >= enemyLaserDelay) {
		for (int j = 0; j < MAX_ENEMY_BULLETS; ++j) {
			if (!enemyLasers[j].active) {
				enemyLasers[j].x = easyEnemyShip[currentEnemyShipIndex].x + enemyspaceship_width * 0.5 - enemylaserwidth / 2;
				enemyLasers[j].y = easyEnemyShip[currentEnemyShipIndex].y;
				enemyLasers[j].active = true;
				lastEnemyLaserTime = currentTime;
				break;
			}
		}
	}
	currentEnemyShipIndex = (currentEnemyShipIndex + 1) % easyenemyshipnumber;
}
void mediumEnemyAutoFireLasers() {
	int currentTime = clock();
	if (mediumEnemyShip[currentEnemyShipIndex].show && currentTime - lastEnemyLaserTime >= enemyLaserDelay) {
		for (int j = 0; j < MAX_ENEMY_BULLETS; ++j) {
			if (!enemyLasers[j].active) {
				enemyLasers[j].x = mediumEnemyShip[currentEnemyShipIndex].x + enemyspaceship_width * 0.5 - enemylaserwidth / 2;
				enemyLasers[j].y = mediumEnemyShip[currentEnemyShipIndex].y;
				enemyLasers[j].active = true;
				lastEnemyLaserTime = currentTime;
				break;
			}
		}
	}
	currentEnemyShipIndex = (currentEnemyShipIndex + 1) % mediumenemyshipnumber;
}
void hardEnemyAutoFireLasers() {
	int currentTime = clock();
	if (hardEnemyShip[currentEnemyShipIndex].show && currentTime - lastEnemyLaserTime >= enemyLaserDelay) {
		for (int j = 0; j < MAX_ENEMY_BULLETS; ++j) {
			if (!enemyLasers[j].active) {
				enemyLasers[j].x = hardEnemyShip[currentEnemyShipIndex].x + enemyspaceship_width * 0.5 - enemylaserwidth / 2;
				enemyLasers[j].y = hardEnemyShip[currentEnemyShipIndex].y;
				enemyLasers[j].active = true;
				lastEnemyLaserTime = currentTime;
				break;
			}
		}
	}
	currentEnemyShipIndex = (currentEnemyShipIndex + 1) % hardenemyshipnumber;
}
void enemyAutoFireLasers() {
	if (currentPage == easy) {
		easyEnemyAutoFireLasers();
	}
	else if (currentPage == medium) {
		mediumEnemyAutoFireLasers();
	}
	else if (currentPage == hard) {
		hardEnemyAutoFireLasers();
	}
}
void initializeBossEnemy() {
	bossEnemy.show = false;
	bossEnemy.life = 120;
}
void initializeExplosions() {
	for (int i = 0; i < MAX_EXPLOSIONS; ++i) {
		explosions[i].active = false;
	}
}
void initializeLives() {
	for (int i = 0; i < MAX_LIVES; ++i) {
		lives[i].active = true;
		lives[i].x = 20 + i * 50;
		lives[i].y = screenheight - 50;
	}
}
void updateEnemyLasers() {
	for (int i = 0; i < MAX_ENEMY_BULLETS; ++i) {
		if (enemyLasers[i].active) {
			enemyLasers[i].y -= enemyLasers[i].speed;
			iShowImage(enemyLasers[i].x, enemyLasers[i].y, enemylaserwidth, enemylaserheight, enemylaser);
			if (checkCollision(enemyLasers[i].x, enemyLasers[i].y, enemylaserwidth, enemylaserheight,
				spaceship_x, spaceship_y, spaceship_width, spaceship_height)) {
				enemyLasers[i].active = false;
				playerLives -= 2;

				if (playerLives <= 0) {
					currentPage = gameover;
					saveHighScore();
					if (!gameOverSoundPlayed) {
						playGameOverSound();
						gameOverSoundPlayed = true;
					}
				}
			}
			if (enemyLasers[i].y < 0) {
				enemyLasers[i].active = false;
			}
		}
	}
}
void updateBossLasers() {
	for (int i = 0; i < MAX_ENEMY_BULLETS; ++i) {
		if (bossLasers[i].active) {
			bossLasers[i].y -= bossLasers[i].speed;
			iShowImage(bossLasers[i].x, bossLasers[i].y, enemylaserwidth, enemylaserheight, bosslaser);
			if (checkCollision(bossLasers[i].x, bossLasers[i].y, enemylaserwidth, enemylaserheight,
				spaceship_x, spaceship_y, spaceship_width, spaceship_height)) {
				bossLasers[i].active = false;
				playerLives -= 2;
			}
			if (bossLasers[i].y < 0) {
				bossLasers[i].active = false;
			}
		}
	}
}
void renderLives() {
	int lifeImageWidth = 30;
	int lifeImageHeight = 30;
	int lifeImageX = 10;
	int lifeImageY = screenheight - 60;

	for (int i = 0; i < playerLives; i++) {
		iShowImage(lifeImageX + i * (lifeImageWidth + 5), lifeImageY, lifeImageWidth, lifeImageHeight, lifeImg);
	}
}
void renderScore() {
	if (currentPage == easy || currentPage == medium || currentPage == hard) {
		iSetColor(255, 255, 255);
		iText(10, screenheight - 20, "Score:", GLUT_BITMAP_HELVETICA_18);
		string scoreText = to_string(playerScore);
		iText(70, screenheight - 20, (char*)scoreText.c_str(), GLUT_BITMAP_HELVETICA_18);

		renderLives();
	}
	else {
		iSetColor(255, 255, 255);
		iText(450, 570, "Score:", GLUT_BITMAP_TIMES_ROMAN_24);
		string scoreText = to_string(playerScore);
		iText(520, 570, (char*)scoreText.c_str(), GLUT_BITMAP_TIMES_ROMAN_24);
	}

}
void checkNewHighScore() {
	if (playerScore > highScoreEntry.score) {
		highScoreEntry.score = playerScore;
		newHighScore = true;
		memset(playerNameInput, 0, sizeof(playerNameInput));
	}
}
void renderHighScore() {
	iSetColor(255, 255, 255);
	if (currentPage == gameover || currentPage == winner) {
		checkNewHighScore();
		if (newHighScore) {
			iText(350, 530, "New High Score! Enter your name:", GLUT_BITMAP_TIMES_ROMAN_24);
			iShowImage(370, 440, button_width, button_height, nameBoxImg);
			iText(380, 470, playerNameInput, GLUT_BITMAP_TIMES_ROMAN_24);
		}
		else {
			iText(420, 480, "High Score: ", GLUT_BITMAP_TIMES_ROMAN_24);
			string highScoreText = to_string(highScoreEntry.score);
			iText(550, 480, (char*)highScoreText.c_str(), GLUT_BITMAP_TIMES_ROMAN_24);
		}
	}
	else {
		iText(450, 410, "Score: ", GLUT_BITMAP_TIMES_ROMAN_24);
		string highScoreText = to_string(highScoreEntry.score);
		iText(550, 410, (char*)highScoreText.c_str(), GLUT_BITMAP_TIMES_ROMAN_24);
		iText(420, 380, "Player Name: ", GLUT_BITMAP_TIMES_ROMAN_24);
		string playerNameText = highScoreEntry.playerName;
		iText(560, 380, (char*)playerNameText.c_str(), GLUT_BITMAP_TIMES_ROMAN_24);
	}
}
void iDraw() {
	iClear();
	if (currentPage == home) {
		drawHomePage();
	}
	else if (currentPage == difficulty) {
		drawDifficultyPage();
	}
	else if (currentPage == easy || currentPage == medium || currentPage == hard) {
		iShowImage(0, y, screenwidth, screenheight, img);
		iShowImage(0, y + screenheight, screenwidth, screenheight, img);
		iShowImage(spaceship_x, spaceship_y, spaceship_width, spaceship_height, a);
		y -= 0.02;
		if (y < -screenheight)
			y = 0;
		for (int i = 0; i < MAX_LASERS; ++i) {
			if (lasers[i].active) {
				lasers[i].y += 0.05;
				if (lasers[i].y > screenheight) {
					lasers[i].active = false;
				}
				else {
					iShowImage(lasers[i].x, lasers[i].y, laser_width, laser_height, b);
				}
			}
		}

		if (playerLives > 0) {
			enemy_spawn();
			renderExplosions();
			renderLives();
			renderScore();
			//enemyAutoFireLasers();
			updateEnemyLasers();
			updateBossEnemy();
			//autoFireBossEnemyLasers();
			updateBossLasers();
		}
		else {
			currentPage = gameover;
			saveHighScore();
			if (!gameOverSoundPlayed) {
				gameOverSoundPlayed = true;
			}
			if (soundOn) {
				playGameOverSound();
			}
		}
		if (bossEnemy.life <= 0)
		{
			currentPage = winner;
			saveHighScore();
			if (!winnerSoundPlayed) {
				winnerSoundPlayed = true;
			}
			if (soundOn) {
				playWinnerSound();
			}
			bossEnemy.life = 120;
		}
	}
	else if (currentPage == about) {
		drawAboutPage();
	}
	else if (currentPage == instruction) {
		drawInstructionPage();
	}
	else if (currentPage == highscore) {
		drawAHighScorePage();
	}
	else if (currentPage == gameover) {
		drawGameOverPage();
	}
	else if (currentPage == winner) {
		drawWinnerPage();
	}
}
void renderExplosions() {
	for (int i = 0; i < MAX_EXPLOSIONS; ++i) {
		if (explosions[i].active) {
			if (soundOn) {
				playExplosionSound();
			}
			iShowImage(explosions[i].x, explosions[i].y, asteroidwidth, asteroidheight, explosionImage);
			explosions[i].time -= 0.01;
			if (explosions[i].time <= 0) {
				explosions[i].active = false;
			}
		}
	}
}
void spawnEasyEnemyShip(int index) {
	easyEnemyShip[index].x = rand() % (screenwidth - spaceship_width);
	easyEnemyShip[index].y = screenheight + rand() % 100;
	easyEnemyShip[index].dx = 2;
	easyEnemyShip[index].dy = 2;
	easyEnemyShip[index].movingRight = rand() % 2;
	easyEnemyShip[index].show = true;
	easyEnemyShip[index].life = 4;
}
void spawnMediumEnemyShip(int index) {
	mediumEnemyShip[index].x = rand() % (screenwidth - spaceship_width);
	mediumEnemyShip[index].y = screenheight + rand() % 100;
	mediumEnemyShip[index].dx = 2;
	mediumEnemyShip[index].dy = 2;
	mediumEnemyShip[index].movingRight = rand() % 2;
	mediumEnemyShip[index].show = true;
	mediumEnemyShip[index].life = 6;
}
void spawnHardEnemyShip(int index) {
	hardEnemyShip[index].x = rand() % (screenwidth - spaceship_width);
	hardEnemyShip[index].y = screenheight + rand() % 100;
	hardEnemyShip[index].dx = 2;
	hardEnemyShip[index].dy = 2;
	hardEnemyShip[index].movingRight = rand() % 2;
	hardEnemyShip[index].show = true;
	hardEnemyShip[index].life = 8;
}
void updateBossEnemy() {
	bossEnemy.y -= bossEnemy.dy;
	if (bossEnemy.y < screenheight / 2) {
		bossEnemy.y = screenheight / 2;
		if (bossEnemy.movingRight) {
			bossEnemy.x += bossEnemy.dx;
			if (bossEnemy.x >= screenwidth - bosswidth) {
				bossEnemy.movingRight = false;
			}
		}
		else {
			bossEnemy.x -= bossEnemy.dx;
			if (bossEnemy.x <= 0) {
				bossEnemy.movingRight = true;
			}
		}
	}
}
void updateEasyEnemyPositions() {
	for (int i = 0; i < easyenemyshipnumber; i++) {
		if (easyEnemyShip[i].show) {
			if (easyEnemyShip[i].y > screenheight / 2) {
				easyEnemyShip[i].y -= easyEnemyShip[i].dy;
			}
			else {
				easyEnemyShip[i].dy = 0;
			}

			if (easyEnemyShip[i].movingRight) {
				easyEnemyShip[i].x += easyEnemyShip[i].dx;
				if (easyEnemyShip[i].x >= screenwidth - enemyspaceship_width) {
					easyEnemyShip[i].movingRight = false;
				}
			}
			else {
				easyEnemyShip[i].x -= easyEnemyShip[i].dx;
				if (easyEnemyShip[i].x <= 0) {
					easyEnemyShip[i].movingRight = true;
				}
			}

			if (easyEnemyShip[i].y <= 0) {
				easyEnemyShip[i].show = false;
				spawnEasyEnemyShip(i);
			}
		}
	}
	updateBossEnemy();
}
void updateMediumEnemyPositions() {
	for (int i = 0; i < mediumenemyshipnumber; i++) {
		if (mediumEnemyShip[i].show) {
			if (mediumEnemyShip[i].y > screenheight / 2) {
				mediumEnemyShip[i].y -= mediumEnemyShip[i].dy;
			}
			else {
				mediumEnemyShip[i].dy = 0;
			}

			if (mediumEnemyShip[i].movingRight) {
				mediumEnemyShip[i].x += mediumEnemyShip[i].dx;
				if (mediumEnemyShip[i].x >= screenwidth - enemyspaceship_width) {
					mediumEnemyShip[i].movingRight = false;
				}
			}
			else {
				mediumEnemyShip[i].x -= mediumEnemyShip[i].dx;
				if (mediumEnemyShip[i].x <= 0) {
					mediumEnemyShip[i].movingRight = true;
				}
			}

			if (mediumEnemyShip[i].y <= 0) {
				mediumEnemyShip[i].show = false;
				spawnMediumEnemyShip(i);
			}
		}
	}
	updateBossEnemy();
}
void updateHardEnemyPositions() {
	for (int i = 0; i < hardenemyshipnumber; i++) {
		if (hardEnemyShip[i].show) {
			if (hardEnemyShip[i].y > screenheight / 2) {
				hardEnemyShip[i].y -= hardEnemyShip[i].dy;
			}
			else {
				hardEnemyShip[i].dy = 0;
			}

			if (hardEnemyShip[i].movingRight) {
				hardEnemyShip[i].x += hardEnemyShip[i].dx;
				if (hardEnemyShip[i].x >= screenwidth - enemyspaceship_width) {
					hardEnemyShip[i].movingRight = false;
				}
			}
			else {
				hardEnemyShip[i].x -= hardEnemyShip[i].dx;
				if (hardEnemyShip[i].x <= 0) {
					hardEnemyShip[i].movingRight = true;
				}
			}

			if (hardEnemyShip[i].y <= 0) {
				hardEnemyShip[i].show = false;
				spawnHardEnemyShip(i);
			}
		}
	}
	updateBossEnemy();
}
void updateEnemyPositions() {
	if (currentPage == easy) {
		updateEasyEnemyPositions();
	}
	else if (currentPage == medium) {
		updateMediumEnemyPositions();
	}
	else if (currentPage == hard) {
		updateHardEnemyPositions();
	}
}
void enemy_spawn() {
	for (int i = 0; i < asteroidnumber; i++) {
		if (asteroid[i].show) {
			iShowImage(asteroid[i].x, asteroid[i].y, asteroidwidth, asteroidheight, c);
		}

		for (int j = 0; j < MAX_LASERS; ++j) {
			if (lasers[j].active && checkCollision(lasers[j].x, lasers[j].y, laser_width, laser_height,
				asteroid[i].x, asteroid[i].y, asteroidwidth, asteroidheight) && playerLives != 0) {
				triggerExplosion(asteroid[i].x, asteroid[i].y);
				lasers[j].active = false;
				asteroid[i].show = false;
				playerScore += 10;
			}
		}

		if (asteroid[i].show && checkCollision(spaceship_x, spaceship_y, spaceship_width, spaceship_height,
			asteroid[i].x, asteroid[i].y, asteroidwidth, asteroidheight)) {
			triggerExplosion(asteroid[i].x, asteroid[i].y);
			asteroid[i].show = false;
			playerLives--;
		}
		if (asteroid[i].y < 0) {
			asteroid[i].show = false;
		}
		if (!asteroid[i].show) {
			if (rand() % 100 < 20) {
				asteroid[i].x = rand() % (screenwidth - asteroidwidth);
				asteroid[i].y = screenheight + rand() % 100;
				asteroid[i].show = true;
			}
		}
	}
	if (playerScore >= 1000) {
		for (int i = 0; i < healthnumber; i++) {
			if (healthitem[i].show) {
				iShowImage(healthitem[i].x, healthitem[i].y, healthwidth, healthheight, healthimg);
			}

			if (healthitem[i].show && checkCollision(spaceship_x, spaceship_y, spaceship_width, spaceship_height, healthitem[i].x, healthitem[i].y, healthwidth, healthheight)) {
				healthitem[i].show = false;
				if (playerLives <= MAX_LIVES) {
					playerLives++;
				}
			}

			if (healthitem[i].y < 0) {
				healthitem[i].show = false;
			}

			if (!healthitem[i].show) {
				if (rand() % 100 < 20) {
					healthitem[i].x = rand() % (screenwidth - healthwidth);
					healthitem[i].y = screenheight + rand() % 100;
					healthitem[i].show = true;
				}
			}
		}
		if (currentPage == easy) {
			for (int i = 0; i < easyenemyshipnumber; i++) {
				if (!easyEnemyShip[i].show) {
					spawnEasyEnemyShip(i);
				}

				if (easyEnemyShip[i].show) {
					iShowImage(easyEnemyShip[i].x, easyEnemyShip[i].y, enemyspaceship_width, enemyspaceship_height, enemySpaceshipImg);

					for (int j = 0; j < MAX_LASERS; j++) {
						if (lasers[j].active && checkCollision(lasers[j].x, lasers[j].y, laser_width, laser_height,
							easyEnemyShip[i].x, easyEnemyShip[i].y, enemyspaceship_width, enemyspaceship_height)) {
							lasers[j].active = false;
							easyEnemyShip[i].life -= 1;
							if (easyEnemyShip[i].life <= 0) {
								triggerExplosion(easyEnemyShip[i].x, easyEnemyShip[i].y);
								easyEnemyShip[i].show = false;
								playerScore += 50;
							}
						}
					}

					if (checkCollision(spaceship_x, spaceship_y, spaceship_width, spaceship_height,
						easyEnemyShip[i].x, easyEnemyShip[i].y, enemyspaceship_width, enemyspaceship_height)) {
						triggerExplosion(easyEnemyShip[i].x, easyEnemyShip[i].y);
						easyEnemyShip[i].show = false;
						playerLives--;
					}
				}
			}
		}
		else if (currentPage == medium) {
			for (int i = 0; i < mediumenemyshipnumber; i++) {
				if (!mediumEnemyShip[i].show) {
					spawnMediumEnemyShip(i);
				}

				if (mediumEnemyShip[i].show) {
					iShowImage(mediumEnemyShip[i].x, mediumEnemyShip[i].y, enemyspaceship_width, enemyspaceship_height, enemySpaceshipImg);

					for (int j = 0; j < MAX_LASERS; j++) {
						if (lasers[j].active && checkCollision(lasers[j].x, lasers[j].y, laser_width, laser_height,
							mediumEnemyShip[i].x, mediumEnemyShip[i].y, enemyspaceship_width, enemyspaceship_height)) {
							lasers[j].active = false;
							mediumEnemyShip[i].life -= 1;
							if (mediumEnemyShip[i].life <= 0) {
								triggerExplosion(mediumEnemyShip[i].x, mediumEnemyShip[i].y);
								mediumEnemyShip[i].show = false;
								playerScore += 50;
							}
						}
					}

					if (checkCollision(spaceship_x, spaceship_y, spaceship_width, spaceship_height,
						mediumEnemyShip[i].x, mediumEnemyShip[i].y, enemyspaceship_width, enemyspaceship_height)) {
						triggerExplosion(mediumEnemyShip[i].x, mediumEnemyShip[i].y);
						mediumEnemyShip[i].show = false;
						playerLives--;
					}
				}
			}
		}
		else if (currentPage == hard) {
			for (int i = 0; i < hardenemyshipnumber; i++) {
				if (!hardEnemyShip[i].show) {
					spawnHardEnemyShip(i);
				}

				if (hardEnemyShip[i].show) {
					iShowImage(hardEnemyShip[i].x, hardEnemyShip[i].y, enemyspaceship_width, enemyspaceship_height, enemySpaceshipImg);

					for (int j = 0; j < MAX_LASERS; j++) {
						if (lasers[j].active && checkCollision(lasers[j].x, lasers[j].y, laser_width, laser_height,
							hardEnemyShip[i].x, hardEnemyShip[i].y, enemyspaceship_width, enemyspaceship_height)) {
							lasers[j].active = false;
							hardEnemyShip[i].life -= 1;
							if (hardEnemyShip[i].life <= 0) {
								triggerExplosion(hardEnemyShip[i].x, hardEnemyShip[i].y);
								hardEnemyShip[i].show = false;
								playerScore += 50;
							}
						}
					}

					if (checkCollision(spaceship_x, spaceship_y, spaceship_width, spaceship_height,
						hardEnemyShip[i].x, hardEnemyShip[i].y, enemyspaceship_width, enemyspaceship_height)) {
						triggerExplosion(hardEnemyShip[i].x, hardEnemyShip[i].y);
						hardEnemyShip[i].show = false;
						playerLives--;
					}
				}
			}
		}
	}
	if (playerScore >= 2000) {
		if (currentPage == easy) {
			for (int i = 0; i < easyenemyshipnumber; i++) {
				easyEnemyShip[i].show = false;
			}
		}
		else if (currentPage == medium) {
			for (int i = 0; i < mediumenemyshipnumber; i++) {
				mediumEnemyShip[i].show = false;
			}
		}
		else if (currentPage == hard) {
			for (int i = 0; i < hardenemyshipnumber; i++) {
				hardEnemyShip[i].show = false;
			}
		}

		if (!bossEnemy.show) {
			bossEnemy.show = true;
			bossEnemy.x = rand() % (screenwidth - bosswidth);
			bossEnemy.y = screenheight + 50;
		}

		if (bossEnemy.show) {
			iShowImage(bossEnemy.x, bossEnemy.y, bosswidth, bossheight, bossimg);

			int lifeImagesToShow;
			int lifeImageWidth = 40;
			int lifeImageSpacing = 15;

			if (currentPage == easy) {
				lifeImagesToShow = (bossEnemy.life + 39) / 40;
			}
			else if (currentPage == medium) {
				lifeImagesToShow = (bossEnemy.life + 23) / 24;
			}
			else if (currentPage == hard) {
				lifeImagesToShow = (bossEnemy.life + 14) / 17;
			}

			int totalLifeWidth = lifeImagesToShow * lifeImageWidth + (lifeImagesToShow - 1) * lifeImageSpacing;
			int startX = bossEnemy.x + (bosswidth - totalLifeWidth) / 2;

			for (int i = 0; i < lifeImagesToShow; ++i) {
				iShowImage(startX + i * (lifeImageWidth + lifeImageSpacing), bossEnemy.y + bossheight + 10, lifeImageWidth, lifeImageWidth, bossLifeImg[i]);
			}

			for (int j = 0; j < MAX_LASERS; j++) {
				if (lasers[j].active && checkCollision(lasers[j].x, lasers[j].y, laser_width, laser_height,
					bossEnemy.x, bossEnemy.y, bosswidth, bossheight)) {

					lasers[j].active = false;
					bossEnemy.life -= 1;

					if (bossEnemy.life <= 0) {
						triggerExplosion(bossEnemy.x, bossEnemy.y);
						bossEnemy.show = false;
						playerScore += 50;
					}
				}
			}

			if (checkCollision(spaceship_x, spaceship_y, spaceship_width, spaceship_height,
				bossEnemy.x, bossEnemy.y, bosswidth, bossheight)) {

				triggerExplosion(bossEnemy.x, bossEnemy.y);
				playerLives--;
			}
		}
	}
}
bool checkCollision(float x1, float y1, float width1, float height1, float x2, float y2, float width2, float height2) {
	return !(x1 > x2 + width2 || x1 + width1 < x2 || y1 > y2 + height2 || y1 + height1 < y2);
}
void triggerExplosion(float x, float y) {
	for (int i = 0; i < MAX_EXPLOSIONS; ++i) {
		if (!explosions[i].active) {
			explosions[i].x = x;
			explosions[i].y = y;
			explosions[i].active = true;
			explosions[i].time = explosionDuration;
			break;
		}
	}
}
void updateBackground() {
	y -= 1.0;
	if (y < -screenheight)
		y = 0;
}
void updateLasers() {
	for (int i = 0; i < MAX_LASERS; ++i) {
		if (lasers[i].active) {
			lasers[i].y += 5.0;
			if (lasers[i].y > screenheight) {
				lasers[i].active = false;
			}
		}
	}
}
void updateAsteroids() {
	for (int i = 0; i < asteroidnumber; ++i) {
		if (asteroid[i].show) {
			asteroid[i].y -= 5.0;
		}
	}
}
void updateHealthItem(){
	for (int i = 0; i < healthnumber; ++i){
		if (healthitem[i].show){
			healthitem[i].y -= 3.0;
		}
	}
}
void updateLives() {
	for (int i = 0; i < MAX_LIVES; ++i) {
		if (lives[i].active && !asteroid[i].show) {
			lives[i].active = false;
		}
	}
}
void autoFireBossEnemyLasers() {
	if (bossEnemy.show){
		for (int i = 0; i < MAX_ENEMY_BULLETS; ++i) {
			if (!bossLasers[i].active) {
				bossLasers[i].active = true;
				bossLasers[i].x = bossEnemy.x + (bosswidth * 0.1) - (laser_width / 2);
				bossLasers[i].y = bossEnemy.y;
				break;
			}
		}
		for (int i = 0; i < MAX_ENEMY_BULLETS; ++i) {
			if (!bossLasers[i].active) {
				bossLasers[i].active = true;
				bossLasers[i].x = bossEnemy.x + (bosswidth * 0.4) - (laser_width / 2);
				bossLasers[i].y = bossEnemy.y;
				break;
			}
		}
		for (int i = 0; i < MAX_ENEMY_BULLETS; ++i) {
			if (!bossLasers[i].active) {
				bossLasers[i].active = true;
				bossLasers[i].x = bossEnemy.x + (bosswidth * 0.6) - (laser_width / 2);
				bossLasers[i].y = bossEnemy.y;
				break;
			}
		}
		for (int i = 0; i < MAX_ENEMY_BULLETS; ++i) {
			if (!bossLasers[i].active) {
				bossLasers[i].active = true;
				bossLasers[i].x = bossEnemy.x + (bosswidth * 0.9) - (laser_width / 2);
				bossLasers[i].y = bossEnemy.y;
				break;
			}
		}
	}
}
void autoFireLasers() {
	for (int i = 0; i < MAX_LASERS; ++i) {
		if (!lasers[i].active) {
			lasers[i].x = spaceship_x + spaceship_width * 0.25 - laser_width / 2;
			lasers[i].y = spaceship_y + spaceship_height - 50;
			lasers[i].active = true;
			break;
		}
	}
	for (int i = 0; i < MAX_LASERS; ++i) {
		if (!lasers[i].active) {
			lasers[i].x = spaceship_x + spaceship_width * 0.75 - laser_width / 2;
			lasers[i].y = spaceship_y + spaceship_height - 50;
			lasers[i].active = true;
			break;
		}
	}
}
void updateGameOverButtonHover(int mx, int my) {
	for (int i = 0; i < numGameOverButtons; i++) {
		if (mx >= gameOverButtons[i].x && mx <= gameOverButtons[i].x + gameOverButtons[i].width &&
			my >= gameOverButtons[i].y && my <= gameOverButtons[i].y + gameOverButtons[i].height) {
			gameOverButtons[i].isHovering = true;
		}
		else {
			gameOverButtons[i].isHovering = false;
		}
	}
}

void iMouseMove(int mx, int my) {
}
void iPassiveMouseMove(int mx, int my) {
	if (currentPage == home) {
		for (int i = 0; i < numButtons; i++) {
			if (mx > homebuttons[i].x && mx < homebuttons[i].x + homebuttons[i].width && my > homebuttons[i].y && my < homebuttons[i].y + homebuttons[i].height) {
				homebuttons[i].isHovering = true;
			}
			else {
				homebuttons[i].isHovering = false;
			}
		}
	}
	else if (currentPage == difficulty) {
		for (int i = 0; i < numDifficulty; i++) {
			if (mx > difficultyButtons[i].x && mx < difficultyButtons[i].x + difficultyButtons[i].width && my > difficultyButtons[i].y && my < difficultyButtons[i].y + difficultyButtons[i].height) {
				difficultyButtons[i].isHovering = true;
			}
			else {
				difficultyButtons[i].isHovering = false;
			}
		}
	}
	else if (currentPage == easy || currentPage == medium || currentPage == hard) {
		spaceship_x = mx - spaceship_width / 2;
		spaceship_y = my - spaceship_height / 2;

		if (spaceship_x < 0)
			spaceship_x = 0;
		if (spaceship_x > screenwidth - spaceship_width)
			spaceship_x = screenwidth - spaceship_width;
		if (spaceship_y < 0)
			spaceship_y = 0;
		if (spaceship_y > screenheight - spaceship_height)
			spaceship_y = screenheight - spaceship_height;
	}
	else if (currentPage == gameover) {
		for (int i = 0; i < numGameOverButtons; i++) {
			if (mx > gameOverButtons[i].x && mx < gameOverButtons[i].x + gameOverButtons[i].width && my > gameOverButtons[i].y && my < gameOverButtons[i].y + gameOverButtons[i].height) {
				gameOverButtons[i].isHovering = true;
			}
			else {
				gameOverButtons[i].isHovering = false;
			}
		}
	}
	else if (currentPage == winner) {
		for (int i = 0; i < numGameOverButtons; i++) {
			if (mx > gameOverButtons[i].x && mx < gameOverButtons[i].x + gameOverButtons[i].width && my > gameOverButtons[i].y && my < gameOverButtons[i].y + gameOverButtons[i].height) {
				gameOverButtons[i].isHovering = true;
			}
			else {
				gameOverButtons[i].isHovering = false;
			}
		}
	}

}
void iMouse(int button, int state, int mx, int my) {

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		//printf("x=%d y=%d\n", mx, my);
		if (currentPage == home)
		{
			if ((mx >= 130 && mx <= 395) && (my >= 380 && my <= 450)){
				startButtonClickHandler();
			}
			else if ((mx >= 130 && mx <= 395) && (my >= 280 && my <= 350)){
				aboutButtonClickHandler();
			}
			else if ((mx >= 600 && mx <= 865) && (my >= 380 && my <= 450)){
				highScoreButtonClickHandler();
			}
			else if ((mx >= 600 && mx <= 865) && (my >= 280 && my <= 350)){
				instructionButtonClickHandler();
			}
			else if ((mx >= 370 && mx <= 600) && (my >= 150 && my <= 220)){
				exitButtonClickHandler();
			}
			for (int i = 0; i < numOptions; i++) {
				if (mx >= options[i].x && mx <= options[i].x + options[i].width && my >= options[i].y && my <= options[i].y + options[i].height) {
					if (i == 0) {
						soundClickHandler();
					}
					else if (i == 1) {
						musicClickHandler();
					}
				}
			}

		}
		else if (currentPage == difficulty) {
			if ((mx >= 38 && mx <= 100) && (my >= 630 && my <= 700)){
				backButtonClickHandler();
			}
			else if ((mx >= 379 && mx <= 600) && (my >= 350 && my <= 420)){
				easyButtonClickHandler();
			}
			else if ((mx >= 370 && mx <= 600) && (my >= 250 && my <= 320)){
				mediumButtonClickHandler();
			}
			else if ((mx >= 370 && mx <= 600) && (my >= 150 && my <= 220)){
				hardButtonClickHandler();
			}
		}
		else if (currentPage == about){
			if ((mx >= 38 && mx <= 100) && (my >= 630 && my <= 700)){
				backButtonClickHandler();
			}
		}
		else if (currentPage == instruction){
			if ((mx >= 38 && mx <= 100) && (my >= 630 && my <= 700)){
				backButtonClickHandler();
			}
		}
		else if (currentPage == highscore){
			if ((mx >= 38 && mx <= 100) && (my >= 630 && my <= 700)){
				backButtonClickHandler();
			}
		}
		else if (currentPage == gameover) {
			if ((mx >= 379 && mx <= 600) && (my >= 350 && my <= 420)){
				startButtonClickHandler();
			}
			else if ((mx >= 370 && mx <= 600) && (my >= 250 && my <= 320)){
				highScoreButtonClickHandler();
			}
			else if ((mx >= 370 && mx <= 600) && (my >= 150 && my <= 220)){
				homeButtonClickHandler();
			}
			else if ((mx >= 370 && mx <= 600) && (my >= 50 && my <= 120)){
				exitButtonClickHandler();
			}
		}
		else if (currentPage == winner) {
			if ((mx >= 379 && mx <= 600) && (my >= 350 && my <= 420)){
				startButtonClickHandler();
			}
			else if ((mx >= 370 && mx <= 600) && (my >= 250 && my <= 320)){
				highScoreButtonClickHandler();
			}
			else if ((mx >= 370 && mx <= 600) && (my >= 150 && my <= 220)){
				homeButtonClickHandler();
			}
			else if ((mx >= 370 && mx <= 600) && (my >= 50 && my <= 120)){
				exitButtonClickHandler();
			}
		}
	}
}
void iKeyboard(unsigned char key) {
	if (currentPage == easy || currentPage == medium || currentPage == hard) {
		if (key == 'w') {
			spaceship_y += 10;
			if (spaceship_y > screenheight - spaceship_height) spaceship_y = screenheight - spaceship_height;
		}
		if (key == 's') {
			spaceship_y -= 10;
			if (spaceship_y < 0) spaceship_y = 0;
		}
		if (key == 'a') {
			spaceship_x -= 10;
			if (spaceship_x < 0) spaceship_x = 0;
		}
		if (key == 'd') {
			spaceship_x += 10;
			if (spaceship_x > screenwidth - spaceship_width) spaceship_x = screenwidth - spaceship_width;
		}
	}
	else if (currentPage == gameover || currentPage == winner) {
		if (newHighScore) {
			if (key == '\r') {
				playerNameInput[playerNameIndex] = '\0';
				highScoreEntry.playerName = playerNameInput;
				newHighScore = false;
				saveHighScore();
				playerNameIndex = 0;
				memset(playerNameInput, 0, sizeof(playerNameInput));
			}
			else if (key == '\b') {
				if (playerNameIndex > 0) {
					playerNameIndex--;
					playerNameInput[playerNameIndex] = '\0';
				}
			}
			else {
				playerNameInput[playerNameIndex] = key;
				playerNameIndex++;
				playerNameInput[playerNameIndex] = '\0';
			}
		}
	}
}
void iSpecialKeyboard(unsigned char key) {
	/*if (key == GLUT_KEY_LEFT) {
	if (musicOn) {
	musicOn = false;
	stopBackgroundMusic();
	}
	else {
	musicOn = true;
	playBackgroundMusic();
	}
	}*/
	if (key == GLUT_KEY_LEFT) {
		spaceship_x -= 10;
		if (spaceship_x < 0) spaceship_x = 0;
	}
	if (key == GLUT_KEY_RIGHT) {
		spaceship_x += 10;
		if (spaceship_x > screenwidth - spaceship_width) spaceship_x = screenwidth - spaceship_width;
	}
	if (key == GLUT_KEY_UP) {
		spaceship_y += 10;
		if (spaceship_y > screenheight - spaceship_height) spaceship_y = screenheight - spaceship_height;
	}
	if (key == GLUT_KEY_DOWN) {
		spaceship_y -= 10;
		if (spaceship_y < 0) spaceship_y = 0;
	}
}
void drawHomePage()
{
	iShowImage(0, 0, screenwidth, screenheight, home_img);
	drawButtons();
	drawOptions();
}
void drawDifficultyPage() {
	iShowImage(0, 0, screenwidth, screenheight, difficulty_img);
	drawDifficultyButtons();
	iShowImage(40, 660, 60, 30, back_button);
}
void drawAboutPage()
{
	iShowImage(0, 0, screenwidth, screenheight, credits_img);
	iShowImage(40, 660, 60, 30, back_button);
}
void drawInstructionPage()
{
	iShowImage(0, 0, screenwidth, screenheight, instruction_img);
	iShowImage(40, 660, 60, 30, back_button);
}
void drawAHighScorePage()
{
	iShowImage(0, 0, screenwidth, screenheight, highscore_img);
	iShowImage(40, 660, 60, 30, back_button);
	renderHighScore();
}
void drawGameOverPage()
{
	iShowImage(0, 0, screenwidth, screenheight, gameover_img);
	drawGameOverButtons();
	renderScore();
	renderHighScore();
}
void drawWinnerPage() {
	iShowImage(0, 0, screenwidth, screenheight, winner_img);
	drawGameOverButtons();
	renderScore();
	renderHighScore();
}
void saveHighScore() {
	ofstream file("highscore.txt");
	if (file.is_open()) {
		file << highScoreEntry.playerName << endl;
		file << highScoreEntry.score << endl;
		file.close();
	}
}
void loadHighScore() {
	ifstream file("highscore.txt");
	if (file.is_open()) {
		getline(file, highScoreEntry.playerName);
		file >> highScoreEntry.score;
		file.close();
	}
}
void musicClickHandler() {
	if (musicOn) {
		options[1].isClicked = true;
		musicOn = false;
		stopBackgroundMusic();
	}
	else {
		options[1].isClicked = false;
		musicOn = true;
		playBackgroundMusic();
	}
}
void soundClickHandler() {
	if (soundOn) {
		options[0].isClicked = true;
		soundOn = false;
	}
	else {
		options[0].isClicked = false;
		soundOn = true;
	}
}
void easyButtonClickHandler() {
	if (soundOn) {
		playButtonPressSound();
	}
	playerLives = MAX_LIVES;
	playerScore = 0;
	initializeAsteroids();
	initializeLasers();
	initializeExplosions();
	initializeEasyEnemySpaceship();
	initializeEnemyLasers();
	initializeBossEnemy();
	initializeBossLaser();
	initializeHealthItem();
	gameOverSoundPlayed = false;
	winnerSoundPlayed = false;
	currentPage = easy;
}
void mediumButtonClickHandler() {
	if (soundOn) {
		playButtonPressSound();
	}
	playerLives = MAX_LIVES;
	playerScore = 0;
	initializeAsteroids();
	initializeLasers();
	initializeExplosions();
	initializeMediumEnemySpaceship();
	initializeEnemyLasers();
	initializeBossEnemy();
	initializeBossLaser();
	initializeHealthItem();
	gameOverSoundPlayed = false;
	winnerSoundPlayed = false;
	currentPage = medium;
}
void hardButtonClickHandler() {
	if (soundOn) {
		playButtonPressSound();
	}
	playerLives = MAX_LIVES;
	playerScore = 0;
	initializeAsteroids();
	initializeLasers();
	initializeExplosions();
	initializeHardEnemySpaceship();
	initializeEnemyLasers();
	initializeBossEnemy();
	initializeBossLaser();
	initializeHealthItem();
	gameOverSoundPlayed = false;
	winnerSoundPlayed = false;
	currentPage = hard;
}
void startButtonClickHandler()
{
	if (soundOn) {
		playButtonPressSound();
	}
	currentPage = difficulty;
}
void aboutButtonClickHandler(){
	if (soundOn) {
		playButtonPressSound();
	}
	currentPage = about;
}
void instructionButtonClickHandler()
{
	if (soundOn) {
		playButtonPressSound();
	}
	currentPage = instruction;
}
void highScoreButtonClickHandler()
{
	if (soundOn) {
		playButtonPressSound();
	}
	currentPage = highscore;
}
void homeButtonClickHandler() {
	if (soundOn) {
		playButtonPressSound();
	}
	gameOverSoundPlayed = false;
	winnerSoundPlayed = false;
	currentPage = home;
}

void exitButtonClickHandler() {
	exit(0);
}
void backButtonClickHandler()
{
	currentPage = home;
}
void loadAllImages(){
	home_img = iLoadImage("./images/home.png");

	homebuttons[0] = { 130, 380, button_width, button_height, iLoadImage("./images/play_button_unhovered.png"), iLoadImage("./images/play_button_hovered.png"), false };
	homebuttons[1] = { 130, 280, button_width, button_height, iLoadImage("./images/credits_button_unhovered.png"), iLoadImage("./images/credits_button_hovered.png"), false };
	homebuttons[2] = { 600, 380, button_width, button_height, iLoadImage("./images/highscore_button_unhovered.png"), iLoadImage("./images/highscore_button_hovered.png"), false };
	homebuttons[3] = { 600, 280, button_width, button_height, iLoadImage("./images/instructions_button_unhovered.png"), iLoadImage("./images/instructions_button_hovered.png"), false };
	homebuttons[4] = { 365, 150, button_width, button_height, iLoadImage("./images/exit_button_unhovered.png"), iLoadImage("./images/exit_button_hovered.png"), false };

	options[0] = { 22, 23, 30, 30, iLoadImage("./images/sound_on.png"), iLoadImage("./images/sound_off.png"), false };
	options[1] = { 73, 23, 30, 30, iLoadImage("./images/music_on.png"), iLoadImage("./images/music_off.png"), false };

	gameOverButtons[0] = { 365, 350, button_width, button_height, iLoadImage("./images/playagain_button_unhovered.png"), iLoadImage("./images/playagain_button_hovered.png"), false };
	gameOverButtons[1] = { 365, 250, button_width, button_height, iLoadImage("./images/highscore_button_unhovered.png"), iLoadImage("./images/highscore_button_hovered.png"), false };
	gameOverButtons[2] = { 365, 150, button_width, button_height, iLoadImage("./images/home_button_unhovered.png"), iLoadImage("./images/home_button_hovered.png"), false };
	gameOverButtons[3] = { 365, 50, button_width, button_height, iLoadImage("./images/exit_button_unhovered.png"), iLoadImage("./images/exit_button_hovered.png"), false };

	difficultyButtons[0] = { 365, 350, button_width, button_height, iLoadImage("./images/easy_button_unhovered.png"), iLoadImage("./images/easy_button_hovered.png"), false };
	difficultyButtons[1] = { 365, 250, button_width, button_height, iLoadImage("./images/medium_button_unhovered.png"), iLoadImage("./images/medium_button_hovered.png"), false };
	difficultyButtons[2] = { 365, 150, button_width, button_height, iLoadImage("./images/hard_button_unhovered.png"), iLoadImage("./images/hard_button_hovered.png"), false };

	credits_img = iLoadImage("./images/credits.png");
	instruction_img = iLoadImage("./images/instructions.png");
	highscore_img = iLoadImage("./images/highscore.png");
	gameover_img = iLoadImage("./images/gameover.png");
	winner_img = iLoadImage("./images/winner.png");
	difficulty_img = iLoadImage("./images/difficulty.png");
	back_button = iLoadImage("./images/back_button.png");

	img = iLoadImage("./images/Background.png");
	a = iLoadImage("./images/player1.png");
	b = iLoadImage("./images/Laser.png");
	enemylaser = iLoadImage("./images/laser3.png");
	c = iLoadImage("./images/asteroid.png");
	healthimg = iLoadImage("./images/life_earn.png");
	explosionImage = iLoadImage("./images/blast.png");
	lifeImg = iLoadImage("./images/life_collect.png");
	enemySpaceshipImg = iLoadImage("./images/enemy_1.png");
	nameBoxImg = iLoadImage("./images/name_box.png");
	bossLifeImg[0] = iLoadImage("./images/boss_heart.png");
	bossLifeImg[1] = iLoadImage("./images/boss_heart.png");
	bossLifeImg[2] = iLoadImage("./images/boss_heart.png");
	bossLifeImg[3] = iLoadImage("./images/boss_heart.png");
	bossLifeImg[4] = iLoadImage("./images/boss_heart.png");
	bossLifeImg[5] = iLoadImage("./images/boss_heart.png");
	bossLifeImg[6] = iLoadImage("./images/boss_heart.png");
	bossimg = iLoadImage("./images/bosss.png");
	bosslaser = iLoadImage("./images/laser4.png");

}
void closeSDL() {
	Mix_FreeChunk(explosionSound);
	Mix_FreeChunk(gameOverSound);
	Mix_FreeChunk(winnerSound);
	Mix_FreeChunk(buttonPressSound);
	explosionSound = NULL;
	gameOverSound = NULL;
	winnerSound = NULL;
	buttonPressSound = NULL;

	Mix_FreeMusic(backgroundMusic);
	backgroundMusic = NULL;

	Mix_CloseAudio();
	Mix_Quit();

	SDL_Quit();
}
int main(int argc, char *argv[]) {
	iInitialize(screenwidth, screenheight, "Astro Clash");
	if (!initSDL()) {
		return -1;
	}

	loadAllImages();
	loadSoundEffects();
	loadHighScore();

	iSetTimer(10, updateBackground);
	iSetTimer(10, updateLasers);
	iSetTimer(10, updateAsteroids);
	iSetTimer(10, updateHealthItem);
	iSetTimer(200, autoFireLasers);
	iSetTimer(10, updateEnemyPositions);
	iSetTimer(400, enemyAutoFireLasers);
	iSetTimer(1500, autoFireBossEnemyLasers);

	playBackgroundMusic();
	iStart();
	closeSDL();
	return 0;
}
