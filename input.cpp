#include "input.h"
#include "config.h"

ButtonState buttons;

// Game field constants for boundary detection
extern const int OFFSET_X = 90;           // From tetris.h
extern const int FIELD_WIDTH = 12;        // From tetris.h  
extern const int FIELD_HEIGHT = 18;       // From tetris.h
extern const int BLOCK_SIZE = 12;         // From tetris.h
extern const int OFFSET_Y = 25;           // From tetris.h

// Only need hold button zone now
HotZone_t holdButtonZone(280, 75, 300, 95); // Hold button under NEXT

static ButtonState lastButtons;
static unsigned long lastTouchTime = 0;
static int lastTouchX = -1, lastTouchY = -1;
static bool hardDropGesture = false;

void initInput() {
  memset(&buttons, 0, sizeof(buttons));
  memset(&lastButtons, 0, sizeof(lastButtons));
}

void updateInput() {
  lastButtons = buttons;
  memset(&buttons, 0, sizeof(buttons));
  
  // Physical buttons
  buttons.btnA = M5.BtnA.isPressed();
  buttons.btnB = M5.BtnB.isPressed();
  
  // Button press detection
  buttons.btnAPressed = buttons.btnA && !lastButtons.btnA;
  buttons.btnBPressed = buttons.btnB && !lastButtons.btnB;
  
  // Touch input - GAME FIELD BOUNDARY system
  if (M5.Touch.ispressed()) {
    TouchPoint_t touch = M5.Touch.getPressPoint();
    
    if (touch.x >= 0 && touch.y >= 0) {
      unsigned long currentTime = millis();
      
      // First touch - start tracking
      if (lastTouchX < 0 || lastTouchY < 0) {
        lastTouchX = touch.x;
        lastTouchY = touch.y;
        lastTouchTime = currentTime;
        hardDropGesture = false;
        return;
      }
      
      unsigned long touchDuration = currentTime - lastTouchTime;
      int deltaX = touch.x - lastTouchX;
      int deltaY = touch.y - lastTouchY;
      
      // Check for swipe up gesture (hard drop) - keep this
      if (deltaY < -30 && touchDuration < 300) {
        if (!hardDropGesture) {
          hardDropGesture = true;
          buttons.up = true;
          buttons.upPressed = true;
        }
      }
      else {
        // Define game field boundaries (with some padding)
        int gameLeft = OFFSET_X - 5;   // Left edge of game field 
        int gameRight = OFFSET_X + (FIELD_WIDTH * BLOCK_SIZE) + 5; // Right edge
        int gameTop = OFFSET_Y - 5;    // Top edge
        int gameBottom = OFFSET_Y + (FIELD_HEIGHT * BLOCK_SIZE) + 5; // Bottom edge
        
        // Check if touch is INSIDE game field
        bool insideGameField = (touch.x >= gameLeft && touch.x <= gameRight && 
                               touch.y >= gameTop && touch.y <= gameBottom);
        
        if (insideGameField) {
          // INSIDE game field - TAP = rotate, HOLD = soft drop 
          if (touchDuration > 300) {
            // HOLD inside game field = soft drop
            buttons.down = true;
          }
          // TAP handled on release
        }
        else {
          // OUTSIDE game field - move pieces
          // Left side = move left, Right side = move right
          if (touch.x < gameLeft) {
            buttons.left = true;
          }
          else if (touch.x > gameRight) {
            // Make sure we're not hitting the hold button
            if (!holdButtonZone.inHotZone(touch)) {
              buttons.right = true;
            }
          }
        }
      }
      
      // Hold button zone (always works)
      if (holdButtonZone.inHotZone(touch)) {
        buttons.btnA = true;
        buttons.btnAPressed = !lastButtons.btnA;
      }
    }
  } 
  else {
    // Touch released - check for TAP inside game field
    if (lastTouchX >= 0 && lastTouchY >= 0) {
      unsigned long touchDuration = millis() - lastTouchTime;
      
      // Define game field boundaries 
      int gameLeft = OFFSET_X - 5;   
      int gameRight = OFFSET_X + (FIELD_WIDTH * BLOCK_SIZE) + 5;
      int gameTop = OFFSET_Y - 5;    
      int gameBottom = OFFSET_Y + (FIELD_HEIGHT * BLOCK_SIZE) + 5;
      
      // Check if tap was INSIDE game field
      bool insideGameField = (lastTouchX >= gameLeft && lastTouchX <= gameRight && 
                             lastTouchY >= gameTop && lastTouchY <= gameBottom);
      
      // Quick TAP inside game field = ROTATE (more sensitive!)
      if (touchDuration < 150 && !hardDropGesture && insideGameField) {
        buttons.joyBtn = true;
        buttons.joyBtnPressed = true;
      }
    }
    
    // Reset tracking
    lastTouchX = -1;
    lastTouchY = -1;
    hardDropGesture = false;
  }
}
