/*
 * Sudoku, a Gamebuino game
 *
 * Copyright 2018, Erwin Bonsma
 */

#include <Gamebuino-Meta.h>

#include "Store.h"

#include "Globals.h"
#include "Constants.h"
#include "Utils.h"

const uint8_t cellIsFixedBit = 1 << 4;

const uint8_t editingModeBit   = 0x01;
const uint8_t autoFixBit       = 0x02; // Only used in editing mode
const uint8_t multiSolutionBit = 0x04; // Only used in editing mode
const uint8_t noSolutionBit    = 0x08; // Only used in editing mode

const int storeBufferSize = numCells + 1;
uint8_t storeBuffer[storeBufferSize];

bool isStoreBufferEmpty() {
  for (int i = 0; i < storeBufferSize; i++) {
    if (storeBuffer[i] != 0) {
      return false;
    }
  }

  return true;
}

int targetBlockIndex(bool userAction) {
  int blockIndex = sudoku.hyperConstraintsEnabled() ? 1 : 0;
  if (!userAction) {
    blockIndex += 2;
  }
  return blockIndex;
}

bool storePuzzle(bool userAction) {
  for (int y = 0; y < numRows; y++) {
    for (int x = 0; x < numCols; x++) {
      uint8_t val = sudoku.getValue(x, y);
      if (sudoku.isFixed(x, y)) {
        val |= cellIsFixedBit;
      }
      storeBuffer[x + y * numCols] = val;
    }
  }

  uint8_t mode = 0;
  if (editingPuzzle && !sudoku.solveInProgress()) {
    // Only store with editing mode when the user did not start solving. This
    // way, storing and loading is a way to enable a pure solve that does not
    // allow the user to modify the puzzle.
    mode |= editingModeBit;
    if (sudoku.isAutoFixEnabled()) {
      mode |= autoFixBit;
    }
    if (solutionCount == SolutionCount::Multiple) {
      mode |= multiSolutionBit;
    }
    else if (solutionCount == SolutionCount::None) {
      mode |= noSolutionBit;
    }
  }
  storeBuffer[numCells] = mode;

  return gb.save.set(
    targetBlockIndex(userAction), (void*)storeBuffer, storeBufferSize
  );
}

bool loadPuzzle(bool userAction) {
  // Clear buffer before reading. Although it should not be needed, better safe
  // than sorry.
  for (int i = 0; i < storeBufferSize; i++) {
    storeBuffer[i] = (uint8_t)0;
  }

  if ( !gb.save.get(
    targetBlockIndex(userAction), (void*)storeBuffer, storeBufferSize
  )) {
    return false;
  }

  if (isStoreBufferEmpty()) {
    return false;
  }

  sudoku.reset(sudoku.hyperConstraintsEnabled());
  for (int y = 0; y < numRows; y++) {
    for (int x = 0; x < numCols; x++) {
      uint8_t val = storeBuffer[x + y * numCols];
      int value = val & 0x0f;
      assertTrue(value <= 9);

      if (value > 0) {
        sudoku.setValue(x, y, value);
      }
      if ((val & cellIsFixedBit) != 0) {
        assertTrue(sudoku.isSet(x, y));
        sudoku.fixValue(x, y);
      }
    }
  }

  uint8_t mode = storeBuffer[numCells];
  editingPuzzle = (mode & editingModeBit) != 0;
  sudoku.setAutoFix((mode & autoFixBit) != 0);
  solutionCount = SolutionCount::One;
  if ((mode & multiSolutionBit) != 0) {
    solutionCount = SolutionCount::Multiple;
  }
  else if ((mode & noSolutionBit) != 0) {
    solutionCount = SolutionCount::None;
  }

  return true;
}

