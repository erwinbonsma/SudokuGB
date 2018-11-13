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

class Sudoku;

class SudokuCell {
  friend class Sudoku;
  friend class Solver;
  friend class Stripper;

  // Index of cell
  int _index;

  Sudoku* _parent;

protected:
  int _value;
  bool _fixed;

  // The constraint groups that this cell is part of
  int _constraintGroup[maxConstraintsPerCell];

  bool _allowedUsesLastConstraint;
  bool _possibleUsesLastConstraint;

  int bitMask(bool applyLastConstraint);

  /* Mask that indicates what (bit) values are allowed.
   *
   * Note: It is only valid when the cell is not yet set.
   */
  int allowedBitMask() { return bitMask(_allowedUsesLastConstraint); }

  /* Mask that indicates what (bit) values are possible. This can be fewer than
   * are allowed when the cell is part of an implicit hyper-box.
   *
   * Note: It is only valid when the cell is not yet set.
   */
  int possibleBitMask() { return bitMask(_possibleUsesLastConstraint); }

public:
  void init(Sudoku* parent, int cellIndex);

  void reset();

  bool isBitAllowed(int bit);
  bool isBitPossible(int bit);

  bool hasOneAllowedValue();
  bool hasOnePossibleValue();

  int getBitValue() { return _value; }
  bool isSet() { return _value != 0; }
  bool isFixed() { return _fixed; }

  int index() { return _index; }
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
  friend class SudokuCell;
  friend class Solver;

  SudokuCell _cells[numCells];

  // Checks for each constraint group the values that still need to be filled.
  int _constraintMask[numConstraintGroups];

  bool _autoFix;
  bool _hyperConstraints;

  int _numFilled;
  int _numFixed;

public:
  // Should be called once.
  void init();

  // Instance "constructors" that reset the puzzle as if creating a new instance.
  void reset(bool hyperConstraints);
  void reset(Sudoku& sudoku);

  // Getters
  SudokuCell& cellAt(int x, int y) { return _cells[x + y * numCols]; }
  SudokuCell& cellAt(int cellIndex) { return _cells[cellIndex]; }
  int getValue(int x, int y);
  bool isFixed(int x, int y);
  bool isSet(int x, int y);
  bool isSolved() { return _numFilled == numCells; }
  bool solveInProgress() { return _numFixed > 0 && _numFilled > _numFixed; }
  int numFilled() { return _numFilled; }

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
