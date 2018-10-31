void initConstraintTables();

class SudokuCell {
  friend class Sudoku;

protected:
  int _colMask;
  int _rowMask;
  int _blockMask;
  int _value;
  bool _fixed;

public:
  void init();
};

class Sudoku {
  SudokuCell _cells[81];

  int _colMasks[9];
  int _rowMasks[9];
  int _blockMasks[9];

  int _numFilled;

protected:
  int _getValue(int cellIndex) { return _cells[cellIndex]._value; }
  int _isFixed(int cellIndex) { return _cells[cellIndex]._fixed; }
  int _isSet(int cellIndex) { return _cells[cellIndex]._value != 0; }
  bool _isAllowed(int cellIndex, int bit);
  int _bit2value(int bit);

  void _updateMasks(int cellIndex, int oldBit, int (*updateFun)(int, int));
  void _clear(int cellIndex);
  void _setValue(int cellIndex, int bit);
  bool _nextValue(int cellIndex);

public:
  void init();

  int getValue(int x, int y);
  bool isFixed(int x, int y);
  bool isSet(int x, int y);

  void clear(int x, int y);
  bool nextValue(int x, int y);

  void draw();
};

extern Sudoku sudoku;
