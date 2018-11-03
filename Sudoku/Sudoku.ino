#include <Gamebuino-Meta.h>

#include "Utils.h"
#include "Globals.h"

int cursorX = 4;
int cursorY = 4;

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
}

void generateNewPuzzle() {
  // Reset the puzzle
  sudoku.init();

  // Solve it to generate a (random) solution
  assertTrue(solver.randomSolve());

  // Now clear as many values as possible to create the actual puzzle
  stripper.randomStrip();

  sudoku.fixValues();
}

void setup() {
  gb.begin();

  SerialUSB.begin(9600);
  while (!SerialUSB);

  initConstraintTables();

  generateNewPuzzle();
}

void loop() {
  while(!gb.update());
  gb.display.clear();

  update();
  sudoku.draw();
}
