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
  int* colIndices = colCells[cell.col()];
  int* rowIndices = rowCells[cell.row()];
  int* boxIndices = boxCells[cell.box()];

  int bit = cell.getBitValue();
  // Try clearing value
  _s.clearValue(cell);

  // Check if there's only one position for any of the constraints
  if (!(
    hasOnePosition(bit, colIndices) ||
    hasOnePosition(bit, rowIndices) ||
    hasOnePosition(bit, boxIndices)
  )) {
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
    debug("strip2: %d\n", i);

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
        debug("strip2: Clearing %d\n", cell.index());
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

