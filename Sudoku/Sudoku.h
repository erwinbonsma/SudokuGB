/*
 * Sudoku, a Gamebuino game
 *
 * Copyright 2018, Erwin Bonsma
 */

#ifndef __SUDOKU_INCLUDED
#define __SUDOKU_INCLUDED

#include "Constants.h"
#include "Utils.h"

bool isPartOfHyperBox(int x, int y);

void initConstraintTables();

// TMP
extern int bitToValue(int bit);

class SudokuCell {
  friend class Sudoku;
  friend class Solver;
  friend class Stripper;

  // Index of cell
  int _index;

protected:
  int _value;
  bool _fixed;

  // The constraint groups that this cell is part of
  int _constraintGroup[maxConstraintsPerCell];

  // Tracks the allowed values for this cell for each constraint group that
  // contains this cell.
  int _constraintMask[maxConstraintsPerCell];

  int _numAllowedConstraints;

public:
  /* Mask that indicates what (bit) values are allowed.
   */
  int allowedBitMask();

  /* Mask that indicates what (bit) values are possible. This can be fewer than
   * are allowed when the cell is part of an implicit hyper-box.
   */
  int possibleBitMask();

  bool isBitAllowed(int bit);
  bool isBitPossible(int bit);

  bool hasOneAllowedValue();
  bool hasOnePossibleValue();


  /* Returns true when the cell is set to a value that is not possible. This can
   * happen when the cell is part of an implicit hyper-box constraint group (as
   * this does not disallow values).
   */
  bool hasImpossibleValue();

  int getBitValue() { return _value; }
  bool isSet() { return _value != 0; }
  bool isFixed() { return _fixed; }

  int index() { return _index; }

  void init(int cellIndex, bool hyperConstraints);
};

//------------------------------------------------------------------------------

enum class AutoSetResult : int {
  AlreadySet,
  CellUpdated,
  MultipleOptions,
  Stuck
};

//------------------------------------------------------------------------------

class Sudoku {
  friend class Solver;

  SudokuCell _cells[numCells];

  // Checks for each constraint group the values that still need to be filled.
  int _constraintMask[numConstraintGroups];

  bool _autoFix;
  bool _hyperConstraints;

  int _numFilled;
  int _numFixed;

public:
  void init();
  void init(bool hyperConstraints);
  void init(Sudoku& sudoku);

  // Getters
  SudokuCell& cellAt(int x, int y) { return _cells[x + y * numCols]; }
  SudokuCell& cellAt(int cellIndex) { return _cells[cellIndex]; }
  int getValue(int x, int y);
  bool isFixed(int x, int y);
  bool isSet(int x, int y);
  bool isSolved() { return _numFilled == numCells; }
  bool solveInProgress() { return _numFixed > 0 && _numFilled > _numFixed; }

  // Setters
  void setValue(int x, int y, int value);
  void clearValue(int x, int y);
  void fixValue(int x, int y);

  /* Sets the cell to the next allowed value. Returns false if the cell could
   * not be changed (because it was not set but no value is allowed)
   */
  bool nextValue(int x, int y);

  void setAutoFix(bool autoFix) { _autoFix = autoFix; }
  bool isAutoFixEnabled() { return _autoFix; }
  bool hyperConstraintsEnabled() { return _hyperConstraints; }

  void fixValues();
  void unfixValues();
  void resetValues();

  void draw();

  // Lower-level methods
  void updateBitMasks(SudokuCell& cell, int oldBit, int (*updateFun)(int, int));
  void clearValue(SudokuCell& cell);
  void setBitValue(SudokuCell& cell, int bit);
  bool nextValue(SudokuCell& cell);

  /* Sets the given cell if it only has one possible value and is not yet set.
   */
  AutoSetResult autoSet(SudokuCell& cell);

  void dump();
};

#endif
