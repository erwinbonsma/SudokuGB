/*
 * Sudoku, a Gamebuino game
 *
 * Copyright 2018, Erwin Bonsma
 */

#include <Gamebuino-Meta.h>

#include "Globals.h"
#include "Sudoku.h"
#include "Utils.h"

// Constraint tables
int constraintCells[numConstraintGroups][constraintGroupSize];

bool isPartOfHyperBox(int x, int y) {
  return ((x + 3) % 4 < 3) && ((y + 3) % 4 < 3);
}

void initConstraintTables() {
  int groupIndex = 0;

  // Column constraints
  for (int i = 0; i < numCols; i++) {
    int cellIndex = i;
    for (int j = 0; j < constraintGroupSize; j++) {
      constraintCells[groupIndex][j] = cellIndex;
      cellIndex += 9;
    }
    groupIndex++;
  }

  // Row constraints
  for (int i = 0; i < numRows; i++) {
    int cellIndex = i * numCols;
    for (int j = 0; j < constraintGroupSize; j++) {
      constraintCells[groupIndex][j] = cellIndex;
      cellIndex++;
    }
    groupIndex++;
  }

  // Box constraints
  for (int i = 0; i < numBoxes; i++) {
    int cellIndex = 3 * (i % 3 + (i / 3) * 9);
    for (int j = 0; j < constraintGroupSize; j++) {
      constraintCells[groupIndex][j] = cellIndex;
      if (j % 3 == 2) {
        cellIndex += 7;
      } else {
        cellIndex++;
      }
    }
    groupIndex++;
  }

  // Hyper-box constraints
  // First the explicit ones
  for (int i = 0; i < 4; i++) {
    int x = 1 + (i % 2) * 4;
    int y = 1 + (i / 2) * 4;
    int cellIndex = x + y * numCols;

    assertTrue(isPartOfHyperBox(x, y));
    for (int j = 0; j < constraintGroupSize; j++) {
      constraintCells[groupIndex][j] = cellIndex;
      if (j % 3 == 2) {
        cellIndex += 7;
      } else {
        cellIndex++;
      }
    }
    groupIndex++;
  }
  // Now the four ones these imply
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < constraintGroupSize; j++) {
      int x = (j % 3) * 4;
      int y = 1 + i * 4 + (j / 3);
      constraintCells[groupIndex][j] = x + y * numCols;
      constraintCells[groupIndex + 1][j] = y + x * numCols;
    }
    groupIndex += 2;
  }
  // Now the last one
  for (int j = 0; j < constraintGroupSize; j++) {
    int x = (j % 3) * 4;
    int y = (j / 3) * 4;
    constraintCells[groupIndex][j] = x + y * numCols;
  }
}

//------------------------------------------------------------------------------
// SudokuCell

void SudokuCell::init(Sudoku* parent, int cellIndex) {
  _parent = parent;
  _index = cellIndex;

  int col = _index % numCols;
  int row = (_index - col) / numCols;
  _constraintGroup[0] = col;
  _constraintGroup[1] = numCols + row;
  _constraintGroup[2] = numCols + numRows + col / 3 + 3 * (row / 3);

  // Determine the hyper-constraint group that this cell belongs to by searching
  // for the group that contains this cell. Due to their more irregular
  // structure, it is harder to calculate this from the cell's position.
  int i = numCols + numRows + numBoxes;
  while (i < numConstraintGroups) {
    int* cellIndices = constraintCells[i];
    for (int j = 0; j < constraintGroupSize; j++) {
      if (cellIndices[j] == _index) {
        _constraintGroup[3] = i;
        return;
      }
    }
    i++;
  }
  assertTrue(false);
}

void SudokuCell::reset() {
  _value = 0;
  _fixed = false;

  if (_parent->hyperConstraintsEnabled()) {
    // The last constraint only applies when hyper-boxes are enabled and this
    // cell is part of one of the four explicit hyper boxes.
    _allowedUsesLastConstraint = (
      _constraintGroup[3] < numExplicitConstraintGroups
    );
    _possibleUsesLastConstraint = true;
  } else {
    _allowedUsesLastConstraint = false;
    _possibleUsesLastConstraint = false;
  }
}

int SudokuCell::bitMask(bool applyLastConstraint) {
  int m = (
    _parent->_constraintMask[_constraintGroup[0]] &
    _parent->_constraintMask[_constraintGroup[1]] &
    _parent->_constraintMask[_constraintGroup[2]]
  );
  if (applyLastConstraint) {
    m &= _parent->_constraintMask[_constraintGroup[3]];
  }
  return m;
}

bool SudokuCell::isBitAllowed(int bit) {
  return (allowedBitMask() & bit) != 0;
}

bool SudokuCell::isBitPossible(int bit) {
  return (possibleBitMask() & bit) != 0;
}

bool SudokuCell::hasOneAllowedValue() {
  assertTrue(_value == 0);
  int m = allowedBitMask();
  // Note: x & (x - 1) clears the right-most bit
  return (m & (m - 1)) == 0;
}

bool SudokuCell::hasOnePossibleValue() {
  assertTrue(_value == 0);
  int m = possibleBitMask();
  // Note: x & (x - 1) clears the right-most bit
  return (m & (m - 1)) == 0;
}

//------------------------------------------------------------------------------
// Sudoku

void Sudoku::init() {
  for (int i = 0; i < numCells; i++) {
    _cells[i].init(this, i);
  }

  _hyperConstraints = false;
}

void Sudoku::reset(bool hyperConstraints) {
  _hyperConstraints = hyperConstraints;
  _autoFix = false;

  _numFilled = 0;
  _numFixed = 0;

  for (int i = 0; i < numCells; i++) {
    _cells[i].reset();
  }

  for (int i = 0; i < numConstraintGroups; i++) {
    _constraintMask[i] = maxBitMask;
  }
}

void Sudoku::reset(Sudoku& sudoku) {
  reset(sudoku.hyperConstraintsEnabled());

  for (int i = 0; i < numCells; i++) {
    int bit = sudoku.cellAt(i).getBitValue();
    if (bit != 0) {
      setBitValue(_cells[i], bit);
    }
  }
}

void Sudoku::updateBitMasks(SudokuCell& cell, int bit, int (*updateFun)(int, int)) {
  for (int i = 0; i < maxConstraintsPerCell; i++) {
    int groupIndex = cell._constraintGroup[i];

    _constraintMask[groupIndex] = (*updateFun)(_constraintMask[groupIndex], bit);
  }
}

void Sudoku::setValue(int x, int y, int value) {
  setBitValue(cellAt(x, y), valueToBit(value));
}

void Sudoku::clearValue(SudokuCell& cell) {
  int oldBit = cell._value;
  assertTrue(oldBit > 0);

  _numFilled--;
  cell._value = 0;

  // Unusual, but can happen while puzzle is being edited
  if (cell._fixed) {
    cell._fixed = false;
    _numFixed--;
  }

  updateBitMasks(cell, oldBit, &setBit);
}

void Sudoku::setBitValue(SudokuCell& cell, int bit) {
  assertTrue(bit != 0);

  bool wasFixed = cell._fixed;

  if (cell.isSet()) {
    clearValue(cell);
  }

  _numFilled++;
  cell._value = bit;
  if (_autoFix || wasFixed) {
    cell._fixed = true;
    _numFixed++;
  }

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

  int m = cell.possibleBitMask();
  if (m == 0) {
    return AutoSetResult::Stuck;
  }

  // Next evaluates to zero if only one bit was set
  if ((m & (m - 1)) != 0) {
    return AutoSetResult::MultipleOptions;
  }

  // Only one value is possible
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

void Sudoku::fixValue(int x, int y) {
  SudokuCell& cell = cellAt(x, y);
  assertTrue(!cell._fixed);

  cell._fixed = true;
  _numFixed++;
}

bool Sudoku::nextValue(int x, int y) {
  return nextValue(cellAt(x, y));
}

void Sudoku::fixValues() {
  for (int i = 0; i < numCells; i++) {
    SudokuCell& cell = cellAt(i);
    if (cell.isSet()) {
      assertTrue(!cell._fixed);
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

void Sudoku::dump() {
#ifdef DEVELOPMENT
  for (int i = 0; i < numCells; i++) {
    int val = bitToValue(cellAt(i).getBitValue());
    if (i % 9 == 8) {
      debug("%d\n", val);
    }
    else if (i % 3 == 2) {
      debug("%d | ", val);
    }
    else {
      debug("%d ", val);
    }
  }
#endif
}
