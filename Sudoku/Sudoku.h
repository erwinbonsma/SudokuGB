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

  // Indices into constraint groups
  int _col; // Column
  int _row; // Row
  int _box; // Box

  // Index of cell
  int _index;

protected:
  int _colMask;
  int _rowMask;
  int _boxMask;
  int _value;
  bool _fixed;

public:
  /* Mask that indicates what (bit) values are allowed.
   */
  int bitMask() { return _colMask & _rowMask & _boxMask; }

  bool isBitAllowed(int bit);
  bool hasOneAllowedValue();

  int getBitValue() { return _value; }
  bool isSet() { return _value != 0; }
  bool isFixed() { return _fixed; }

  int index() { return _index; }
  int col() { return _col; }
  int row() { return _row; }
  int box() { return _box; }

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

  int _colMasks[numConstraintGroups];
  int _rowMasks[numConstraintGroups];
  int _boxMasks[numConstraintGroups];

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
