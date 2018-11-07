#include <Gamebuino-Meta.h>

#include "Globals.h"
#include "Sudoku.h"
#include "Utils.h"

// Constraint tables
int colCells[numConstraintGroups][constraintGroupSize];
int rowCells[numConstraintGroups][constraintGroupSize];
int boxCells[numConstraintGroups][constraintGroupSize];

void initConstraintTables() {
  for (int i = 0; i < numConstraintGroups; i++) {
    int colIndex = i;
    int rowIndex = i * numCols;
    int boxIndex = 3 * (i % 3 + (i / 3) * 9);

    for (int j = 0; j < constraintGroupSize; j++) {
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
  _colMask = maxBitMask;
  _rowMask = maxBitMask;
  _boxMask = maxBitMask;

  _index = cellIndex;
  _col = _index % numCols;
  _row = (_index - _col) / numCols;
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
  for (int i = 0; i < numCells; i++) {
    _cells[i].init(i);
  }

  for (int i = 0; i < numConstraintGroups; i++) {
    _colMasks[i] = maxBitMask;
    _rowMasks[i] = maxBitMask;
    _boxMasks[i] = maxBitMask;
  }

  _numFilled = 0;
  _numFixed = 0;
}

void Sudoku::init(Sudoku& sudoku) {
  init();

  for (int i = 0; i < numCells; i++) {
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

  for (int i = 0; i < constraintGroupSize; i++) {
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

  for (int i = 0; i < numValues; i++) {
    if (bit == 0) {
      bit = 1;
    }
    else if (bit == maxBitValue) {
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
  for (int i = 0; i < numCells; i++) {
    SudokuCell& cell = cellAt(i);
    if (cell.isSet()) {
      cell._fixed = true;
      _numFixed++;
    }
  }
}

void Sudoku::unfixValues() {
  for (int i = 0; i < numCells; i++) {
    SudokuCell& cell = cellAt(i);
    if (cell.isFixed()) {
      cell._fixed = false;
      _numFixed--;
    }
  }
}

void Sudoku::resetValues() {
  for (int i = 0; i < numCells; i++) {
    SudokuCell& cell = cellAt(i);
    if (cell.isSet() && !cell.isFixed()) {
      clearValue(cell);
    }
  }
}

