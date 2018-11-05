#include <Gamebuino-Meta.h>

#include "Globals.h"
#include "Sudoku.h"
#include "Utils.h"

// Constraint tables
int colCells[numConstraintGroups][constraintGroupSize];
int rowCells[numConstraintGroups][constraintGroupSize];
int boxCells[numConstraintGroups][constraintGroupSize];

void initConstraintTables() {
  for (int i = 0; i < numConstraintGroups; i++) {
    int colIndex = i;
    int rowIndex = i * numCols;
    int boxIndex = 3 * (i % 3 + (i / 3) * 9);

    for (int j = 0; j < constraintGroupSize; j++) {
      colCells[i][j] = colIndex;
      rowCells[i][j] = rowIndex;
      boxCells[i][j] = boxIndex;

      colIndex += 9;
      rowIndex += 1;
      if (j % 3 == 2) {
        boxIndex += 7;
      } else {
        boxIndex += 1;
      }
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

//------------------------------------------------------------------------------
// SudokuCell

void SudokuCell::init(int cellIndex) {
  _value = 0;
  _fixed = false;
  _colMask = maxBitMask;
  _rowMask = maxBitMask;
  _boxMask = maxBitMask;

  _index = cellIndex;
  _col = _index % numCols;
  _row = (_index - _col) / numCols;
  _box = _col / 3 + 3 * (_row / 3);
}

bool SudokuCell::isBitAllowed(int bit) {
  if (_fixed) {
    return _value == bit;
  }

  return (
    (_colMask & bit) != 0 &&
    (_rowMask & bit) != 0 &&
    (_boxMask & bit) != 0
  );
}

bool SudokuCell::hasOneAllowedValue() {
  int m = bitMask();
  // Note: x & (x - 1) clears the right-most bit
  return (m & (m - 1)) == 0;
}

//------------------------------------------------------------------------------
// Sudoku

void Sudoku::init() {
  for (int i = 0; i < numCells; i++) {
    _cells[i].init(i);
  }

  for (int i = 0; i < numConstraintGroups; i++) {
    _colMasks[i] = maxBitMask;
    _rowMasks[i] = maxBitMask;
    _boxMasks[i] = maxBitMask;
  }

  _numFilled = 0;
}

void Sudoku::init(Sudoku& sudoku) {
  init();

  for (int i = 0; i < numCells; i++) {
    int bit = sudoku.cellAt(i).getBitValue();
    if (bit != 0) {
      setBitValue(_cells[i], bit);
    }
  }
}

void Sudoku::updateBitMasks(SudokuCell& cell, int bit, int (*updateFun)(int, int)) {
  int* colIndices = colCells[cell.col()];
  int* rowIndices = rowCells[cell.row()];
  int* boxIndices = boxCells[cell.box()];

  for (int i = 0; i < constraintGroupSize; i++) {
    int ci = colIndices[i];
    if (ci != cell.index()) {
      SudokuCell& cell2 = _cells[ci];
      cell2._colMask = (*updateFun)(cell2._colMask, bit);
    }

    ci = rowIndices[i];
    if (ci != cell.index()) {
      SudokuCell& cell2 = _cells[ci];
      cell2._rowMask = (*updateFun)(cell2._rowMask, bit);
    }

    ci = boxIndices[i];
    if (ci != cell.index()) {
      SudokuCell& cell2 = _cells[ci];
      cell2._boxMask = (*updateFun)(cell2._boxMask, bit);
    }
  }

  _colMasks[cell.col()] = (*updateFun)(_colMasks[cell.col()], bit);
  _rowMasks[cell.row()] = (*updateFun)(_rowMasks[cell.row()], bit);
  _boxMasks[cell.box()] = (*updateFun)(_boxMasks[cell.box()], bit);
}

void Sudoku::setValue(int x, int y, int value) {
  setBitValue(cellAt(x, y), valueToBit(value));
}

void Sudoku::clearValue(SudokuCell& cell) {
  int oldBit = cell._value;
  assertTrue(oldBit > 0);

  _numFilled--;
  cell._value = 0;

  updateBitMasks(cell, oldBit, &setBit);
}

void Sudoku::setBitValue(SudokuCell& cell, int bit) {
  assertTrue(bit != 0);

  if (cell.isSet()) {
    clearValue(cell);
  }

  _numFilled++;
  cell._value = bit;

  updateBitMasks(cell, bit, clearBit);
}

bool Sudoku::nextValue(SudokuCell& cell) {
  int bit = cell.getBitValue();

  for (int i = 0; i < numValues; i++) {
    if (bit == 0) {
      bit = 1;
    }
    else if (bit == maxBitValue) {
      bit = 0;
    }
    else {
      bit <<= 1;
    }

    if (bit == 0) {
      // Can always clear
      clearValue(cell);
      return true;
    }

    if (cell.isBitAllowed(bit)) {
      setBitValue(cell, bit);
      return true;
    }
  }

  // Cannot set to a different value
  return false;
}

AutoSetResult Sudoku::autoSet(SudokuCell& cell) {
  if (cell.isSet()) {
    return AutoSetResult::AlreadySet;
  }

  int m = cell.bitMask();
  if (m == 0) {
    return AutoSetResult::Stuck;
  }

  // Next evaluates to zero if only one bit was set
  if (!cell.hasOneAllowedValue()) {
    return AutoSetResult::MultipleOptions;
  }

  // Only one value is possible
  //SerialUSB.printf("autoSet %d => %d (was: %d)\n", cell.index(), m, cell.getBitValue());
  setBitValue(cell, m);
  return AutoSetResult::CellUpdated;
}

int Sudoku::getValue(int x, int y) {
  return bitToValue(cellAt(x, y).getBitValue());
}

bool Sudoku::isFixed(int x, int y) {
  return cellAt(x, y).isFixed();
}

bool Sudoku::isSet(int x, int y) {
  return cellAt(x, y).isSet();
}

void Sudoku::clearValue(int x, int y) {
  clearValue(cellAt(x, y));
}

bool Sudoku::nextValue(int x, int y) {
  return nextValue(cellAt(x, y));
}

void Sudoku::fixValues() {
  for (int i = 0; i < numCells; i++) {
    SudokuCell& cell = cellAt(i);
    if (cell.isSet()) {
      cell.fix();
    }
  }
}

void Sudoku::resetValues() {
  for (int i = 0; i < numCells; i++) {
    SudokuCell& cell = cellAt(i);
    if (cell.isSet() && !cell.isFixed()) {
      clearValue(cell);
    }
  }
}

void drawLines(int i) {
  gb.display.drawLine(8 + i * 7, 0, 8 + i * 7, 63);
  gb.display.drawLine(8, i * 7, 71, i * 7);
}

void drawCell(int x, int y) {
  gb.display.fillRect(9 + x * 7, 1 + y * 7, 6, 6);
}

void drawValue(int x, int y, int value) {
  gb.display.setCursor(x * 7 + 11, y * 7 + 2);
  gb.display.print(value);
}

int solvedCount;

const uint8_t solveImageData[] = {
  74, 9, 1, 0, 1, 0xFF, 1,
  0x00, 0x0a, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0xcc, 0x00, 0x00,
  0xcc, 0x0c, 0xcc, 0xcc, 0xc0, 0xcc, 0x00, 0x00, 0xcc, 0x00,
  0x00, 0x00, 0xbb, 0xbb, 0x00, 0x00, 0xbb, 0xbb, 0x00, 0xbb,
  0x00, 0xbb, 0x0b, 0xbb, 0xbb, 0xb0, 0xaa,
  0x00, 0xaa, 0xaa, 0xaa, 0xa0, 0x00, 0x00, 0xcc, 0x00, 0x00,
  0xcc, 0x0c, 0xcc, 0xcc, 0xc0, 0xcc, 0x00, 0x00, 0xcc, 0x00,
  0x00, 0x00, 0xbb, 0xbb, 0xb0, 0x0b, 0xbb, 0xbb, 0xb0, 0xbb,
  0xb0, 0xbb, 0x0b, 0xbb, 0xbb, 0xb0, 0xaa,
  0x0a, 0xaa, 0x0a, 0x0a, 0xaa, 0x00, 0x00, 0xcc, 0x00, 0x00,
  0xcc, 0x0c, 0xc0, 0x00, 0x00, 0xcc, 0x00, 0x00, 0xcc, 0x00,
  0x00, 0x00, 0xbb, 0x0b, 0xbb, 0x0b, 0xb0, 0x0b, 0xb0, 0xbb,
  0xbb, 0xbb, 0x0b, 0xb0, 0x00, 0x00, 0xaa,
  0x0a, 0xaa, 0x0a, 0x0a, 0xaa, 0x00, 0x00, 0xcc, 0x0c, 0xc0,
  0xcc, 0x0c, 0xcc, 0xc0, 0x00, 0xcc, 0x00, 0x00, 0xcc, 0x00,
  0x00, 0x00, 0xbb, 0x00, 0xbb, 0x0b, 0xb0, 0x0b, 0xb0, 0xbb,
  0xbb, 0xbb, 0x0b, 0xbb, 0xb0, 0x00, 0xaa,
  0x0a, 0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0xcc, 0x0c, 0xc0,
  0xcc, 0x0c, 0xcc, 0xc0, 0x00, 0xcc, 0x00, 0x00, 0xcc, 0x00,
  0x00, 0x00, 0xbb, 0x00, 0xbb, 0x0b, 0xb0, 0x0b, 0xb0, 0xbb,
  0x0b, 0xbb, 0x0b, 0xbb, 0xb0, 0x00, 0xaa,
  0x0a, 0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x00, 0xcc, 0x0c, 0xc0,
  0xcc, 0x0c, 0xcc, 0xc0, 0x00, 0xcc, 0x00, 0x00, 0xcc, 0x00,
  0x00, 0x00, 0xbb, 0x00, 0xbb, 0x0b, 0xb0, 0x0b, 0xb0, 0xbb,
  0x00, 0xbb, 0x0b, 0xbb, 0xb0, 0x00, 0xaa,
  0x0a, 0xa0, 0x00, 0x00, 0xaa, 0x00, 0x00, 0xcc, 0x0c, 0xc0,
  0xcc, 0x0c, 0xc0, 0x00, 0x00, 0xcc, 0x00, 0x00, 0xcc, 0x00,
  0x00, 0x00, 0xbb, 0x0b, 0xbb, 0x0b, 0xb0, 0x0b, 0xb0, 0xbb,
  0x00, 0xbb, 0x0b, 0xb0, 0x00, 0x00, 0x00,
  0x00, 0xaa, 0x00, 0x0a, 0xa0, 0x00, 0x00, 0x0c, 0xcc, 0xcc,
  0xc0, 0x0c, 0xcc, 0xcc, 0xc0, 0xcc, 0xcc, 0xc0, 0xcc, 0xcc,
  0xc0, 0x00, 0xbb, 0xbb, 0xb0, 0x0b, 0xbb, 0xbb, 0xb0, 0xbb,
  0x00, 0xbb, 0x0b, 0xbb, 0xbb, 0xb0, 0xaa,
  0x00, 0x0a, 0xaa, 0xaa, 0x00, 0x00, 0x00, 0x0c, 0xc0, 0x0c,
  0xc0, 0x0c, 0xcc, 0xcc, 0xc0, 0xcc, 0xcc, 0xc0, 0xcc, 0xcc,
  0xc0, 0x00, 0xbb, 0xbb, 0x00, 0x00, 0xbb, 0xbb, 0x00, 0xbb,
  0x00, 0xbb, 0x0b, 0xbb, 0xbb, 0xb0, 0xaa
};
Image solveImage = Image(solveImageData);

ColorIndex solvedColor(int col, int row) {
  int x = col + solvedCount / 4 - 12;
  if (x < 0 || x >= 74) {
    return INDEX_BLACK;
  }
  return solveImage.getPixelIndex(x, row);
}

//------------------------------------------------------------------------------

/* Light color gradients.
 *
 * These vary from black to respective GB Yellow, GB Light Blue, and GB Light
 * Green in 13 steps. The gradients have been created using
 * http://www.perbang.dk/rgbgradient/.
 *
 * The resulting RGB colors have been programmatically converted using
 * gb.createColor to the arrays with RGB565 values below.
 */
const uint8_t numGradientColors = 13;
const uint16_t gradientColorsYellow[numGradientColors] = {
  0x0000, 0x1080, 0x2920, 0x39c0, 0x5260, 0x6300, 0x7b80, 0x8c20,
  0xa4c0, 0xb560, 0xce00, 0xde80, 0xf720
};
const uint16_t gradientColorsLightBlue[numGradientColors] = {
  0x0000, 0x0862, 0x10e5, 0x1967, 0x29ea, 0x326d, 0x3aef, 0x4b72,
  0x53f5, 0x5c77, 0x6cda, 0x755d, 0x7ddf
};
const uint16_t gradientColorsLightGreen[numGradientColors] = {
  0x0000, 0x0880, 0x1101, 0x2182, 0x2a23, 0x32a4, 0x4325, 0x4bc6,
  0x5c47, 0x64c8, 0x6d69, 0x7dea, 0x866b
};

Color solveColorForLight(int x, int y) {
  int col = (x == 0) ? -2 : 9;
  int row0 = y * 2;
  ColorIndex color = INDEX_BLACK;
  int intensity = 0;

  for (int i = 0; i < 2; i++) {
    int intensityDelta = (solvedCount % 4);
    if (i == 0) {
      intensityDelta = 4 - intensityDelta;
    }
    for (int j = 0; j < 3; j++) {
      ColorIndex c = solvedColor(col + i, row0 + j);
      if (c != INDEX_BLACK) {
        color = c;
        intensity += intensityDelta;
      }
    }
  }
  assertTrue(intensity >= 0 && intensity < numGradientColors);
  if (color == INDEX_YELLOW) {
    return (Color)gradientColorsYellow[intensity];
  }
  if (color == INDEX_LIGHTBLUE) {
    return (Color)gradientColorsLightBlue[intensity];
  }
  if (color == INDEX_LIGHTGREEN) {
    return (Color)gradientColorsLightGreen[intensity];
  }
  return BLACK;
}

void solveLights() {
  for (int x = 0; x < 2; x++) {
    for (int y = 0; y < 4; y++) {
      gb.lights.drawPixel(x, y, solveColorForLight(x, y));
    }
  }
}

void Sudoku::draw() {
  gb.display.setColor(DARKGRAY);
  for (int i = 0; i <= numCols; i++) {
    if (i % 3 != 0) {
      drawLines(i);
    }
  }
  gb.display.setColor(GRAY);
  for (int i = 0; i <= numCols; i += 3) {
    drawLines(i);
  }

  if (isSolved()) {
    solvedCount = 1 + solvedCount % 360;
    solveLights();
  } else {
    solvedCount = 0;

    gb.display.setColor(BLUE);
    drawCell(cursorX, cursorY);
  }

  for (int x = 0; x < numCols; x++) {
    for (int y = 0; y < numRows; y++) {
      if (solvedCount > 0) {
        ColorIndex color = solvedColor(x, y);
        if (color != INDEX_BLACK) {
          gb.display.setColor(color);
          drawCell(x, y);
        }
      }
      if (sudoku.isSet(x, y)) {
        if (sudoku.isFixed(x, y)) {
          gb.display.setColor(WHITE);
        } else {
          gb.display.setColor(LIGHTBLUE);
        }
        drawValue(x, y, getValue(x, y));
      }
    }
  }
}

