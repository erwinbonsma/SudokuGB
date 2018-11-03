#include <Gamebuino-Meta.h>

#include "Globals.h"
#include "Sudoku.h"
#include "Utils.h"

// Constraint tables
int colCells[9][9];
int rowCells[9][9];
int boxCells[9][9];

void initConstraintTables() {
  for (int i = 0; i < 9; i++) {
    int colIndex = i;
    int rowIndex = i * 9;
    int boxIndex = 3 * (i % 3 + (i / 3) * 9);

    for (int j = 0; j < 9; j++) {
      colCells[i][j] = colIndex;
      rowCells[i][j] = rowIndex;
      boxCells[i][j] = boxIndex;

      colIndex += 9;
      rowIndex += 1;
      if (j % 3 == 2) {
        boxIndex += 7;
      } else {
        boxIndex += 1;
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

int bitToValue(int bit) {
  int value = 0;
  while (bit > 0) {
    value++;
    bit >>= 1;
  }
  return value;
}

int valueToBit(int value) {
  return 1 << (value - 1);
}

//------------------------------------------------------------------------------
// SudokuCell

void SudokuCell::init(int cellIndex) {
  _value = 0;
  _fixed = false;
  _colMask = 511;
  _rowMask = 511;
  _boxMask = 511;

  _index = cellIndex;
  _col = _index % 9;
  _row = (_index - _col) / 9;
  _box = _col / 3 + 3 * (_row / 3);
}

bool SudokuCell::isBitAllowed(int bit) {
  if (_fixed) {
    return _value == bit;
  }

  return (
    (_colMask & bit) != 0 &&
    (_rowMask & bit) != 0 &&
    (_boxMask & bit) != 0
  );
}

bool SudokuCell::hasOneAllowedValue() {
  int m = bitMask();
  // Note: x & (x - 1) clears the right-most bit
  return (m & (m - 1)) == 0;
}

//------------------------------------------------------------------------------
// Sudoku

void Sudoku::init() {
  for (int i = 0; i < 81; i++) {
    _cells[i].init(i);
  }

  for (int i = 0; i < 9; i++) {
    _colMasks[i] = 511;
    _rowMasks[i] = 511;
    _boxMasks[i] = 511;
  }

  _numFilled = 0;
}

void Sudoku::init(Sudoku& sudoku) {
  init();

  for (int i = 0; i < 81; i++) {
    int bit = sudoku.cellAt(i).getBitValue();
    if (bit != 0) {
      setBitValue(_cells[i], bit);
    }
  }
}

void Sudoku::updateBitMasks(SudokuCell& cell, int bit, int (*updateFun)(int, int)) {
  int* colIndices = colCells[cell.col()];
  int* rowIndices = rowCells[cell.row()];
  int* boxIndices = boxCells[cell.box()];

  for (int i = 0; i < 9; i++) {
    int ci = colIndices[i];
    if (ci != cell.index()) {
      SudokuCell& cell2 = _cells[ci];
      cell2._colMask = (*updateFun)(cell2._colMask, bit);
    }

    ci = rowIndices[i];
    if (ci != cell.index()) {
      SudokuCell& cell2 = _cells[ci];
      cell2._rowMask = (*updateFun)(cell2._rowMask, bit);
    }

    ci = boxIndices[i];
    if (ci != cell.index()) {
      SudokuCell& cell2 = _cells[ci];
      cell2._boxMask = (*updateFun)(cell2._boxMask, bit);
    }
  }

  _colMasks[cell.col()] = (*updateFun)(_colMasks[cell.col()], bit);
  _rowMasks[cell.row()] = (*updateFun)(_rowMasks[cell.row()], bit);
  _boxMasks[cell.box()] = (*updateFun)(_boxMasks[cell.box()], bit);
}

void Sudoku::setValue(int x, int y, int value) {
  setBitValue(cellAt(x, y), valueToBit(value));
}

void Sudoku::clearValue(SudokuCell& cell) {
  int oldBit = cell._value;
  assertTrue(oldBit > 0);

  _numFilled--;
  cell._value = 0;

  updateBitMasks(cell, oldBit, &setBit);
}

void Sudoku::setBitValue(SudokuCell& cell, int bit) {
  assertTrue(bit != 0);

  if (cell.isSet()) {
    clearValue(cell);
  }

  _numFilled++;
  cell._value = bit;

  updateBitMasks(cell, bit, clearBit);
}

bool Sudoku::nextValue(SudokuCell& cell) {
  int bit = cell.getBitValue();

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
      clearValue(cell);
      return true;
    }

    if (cell.isBitAllowed(bit)) {
      setBitValue(cell, bit);
      return true;
    }
  }

  // Cannot set to a different value
  return false;
}

AutoSetResult Sudoku::autoSet(SudokuCell& cell) {
  if (cell.isSet()) {
    return AutoSetResult::AlreadySet;
  }

  int m = cell.bitMask();
  if (m == 0) {
    return AutoSetResult::Stuck;
  }

  // Next evaluates to zero if only one bit was set
  if (!cell.hasOneAllowedValue()) {
    return AutoSetResult::MultipleOptions;
  }

  // Only one value is possible
  //SerialUSB.printf("autoSet %d => %d (was: %d)\n", cell.index(), m, cell.getBitValue());
  setBitValue(cell, m);
  return AutoSetResult::CellUpdated;
}

int Sudoku::getValue(int x, int y) {
  return bitToValue(cellAt(x, y).getBitValue());
}

bool Sudoku::isFixed(int x, int y) {
  return cellAt(x, y).isFixed();
}

bool Sudoku::isSet(int x, int y) {
  return cellAt(x, y).isSet();
}

void Sudoku::clearValue(int x, int y) {
  clearValue(cellAt(x, y));
}

bool Sudoku::nextValue(int x, int y) {
  return nextValue(cellAt(x, y));
}

void Sudoku::fixValues() {
  for (int i = 0; i < 81; i++) {
    SudokuCell& cell = cellAt(i);
    if (cell.isSet()) {
      cell.fix();
    }
  }
}

void Sudoku::resetValues() {
  for (int i = 0; i < 81; i++) {
    SudokuCell& cell = cellAt(i);
    if (cell.isSet() && !cell.isFixed()) {
      clearValue(cell);
    }
  }
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

  gb.display.setColor(BLUE);
  drawCell(cursorX, cursorY);

  for (int x = 0; x < 9; x++) {
    for (int y = 0; y < 9; y++) {
      if (sudoku.isSet(x, y)) {
        if (sudoku.isFixed(x, y)) {
          gb.display.setColor(WHITE);
        } else {
          gb.display.setColor(LIGHTBLUE);
        }
        drawValue(x, y, getValue(x, y));
      }
    }
  }
}

