/*
 * Sudoku, a Gamebuino game
 *
 * Copyright 2018, Erwin Bonsma
 */

#ifndef __SUDOKU_INCLUDED
#define __SUDOKU_INCLUDED

#include "Constants.h"
#include "Utils.h"

void initConstraintTables();

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
  int _constraintGroup[numConstraintsPerCell];

  // Tracks the allowed values for this cell for each constraint group that
  // contains this cell.
  int _constraintMask[numConstraintsPerCell];

public:
  /* Mask that indicates what (bit) values are allowed.
   */
  int bitMask();

  bool isBitAllowed(int bit);
  bool hasOneAllowedValue();

  int getBitValue() { return _value; }
  bool isSet() { return _value != 0; }
  bool isFixed() { return _fixed; }

  int index() { return _index; }

  void init(int cellIndex);
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

  int _numFilled;
  int _numFixed;

public:
  void init();
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
  bool nextValue(int x, int y);

  void setAutoFix(bool autoFix) { _autoFix = autoFix; }
  bool isAutoFixEnabled() { return _autoFix; }

  void fixValues();
  void unfixValues();
  void resetValues();

  void draw();

  // Lower-level methods
  void updateBitMasks(SudokuCell& cell, int oldBit, int (*updateFun)(int, int));
  void clearValue(SudokuCell& cell);
  void setBitValue(SudokuCell& cell, int bit);
  bool nextValue(SudokuCell& cell);

  /* Sets the given cell if it only has one allowed value and is not yet set.
   */
  AutoSetResult autoSet(SudokuCell& cell);

#ifdef DEVELOPMENT
  void dump();
#endif
};

#endif
