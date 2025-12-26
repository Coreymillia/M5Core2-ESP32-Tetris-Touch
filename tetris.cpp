// tetris.cpp - Enhanced Tetris for M5Core2 with touch controls
#include "tetris.h"
#include "display.h"
#include "input.h"

TetrisGame tetrisGame;

void TetrisGame::init() {
  // Initialize field
  for (int y = 0; y < FIELD_HEIGHT; y++) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
      field[y][x] = 0;
    }
  }
  
  // Tetris piece definitions
  int tempPieces[7][4][2][4] = {
    // O piece - square
    {{{0,1,0,1},{0,0,1,1}},{{0,1,0,1},{0,0,1,1}},{{0,1,0,1},{0,0,1,1}},{{0,1,0,1},{0,0,1,1}}},
    // I piece - line
    {{{0,0,0,0},{-1,0,1,2}},{{-1,0,1,2},{0,0,0,0}},{{0,0,0,0},{-1,0,1,2}},{{-1,0,1,2},{0,0,0,0}}},
    // T piece
    {{{0,0,0,1},{-1,0,1,0}},{{1,0,-1,0},{0,0,0,-1}},{{0,0,0,-1},{-1,0,1,0}},{{1,0,-1,0},{0,0,0,1}}},
    // S piece
    {{{0,-1,0,1},{0,0,1,1}},{{0,1,1,0},{0,0,-1,1}},{{0,-1,0,1},{0,0,1,1}},{{0,1,1,0},{0,0,-1,1}}},
    // Z piece
    {{{0,-1,0,1},{0,0,-1,-1}},{{0,0,1,1},{0,-1,0,1}},{{0,-1,0,1},{0,0,-1,-1}},{{0,0,1,1},{0,-1,0,1}}},
    // J piece
    {{{1,0,-1,1},{0,0,0,-1}},{{0,-1,0,0},{0,0,1,2}},{{0,1,2,0},{0,0,0,1}},{{1,0,0,0},{1,1,0,-1}}},
    // L piece
    {{{0,0,1,2},{-1,0,0,0}},{{-1,0,0,0},{1,1,0,-1}},{{1,1,0,-1},{1,0,0,0}},{{1,0,0,0},{-1,-1,0,1}}}
  };
  memcpy(pieces, tempPieces, sizeof(pieces));
  
  // Colors for pieces
  pieceColors[0] = COLOR_YELLOW;
  pieceColors[1] = COLOR_CYAN;
  pieceColors[2] = COLOR_PURPLE;
  pieceColors[3] = COLOR_GREEN;
  pieceColors[4] = COLOR_RED;
  pieceColors[5] = COLOR_BLUE;
  pieceColors[6] = COLOR_ORANGE;
  
  score = 0;
  level = 1;
  linesCleared = 0;
  dropSpeed = 500;
  gameOver = false;
  needsRedraw = true;  // Force border redraw on init
  lastDropTime = 0;
  lockDelayActive = false;
  
  // Modern features
  heldPiece = -1;
  nextPiece = random(0, 7);
  canHold = true;
  
  newPiece(false);
}

void TetrisGame::update() {
  handleInput();
  
  if (millis() - lastDropTime > dropSpeed) {
    posY++;
    if (test(posY, posX, currentPiece, currentRot)) {
      posY--;
      // Start lock delay when piece hits bottom
      if (!lockDelayActive) {
        lockDelayActive = true;
        lockDelayStart = millis();
      }
      
      // Check if lock delay has expired (500ms)
      if (millis() - lockDelayStart >= 500) {
        placePiece();
        clearLines();
        newPiece(true);
        if (test(posY, posX, currentPiece, currentRot)) {
          gameOver = true;
        }
      }
    } else {
      lockDelayActive = false;
    }
    lastDropTime = millis();
  }
}

void TetrisGame::draw() {
  static int lastScore = -1;
  
  // Check if we need to redraw the screen and border
  if (needsRedraw) {
    clearDisplay();
    
    // Draw thick, colorful border around game field
    // Outer border (thick blue)
    M5.Lcd.fillRect(OFFSET_X-4, OFFSET_Y-4, FIELD_WIDTH*BLOCK_SIZE+8, 4, COLOR_BLUE);  // Top
    M5.Lcd.fillRect(OFFSET_X-4, OFFSET_Y+FIELD_HEIGHT*BLOCK_SIZE, FIELD_WIDTH*BLOCK_SIZE+8, 4, COLOR_BLUE);  // Bottom  
    M5.Lcd.fillRect(OFFSET_X-4, OFFSET_Y-4, 4, FIELD_HEIGHT*BLOCK_SIZE+8, COLOR_BLUE);  // Left
    M5.Lcd.fillRect(OFFSET_X+FIELD_WIDTH*BLOCK_SIZE, OFFSET_Y-4, 4, FIELD_HEIGHT*BLOCK_SIZE+8, COLOR_BLUE);  // Right
    
    // Inner border (cyan accent)
    M5.Lcd.fillRect(OFFSET_X-2, OFFSET_Y-2, FIELD_WIDTH*BLOCK_SIZE+4, 2, COLOR_CYAN);  // Top
    M5.Lcd.fillRect(OFFSET_X-2, OFFSET_Y+FIELD_HEIGHT*BLOCK_SIZE, FIELD_WIDTH*BLOCK_SIZE+4, 2, COLOR_CYAN);  // Bottom
    M5.Lcd.fillRect(OFFSET_X-2, OFFSET_Y-2, 2, FIELD_HEIGHT*BLOCK_SIZE+4, COLOR_CYAN);  // Left  
    M5.Lcd.fillRect(OFFSET_X+FIELD_WIDTH*BLOCK_SIZE, OFFSET_Y-2, 2, FIELD_HEIGHT*BLOCK_SIZE+4, COLOR_CYAN);  // Right
    
    needsRedraw = false;
  }
  
  // Draw field blocks
  for (int y = 0; y < FIELD_HEIGHT; y++) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
      int px = OFFSET_X + x * BLOCK_SIZE;
      int py = OFFSET_Y + y * BLOCK_SIZE;
      if (field[y][x] > 0) {
        M5.Lcd.fillRect(px, py, BLOCK_SIZE-1, BLOCK_SIZE-1, pieceColors[field[y][x]-1]);
        M5.Lcd.drawRect(px, py, BLOCK_SIZE-1, BLOCK_SIZE-1, COLOR_WHITE);
      } else {
        M5.Lcd.fillRect(px, py, BLOCK_SIZE-1, BLOCK_SIZE-1, COLOR_BLACK);
      }
    }
  }
  
  // Draw ghost piece
  drawGhostPiece();
  
  // Draw current piece
  for (int i = 0; i < 4; i++) {
    int x = posX + pieces[currentPiece][currentRot][1][i];
    int y = posY + pieces[currentPiece][currentRot][0][i];
    if (y >= 0 && y < FIELD_HEIGHT && x >= 0 && x < FIELD_WIDTH) {
      int px = OFFSET_X + x * BLOCK_SIZE;
      int py = OFFSET_Y + y * BLOCK_SIZE;
      M5.Lcd.fillRect(px, py, BLOCK_SIZE-1, BLOCK_SIZE-1, pieceColors[currentPiece]);
      M5.Lcd.drawRect(px, py, BLOCK_SIZE-1, BLOCK_SIZE-1, COLOR_WHITE);
    }
  }
  
  // Draw UI elements
  drawHoldPiece();
  drawNextPiece();
  // Removed drawHoldButton() - hold still works via touch zones
  
  // Update score when it changes
  if (score != lastScore) {
    M5.Lcd.fillRect(180, 5, 80, 15, COLOR_BLACK); // Clear old score
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(COLOR_WHITE);
    M5.Lcd.setCursor(180, 5);
    M5.Lcd.print("Score: ");
    M5.Lcd.print(score);
    lastScore = score;
  }
  
  // Control text removed for cleaner look
}

void TetrisGame::handleInput() {
  static unsigned long lastMove = 0;
  static unsigned long lastLeftRight = 0;
  static bool buttonHeld = false;
  
  // Left movement
  if (buttons.left && millis() - lastLeftRight > 80) {
    posX--;
    if (test(posY, posX, currentPiece, currentRot)) posX++;
    else if (lockDelayActive) lockDelayStart = millis(); // Reset lock delay
    lastLeftRight = millis();
  }
  // Right movement
  else if (buttons.right && millis() - lastLeftRight > 80) {
    posX++;
    if (test(posY, posX, currentPiece, currentRot)) posX--;
    else if (lockDelayActive) lockDelayStart = millis();
    lastLeftRight = millis();
  }
  
  if (millis() - lastMove < 100) return;
  
  // Hard drop (swipe down gesture)
  if (buttons.upPressed && !buttonHeld) {
    while(!test(posY + 1, posX, currentPiece, currentRot)) {
      posY++;
      score += 2; // Award more points for hard drop
    }
    lockDelayActive = false;
    lastDropTime = 0; // Force immediate lock
    lastMove = millis();
    buttonHeld = true;
  }
  // Soft drop (touch bottom area)
  else if (buttons.down) {
    posY++;
    if (test(posY, posX, currentPiece, currentRot)) {
      posY--;
    } else {
      score++; // Award points for soft drop
      lockDelayActive = false;
    }
    lastMove = millis();
    buttonHeld = true;
  }
  // Rotate (center tap)
  else if (buttons.joyBtnPressed && !buttonHeld) {
    int newRot = (currentRot + 1) % 4;
    if (!test(posY, posX, currentPiece, newRot)) {
      currentRot = newRot;
      if (lockDelayActive) lockDelayStart = millis();
    }
    lastMove = millis();
    buttonHeld = true;
  }
  // Hold piece (Button A)
  else if (buttons.btnAPressed && !buttonHeld) {
    holdPiece();
    lastMove = millis();
    buttonHeld = true;
  }
  else {
    buttonHeld = false;
  }
}

bool TetrisGame::test(int y, int x, int piece, int rot) {
  for (int i = 0; i < 4; i++) {
    int px = x + pieces[piece][rot][1][i];
    int py = y + pieces[piece][rot][0][i];
    
    if (px < 0 || px >= FIELD_WIDTH || py >= FIELD_HEIGHT) return true;
    if (py >= 0 && field[py][px] > 0) return true;
  }
  return false;
}

void TetrisGame::placePiece() {
  for (int i = 0; i < 4; i++) {
    int x = posX + pieces[currentPiece][currentRot][1][i];
    int y = posY + pieces[currentPiece][currentRot][0][i];
    if (y >= 0 && y < FIELD_HEIGHT && x >= 0 && x < FIELD_WIDTH) {
      field[y][x] = currentPiece + 1;
    }
  }
}

void TetrisGame::clearLines() {
  int linesThisClear = 0;
  
  for (int y = FIELD_HEIGHT - 1; y >= 0; y--) {
    bool fullLine = true;
    for (int x = 0; x < FIELD_WIDTH; x++) {
      if (field[y][x] == 0) {
        fullLine = false;
        break;
      }
    }
    
    if (fullLine) {
      linesThisClear++;
      score += 100;
      for (int yy = y; yy > 0; yy--) {
        for (int x = 0; x < FIELD_WIDTH; x++) {
          field[yy][x] = field[yy-1][x];
        }
      }
      for (int x = 0; x < FIELD_WIDTH; x++) {
        field[0][x] = 0;
      }
      y++;
    }
  }
  
  // Update lines cleared and check for level up
  if (linesThisClear > 0) {
    linesCleared += linesThisClear;
    
    // Level up every 10 lines
    int newLevel = 1 + (linesCleared / 10);
    if (newLevel > level) {
      level = newLevel;
      // Speed up (reduce dropSpeed by 10% per level)
      dropSpeed = max(100, 500 - (level - 1) * 40);
    }
  }
}

void TetrisGame::newPiece(bool setPiece) {
  if (setPiece) {
    canHold = true; // Reset hold ability
  }
  
  // Use next piece, generate new next
  if (nextPiece >= 0) {
    currentPiece = nextPiece;
    nextPiece = random(0, 7);
  } else {
    currentPiece = random(0, 7);
    nextPiece = random(0, 7);
  }
  
  currentRot = 0;
  posX = FIELD_WIDTH / 2 - 1;
  posY = 0;
  
  lockDelayActive = false;
}

void TetrisGame::holdPiece() {
  if (!canHold) return;
  
  if (heldPiece == -1) {
    heldPiece = currentPiece;
    newPiece(false);
  } else {
    int temp = currentPiece;
    currentPiece = heldPiece;
    heldPiece = temp;
    currentRot = 0;
    posX = FIELD_WIDTH / 2 - 1;
    posY = 0;
  }
  
  canHold = false;
  lockDelayActive = false;
}

int TetrisGame::calculateDropDistance() {
  int dropDist = 0;
  for (int testY = posY + 1; testY < FIELD_HEIGHT; testY++) {
    if (!test(testY, posX, currentPiece, currentRot)) {
      dropDist = testY - posY;
    } else {
      break;
    }
  }
  return dropDist;
}

void TetrisGame::drawGhostPiece() {
  int dropDist = calculateDropDistance();
  if (dropDist > 0) {
    int ghostY = posY + dropDist;
    for (int i = 0; i < 4; i++) {
      int x = posX + pieces[currentPiece][currentRot][1][i];
      int y = ghostY + pieces[currentPiece][currentRot][0][i];
      if (y >= 0 && y < FIELD_HEIGHT && x >= 0 && x < FIELD_WIDTH) {
        int px = OFFSET_X + x * BLOCK_SIZE;
        int py = OFFSET_Y + y * BLOCK_SIZE;
        M5.Lcd.drawRect(px, py, BLOCK_SIZE-1, BLOCK_SIZE-1, 0x4208); // Gray
      }
    }
  }
}

void TetrisGame::drawMiniPiece(int pieceType, int x, int y, int scale) {
  if (pieceType < 0 || pieceType > 6) return;
  for (int i = 0; i < 4; i++) {
    int px = x + (pieces[pieceType][0][1][i] * scale);
    int py = y + (pieces[pieceType][0][0][i] * scale);
    M5.Lcd.fillRect(px, py, scale-1, scale-1, pieceColors[pieceType]);
  }
}

void TetrisGame::drawHoldPiece() {
  static int lastLines = -1;
  
  // Hold piece area - moved to top left corner
  M5.Lcd.fillRect(10, 30, 40, 40, COLOR_BLACK);
  M5.Lcd.drawRect(9, 29, 42, 42, COLOR_WHITE);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(COLOR_WHITE);
  M5.Lcd.setCursor(20, 75);
  M5.Lcd.print("HOLD");
  
  if (heldPiece >= 0) {
    drawMiniPiece(heldPiece, 17, 37, 6);
  }
  
  // Draw line counter below hold piece (only when changed)
  if (linesCleared != lastLines) {
    M5.Lcd.fillRect(10, 85, 60, 15, COLOR_BLACK);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(COLOR_CYAN);
    M5.Lcd.setCursor(10, 85);
    M5.Lcd.print("Lines: ");
    M5.Lcd.print(linesCleared);
    lastLines = linesCleared;
  }
}

void TetrisGame::drawNextPiece() {
  // Next piece area  
  M5.Lcd.fillRect(260, 30, 40, 40, COLOR_BLACK);
  M5.Lcd.drawRect(259, 29, 42, 42, COLOR_WHITE);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(COLOR_WHITE);
  M5.Lcd.setCursor(265, 75);
  M5.Lcd.print("NEXT");
  
  if (nextPiece >= 0) {
    drawMiniPiece(nextPiece, 267, 37, 6);
  }
}

void TetrisGame::drawHoldButton() {
  // Simple hold button under NEXT box
  M5.Lcd.fillCircle(290, 85, 10, COLOR_RED);      // Red circle
  M5.Lcd.drawCircle(290, 85, 10, COLOR_WHITE);    // White outline
  
  // "H" text
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(COLOR_WHITE);
  M5.Lcd.setCursor(287, 81);
  M5.Lcd.print("H");
}

void TetrisGame::drawControlBoxes() {
  // Calculate positions - MUCH larger boxes for human fingers
  int gameplayBottom = OFFSET_Y + FIELD_HEIGHT * BLOCK_SIZE + 4; // Just below thick border
  int screenBottom = 240;
  
  // Force boxes to start much higher to have room for large boxes
  gameplayBottom = 160; // Give 80 pixels for large control boxes!
  
  int availableHeight = screenBottom - gameplayBottom; // 80 pixels total
  int boxHeight = (availableHeight - 15) / 2; // About 32-33px each box - 3x taller!
  int boxWidth = 60; // Keep width the same
  int leftX = 25;   // Left side boxes
  int rightX = 320 - boxWidth - 25; // Right side boxes
  
  // Top row Y position (rotation boxes)
  int topY = gameplayBottom + 5;
  // Bottom row Y position (movement boxes) 
  int bottomY = topY + boxHeight + 15; // 15px gap between boxes
  
  // Left side boxes
  // Top left: Counter-clockwise rotation - MUCH TALLER
  M5.Lcd.fillRect(leftX, topY, boxWidth, boxHeight, 0x2104); // Dark gray
  M5.Lcd.drawRect(leftX, topY, boxWidth, boxHeight, COLOR_CYAN);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(COLOR_WHITE);
  M5.Lcd.setCursor(leftX + 10, topY + boxHeight/2 - 3);
  M5.Lcd.print("↺CCW");
  
  // Bottom left: Move left - MUCH TALLER
  M5.Lcd.fillRect(leftX, bottomY, boxWidth, boxHeight, 0x2104); // Dark gray
  M5.Lcd.drawRect(leftX, bottomY, boxWidth, boxHeight, COLOR_BLUE);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(COLOR_WHITE);
  M5.Lcd.setCursor(leftX + 15, bottomY + boxHeight/2 - 3);
  M5.Lcd.print("←LEFT");
  
  // Right side boxes
  // Top right: Clockwise rotation - MUCH TALLER
  M5.Lcd.fillRect(rightX, topY, boxWidth, boxHeight, 0x2104); // Dark gray
  M5.Lcd.drawRect(rightX, topY, boxWidth, boxHeight, COLOR_CYAN);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(COLOR_WHITE);
  M5.Lcd.setCursor(rightX + 15, topY + boxHeight/2 - 3);
  M5.Lcd.print("CW↻");
  
  // Bottom right: Move right - MUCH TALLER
  M5.Lcd.fillRect(rightX, bottomY, boxWidth, boxHeight, 0x2104); // Dark gray
  M5.Lcd.drawRect(rightX, bottomY, boxWidth, boxHeight, COLOR_BLUE);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(COLOR_WHITE);
  M5.Lcd.setCursor(rightX + 5, bottomY + boxHeight/2 - 3);
  M5.Lcd.print("RIGHT→");
}