#include "Sudoku.h"
#include "Solver.h"
#include "Stripper.h"

extern int cursorX;
extern int cursorY;

extern SolutionCount solutionCount;

extern Sudoku sudoku;

extern Solver solver;
extern Stripper stripper;

// Constraint tables, implemented in Sudoku.cpp
extern int colCells[numConstraintGroups][constraintGroupSize];
extern int rowCells[numConstraintGroups][constraintGroupSize];
extern int boxCells[numConstraintGroups][constraintGroupSize];

