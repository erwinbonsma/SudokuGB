#include <Gamebuino-Meta.h>

#include "Globals.h"
#include "Sudoku.h"

Sudoku sudoku;

void SudokuCell::init() {
  _value = 0;
  _fixed = false;
  _colMask = 511;
  _rowMask = 511;
  _blockMask = 511;
}

void Sudoku::init() {
  for (int i = 0; i < 81; i++) {
    _cells[i].init();
  }

  for (int i = 0; i < 9; i++) {
    _colMasks[i] = 511;
    _rowMasks[i] = 511;
    _blockMasks[i] = 511;
  }

  _numFilled = 0;
}

int Sudoku::_bit2value(int bit) {
  int value = 0;
  while (bit > 0) {
    value++;
    bit >>= 1;
  }
  return value;
}

int Sudoku::getValue(int x, int y) {
  return _bit2value(_getValue(x + y * 9));
}

void drawLines(int i) {
  gb.display.drawLine(8 + i * 7, 0, 8 + i * 7, 63);
  gb.display.drawLine(8, i * 7, 71, i * 7);
}

void drawCell(int x, int y) {
  gb.display.fillRect(9 + x * 7, 1 + y * 7, 6, 6);
}

void Sudoku::draw() {
  gb.display.setColor(DARKGRAY);
  for (int i = 0; i <= 9; i++) {
    if (i % 3 != 0) {
      drawLines(i);
    }
  }
  gb.display.setColor(GRAY);
  for (int i = 0; i <= 9; i += 3) {
    drawLines(i);
  }

  gb.display.setColor(BROWN);
  drawCell(cursorX, cursorY);
}



