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

void generateNewPuzzle() {
  // Reset the puzzle
  sudoku.reset(true);

  // Solve it to generate a (random) solution
  //assertTrue(solver.randomSolve());
  assertTrue(solver.solve());

#ifdef DEVELOPMENT_OLD
  // Near solve
  int i = 0;
  while (sudoku.cellAt(i).isFixed()) {
    i++;
  }
  sudoku.clearValue(sudoku.cellAt(i));
#else
  // Now clear as many values as possible to create the actual puzzle
  //stripper.randomStrip();
  stripper.strip();
#endif

  sudoku.fixValues();
  solutionCount = SolutionCount::One;
  editingPuzzle = false;
}

void createNewPuzzle() {
  // Clear puzzle
  sudoku.reset(sudoku.hyperConstraintsEnabled());
  sudoku.setAutoFix(true);
  solutionCount = SolutionCount::Multiple;
  editingPuzzle = true;
}

const char* menuEntries[] = {
  "Back to puzzle",
  "Save puzzle",
  "Load puzzle",
  "Reset puzzle",
  "New puzzle",
  "Create puzzle"
};

void mainMenu() {
  uint8_t entry = gb.gui.menu("Main menu", menuEntries);

  switch (entry) {
    case 1:
      if (storePuzzle()) {
        gb.gui.popup("Puzzle saved", 40);
      }
      break;
    case 2:
      loadPuzzle();
      break;
    case 3:
      resetPuzzle();
      break;
    case 4:
      // Initiate puzzle creation, but wait a few frames before generating puzzle,
      // so OK sound is not (too) abruptly aborted
      generateNewPuzzleCountdown = 10;
      break;
    case 5:
      createNewPuzzle();
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
#ifdef DEVELOPMENT
    debug("canUpdateCell = %d\n", canUpdateCell);
    debug("editingPuzzle = %d, solveInProgress = %d\n", editingPuzzle, sudoku.solveInProgress());
    sudoku.dump();
#endif
    if (canUpdateCell) {
      if (sudoku.nextValue(cursorCol, cursorRow)) {
        return true;
      } else {
        gb.sound.fx(sfxNoValue);
      }
    }
  }
  else if (gb.buttons.pressed(BUTTON_B)) {
    debug("canClearCell = %d\n", canClearCell);
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
    debug("cellChanged: solveInProgress = %d, solCount = %d\n", sudoku.solveInProgress(), solutionCount);
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

  // Trigger puzzle generation in 2nd frame. This way generate message is still
  // shown first
  generateNewPuzzleCountdown = 2;
}

void loop() {
  while(!gb.update());
  gb.display.clear();
  gb.lights.clear();

  if (generateNewPuzzleCountdown > 0) {
    generateNewPuzzleCountdown--;

    if (generateNewPuzzleCountdown == 0) {
      gb.sound.stop(0); // Stop OK sound from menu
      generateNewPuzzle();
    }

    gb.display.setCursor(6, 28);
    gb.display.setColor(WHITE);
    gb.display.println("Generating puzzle");
  }
  else {
    draw(sudoku);
    update();
  }
}
