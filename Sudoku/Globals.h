#include "Sudoku.h"
#include "Solver.h"

extern int cursorX;
extern int cursorY;

extern Sudoku sudoku;
extern Sudoku sudokuForSolver;

extern Solver solver;

// Constraint tables, implemented in Sudoku.cpp
extern int colCells[9][9];
extern int rowCells[9][9];
extern int boxCells[9][9];

