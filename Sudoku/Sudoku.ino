/*
 * Sudoku, a Gamebuino game
 *
 * Copyright 2018, Erwin Bonsma
 */

#include <Gamebuino-Meta.h>

#include "Utils.h"
#include "Globals.h"
#include "Drawing.h"
#include "Store.h"
#include "Progress.h"
#include "Strings.h"

// Globals
int cursorCol = 4;
int cursorRow = 4;
bool editingPuzzle = false;

Sudoku sudoku;

Solver solver(sudoku);
Stripper stripper(sudoku, solver);
SolutionCount solutionCount;

// Locals
int generateNewPuzzleCountdown;
bool wasSolved = false;

const Gamebuino_Meta::Sound_FX sfxNoValue[] = {
  { Gamebuino_Meta::Sound_FX_Wave::SQUARE, 0, 128, 0, 0, 75, 2 }
};

const Gamebuino_Meta::Sound_FX sfxSolved[] = {
  { Gamebuino_Meta::Sound_FX_Wave::SQUARE, 1, 127, 0, 0, 60, 3 },
  { Gamebuino_Meta::Sound_FX_Wave::NOISE, 1, 0, 0, 0, 0, 1 },
  { Gamebuino_Meta::Sound_FX_Wave::SQUARE, 1, 127, 0, 0, 56, 3 },
  { Gamebuino_Meta::Sound_FX_Wave::NOISE, 1, 0, 0, 0, 0, 1 },
  { Gamebuino_Meta::Sound_FX_Wave::SQUARE, 1, 127, 0, 0, 56, 3 },
  { Gamebuino_Meta::Sound_FX_Wave::NOISE, 1, 0, 0, 0, 0, 1 },
  { Gamebuino_Meta::Sound_FX_Wave::SQUARE, 0, 170, -2, 0, 50, 9 }
};

void resetPuzzle() {
  sudoku.resetValues();
}

/* Initiates puzzle generation.
 *
 * As this takes several frames, it should not be executed as part of normal
 * flow. It will invoke gb.update() periodically to show progress. However, it
 * should not be relied upon that this is done often enough. Playing sounds may
 * therefore be distorted and are therefore stopped.
 */
void startPuzzleGeneration() {
  gb.sound.stop(0); // Stop any sound from playing (e.g. OK sound from menu)

  // Reset the puzzle
  sudoku.reset(sudoku.hyperConstraintsEnabled());

  // Solve it to generate a (random) solution
  assertTrue(solver.randomSolve());

  // Now clear as many values as possible to create the actual puzzle
  stripper.randomStrip();

  sudoku.fixValues();
  solutionCount = SolutionCount::One;
  editingPuzzle = false;
}

void generateNewPuzzle(bool delay) {
  if (delay) {
    // Initiate puzzle creation, but wait a few frames before generating puzzle,
    // so OK sound is not (too) abruptly aborted
    generateNewPuzzleCountdown = 8;
  } else {
    // Trigger puzzle generation in 2nd frame. This way generate message is
    // still shown first
    generateNewPuzzleCountdown = 2;
  }
}

void createNewPuzzle() {
  // Clear puzzle
  sudoku.reset(sudoku.hyperConstraintsEnabled());
  sudoku.setAutoFix(true);
  solutionCount = SolutionCount::Multiple;
  editingPuzzle = true;
}

#define NUM_MENU_ENTRIES 7
const char* menuEntries[NUM_MENU_ENTRIES];

int initMenuEntries() {
  int i = 0;
  int langIndex = getLanguageIndex();

  while (i < 5) {
    menuEntries[i] = menuStrings[i][langIndex];
    i++;
  }
  menuEntries[i++] = (
    sudoku.hyperConstraintsEnabled()
    ? menuStrings[6]
    : menuStrings[5]
  )[langIndex];
  menuEntries[i++] = menuStrings[7][langIndex];
  return i;
}

void mainMenu() {
  int numItems = initMenuEntries();
  uint8_t entry = gb.gui.menu(
    menuTitle[getLanguageIndex()], menuEntries, numItems
  );

  switch (entry) {
    case 0:
      if (storePuzzle(true)) {
        gb.gui.popup(puzzleSaved, 40);
      }
      break;
    case 1:
      if (!loadPuzzle(true)) {
        gb.gui.popup(loadFailed, 40);
      }
      break;
    case 2:
      resetPuzzle();
      break;
    case 3:
      generateNewPuzzle(true);
      break;
    case 4:
      createNewPuzzle();
      break;
    case 5:
      // Auto-store current puzzle.
      storePuzzle(false);
      sudoku.reset(!sudoku.hyperConstraintsEnabled());
      if (!loadPuzzle(false)) {
        // No puzzle was auto-stored yet. Generate one.
        generateNewPuzzle(true);
      }
      break;
  }
}

// Returns true if cursor did move.
bool handleCursorMove() {
  bool cursorMoved = true;

  if (gb.buttons.pressed(BUTTON_LEFT)) {
    cursorCol = (cursorCol + numCols - 1) % numCols;
  }
  else if (gb.buttons.pressed(BUTTON_RIGHT)) {
    cursorCol = (cursorCol + 1) % numCols;
  }
  else if (gb.buttons.pressed(BUTTON_UP)) {
    cursorRow = (cursorRow + numRows - 1) % numRows;
  }
  else if (gb.buttons.pressed(BUTTON_DOWN)) {
    cursorRow = (cursorRow + 1) % numRows;
  }
  else {
    cursorMoved = false;
  }

  return cursorMoved;
}

// Returns true if cell value was changed
bool handleCellChange() {
  bool canUpdateCell = (
    !sudoku.isFixed(cursorCol, cursorRow) ||
    (editingPuzzle && !sudoku.solveInProgress())
  );
  bool canClearCell = sudoku.isSet(cursorCol, cursorRow) && (
    canUpdateCell ||
    (editingPuzzle && solutionCount == SolutionCount::None)
  );

  if (gb.buttons.pressed(BUTTON_A)) {
    if (canUpdateCell) {
      if (sudoku.nextValue(cursorCol, cursorRow)) {
        return true;
      } else {
        gb.sound.fx(sfxNoValue);
      }
    }
  }
  else if (gb.buttons.pressed(BUTTON_B)) {
    if (canClearCell) {
      sudoku.clearValue(cursorCol, cursorRow);
      return true;
    }
  }

  return false;
}

void update() {
  handleCursorMove();

  if (handleCellChange()) {
    if (editingPuzzle && !sudoku.solveInProgress()) {
      solutionCount = solver.countSolutions();
      sudoku.setAutoFix(solutionCount != SolutionCount::One);
    }
  }

  if (gb.buttons.pressed(BUTTON_MENU)) {
    mainMenu();
  }

  if (sudoku.isSolved() && !wasSolved) {
    gb.sound.fx(sfxSolved);
  }
  wasSolved = sudoku.isSolved();
}

void setup() {
  gb.begin();

#ifdef DEVELOPMENT
  initDebugLog();

  int gradientLightColors[39] = {
    // Yellow
    0x000000, 0x141300, 0x282600, 0x3c3900,
    0x504d01, 0x646001, 0x787301, 0x8c8601,
    0xa09a02, 0xb4ad02, 0xc8c002, 0xdcd302,
    0xf1e703,

    // Light blue
    0x000000, 0x0a0f15, 0x141f2a, 0x1f2e3f,
    0x293e55, 0x344d6a, 0x3e5d7f, 0x486d94,
    0x537caa, 0x5d8cbf, 0x689bd4, 0x72abe9,
    0x7dbbff,

    // Light green
    0x000000, 0x0b1107, 0x16220e, 0x213316,
    0x2c451d, 0x375624, 0x42672c, 0x4d7833,
    0x588a3a, 0x639b42, 0x6eac49, 0x79bd50,
    0x85cf58
  };

  for (int i = 0; i < 39; i++) {
    int hex = gradientLightColors[i];
    Color gbc = gb.createColor((hex >> 16) & 255, (hex >> 8) & 255, hex & 255);
    SerialUSB.printf("0x%04x, ", gbc);
  }
  SerialUSB.printf("\n");
#endif

  initConstraintTables();
  sudoku.init();

  generateNewPuzzle(false);
}

void loop() {
  while(!gb.update());
  gb.display.clear();
  gb.lights.clear();

  if (generateNewPuzzleCountdown > 0) {
    generateNewPuzzleCountdown--;

    if (generateNewPuzzleCountdown == 0) {
      startPuzzleGeneration();
    } else {
      signalPuzzleGenerationProgress(0, 100);
    }
  }
  else {
    draw(sudoku);
    update();
  }
}
