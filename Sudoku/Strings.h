/*
 * Sudoku, a Gamebuino game
 *
 * Copyright 2018, Erwin Bonsma
 */

#ifndef __STRINGS_INCLUDED
#define __STRINGS_INCLUDED

#include <Gamebuino-Meta.h>

#define NUM_LANG 2
#define NUM_MENU_STRINGS 8

extern const char* menuStrings[NUM_MENU_STRINGS][NUM_LANG];
extern const char* menuTitle[NUM_LANG];

extern const MultiLang puzzleSaved[NUM_LANG];
extern const MultiLang loadFailed[NUM_LANG];
extern const MultiLang generatingPuzzle[NUM_LANG];

int getLanguageIndex();

#endif
