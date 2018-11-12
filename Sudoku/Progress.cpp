/*
 * Sudoku, a Gamebuino game
 *
 * Copyright 2018, Erwin Bonsma
 */

#include <Gamebuino-Meta.h>

#include "Progress.h"

#include "Constants.h"

void drawPuzzleGenerationProgress(int numSteps, int maxSteps) {
  gb.display.setCursor(6, 26);
  gb.display.setColor(WHITE);
  gb.display.println("Generating puzzle");

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

