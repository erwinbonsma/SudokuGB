/*
 * Sudoku, a Gamebuino game
 *
 * Copyright 2018, Erwin Bonsma
 */

/* Can store four puzzles:
 * 0: Normal sudoku, stored by user
 * 1: Hyper sudoku, stored by user
 * 2: Normal sudoku, stored when enabling hyper mode
 * 3: Hyper sudoku, stored when disabling hyper mode
 */
#define SAVEBLOCK_NUM 4

// Should match storeBufferSize in Store.cpp
#define SAVECONF_DEFAULT_BLOBSIZE 82
