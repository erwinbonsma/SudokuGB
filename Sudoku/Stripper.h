#ifndef __STRIPPER_INCLUDED
#define __STRIPPER_INCLUDED

#include "Solver.h"
#include "Sudoku.h"

//------------------------------------------------------------------------------

class Stripper {
  // The puzzle to strip
  Sudoku& _s;

  Solver& _solver;

  // Permutation
  int _p[81];

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
