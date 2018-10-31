#include <Gamebuino-Meta.h>

#include "Globals.h"
#include "Sudoku.h"

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
}

void setup() {
  gb.begin();

  sudoku.init();
}

void loop() {
  while(!gb.update());
  gb.display.clear();

  update();
  sudoku.draw();
}
