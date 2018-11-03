#include "Sudoku.h"
#include "Solver.h"
#include "Stripper.h"

extern int cursorX;
extern int cursorY;

extern Sudoku sudoku;

extern Solver solver;
extern Stripper stripper;

// Constraint tables, implemented in Sudoku.cpp
extern int colCells[9][9];
extern int rowCells[9][9];
extern int boxCells[9][9];

