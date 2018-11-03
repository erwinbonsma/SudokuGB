#ifndef __SOLVER_INCLUDED
#define __SOLVER_INCLUDED

#include "Sudoku.h"

//----------------------------------------------------------------------------------------

class Solver {

  // The puzzle to solve
  Sudoku& _s;

  // Returns the total cells that have been automatically set
  int _totalAutoSet;

  // Stack of cell indices that were automatically set
  int _autoSetCells[81];

  // Determines if the solver should restore the puzzle to its original position or not
  bool _restore;

  /* Invoked after a cell has been automatically set. It records the cell to enable
   * backtracking. Furthermore, it checks if more cells can be automatically set.
   *
   * Returns true when stuck to signal that backtracking is required.
   */
  bool postAutoSet(SudokuCell& cell);

  /* Sets the given cell if it only has one possible value.
   *
   * Returns true when stuck to signal that backtracking is required.
   */
  bool checkSingleValue(int cellIndex);

  /* Checks for all values that are not yet filled in within a given group (row, column or
   * block) if there is only possible cell that can be assigned this value. If so it sets
   * this cell and continues the recursion
   *
   * Returns true when stuck to signal that backtracking is required.
   */
  bool checkSinglePosition(int mask, int* cellIndices);

  /* Invoked after a cell has been set. It checks if from this other cells can be
   * automatically set, and if so, does this.
   *
   * Returns true when stuck to signal that backtracking is required.
   */
  bool postSet(SudokuCell& cell);

  /* Undos the last "num" cells that have been set automatically by checkSingleValue and
   * checkSinglePosition.
   */
  void autoClear(int num);

  bool solve(int n);

public:
  Solver(Sudoku& s);

  bool solve();
};

#endif
