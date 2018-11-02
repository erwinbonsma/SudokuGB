void initConstraintTables();

class SudokuCell {
  friend class Sudoku;

  // Indices into constraint groups
  int _x; // Column
  int _y; // Row
  int _b; // Block

  // Index of cell
  int _index;

protected:
  int _colMask;
  int _rowMask;
  int _blockMask;
  int _value;
  bool _fixed;

public:
  /* Mask that indicates what (bit) values are allowed.
   */
  int bitMask() { return _colMask & _rowMask & _blockMask; }

  bool isBitAllowed(int bit);

  int getBitValue() { return _value; }
  int isSet() { return _value != 0; }
  bool isFixed() { return _fixed; }

  void init(int cellIndex);
};

enum class AutoSetResult : int {
  AlreadySet,
  CellUpdated,
  MultipleOptions,
  Stuck
};

class Sudoku {
  SudokuCell _cells[81];

  int _colMasks[9];
  int _rowMasks[9];
  int _blockMasks[9];

  int _numFilled;

public:
  void init();

  // Getters
  SudokuCell& cellAt(int x, int y) { return _cells[x + y * 9]; }
  SudokuCell& cellAt(int cellIndex) { return _cells[cellIndex]; }
  int getValue(int x, int y);
  bool isFixed(int x, int y);
  bool isSet(int x, int y);

  // Setters
  void clearValue(int x, int y);
  bool nextValue(int x, int y);

  void draw();

  // Lower-level methods
  void updateBitMasks(SudokuCell& cell, int oldBit, int (*updateFun)(int, int));
  void clearValue(SudokuCell& cell);
  void setBitValue(SudokuCell& cell, int bit);
  bool nextValue(SudokuCell& cell);

  /* Sets the given cell if it only has one allowed value and is not yet set.
   */
  AutoSetResult autoSet(SudokuCell& cell);
};

extern Sudoku sudoku;
