#include <Gamebuino-Meta.h>

#include "Utils.h"
#include "Solver.h"
#include "Globals.h"

Solver::Solver(Sudoku& s) : _s(s) {
  for (int i = 0; i < 81; i++) {
    _offsets[i] = 0;
  }
}

bool Solver::postAutoSet(SudokuCell& cell) {
  // Record the cell that has been auto-set, to enable undo when backtracking
  _autoSetCells[_totalAutoSet++] = cell.index();

  SerialUSB.printf("postAutoSet: totalAutoSet = %d, cell = %d\n", _totalAutoSet, cell.index());

  // Recurse to maybe set more
  return postSet(cell);
}

void Solver::autoClear(int num) {
  while (num > 0) {
    SerialUSB.printf("autoClear: totalAutoSet = %d, cell = %d\n", _totalAutoSet, _s.cellAt(_autoSetCells[_totalAutoSet - 1]));

    _s.clearValue(_s.cellAt(_autoSetCells[--_totalAutoSet]));
    num--;
  }
}

bool Solver::checkSingleValue(int cellIndex) {
  SudokuCell& cell = _s.cellAt(cellIndex);
  AutoSetResult result = _s.autoSet(cell);

  if (result == AutoSetResult::CellUpdated) {
    SerialUSB.printf("Autoset 1-val: %d = %d\n", cellIndex, cell.getBitValue());
    return postAutoSet(cell);
  }

  // Signal when stuck
  return (result == AutoSetResult::Stuck);
}

bool Solver::checkSinglePosition(int mask, int* cellIndices) {
  for (int bit = 1; bit < 512; bit *= 2) {
    if ((mask & bit) > 0) {
      // Value not yet set in given group. Check possible positions

      int i = 0;
      int cnt = 0;
      int posIndex = -1;
      while (i < 9 && cnt < 2) {
        int ci = cellIndices[i];
        SudokuCell& cell = _s.cellAt(ci);
        if (!cell.isSet() && cell.isBitAllowed(bit)
        ) {
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
        SerialUSB.printf("Autoset 1-pos: %d = %d\n", posIndex, bit);
        _s.setBitValue(cell, bit);
        return postAutoSet(cell);
      }
    }
  }

  return false;
}

bool Solver::postSet(SudokuCell& cell) {
  SerialUSB.printf("postSet: cell = %d\n", cell.index());

  int* colIndices = colCells[cell.col()];
  int* rowIndices = rowCells[cell.row()];
  int* boxIndices = boxCells[cell.box()];

  for (int i = 0; i < 9; i++) {
    if (
      checkSingleValue(colIndices[i]) ||
      checkSingleValue(rowIndices[i]) ||
      checkSingleValue(boxIndices[i])
    ) {
      return true; // Stuck
    }
  }

  if (
    checkSinglePosition(_s._colMasks[cell.col()], colIndices) ||
    checkSinglePosition(_s._rowMasks[cell.row()], rowIndices) ||
    checkSinglePosition(_s._boxMasks[cell.box()], boxIndices)
  ) {
    return true; // Stuck
  }

  return false;
}

bool Solver::solve(int n) {
  if (n == 81) {
    return true; // Solved
  }

  SudokuCell& cell = _s.cellAt(n);
  if (cell.isSet()) {
    // Cell already auto-filled
    return solve(n + 1);
  }

  int i = 0;
  bool solved = false;
  int totalAutoSetBefore = _totalAutoSet;
  int bit = 1 << _offsets[n];
  while (i < 9 && !solved) {
    if (cell.isBitAllowed(bit)) {
      SerialUSB.printf("%d = %d\n", n, bit);
      _s.setBitValue(cell, bit);

      bool stuck = postSet(cell);
      if (!stuck) {
        if (solve(n + 1)) {
          if (_restore) {
            solved = true;
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
    if (bit > 256) {
      bit = 1;
    }
  }

  return solved;
}

bool Solver::solve() {
  _restore = false;
  _totalAutoSet = 0;
  return solve(0);
}

bool Solver::randomSolve() {
  for (int i = 0; i < 81; i++) {
    _offsets[i] = rand() % 9;
  }
  return solve();
}

