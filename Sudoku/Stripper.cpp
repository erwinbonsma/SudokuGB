#include "Stripper.h"

#include "Globals.h"
#include "Utils.h"

Stripper::Stripper(Sudoku& sudoku, Solver& solver)
  : _s(sudoku), _solver(solver) {

  for (int i = 0; i < 81; i++) {
    _p[i] = i;
  }
}

bool Stripper::hasOnePosition(int bit, int* cellIndices) {
  int cnt = 0;

  for (int i = 0; i < 9; i++) {
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
  for (int i = 0; i < 81; i++) {
    SudokuCell& cell = _s.cellAt(_p[i]);
    if (cell.hasOneAllowedValue()) {
      _s.clearValue(cell);
    }
    else {
      clearIfOnlyAllowedPosition(cell);
    }
  }
}

void Stripper::strip() {
  strip1();
}

void Stripper::randomStrip() {
  permute(_p, 81);
  strip();
}

