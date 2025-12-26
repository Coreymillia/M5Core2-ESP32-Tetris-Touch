// input.h - Touch input handling for M5Core2
#ifndef INPUT_H
#define INPUT_H

#include "config.h"

struct ButtonState {
  bool up;
  bool down;
  bool left;
  bool right;
  bool btnA;
  bool btnB;
  bool joyBtn;
  bool upPressed;
  bool downPressed;
  bool leftPressed;
  bool rightPressed;
  bool btnAPressed;
  bool btnBPressed;
  bool joyBtnPressed;
};

extern ButtonState buttons;

void initInput();
void updateInput();

#endif