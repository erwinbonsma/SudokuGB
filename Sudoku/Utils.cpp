/*
 * Sudoku, a Gamebuino game
 *
 * Copyright 2018, Erwin Bonsma
 */

#include <Gamebuino-Meta.h>

#include "Utils.h"

void initDebugLog() {
  SerialUSB.begin(9600);
  while (!SerialUSB);
}

void permute(int* list, int len) {
  for (int i = 0; i < len; i++) {
    int j = i + rand() % (len - i);
    if (i != j) {
      int v = list[i];
      list[i] = list[j];
      list[j] = v;
    }
  }
}

int setBit(int mask, int bit) {
  return (mask | bit);
}

int clearBit(int mask, int bit) {
  return (mask & ~bit);
}

int bitToValue(int bit) {
  int value = 0;
  while (bit > 0) {
    value++;
    bit >>= 1;
  }
  return value;
}

int valueToBit(int value) {
  return 1 << (value - 1);
}

void assertFailed(const char *function, const char *file, int lineNo, const char *expression) {
  if (SerialUSB) {
    SerialUSB.println("=== ASSERT FAILED ===");
    SerialUSB.println(function);
    SerialUSB.println(file);
    SerialUSB.println(lineNo, DEC);
    SerialUSB.println(expression);
    SerialUSB.flush();
  }

  while (1) {
    if (gb.update()) {
      gb.display.clear();
      gb.display.setColor(RED);
      gb.display.println("Assert failed:");
      gb.display.println(function);
      gb.display.println(file);
      gb.display.println(lineNo, DEC);
      gb.display.println(expression);
    }
  }
}
