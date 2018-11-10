/*
 * Sudoku, a Gamebuino game
 *
 * Copyright 2018, Erwin Bonsma
 */

#include "Sudoku.h"
#include "Solver.h"
#include "Stripper.h"

extern int cursorCol;
extern int cursorRow;
extern bool editingPuzzle;

extern SolutionCount solutionCount;

extern Sudoku sudoku;

extern Solver solver;
extern Stripper stripper;

// Constraint tables, implemented in Sudoku.cpp
extern int constraintCells[numConstraintGroups][constraintGroupSize];

