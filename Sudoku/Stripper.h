/*
 * Sudoku, a Gamebuino game
 *
 * Copyright 2018, Erwin Bonsma
 */

#ifndef __STRIPPER_INCLUDED
#define __STRIPPER_INCLUDED

#include "Solver.h"
#include "Sudoku.h"

//------------------------------------------------------------------------------

/* Clears values from a Sudoku that are not needed to ensure it has a unique
 * solution until no more values can be cleared.
 */
class Stripper {
  // The puzzle to strip
  Sudoku& _s;

  Solver& _solver;

  // Permutation
  int _p[numCells];

protected:
  bool hasOnePosition(int bit, int* cellIndices);

  void clearIfOnlyAllowedPosition(SudokuCell& cell);

  /* First stripping phase. All cells are cleared whose value can be directly
   * inferred given the other filled cells.
   */
  void strip1();

  /* Second stripping phase. It clears all cells whose value is not required to
   * ensure the solution remains unique.
   */
  void strip2();

public:
  Stripper(Sudoku& sudoku, Solver& solver);

  void strip();
  void randomStrip();
};

#endif
