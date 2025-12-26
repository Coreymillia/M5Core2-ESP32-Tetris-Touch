// display.cpp - Display utilities for M5Core2
#include "display.h"

void initDisplay() {
  M5.Lcd.setRotation(SCREEN_ROTATION);
  M5.Lcd.fillScreen(COLOR_BLACK);
  M5.Lcd.setTextColor(COLOR_WHITE);
  M5.Lcd.setTextSize(1);
}

void clearDisplay() {
  M5.Lcd.fillScreen(COLOR_BLACK);
}