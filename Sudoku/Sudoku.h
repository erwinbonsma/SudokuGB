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
  int _bit2value(int bit);

public:
  void init();

  int getValue(int x, int y);

  void draw();
};

extern Sudoku sudoku;
