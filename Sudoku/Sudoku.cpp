#include <Gamebuino-Meta.h>

#include "Globals.h"
#include "Sudoku.h"
#include "Utils.h"

Sudoku sudoku;

// Constraint tables
int colCells[9][9];
int rowCells[9][9];
int blockCells[9][9];

void initConstraintTables() {
  for (int i = 0; i < 9; i++) {
    int colIndex = i;
    int rowIndex = i * 9;
    int blockIndex = 3 * (i % 3 + (i / 3) * 9);

    for (int j = 0; j < 9; j++) {
      colCells[i][j] = colIndex;
      rowCells[i][j] = rowIndex;
      blockCells[i][j] = blockIndex;

      colIndex += 9;
      rowIndex += 1;
      if (j % 3 == 2) {
        blockIndex += 7;
      } else {
        blockIndex += 1;
      }
    }
  }
}

int setBit(int mask, int bit) {
  return (mask | bit);
}

int clearBit(int mask, int bit) {
  return (mask & ~bit);
}

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

bool Sudoku::_isAllowed(int cellIndex, int bit) {
  SudokuCell& cell = _cells[cellIndex];

  if (cell._fixed) {
    return cell._value == bit;
  }

  return (
    (cell._colMask & bit) != 0 &&
    (cell._rowMask & bit) != 0 &&
    (cell._blockMask & bit) != 0
  );
}

void Sudoku::_updateMasks(int cellIndex, int bit, int (*updateFun)(int, int)) {
  int x = cellIndex % 9;
  int y = (cellIndex - x) / 9;
  int blk = x/3 + 3 * (y/3);

  int* colIndices = colCells[x];
  int* rowIndices = rowCells[y];
  int* blockIndices = blockCells[blk];

  for (int i = 0; i < 9; i++) {
    int ci = colIndices[i];
    if (ci != cellIndex) {
      SudokuCell& cell = _cells[ci];
      cell._colMask = (*updateFun)(cell._colMask, bit);
    }

    ci = rowIndices[i];
    if (ci != cellIndex) {
      SudokuCell& cell = _cells[ci];
      cell._rowMask = (*updateFun)(cell._rowMask, bit);
    }

    ci = blockIndices[i];
    if (ci != cellIndex) {
      SudokuCell& cell = _cells[ci];
      cell._blockMask = (*updateFun)(cell._blockMask, bit);
    }
  }

  //colMasks[
}

void Sudoku::_clear(int cellIndex) {
  SudokuCell& cell = _cells[cellIndex];

  int oldBit = cell._value;
  assertTrue(oldBit > 0);

  _numFilled--;
  cell._value = 0;

  _updateMasks(cellIndex, oldBit, &setBit);
}

void Sudoku::_setValue(int cellIndex, int bit) {
  assertTrue(bit != 0);

  SudokuCell& cell = _cells[cellIndex];

  if (cell._value != 0) {
    _clear(cellIndex);
  }

  _numFilled++;
  cell._value = bit;

  _updateMasks(cellIndex, bit, clearBit);
}

bool Sudoku::_nextValue(int cellIndex) {
  int bit = _getValue(cellIndex);

  for (int i = 0; i < 9; i++) {
    if (bit == 0) {
      bit = 1;
    }
    else if (bit == 256) {
      bit = 0;
    }
    else {
      bit <<= 1;
    }

    if (bit == 0) {
      // Can always clear
      _clear(cellIndex);
      return true;
    }

    if (_isAllowed(cellIndex, bit)) {
      _setValue(cellIndex, bit);
      return true;
    }
  }

  // Cannot set to a different value
  return false;
}

int Sudoku::getValue(int x, int y) {
  return _bit2value(_getValue(x + y * 9));
}

bool Sudoku::isFixed(int x, int y) {
  return _isFixed(x + y * 9);
}

bool Sudoku::isSet(int x, int y) {
  return _isSet(x + y * 9);
}

void Sudoku::clear(int x, int y) {
  _clear(x + y * 9);
}

bool Sudoku::nextValue(int x, int y) {
  return _nextValue(x + y * 9);
}


void drawLines(int i) {
  gb.display.drawLine(8 + i * 7, 0, 8 + i * 7, 63);
  gb.display.drawLine(8, i * 7, 71, i * 7);
}

void drawCell(int x, int y) {
  gb.display.fillRect(9 + x * 7, 1 + y * 7, 6, 6);
}

void drawValue(int x, int y, int value) {
  gb.display.setCursor(x * 7 + 11, y * 7 + 2);
  gb.display.print(value);
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

  gb.display.setColor(DARKGRAY);
  drawCell(cursorX, cursorY);

  for (int x = 0; x < 9; x++) {
    for (int y = 0; y < 9; y++) {
      if (sudoku.isSet(x, y)) {
        if (sudoku.isFixed(x, y)) {
          gb.display.setColor(WHITE);
        } else {
          gb.display.setColor(BEIGE);
        }
        drawValue(x, y, getValue(x, y));
      }
    }
  }
}



