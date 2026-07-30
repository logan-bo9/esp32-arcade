#include <TFT_eSPI.h>
#include <Preferences.h>

TFT_eSPI tft = TFT_eSPI();
Preferences prefs;

const int JOY_X = 1;
const int JOY_Y = 2;
const int BUTTON_START = 4;
const int BUTTON_PAUSE = 5;
const int BUTTON_BACK  = 6;
const int BUZZER = 14;

const int LOW_LIMIT = 900;
const int HIGH_LIMIT = 3200;

const int MENU_ITEMS = 3;
const char* menuItems[MENU_ITEMS] = {
  "Snake",
  "Flappy Bird",
  "Doodle Jump"
};

int selectedItem = 0;

enum GameState {
  STATE_MENU,
  STATE_DIFFICULTY,
  STATE_SNAKE,
  STATE_GAMEOVER,
  STATE_FLAPPY,
  STATE_FLAPPY_GAMEOVER,
  STATE_DOODLE,
  STATE_DOODLE_GAMEOVER,
  STATE_DOODLE_DIFFICULTY
};

GameState gameState = STATE_MENU;

unsigned long lastInputTime = 0;
const unsigned long INPUT_DELAY = 220;

// ---------- Snake ----------
const int GRID_SIZE = 15;
const int TOP_BAR_HEIGHT = 30;
const int GRID_W = 21;
const int GRID_H = 14;
const int FIELD_OFFSET_X = 2;
const int FIELD_OFFSET_Y = TOP_BAR_HEIGHT;
const int SNAKE_MAX = 60;

int snakeX[SNAKE_MAX];
int snakeY[SNAKE_MAX];
int snakeLength = 3;

int dirX = 1;
int dirY = 0;

int foodX, foodY;

unsigned long lastMoveTime = 0;
bool gameOver = false;

int score = 0;

uint16_t colorWall, colorSand1, colorSand2;

const int DIFF_COUNT = 3;
const char* diffNames[DIFF_COUNT] = { "Leicht", "Mittel", "Schwer" };
const unsigned long diffSpeed[DIFF_COUNT] = { 300, 200, 120 };
const char* diffKeys[DIFF_COUNT] = { "hs_easy", "hs_medium", "hs_hard" };

int difficulty = 0;
int diffSelect = 0;
unsigned long currentMoveInterval = 300;

int highscores[DIFF_COUNT];

const int GO_ITEMS = 3;
const char* goItems[GO_ITEMS] = {
  "Nochmal spielen",
  "Schwierigkeit wechseln",
  "Zurueck zum Menue"
};
int goSelect = 0;

// ---------- Flappy Bird ----------
const int SKY_COLOR = TFT_CYAN;
const int GROUND_HEIGHT = 20;
const int PLAY_TOP = TOP_BAR_HEIGHT;
const int PLAY_BOTTOM = 240 - GROUND_HEIGHT;

float birdY;
float birdVelocity;
const float GRAVITY = 0.55f;
const float JUMP_IMPULSE = -6.5f;
const int BIRD_X = 60;
const int BIRD_SIZE = 14;

const int PIPE_COUNT = 3;
float pipeX[PIPE_COUNT];
int pipeGapY[PIPE_COUNT];
bool pipeScored[PIPE_COUNT];

const int PIPE_WIDTH = 26;
const int PIPE_GAP = 70;
const float PIPE_SPEED = 2.2f;
const int PIPE_SPACING = 130;

int flappyScore = 0;
int flappyHighscore = 0;

bool flappyGameOver = false;
unsigned long lastFrameTime = 0;
const unsigned long FRAME_INTERVAL = 30;

const int FLAP_GO_ITEMS = 2;
const char* flapGoItems[FLAP_GO_ITEMS] = {
  "Nochmal spielen",
  "Zurueck zum Menue"
};
int flapGoSelect = 0;

void beep(int frequency, int duration) {
  tone(BUZZER, frequency, duration);
}

// ---------- Hauptmenue ----------
void drawMenu() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(TC_DATUM);

  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawString("ESP32 ARCADE", 160, 22, 4);

  tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
  tft.drawString("Joystick: Auswahl", 160, 55, 2);

  for (int i = 0; i < MENU_ITEMS; i++) {
    int y = 95 + i * 42;

    if (i == selectedItem) {
      tft.fillRoundRect(65, y - 7, 190, 32, 6, TFT_BLUE);
      tft.setTextColor(TFT_WHITE, TFT_BLUE);
      tft.drawString(menuItems[i], 160, y, 2);
    } else {
      tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
      tft.drawString(menuItems[i], 160, y, 2);
    }
  }

  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawString("Button 1: Start", 160, 218, 2);

  tft.setTextDatum(TL_DATUM);
}

// ---------- Snake: Schwierigkeit ----------
void drawDifficulty() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(TC_DATUM);

  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawString("SCHWIERIGKEIT", 160, 22, 4);

  for (int i = 0; i < DIFF_COUNT; i++) {
    int y = 75 + i * 40;
    String label = String(diffNames[i]) + "  (HI: " + String(highscores[i]) + ")";

    if (i == diffSelect) {
      tft.fillRoundRect(45, y - 7, 230, 32, 6, TFT_BLUE);
      tft.setTextColor(TFT_WHITE, TFT_BLUE);
      tft.drawString(label, 160, y, 2);
    } else {
      tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
      tft.drawString(label, 160, y, 2);
    }
  }

  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawString("Button 1: Bestaetigen", 160, 205, 2);
  tft.drawString("Button 3: Zurueck", 160, 225, 2);

  tft.setTextDatum(TL_DATUM);
}

// ---------- Snake: Spiel ----------
void resetSnake() {
  snakeLength = 3;
  dirX = 1;
  dirY = 0;
  gameOver = false;
  score = 0;

  int startX = GRID_W / 2;
  int startY = GRID_H / 2;

  for (int i = 0; i < snakeLength; i++) {
    snakeX[i] = startX - i;
    snakeY[i] = startY;
  }

  foodX = random(1, GRID_W - 1);
  foodY = random(1, GRID_H - 1);
}

void drawHUD() {
  tft.fillRect(0, 0, 320, TOP_BAR_HEIGHT, TFT_BLACK);

  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Score: " + String(score), 8, 4, 4);

  tft.setTextDatum(TR_DATUM);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawString("HI: " + String(highscores[difficulty]), 312, 4, 4);

  tft.setTextDatum(TL_DATUM);
}

void drawCell(int gx, int gy) {
  int px = FIELD_OFFSET_X + gx * GRID_SIZE;
  int py = FIELD_OFFSET_Y + gy * GRID_SIZE;

  bool isWall = (gx == 0 || gx == GRID_W - 1 || gy == 0 || gy == GRID_H - 1);

  if (isWall) {
    tft.fillRect(px, py, GRID_SIZE, GRID_SIZE, colorWall);
  } else {
    uint16_t baseColor = ((gx * 31 + gy * 17) % 5 == 0) ? colorSand2 : colorSand1;
    tft.fillRect(px, py, GRID_SIZE, GRID_SIZE, baseColor);

    if ((gx * 13 + gy * 7) % 6 == 0) {
      tft.fillRect(px + 4, py + 4, 3, 3, colorSand2);
    }
  }
}

void drawField() {
  for (int gx = 0; gx < GRID_W; gx++) {
    for (int gy = 0; gy < GRID_H; gy++) {
      drawCell(gx, gy);
    }
  }
}

void drawSnakeStart() {
  tft.fillScreen(TFT_BLACK);
  drawHUD();
  drawField();

  tft.fillRect(FIELD_OFFSET_X + foodX * GRID_SIZE, FIELD_OFFSET_Y + foodY * GRID_SIZE, GRID_SIZE, GRID_SIZE, TFT_RED);

  for (int i = 0; i < snakeLength; i++) {
    uint16_t color = (i == 0) ? TFT_GREEN : TFT_DARKGREEN;
    tft.fillRect(FIELD_OFFSET_X + snakeX[i] * GRID_SIZE, FIELD_OFFSET_Y + snakeY[i] * GRID_SIZE, GRID_SIZE, GRID_SIZE, color);
  }
}

void drawGameOver() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(TC_DATUM);

  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.drawString("GAME OVER", 160, 25, 4);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Score: " + String(score) + "   Highscore: " + String(highscores[difficulty]), 160, 62, 2);

  for (int i = 0; i < GO_ITEMS; i++) {
    int y = 100 + i * 40;

    if (i == goSelect) {
      tft.fillRoundRect(45, y - 7, 230, 32, 6, TFT_BLUE);
      tft.setTextColor(TFT_WHITE, TFT_BLUE);
      tft.drawString(goItems[i], 160, y, 2);
    } else {
      tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
      tft.drawString(goItems[i], 160, y, 2);
    }
  }

  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawString("Button 1: Bestaetigen", 160, 225, 2);

  tft.setTextDatum(TL_DATUM);
}

void moveSnake() {
  int newX = snakeX[0] + dirX;
  int newY = snakeY[0] + dirY;

  bool hitWall = (newX == 0 || newX == GRID_W - 1 || newY == 0 || newY == GRID_H - 1);
  bool hitSelf = false;

  for (int i = 0; i < snakeLength; i++) {
    if (snakeX[i] == newX && snakeY[i] == newY) hitSelf = true;
  }

  if (hitWall || hitSelf) {
    gameOver = true;
    beep(200, 300);

    if (score > highscores[difficulty]) {
      highscores[difficulty] = score;
      prefs.putInt(diffKeys[difficulty], highscores[difficulty]);
    }

    goSelect = 0;
    gameState = STATE_GAMEOVER;
    drawGameOver();
    return;
  }

  int tailX = snakeX[snakeLength - 1];
  int tailY = snakeY[snakeLength - 1];

  bool ateFood = (newX == foodX && newY == foodY);

  for (int i = snakeLength; i > 0; i--) {
    snakeX[i] = snakeX[i - 1];
    snakeY[i] = snakeY[i - 1];
  }

  snakeX[0] = newX;
  snakeY[0] = newY;

  tft.fillRect(FIELD_OFFSET_X + snakeX[1] * GRID_SIZE, FIELD_OFFSET_Y + snakeY[1] * GRID_SIZE, GRID_SIZE, GRID_SIZE, TFT_DARKGREEN);
  tft.fillRect(FIELD_OFFSET_X + snakeX[0] * GRID_SIZE, FIELD_OFFSET_Y + snakeY[0] * GRID_SIZE, GRID_SIZE, GRID_SIZE, TFT_GREEN);

  if (ateFood) {
    if (snakeLength < SNAKE_MAX - 1) snakeLength++;
    score += 10;
    beep(900, 60);

    drawHUD();

    foodX = random(1, GRID_W - 1);
    foodY = random(1, GRID_H - 1);
    tft.fillRect(FIELD_OFFSET_X + foodX * GRID_SIZE, FIELD_OFFSET_Y + foodY * GRID_SIZE, GRID_SIZE, GRID_SIZE, TFT_RED);
  } else {
    drawCell(tailX, tailY);
  }
}

void handleSnakeInput() {
  int x = analogRead(JOY_X);
  int y = analogRead(JOY_Y);

  unsigned long now = millis();

  if (now - lastInputTime > INPUT_DELAY) {
    if (x < LOW_LIMIT && dirY == 0) {
      dirX = 0; dirY = -1;
      lastInputTime = now;
    }
    else if (x > HIGH_LIMIT && dirY == 0) {
      dirX = 0; dirY = 1;
      lastInputTime = now;
    }
    else if (y < LOW_LIMIT && dirX == 0) {
      dirX = 1; dirY = 0;
      lastInputTime = now;
    }
    else if (y > HIGH_LIMIT && dirX == 0) {
      dirX = -1; dirY = 0;
      lastInputTime = now;
    }
  }
}

void startSnakeGame() {
  currentMoveInterval = diffSpeed[difficulty];
  resetSnake();
  gameState = STATE_SNAKE;
  drawSnakeStart();
}

// ---------- Flappy Bird ----------
void resetFlappy() {
  birdY = (PLAY_TOP + PLAY_BOTTOM) / 2.0f;
  birdVelocity = 0;
  flappyScore = 0;
  flappyGameOver = false;

  for (int i = 0; i < PIPE_COUNT; i++) {
    pipeX[i] = 320 + i * PIPE_SPACING;
    pipeGapY[i] = random(PLAY_TOP + 40, PLAY_BOTTOM - 40);
    pipeScored[i] = false;
  }
}

void drawFlappyStatic() {
  tft.fillScreen(TFT_BLACK);

  tft.fillRect(0, PLAY_TOP, 320, PLAY_BOTTOM - PLAY_TOP, SKY_COLOR);
  tft.fillRect(0, PLAY_BOTTOM, 320, GROUND_HEIGHT, tft.color565(180, 140, 60));

  tft.fillRect(0, 0, 320, TOP_BAR_HEIGHT, TFT_BLACK);
  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Score: " + String(flappyScore), 8, 4, 4);
  tft.setTextDatum(TR_DATUM);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawString("HI: " + String(flappyHighscore), 312, 4, 4);
  tft.setTextDatum(TL_DATUM);

  tft.fillRect(BIRD_X, (int)birdY, BIRD_SIZE, BIRD_SIZE, TFT_YELLOW);
}

void drawFlappyFrame(float oldBirdY) {
  int clearWidth = (int)PIPE_SPEED + 2;

  for (int i = 0; i < PIPE_COUNT; i++) {
    int px = (int)pipeX[i];
    if (px + PIPE_WIDTH + clearWidth < 0 || px > 320) continue;

    int gapTop = pipeGapY[i] - PIPE_GAP / 2;
    int gapBottom = pipeGapY[i] + PIPE_GAP / 2;

    tft.fillRect(px + PIPE_WIDTH, PLAY_TOP, clearWidth, PLAY_BOTTOM - PLAY_TOP, SKY_COLOR);

    tft.fillRect(px, PLAY_TOP, PIPE_WIDTH, gapTop - PLAY_TOP, TFT_DARKGREEN);
    tft.fillRect(px, gapBottom, PIPE_WIDTH, PLAY_BOTTOM - gapBottom, TFT_DARKGREEN);
  }

  tft.fillRect(BIRD_X, (int)oldBirdY, BIRD_SIZE, BIRD_SIZE, SKY_COLOR);
  tft.fillRect(BIRD_X, (int)birdY, BIRD_SIZE, BIRD_SIZE, TFT_YELLOW);

  tft.fillRect(0, 0, 100, TOP_BAR_HEIGHT, TFT_BLACK);
  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Score: " + String(flappyScore), 8, 4, 4);
}

void drawFlappyGameOver() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(TC_DATUM);

  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.drawString("GAME OVER", 160, 25, 4);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Score: " + String(flappyScore) + "   Highscore: " + String(flappyHighscore), 160, 62, 2);

  for (int i = 0; i < FLAP_GO_ITEMS; i++) {
    int y = 110 + i * 40;

    if (i == flapGoSelect) {
      tft.fillRoundRect(45, y - 7, 230, 32, 6, TFT_BLUE);
      tft.setTextColor(TFT_WHITE, TFT_BLUE);
      tft.drawString(flapGoItems[i], 160, y, 2);
    } else {
      tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
      tft.drawString(flapGoItems[i], 160, y, 2);
    }
  }

  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawString("Button 1: Bestaetigen", 160, 210, 2);

  tft.setTextDatum(TL_DATUM);
}

void startFlappyGame() {
  resetFlappy();
  gameState = STATE_FLAPPY;
  drawFlappyStatic();
  lastFrameTime = millis();
}

void updateFlappy() {
  float oldBirdY = birdY;

  birdVelocity += GRAVITY;
  birdY += birdVelocity;

  for (int i = 0; i < PIPE_COUNT; i++) {
    pipeX[i] -= PIPE_SPEED;

    if (pipeX[i] + PIPE_WIDTH < 0) {
      pipeX[i] += PIPE_COUNT * PIPE_SPACING;
      pipeGapY[i] = random(PLAY_TOP + 40, PLAY_BOTTOM - 40);
      pipeScored[i] = false;
    }

    if (!pipeScored[i] && pipeX[i] + PIPE_WIDTH < BIRD_X) {
      pipeScored[i] = true;
      flappyScore++;
      beep(900, 50);
    }
  }

  if (birdY < PLAY_TOP || birdY + BIRD_SIZE > PLAY_BOTTOM) {
    flappyGameOver = true;
  }

  for (int i = 0; i < PIPE_COUNT; i++) {
    int px = (int)pipeX[i];
    bool overlapX = (BIRD_X + BIRD_SIZE > px && BIRD_X < px + PIPE_WIDTH);

    if (overlapX) {
      int gapTop = pipeGapY[i] - PIPE_GAP / 2;
      int gapBottom = pipeGapY[i] + PIPE_GAP / 2;

      if (birdY < gapTop || birdY + BIRD_SIZE > gapBottom) {
        flappyGameOver = true;
      }
    }
  }

  if (flappyGameOver) {
    beep(200, 300);

    if (flappyScore > flappyHighscore) {
      flappyHighscore = flappyScore;
      prefs.putInt("hs_flappy", flappyHighscore);
    }

    flapGoSelect = 0;
    gameState = STATE_FLAPPY_GAMEOVER;
    drawFlappyGameOver();
    return;
  }

  drawFlappyFrame(oldBirdY);
}
// ---------- Doodle Jump ----------
const int PLAT_COUNT = 7;  // maximale Array-Groesse, groesster Wert aller Schwierigkeiten

const int DJ_DIFF_COUNT = 3;
const char* djDiffNames[DJ_DIFF_COUNT] = { "Leicht", "Mittel", "Schwer" };
const float djDiffSpeeds[DJ_DIFF_COUNT] = { 1.0f, 1.6f, 2.4f };
const int djDiffCounts[DJ_DIFF_COUNT] = { 7, 6, 5 };
const char* djDiffKeys[DJ_DIFF_COUNT] = { "hs_dj_easy", "hs_dj_med", "hs_dj_hard" };

int djDifficulty = 0;
int djDiffSelect = 0;
int djPlatCount = 7;
float djPlatformSpeed = 1.0f;

int djHighscores[DJ_DIFF_COUNT];
const int PLAT_WIDTH = 50;
const int PLAT_HEIGHT = 14;
const int PLAYER_SIZE = 16;

const float DJ_GRAVITY = 0.35f;
const float JUMP_VELOCITY = -8.0f;
const float DJ_MOVE_SPEED = 3.0f;
const float PLATFORM_SPEED = 1.2f;

const int DJ_PLAY_TOP = TOP_BAR_HEIGHT;
const int DJ_PLAY_BOTTOM = 240;

float platX[PLAT_COUNT];
float platY[PLAT_COUNT];

float playerX, playerY;
float playerVX, playerVY;
bool onGround;

int djScore = 0;
int djHighscore = 0;
bool djGameOver = false;

const int DJ_GO_ITEMS = 2;
const char* djGoItems[DJ_GO_ITEMS] = {
  "Nochmal spielen",
  "Zurueck zum Menue"
};
int djGoSelect = 0;

void resetDoodle() {
  playerX = 160 - PLAYER_SIZE / 2;
  playerY = DJ_PLAY_BOTTOM - 40;
  playerVX = 0;
  playerVY = 0;
  onGround = true;
  djScore = 0;
  djGameOver = false;

  djPlatformSpeed = djDiffSpeeds[djDifficulty];
  djPlatCount = djDiffCounts[djDifficulty];

  int spacing = (DJ_PLAY_BOTTOM - DJ_PLAY_TOP) / djPlatCount;

  for (int i = 0; i < djPlatCount; i++) {
    platX[i] = random(10, 320 - PLAT_WIDTH - 10);
    platY[i] = DJ_PLAY_BOTTOM - i * spacing - 10;
  }

  platX[0] = playerX - (PLAT_WIDTH - PLAYER_SIZE) / 2;
  platY[0] = playerY + PLAYER_SIZE;
}

void drawCloud(int px, int py) {
  tft.fillCircle(px + 12, py + 7, 8, TFT_WHITE);
  tft.fillCircle(px + 25, py + 4, 10, TFT_WHITE);
  tft.fillCircle(px + 38, py + 7, 8, TFT_WHITE);
  tft.fillRect(px + 6, py + 7, PLAT_WIDTH - 12, 6, TFT_WHITE);
}

void eraseCloud(int px, int py) {
  tft.fillRect(px - 4, py - 12, PLAT_WIDTH + 8, PLAT_HEIGHT + 20, TFT_CYAN);
}

void drawDoodleStatic() {
  tft.fillScreen(TFT_BLACK);

  tft.fillRect(0, DJ_PLAY_TOP, 320, DJ_PLAY_BOTTOM - DJ_PLAY_TOP, TFT_CYAN);

  tft.fillRect(0, 0, 320, TOP_BAR_HEIGHT, TFT_BLACK);
  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Score: " + String(djScore), 8, 4, 4);
  tft.setTextDatum(TR_DATUM);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawString("HI: " + String(djHighscores[djDifficulty]), 312, 4, 4);
  tft.setTextDatum(TL_DATUM);

  for (int i = 0; i < djPlatCount; i++) {
    drawCloud((int)platX[i], (int)platY[i]);
  }

  tft.fillRect((int)playerX, (int)playerY, PLAYER_SIZE, PLAYER_SIZE, TFT_ORANGE);
}

void drawDoodleDifficulty() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(TC_DATUM);

  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawString("SCHWIERIGKEIT", 160, 22, 4);

  for (int i = 0; i < DJ_DIFF_COUNT; i++) {
    int y = 75 + i * 40;
    String label = String(djDiffNames[i]) + "  (HI: " + String(djHighscores[i]) + ")";

    if (i == djDiffSelect) {
      tft.fillRoundRect(45, y - 7, 230, 32, 6, TFT_BLUE);
      tft.setTextColor(TFT_WHITE, TFT_BLUE);
      tft.drawString(label, 160, y, 2);
    } else {
      tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
      tft.drawString(label, 160, y, 2);
    }
  }

  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawString("Button 1: Bestaetigen", 160, 205, 2);
  tft.drawString("Button 3: Zurueck", 160, 225, 2);

  tft.setTextDatum(TL_DATUM);
}

void startDoodleGame() {
  resetDoodle();
  gameState = STATE_DOODLE;
  drawDoodleStatic();
  lastFrameTime = millis();
}

void drawDoodleGameOver() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(TC_DATUM);

  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.drawString("GAME OVER", 160, 25, 4);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Score: " + String(djScore) + "   Highscore: " + String(djHighscores[djDifficulty]), 160, 62, 2);

  for (int i = 0; i < DJ_GO_ITEMS; i++) {
    int y = 110 + i * 40;

    if (i == djGoSelect) {
      tft.fillRoundRect(45, y - 7, 230, 32, 6, TFT_BLUE);
      tft.setTextColor(TFT_WHITE, TFT_BLUE);
      tft.drawString(djGoItems[i], 160, y, 2);
    } else {
      tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
      tft.drawString(djGoItems[i], 160, y, 2);
    }
  }

  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawString("Button 1: Bestaetigen", 160, 205, 2);

  tft.setTextDatum(TL_DATUM);
}

void updateDoodle() {
  float oldPlayerX = playerX;
  float oldPlayerY = playerY;

  int y = analogRead(JOY_Y);
  if (y < LOW_LIMIT) playerVX = DJ_MOVE_SPEED;
  else if (y > HIGH_LIMIT) playerVX = -DJ_MOVE_SPEED;
  else playerVX = 0;

  playerX += playerVX;
  if (playerX < -PLAYER_SIZE) playerX = 320;
  if (playerX > 320) playerX = -PLAYER_SIZE;

  playerVY += DJ_GRAVITY;
  float newPlayerY = playerY + playerVY;

  onGround = false;

  for (int i = 0; i < djPlatCount; i++) {
    eraseCloud((int)platX[i], (int)platY[i]);
    platY[i] += djPlatformSpeed;

    if (platY[i] > DJ_PLAY_BOTTOM) {
      platY[i] = DJ_PLAY_TOP - 2;
      platX[i] = random(10, 320 - PLAT_WIDTH - 10);
    }

    if (playerVY > 0) {
      bool overlapX = (playerX + PLAYER_SIZE > platX[i] && playerX < platX[i] + PLAT_WIDTH);
      bool crossesTop = (playerY + PLAYER_SIZE <= platY[i] && newPlayerY + PLAYER_SIZE >= platY[i]);

      if (overlapX && crossesTop) {
        newPlayerY = platY[i] - PLAYER_SIZE;
        playerVY = JUMP_VELOCITY;
        onGround = true;
        beep(700, 30);
        djScore += 5;
      }
    }

    drawCloud((int)platX[i], (int)platY[i]);
  }

  playerY = newPlayerY;
  if (playerY < DJ_PLAY_TOP) {
    playerY = DJ_PLAY_TOP;
  }

  tft.fillRect((int)oldPlayerX, (int)oldPlayerY, PLAYER_SIZE, PLAYER_SIZE, TFT_CYAN);
  tft.fillRect((int)playerX, (int)playerY, PLAYER_SIZE, PLAYER_SIZE, TFT_ORANGE);

  tft.fillRect(0, 0, 320, TOP_BAR_HEIGHT, TFT_BLACK);
  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Score: " + String(djScore), 8, 4, 4);
  tft.setTextDatum(TR_DATUM);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawString("HI: " + String(djHighscores[djDifficulty]), 312, 4, 4);
  tft.setTextDatum(TL_DATUM);

  if (playerY > DJ_PLAY_BOTTOM) {
    djGameOver = true;
    beep(200, 300);

    if (djScore > djHighscores[djDifficulty]) {
      djHighscores[djDifficulty] = djScore;
      prefs.putInt(djDiffKeys[djDifficulty], djHighscores[djDifficulty]);
    }

    djGoSelect = 0;
    gameState = STATE_DOODLE_GAMEOVER;
    drawDoodleGameOver();
  }
}

// ---------- Setup / Loop ----------
void setup() {
  pinMode(BUTTON_START, INPUT_PULLUP);
  pinMode(BUTTON_PAUSE, INPUT_PULLUP);
  pinMode(BUTTON_BACK, INPUT_PULLUP);

  analogReadResolution(12);
  randomSeed(analogRead(JOY_X) + millis());

  prefs.begin("arcade", false);
  for (int i = 0; i < DIFF_COUNT; i++) {
    highscores[i] = prefs.getInt(diffKeys[i], 0);
  }
  flappyHighscore = prefs.getInt("hs_flappy", 0);
  for (int i = 0; i < DJ_DIFF_COUNT; i++) {
  djHighscores[i] = prefs.getInt(djDiffKeys[i], 0);
  }
  tft.init();
  tft.setRotation(3);

  colorWall  = tft.color565(120, 72, 30);
  colorSand1 = tft.color565(237, 201, 175);
  colorSand2 = tft.color565(222, 184, 135);

  drawMenu();
}

void loop() {
  unsigned long now = millis();

  if (gameState == STATE_MENU) {
    int x = analogRead(JOY_X);

    if (now - lastInputTime > INPUT_DELAY) {
      if (x < LOW_LIMIT) {
        selectedItem--;
        if (selectedItem < 0) selectedItem = MENU_ITEMS - 1;
        beep(600, 45);
        drawMenu();
        lastInputTime = now;
      }
      else if (x > HIGH_LIMIT) {
        selectedItem++;
        if (selectedItem >= MENU_ITEMS) selectedItem = 0;
        beep(850, 45);
        drawMenu();
        lastInputTime = now;
      }
    }

    if (digitalRead(BUTTON_START) == LOW && now - lastInputTime > INPUT_DELAY) {
      beep(1100, 90);
      lastInputTime = now;

      if (selectedItem == 0) {
        diffSelect = difficulty;
        gameState = STATE_DIFFICULTY;
        drawDifficulty();
      }
      else if (selectedItem == 1) {
        startFlappyGame();
      }
      else if (selectedItem == 2) {
      djDiffSelect = djDifficulty;
      gameState = STATE_DOODLE_DIFFICULTY;
      drawDoodleDifficulty();
      }
    }
  }

  else if (gameState == STATE_DIFFICULTY) {
    int x = analogRead(JOY_X);

    if (now - lastInputTime > INPUT_DELAY) {
      if (x < LOW_LIMIT) {
        diffSelect--;
        if (diffSelect < 0) diffSelect = DIFF_COUNT - 1;
        beep(600, 45);
        drawDifficulty();
        lastInputTime = now;
      }
      else if (x > HIGH_LIMIT) {
        diffSelect++;
        if (diffSelect >= DIFF_COUNT) diffSelect = 0;
        beep(850, 45);
        drawDifficulty();
        lastInputTime = now;
      }
    }

    if (digitalRead(BUTTON_START) == LOW && now - lastInputTime > INPUT_DELAY) {
      beep(1100, 90);
      lastInputTime = now;
      difficulty = diffSelect;
      startSnakeGame();
    }

    if (digitalRead(BUTTON_BACK) == LOW && now - lastInputTime > INPUT_DELAY) {
      beep(500, 90);
      lastInputTime = now;
      gameState = STATE_MENU;
      drawMenu();
    }
  }

  else if (gameState == STATE_SNAKE) {
    if (digitalRead(BUTTON_BACK) == LOW && now - lastInputTime > INPUT_DELAY) {
      beep(500, 90);
      lastInputTime = now;
      gameState = STATE_MENU;
      drawMenu();
      return;
    }

    if (!gameOver) {
      handleSnakeInput();

      if (now - lastMoveTime > currentMoveInterval) {
        moveSnake();
        lastMoveTime = now;
      }
    }
  }

  else if (gameState == STATE_GAMEOVER) {
    int x = analogRead(JOY_X);

    if (now - lastInputTime > INPUT_DELAY) {
      if (x < LOW_LIMIT) {
        goSelect--;
        if (goSelect < 0) goSelect = GO_ITEMS - 1;
        beep(600, 45);
        drawGameOver();
        lastInputTime = now;
      }
      else if (x > HIGH_LIMIT) {
        goSelect++;
        if (goSelect >= GO_ITEMS) goSelect = 0;
        beep(850, 45);
        drawGameOver();
        lastInputTime = now;
      }
    }

    if (digitalRead(BUTTON_START) == LOW && now - lastInputTime > INPUT_DELAY) {
      beep(1100, 90);
      lastInputTime = now;

      switch (goSelect) {
        case 0: startSnakeGame(); break;
        case 1:
          diffSelect = difficulty;
          gameState = STATE_DIFFICULTY;
          drawDifficulty();
          break;
        case 2:
          gameState = STATE_MENU;
          drawMenu();
          break;
      }
    }
  }

  else if (gameState == STATE_FLAPPY) {
    if (digitalRead(BUTTON_BACK) == LOW && now - lastInputTime > INPUT_DELAY) {
      beep(500, 90);
      lastInputTime = now;
      gameState = STATE_MENU;
      drawMenu();
      return;
    }

    if (digitalRead(BUTTON_START) == LOW && now - lastInputTime > 150) {
      birdVelocity = JUMP_IMPULSE;
      beep(700, 40);
      lastInputTime = now;
    }

    if (now - lastFrameTime > FRAME_INTERVAL) {
      updateFlappy();
      lastFrameTime = now;
    }
  }

  else if (gameState == STATE_FLAPPY_GAMEOVER) {
    int x = analogRead(JOY_X);

    if (now - lastInputTime > INPUT_DELAY) {
      if (x < LOW_LIMIT) {
        flapGoSelect--;
        if (flapGoSelect < 0) flapGoSelect = FLAP_GO_ITEMS - 1;
        beep(600, 45);
        drawFlappyGameOver();
        lastInputTime = now;
      }
      else if (x > HIGH_LIMIT) {
        flapGoSelect++;
        if (flapGoSelect >= FLAP_GO_ITEMS) flapGoSelect = 0;
        beep(850, 45);
        drawFlappyGameOver();
        lastInputTime = now;
      }
    }

    if (digitalRead(BUTTON_START) == LOW && now - lastInputTime > INPUT_DELAY) {
      beep(1100, 90);
      lastInputTime = now;

      switch (flapGoSelect) {
        case 0: startFlappyGame(); break;
        case 1:
          gameState = STATE_MENU;
          drawMenu();
          break;
      }
    }
  }
  else if (gameState == STATE_DOODLE) {
  if (digitalRead(BUTTON_BACK) == LOW && now - lastInputTime > INPUT_DELAY) {
    beep(500, 90);
    lastInputTime = now;
    gameState = STATE_MENU;
    drawMenu();
    return;
  }

  if (now - lastFrameTime > FRAME_INTERVAL) {
    updateDoodle();
    lastFrameTime = now;
  }
}

  else if (gameState == STATE_DOODLE_GAMEOVER) {
  int x = analogRead(JOY_X);

  if (now - lastInputTime > INPUT_DELAY) {
    if (x < LOW_LIMIT) {
      djGoSelect--;
      if (djGoSelect < 0) djGoSelect = DJ_GO_ITEMS - 1;
      beep(600, 45);
      drawDoodleGameOver();
      lastInputTime = now;
    }
    else if (x > HIGH_LIMIT) {
      djGoSelect++;
      if (djGoSelect >= DJ_GO_ITEMS) djGoSelect = 0;
      beep(850, 45);
      drawDoodleGameOver();
      lastInputTime = now;
    }
  }

  if (digitalRead(BUTTON_START) == LOW && now - lastInputTime > INPUT_DELAY) {
    beep(1100, 90);
    lastInputTime = now;

    switch (djGoSelect) {
      case 0: startDoodleGame(); break;
      case 1:
        gameState = STATE_MENU;
        drawMenu();
        break;
    }
  }
  }
  else if (gameState == STATE_DOODLE_DIFFICULTY) {
  int x = analogRead(JOY_X);

  if (now - lastInputTime > INPUT_DELAY) {
    if (x < LOW_LIMIT) {
      djDiffSelect--;
      if (djDiffSelect < 0) djDiffSelect = DJ_DIFF_COUNT - 1;
      beep(600, 45);
      drawDoodleDifficulty();
      lastInputTime = now;
    }
    else if (x > HIGH_LIMIT) {
      djDiffSelect++;
      if (djDiffSelect >= DJ_DIFF_COUNT) djDiffSelect = 0;
      beep(850, 45);
      drawDoodleDifficulty();
      lastInputTime = now;
    }
  }

  if (digitalRead(BUTTON_START) == LOW && now - lastInputTime > INPUT_DELAY) {
    beep(1100, 90);
    lastInputTime = now;
    djDifficulty = djDiffSelect;
    startDoodleGame();
  }

  if (digitalRead(BUTTON_BACK) == LOW && now - lastInputTime > INPUT_DELAY) {
    beep(500, 90);
    lastInputTime = now;
    gameState = STATE_MENU;
    drawMenu();
  }
  }
}
