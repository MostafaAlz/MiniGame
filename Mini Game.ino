#include <LedControl.h>
#include <TM1637Display.h>


const int DIN = 8;
const int CS = 7;
const int CLK = 13;
const int JOY_X_PIN = A0;
const int JOY_Y_PIN = A1;
const int JOY_SW_PIN = 9;


LedControl lc = LedControl(DIN, CLK, CS, 1);
TM1637Display display(5, 4); // CLK to PWM pin 5, DIO to PWM pin 4


int snakeX, snakeY;
int xDirection = 0;
int yDirection = 0;
bool isGameOver = false;


unsigned long lastMoveTime = 0;
const int initialMoveInterval = 200; // Adjust this value for snake's initial movement speed
int moveInterval = initialMoveInterval;


unsigned long gameStartTime = 0;
unsigned long gameTime = 0;
bool isGameStarted = false;


void setup() {
  pinMode(JOY_SW_PIN, INPUT_PULLUP);
  lc.shutdown(0, false);
  lc.setIntensity(0, 15);
  lc.clearDisplay(0);
  display.setBrightness(7); // Set brightness (0-7)
  resetSnake();
  drawSnake();
  drawBorder();
  gameStartTime = millis();
}

void resetSnake() {
  snakeX = 3; // Start at the center of the matrix
  snakeY = 3; // Start at the center of the matrix
  xDirection = 0;
  yDirection = 0;
}

void drawSnake() {
  lc.setLed(0, snakeX, snakeY, true);
}

void clearSnake() {
  lc.setLed(0, snakeX, snakeY, false);
}

void checkCollisions() {
  if (snakeX == 0 || snakeX == 7 || snakeY == 0 || snakeY == 7) {
    isGameOver = true; // Border collision
    lc.clearDisplay(0); // Clear the matrix when the game is over
    delay(1000); // Pause for 1 second before restarting
    isGameOver = false;
    resetSnake();
    drawSnake();
    drawBorder();
    moveInterval = initialMoveInterval; // Reset the snake's speed
    gameStartTime = millis(); // Reset the game start time
    gameTime = 0; // Reset the game time
    isGameStarted = false; // Reset the game start flag
  }
}

void moveSnake() {
  snakeX += xDirection;
  snakeY += yDirection;
}

void readJoystick() {
  int xVal = analogRead(JOY_X_PIN);
  int yVal = analogRead(JOY_Y_PIN);


  if (!isGameStarted) {
    if (xVal < 300 || xVal > 700 || yVal < 300 || yVal > 700) {
      isGameStarted = true;
      gameStartTime = millis();
    }
  }

  if (xVal < 300 && xDirection == 0) {
    xDirection = -1;
    yDirection = 0;
  } else if (xVal > 700 && xDirection == 0) {
    xDirection = 1;
    yDirection = 0;
  } else if (yVal < 300 && yDirection == 0) {
    xDirection = 0;
    yDirection = -1;
  } else if (yVal > 700 && yDirection == 0) {
    xDirection = 0;
    yDirection = 1;
  }
}

void drawBorder() {
  for (int i = 0; i < 8; i++) {
    lc.setLed(0, i, 0, true); // Top border
    lc.setLed(0, i, 7, true); // Bottom border
    lc.setLed(0, 0, i, true); // Left border
    lc.setLed(0, 7, i, true); // Right border
  }
}

void loop() {
  if (!isGameOver) {
    unsigned long currentTime = millis();


    if (currentTime - lastMoveTime >= moveInterval) {
      lastMoveTime = currentTime;

      clearSnake();
      readJoystick();
      moveSnake();
      checkCollisions();
      drawSnake();


      if (isGameStarted) {
        gameTime = millis() - gameStartTime; // Update game time


        // Display game time in minutes and seconds
        int seconds = gameTime / 1000;
        int minutes = seconds / 60;
        display.showNumberDecEx(minutes * 100 + (seconds % 60), (1 << 1) | (1 << 3), true); // Display game time in minutes and seconds with decimal points
      }
    }
  }
}