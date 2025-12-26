# M5Core2 Tetris

A classic Tetris game adapted for the M5Stack Core2 with touch screen controls.

## Features

- Full Tetris gameplay with modern features:
  - Hold piece functionality
  - Ghost piece preview
  - Line clearing with scoring
  - Progressive speed increase
  - Lock delay system

- Touch-based controls optimized for M5Core2:
  - **Left side tap or hold**: Move piece left
  - **Right side tap or hold**: Move piece right  
  - **Center tap**: Rotate piece
  - **Hold gameplay area**: Soft drop
  - **Swipe UP**: Hard drop
  - **Button A**: Hold piece

- Enhanced display:
  - 320x240 resolution optimized layout
  - Color-coded piece types
  - Score and statistics display
  - Visual touch guides

## Hardware Required

- M5Stack Core2

## Installation

1. Install PlatformIO
2. Clone this repository
3. Build and upload:
   ```bash
   platformio run --target upload
   ```

## Controls

The game uses the M5Core2's touch screen divided into regions:

```
┌─────────┬─────────┬─────────┐
│  LEFT   │ CENTER  │  RIGHT  │
│  MOVE   │ ROTATE  │  MOVE   │
│         │         │         │
├─────────┴─────────┴─────────┤
│        SOFT DROP             │
│      (Bottom Area)           │
└─────────────────────────────┘
```

- **Swipe Down**: Hard drop (instant drop)
- **Button A** (left side): Hold current piece
- **Button B** (right side): Not used

## Differences from Original M5StickC Version

- Adapted from 240x135 to 320x240 display
- Replaced joystick controls with touch zones
- Larger block size for better visibility
- Enhanced UI layout for touch interface
- Maintained all core tetris mechanics

## Code Structure

- `main.cpp` - Main game loop and splash screen
- `tetris.cpp/h` - Core tetris game logic
- `input.cpp/h` - Touch input handling
- `display.cpp/h` - Display utilities
- `config.h` - Configuration constants

## Build Details

- Platform: ESP32
- Framework: Arduino
- Board: M5Stack Core2
- Libraries: M5Core2

The game maintains the same excellent tetris mechanics from the original M5StickC version while providing an enhanced touch-based experience on the larger M5Core2 screen.
