/*
 * Sudoku, a Gamebuino game
 *
 * Copyright 2018, Erwin Bonsma
 */

#ifndef __SOLVER_INCLUDED
#define __SOLVER_INCLUDED

#include "Sudoku.h"

//------------------------------------------------------------------------------

enum class SolutionCount : int {
  None = 0,
  One = 1,
  Multiple = 2
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

  int _numActiveConstraints;

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

  /* Sets the masks for the implicit constraints. This needs to happen on
   * puzzles that the user has been editing because he is allowed to violate
   * these constraints which can cause the bookkeeping to be corrupted. This
   * does not matter while the user is editing (as only the "allowed" masks are
   * used) but needs to be fixed before the solver starts (as it uses the
   * implicit "possible" masks).
   */
  bool setImplicitMasks();

  /* Tries to set any cells that can only have one value either because it can
   * have no other values, or because it is the only cell in a specific
   * constraint group that can have this value.
   *
   * This method can be called before the recursive solve starts in case there
   * may be cells that can be automatically set. This can significantly speed
   * up the solve time, especially when it becomes clear that the puzzle is not
   * solvable.
   */
  bool initialAutoSet();

  // Returns "true" if the termination criterion has been reached.
  bool solve(int n);

  /* Starts solving the possible. Returns the number of solutions found.
   *
   * The "restore" setting specifies if the puzzle should be restored to its
   * original state.
   * The setting "numSolutionsToFind" determines how many solutions to find
   * before terminating. This is typically one (to check if the puzzle can be
   * solved) or two (to establish if the solution is unique or not).
   */
  int findSolutions(bool restore, int numSolutionsToFind);

public:
  Solver(Sudoku& s);

  Sudoku& sudoku() { return _s; }

  bool solve();
  bool randomSolve();
  bool isSolvable();
  SolutionCount countSolutions();
};

#endif
