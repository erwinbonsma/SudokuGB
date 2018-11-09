/*
 * Sudoku, a Gamebuino game
 *
 * Copyright 2018, Erwin Bonsma
 */

#ifndef __UTILS_INCLUDED
#define __UTILS_INCLUDED

// Comment out next line to enable development features
#define DEVELOPMENT

void initDebugLog();

#ifdef DEVELOPMENT
  #define debug(format, ...) SerialUSB.printf(format, __VA_ARGS__);
#else
  #define debug(format, ...)
#endif

void permute(int* list, int len);

void assertFailed(const char *function, const char *file, int lineNo, const char *expression);

#define assertTrue(condition) \
if (!(condition)) { \
  assertFailed(__func__, __FILE__, __LINE__, #condition); \
}

#endif
