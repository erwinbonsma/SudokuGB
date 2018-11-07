#ifndef __SOLVER_INCLUDED
#define __SOLVER_INCLUDED

#include "Sudoku.h"

//------------------------------------------------------------------------------

enum class SolutionCount : int {
  Multiple,
  One,
  None
};

//------------------------------------------------------------------------------

class Solver {

  // The puzzle to solve
  Sudoku& _s;

  // Returns the total cells that have been automatically set
  int _totalAutoSet;

  // Stack of cell indices that were automatically set
  int _autoSetCells[numCells];

  // Offsets that help to randomize the solve (mainly useful for generating
  // puzzles)
  int _offsets[numCells];

  // Specifies if the solver should restore the puzzle to its original position
  // or not
  bool _restore;

  // Specifies many solutions the solver should find before terminating.
  int _numSolutionsToFind;

  int _numSolutionsFound;

  /* Invoked after a cell has been automatically set. It records the cell to
   * enable backtracking. Furthermore, it checks if more cells can be
   * automatically set.
   *
   * Returns true when stuck to signal that backtracking is required.
   */
  bool postAutoSet(SudokuCell& cell);

  /* Sets the given cell if it only has one possible value.
   *
   * Returns true when stuck to signal that backtracking is required.
   */
  bool checkSingleValue(int cellIndex);

  /* Checks for all values that are not yet filled in within a given group (row,
   * column or block) if there is only possible cell that can be assigned this
   * value. If so it sets this cell and continues the recursion
   *
   * Returns true when stuck to signal that backtracking is required.
   */
  bool checkSinglePosition(int mask, int* cellIndices);

  /* Invoked after a cell has been set. It checks if from this other cells can
   * be automatically set, and if so, does this.
   *
   * Returns true when stuck to signal that backtracking is required.
   */
  bool postSet(SudokuCell& cell);

  /* Undos the last "num" cells that have been set automatically by
   * checkSingleValue and checkSinglePosition.
   */
  void autoClear(int num);

  // Returns "true" if the termination criterion has been reached.
  bool solve(int n);

public:
  Solver(Sudoku& s);

  Sudoku& sudoku() { return _s; }

  bool solve();
  bool randomSolve();
  bool isSolvable();
  SolutionCount countSolutions();
};

#endif
