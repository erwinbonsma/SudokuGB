#include <Gamebuino-Meta.h>

#include "Utils.h"
#include "Globals.h"

int cursorX = 4;
int cursorY = 4;
int generateNewPuzzleCountdown;

void generateNewPuzzle() {
  // Reset the puzzle
  sudoku.init();

  // Solve it to generate a (random) solution
  assertTrue(solver.randomSolve());

#ifdef DEVELOPMENT
  // Near solve
  int i = 0;
  while (sudoku.cellAt(i).isFixed()) {
    i++;
  }
  sudoku.clearValue(sudoku.cellAt(i));
#else
  // Now clear as many values as possible to create the actual puzzle
  stripper.randomStrip();
#endif

  sudoku.fixValues();
}

const char* menuEntries[] = {
  "Back to puzzle",
  "Reset puzzle",
  "New puzzle"
};

void mainMenu() {
  uint8_t entry = gb.gui.menu("Main menu", menuEntries);

  if (entry == 1) {
    sudoku.resetValues();
  }
  else if (entry == 2) {
    // Initiate puzzle creation, but wait a few frames before generating puzzle,
    // so OK sound is not (too) abruptly aborted
    generateNewPuzzleCountdown = 10;
  }
}

void update() {
  if (gb.buttons.pressed(BUTTON_LEFT)) {
    cursorX = (cursorX + numCols - 1) % numCols;
  }
  else if (gb.buttons.pressed(BUTTON_RIGHT)) {
    cursorX = (cursorX + 1) % numCols;
  }
  else if (gb.buttons.pressed(BUTTON_UP)) {
    cursorY = (cursorY + numRows - 1) % numRows;
  }
  else if (gb.buttons.pressed(BUTTON_DOWN)) {
    cursorY = (cursorY + 1) % numRows;
  }

  if (!sudoku.isFixed(cursorX, cursorY)) {
    if (gb.buttons.pressed(BUTTON_A)) {
      if (!sudoku.nextValue(cursorX, cursorY)) {
        // TODO: Sfx
      }
    }
    else if (gb.buttons.pressed(BUTTON_B)) {
      if (sudoku.isSet(cursorX, cursorY)) {
        sudoku.clearValue(cursorX, cursorY);
      }
    }
  }

  if (gb.buttons.pressed(BUTTON_MENU)) {
    mainMenu();
  }
}

void setup() {
  gb.begin();

#ifdef DEVELOPMENT
  initDebugLog();

  int gradientLightColors[39] = {
    // Yellow
    0x000000, 0x141300, 0x282600, 0x3c3900,
    0x504d01, 0x646001, 0x787301, 0x8c8601,
    0xa09a02, 0xb4ad02, 0xc8c002, 0xdcd302,
    0xf1e703,

    // Light blue
    0x000000, 0x0a0f15, 0x141f2a, 0x1f2e3f,
    0x293e55, 0x344d6a, 0x3e5d7f, 0x486d94,
    0x537caa, 0x5d8cbf, 0x689bd4, 0x72abe9,
    0x7dbbff,

    // Light green
    0x000000, 0x0b1107, 0x16220e, 0x213316,
    0x2c451d, 0x375624, 0x42672c, 0x4d7833,
    0x588a3a, 0x639b42, 0x6eac49, 0x79bd50,
    0x85cf58
  };

  for (int i = 0; i < 39; i++) {
    int hex = gradientLightColors[i];
    Color gbc = gb.createColor((hex >> 16) & 255, (hex >> 8) & 255, hex & 255);
    SerialUSB.printf("0x%04x, ", gbc);
  }
#endif

  initConstraintTables();

  // Trigger puzzle generation in 2nd frame. This way generate message is still
  // shown first
  generateNewPuzzleCountdown = 2;
}

void loop() {
  while(!gb.update());
  gb.display.clear();
  gb.lights.clear();

  if (generateNewPuzzleCountdown > 0) {
    generateNewPuzzleCountdown--;

    if (generateNewPuzzleCountdown == 0) {
      gb.sound.stop(0); // Stop OK sound from menu
      generateNewPuzzle();
    }

    gb.display.setCursor(6, 28);
    gb.display.setColor(WHITE);
    gb.display.println("Generating puzzle");
  }
  else {
    sudoku.draw();
    update();
  }
}
