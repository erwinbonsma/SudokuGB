#include "Stripper.h"

#include "Globals.h"

Stripper::Stripper(Sudoku& sudoku, Solver& solver)
  : _s(sudoku), _solver(solver) {
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
    SudokuCell& cell = _s.cellAt(i);
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

