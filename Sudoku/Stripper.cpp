/*
 * Sudoku, a Gamebuino game
 *
 * Copyright 2018, Erwin Bonsma
 */

#include <Gamebuino-Meta.h>

#include "Stripper.h"

#include "Globals.h"
#include "Utils.h"
#include "Progress.h"

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

bool Stripper::hasOnePosition(int bit, SudokuCell& cell) {
  int i = maxConstraintsPerCell;
  if (!cell._possibleUsesLastConstraint) {
    i--;
  }

  while (--i >= 0) {
    int groupIndex = cell._constraintGroup[i];
    if (hasOnePosition(bit, constraintCells[groupIndex])) {
      return true;
    }
  }

  return false;
}

void Stripper::strip1() {
  signalPuzzleGenerationProgress(1, progressBarLen);

  for (int i = 0; i < numCells; i++) {
    SudokuCell& cell = _s.cellAt(_p[i]);
    int bit = cell.getBitValue();

    // Try clearing value
    _s.clearValue(cell);

    if (!cell.hasOnePossibleValue() && !hasOnePosition(bit, cell)) {
      // Undo clear
      _s.setBitValue(cell, bit);
    }
  }
}

void Stripper::strip2() {
  int numFilledAtStart = _s.numFilled();
  int numClearAttempts = 0;
  signalPuzzleGenerationProgress(2, 2 + numFilledAtStart);

  for (int i = 0; i < numCells; i++) {
    SudokuCell& cell = _s.cellAt(_p[i]);
    int bit0 = cell.getBitValue();
    if (bit0 > 0) {
      int bit = 1;
      bool unique = true;
      while (bit <= maxBitValue && unique) {
        if (bit != bit0 && cell.isBitPossible(bit)) {
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

      numClearAttempts++;
      signalPuzzleGenerationProgress(2 + numClearAttempts, 2 + numFilledAtStart);
    }
  }

  signalPuzzleGenerationProgress(100, 100);
}

void Stripper::strip() {
  strip1();
  debug("Solutions after strip1: %d\n", solver.countSolutions());
  strip2();
  debug("Solutions after strip2: %d\n", solver.countSolutions());
}

void Stripper::randomStrip() {
  permute(_p, numCells);
  strip();
}

