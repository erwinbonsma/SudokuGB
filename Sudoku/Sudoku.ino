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

  // Now clear as many values as possible to create the actual puzzle
  stripper.randomStrip();

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
    cursorX = (cursorX + 8) % 9;
  }
  else if (gb.buttons.pressed(BUTTON_RIGHT)) {
    cursorX = (cursorX + 1) % 9;
  }
  else if (gb.buttons.pressed(BUTTON_UP)) {
    cursorY = (cursorY + 8) % 9;
  }
  else if (gb.buttons.pressed(BUTTON_DOWN)) {
    cursorY = (cursorY + 1) % 9;
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

  SerialUSB.begin(9600);
  while (!SerialUSB);

  initConstraintTables();

  // Trigger puzzle generation in 2nd frame. This way generate message is still
  // shown first
  generateNewPuzzleCountdown = 2;
}

void loop() {
  while(!gb.update());
  gb.display.clear();

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
