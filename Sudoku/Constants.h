/*
 * Sudoku, a Gamebuino game
 *
 * Copyright 2018, Erwin Bonsma
 */

#ifndef __CONSTANTS_INCLUDED
#define __CONSTANTS_INCLUDED

const int numCols = 9;
const int numRows = 9;
const int numBoxes = 9;

const int numValues = 9;
const int numCells = numRows * numCols;

const int numConstraintGroups = numCols + numRows + numBoxes;
const int numConstraintsPerCell = 3;
const int constraintGroupSize = numValues;

const int maxBitValue = 1 << (numValues - 1);
// The bit mask with bits for each value set
const int maxBitMask = (1 << numValues) - 1;

#endif
