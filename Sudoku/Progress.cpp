/*
 * Sudoku, a Gamebuino game
 *
 * Copyright 2018, Erwin Bonsma
 */

#include <Gamebuino-Meta.h>

#include "Progress.h"

#include "Constants.h"
#include "Strings.h"

void drawPuzzleGenerationProgress(int numSteps, int maxSteps) {
  const char* text = gb.language.get(generatingPuzzle);
  gb.display.setCursor(40 - 2 * strlen(text), 26);
  gb.display.setColor(WHITE);
  gb.display.println(text);

  int progressLen = (progressBarLen * numSteps) / maxSteps;
  gb.display.setColor(GRAY);
  gb.display.drawRect(39 - progressBarLen / 2, 35, progressBarLen + 2, 5);
  gb.display.setColor(BLUE);
  gb.display.fillRect(40 - progressBarLen / 2, 36, progressLen, 3);
}

void signalPuzzleGenerationProgress(int numSteps, int maxSteps) {
  drawPuzzleGenerationProgress(numSteps, maxSteps);
  gb.update();
}

