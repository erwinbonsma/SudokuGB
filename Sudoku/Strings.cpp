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
  { "Reset puzzle", "R�initialiser" },
  { "New puzzle", "Nouveau puzzle" },
  { "Create puzzle", "Cr�er un puzzle" },
  { "Enable hyper mode", "Activer hyper" },
  { "Disable hyper mode", "D�sactiver hyper" },
  { "Return", "Retour" }
};

const char* menuTitle[NUM_LANG] = {
  "Main menu", "Menu principal"
};

const MultiLang puzzleSaved[NUM_LANG] {
  { LANG_EN, "Puzzle saved" },
  { LANG_FR, "Puzzle sauvegard�" }
};

const MultiLang loadFailed[NUM_LANG] {
  { LANG_EN, "Load failed" },
  { LANG_FR, "Chargement rat�" }
};

const MultiLang generatingPuzzle[NUM_LANG] {
  { LANG_EN, "Generating puzzle" },
  { LANG_FR, "Nouveau puzzle" }
};

int getLanguageIndex() {
  switch (gb.language.getCurrentLang()) {
    case LANG_FR: return 1;
    default: return 0;
  }
}

