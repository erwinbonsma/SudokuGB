/*
 * Sudoku, a Gamebuino game
 *
 * Copyright 2018, Erwin Bonsma
 */


/* Source file is in UTF8 encoding. It needs to be converted to iso-8859-1.
 * This can be done as follows:
 *   iconv -f utf-8 -t iso-8859-1 Strings.cpp.utf8 > Strings.cpp
 */

#include "Strings.h"

const char* menuStrings[NUM_MENU_STRINGS][NUM_LANG] {
  { "Save puzzle", "Sauver le puzzle" },
  { "Load puzzle", "Charger le puzzle" },
  { "Reset puzzle", "Recommencer" },
  { "New puzzle", "Nouveau puzzle" },
  { "Create puzzle", "Cr�er un puzzle" },
  { "Enable hyper mode", "Mode hyper actif" },
  { "Disable hyper mode", "Mode hyper inactif" },
  { "Return", "Revenir au puzzle" }
};

const char* menuTitle[NUM_LANG] = {
  "SUDOKU by eriban", "SUDOKU par eriban"
};

const MultiLang puzzleSaved[NUM_LANG] {
  { LANG_EN, "Puzzle saved" },
  { LANG_FR, "Puzzle sauvegard�" }
};

const MultiLang loadFailed[NUM_LANG] {
  { LANG_EN, "Load failed" },
  { LANG_FR, "Echec du chargement" }
};

const MultiLang generatingPuzzle[NUM_LANG] {
  { LANG_EN, "Generating puzzle" },
  { LANG_FR, "Cr�ation du puzzle" }
};

int getLanguageIndex() {
  switch (gb.language.getCurrentLang()) {
    case LANG_FR: return 1;
    default: return 0;
  }
}

