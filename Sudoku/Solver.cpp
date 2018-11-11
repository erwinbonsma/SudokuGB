/*
 * Sudoku, a Gamebuino game
 *
 * Copyright 2018, Erwin Bonsma
 */

#include <Gamebuino-Meta.h>

#include "Utils.h"
#include "Solver.h"
#include "Globals.h"

Solver::Solver(Sudoku& s) : _s(s) {
  for (int i = 0; i < numCells; i++) {
    _offsets[i] = 0;
  }
}

bool Solver::postAutoSet(SudokuCell& cell) {
  // Record the cell that has been auto-set, to enable undo when backtracking
  _autoSetCells[_totalAutoSet++] = cell.index();

  // Recurse to maybe set more
  return postSet(cell);
}

void Solver::autoClear(int num) {
  while (num-- > 0) {
    _s.clearValue(_s.cellAt(_autoSetCells[--_totalAutoSet]));
  }
}

bool Solver::checkSingleValue(int cellIndex) {
  SudokuCell& cell = _s.cellAt(cellIndex);
  AutoSetResult result = _s.autoSet(cell);

  if (result == AutoSetResult::CellUpdated) {
    return postAutoSet(cell);
  }

  // Signal when stuck
  return (result == AutoSetResult::Stuck);
}

bool Solver::checkSinglePosition(int mask, int* cellIndices) {
  for (int bit = 1; bit <= maxBitValue; bit *= 2) {
    if ((mask & bit) != 0) {
      // Value not yet set in given group. Check possible positions

      int i = 0;
      int cnt = 0;
      int posIndex = -1;
      while (i < constraintGroupSize && cnt < 2) {
        int ci = cellIndices[i];
        SudokuCell& cell = _s.cellAt(ci);
        if (!cell.isSet() && cell.isBitPossible(bit)) {
          posIndex = ci;
          cnt++;
        }
        i++;
      }
      if (cnt == 0) {
        return true;
      }
      if (cnt == 1) {
        SudokuCell& cell = _s.cellAt(posIndex);
        _s.setBitValue(cell, bit);
        return postAutoSet(cell);
      }
    }
  }

  return false;
}

bool Solver::postSet(SudokuCell& cell) {
  for (int i = 0; i < maxConstraintsPerCell; i++) {
    int groupIndex = cell._constraintGroup[i];
    int* cellIndices = constraintCells[groupIndex];
    for (int j = 0; j < constraintGroupSize; j++) {
      if (checkSingleValue(cellIndices[j])) {
        return true; // Stuck
      }
    }
  }

  for (int i = _numActiveConstraints; --i >= 0; ) {
    if (checkSinglePosition(_s._constraintMask[i], constraintCells[i])) {
      return true; // Stuck
    }
  }

  return false;
}

bool Solver::setImplicitMasks() {
  if (!_s.hyperConstraintsEnabled()) {
    return false;
  }

  for (int i = numExplicitConstraintGroups; i < numConstraintGroups; i++) {
    int m = maxBitMask;
    int* cellIndices = constraintCells[i];
    for (int j = 0; j < constraintGroupSize; j++) {
      int val = _s.cellAt(cellIndices[j]).getBitValue();
      if (val > 0) {
        if ((m & val) != 0) {
          m = clearBit(m, val);
        } else {
          // Within this implicit constraint group one value occurs more than
          // once. That is allowed, but prevents a solution.
          return true; // Stuck
        }
      }
    }
    _s._constraintMask[i] = m;
  }

  return false;
}

bool Solver::initialAutoSet() {
  // Check if each cell still has possible values
  for (int i = 0; i < numCells; i++) {
    if (checkSingleValue(i)) {
      return true; // Stuck
    }
  }

  // Checks if each value in a constraint group still has allowed positions
  for (int i = _numActiveConstraints; --i >= 0; ) {
    if (checkSinglePosition(_s._constraintMask[i], constraintCells[i])) {
      return true; // Stuck
    }
  }

  return false;
}

bool Solver::solve(int n) {
  if (n == numCells) {
    _numSolutionsFound++;
    return (_numSolutionsFound == _numSolutionsToFind);
  }

  SudokuCell& cell = _s.cellAt(n);
  if (cell.isSet()) {
    // Cell already auto-filled
    return solve(n + 1);
  }

  int i = 0;
  bool terminate = false;
  int totalAutoSetBefore = _totalAutoSet;
  int bit = 1 << _offsets[n];
  while (i < numValues && !terminate) {
    if (cell.isBitPossible(bit)) {
      _s.setBitValue(cell, bit);

      bool stuck = postSet(cell);
      if (!stuck) {
        if (solve(n + 1)) {
          if (_restore) {
            terminate = true;
          }
          else {
            return true;
          }
        }
      }

      autoClear(_totalAutoSet - totalAutoSetBefore);
      _s.clearValue(cell);
    }

    i++;
    bit <<= 1;
    if (bit > maxBitValue) {
      bit = 1;
    }
  }

  return terminate;
}

int Solver::findSolutions(bool restore, int numSolutionsToFind) {
  _restore = restore;
  _numSolutionsToFind = numSolutionsToFind;

  _numSolutionsFound = 0;
  _totalAutoSet = 0;
  _numActiveConstraints = (
    _s.hyperConstraintsEnabled()
    ? numConstraintGroups
    : numBasicConstraintGroups
  );

  if (!setImplicitMasks()) {
    if (!initialAutoSet()) {
      solve(0);
    }

    if (restore) {
      // Clear cells set by autoSet()
      autoClear(_totalAutoSet);
    }
  }

  return _numSolutionsFound;
}

bool Solver::solve() {
  return (findSolutions(false, 1) == 1);
}

bool Solver::randomSolve() {
  for (int i = 0; i < numCells; i++) {
    _offsets[i] = rand() % numValues;
  }
  return solve();
}

bool Solver::isSolvable() {
  return (findSolutions(true, 1) == 1);
}

SolutionCount Solver::countSolutions() {
  return (SolutionCount)findSolutions(true, 2);
}

