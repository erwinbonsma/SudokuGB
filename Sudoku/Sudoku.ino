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

  int gradientLightColors[8] = {
    0x000000, 0x504d01, 0xa09a02, 0xf1e703,
    0x000000, 0x293e55, 0x537caa, 0x7dbbff
  };

  for (int i = 0; i < 8; i++) {
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
