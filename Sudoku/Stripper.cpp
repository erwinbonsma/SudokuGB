/*
 * Sudoku, a Gamebuino game
 *
 * Copyright 2018, Erwin Bonsma
 */

#include <Gamebuino-Meta.h>

#include "Stripper.h"

#include "Globals.h"
#include "Utils.h"

Stripper::Stripper(Sudoku& sudoku, Solver& solver)
  : _s(sudoku), _solver(solver) {

  assertTrue( &(_solver.sudoku()) == &_s );

  for (int i = 0; i < numCells; i++) {
    _p[i] = i;
  }
}

bool Stripper::hasOnePosition(int bit, int* cellIndices) {
  int cnt = 0;

  for (int i = 0; i < constraintGroupSize; i++) {
    if (_s.cellAt(cellIndices[i]).isBitAllowed(bit)) {
      cnt++;
      if (cnt > 1) {
        return false;
      }
    }
  }

  return true;
}

void Stripper::clearIfOnlyAllowedPosition(SudokuCell& cell) {
  int bit = cell.getBitValue();

  // Try clearing value
  _s.clearValue(cell);

  bool onePos = false;
  for (int i = 0; i < numConstraintsPerCell; i++) {
    int groupIndex = cell._constraintGroup[i];
    if (hasOnePosition(bit, constraintCells[groupIndex])) {
      onePos = true;
    }
  }

  if (!onePos) {
    // Undo clear
    _s.setBitValue(cell, bit);
  }
}

void Stripper::strip1() {
  for (int i = 0; i < numCells; i++) {
    SudokuCell& cell = _s.cellAt(_p[i]);
    if (cell.hasOneAllowedValue()) {
      _s.clearValue(cell);
    }
    else {
      clearIfOnlyAllowedPosition(cell);
    }
  }
}

void Stripper::strip2() {
  for (int i = 0; i < numCells; i++) {
    SudokuCell& cell = _s.cellAt(_p[i]);
    int bit0 = cell.getBitValue();
    if (bit0 > 0) {
      int bit = 1;
      bool unique = true;
      while (bit <= maxBitValue && unique) {
        if (bit != bit0 && cell.isBitAllowed(bit)) {
          _s.setBitValue(cell, bit);
          if (_solver.isSolvable()) {
            unique = false;
          }
        }
        bit <<= 1;
      }
      if (unique) {
        _s.clearValue(cell);
      } else {
        // Restore cell to its original value
        _s.setBitValue(cell, bit0);
      }
    }
  }
}

void Stripper::strip() {
  strip1();
  strip2();
}

void Stripper::randomStrip() {
  permute(_p, numCells);
  strip();
}

