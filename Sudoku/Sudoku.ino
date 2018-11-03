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

void setup() {
  gb.begin();

  SerialUSB.begin(9600);
  while (!SerialUSB);

  initConstraintTables();

  sudokuForSolver.init();
  assertTrue(solver.randomSolve());
  stripper.strip();
  sudoku.init(sudokuForSolver);
}

void loop() {
  while(!gb.update());
  gb.display.clear();

  update();
  sudoku.draw();
}
