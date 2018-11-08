#ifndef __CONSTANTS_INCLUDED
#define __CONSTANTS_INCLUDED

const int numRows = 9;
const int numCols = 9;

const int numValues = 9;
const int numCells = numRows * numCols;

const int numConstraintGroups = 9;
const int constraintGroupSize = numValues;

const int maxBitValue = 1 << (numValues - 1);
// The bit mask with bits for each value set
const int maxBitMask = (1 << numValues) - 1;

#endif
