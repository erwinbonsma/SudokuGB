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
const int numHyperBoxes = 4;
// Even though there are only four visible hyper-boxes, there are five implicit ones.
const int numImplicitHyperBoxes = 5;

const int numValues = 9;
const int numCells = numRows * numCols;

const int numBasicConstraintGroups = numCols + numRows + numBoxes;
const int numExplicitConstraintGroups = numBasicConstraintGroups + numHyperBoxes;
const int numConstraintGroups = numExplicitConstraintGroups + numImplicitHyperBoxes;
const int maxConstraintsPerCell = 4;
const int constraintGroupSize = numValues;

const int maxBitValue = 1 << (numValues - 1);
// The bit mask with bits for each value set
const int maxBitMask = (1 << numValues) - 1;

const int progressBarLen = 64;

#endif
