#include "utils.h"
#include "constants.h"
#include <array>

std::size_t convert2DIndicesTo1DIndex(std::size_t row, std::size_t col,
                                      std::size_t width) {
  return row * width + col;
}

std::array<Byte, DISPLAY_WIDTH * DISPLAY_HEIGHT> getTestBuffer() {
  std::array<Byte, DISPLAY_WIDTH * DISPLAY_HEIGHT> testBuffer{};

  for (int y{}; y < DISPLAY_HEIGHT; ++y) {
    for (int x{}; x < DISPLAY_WIDTH; ++x) {
      if (((x / 8) + (y / 8)) % 2 == 0) {
        testBuffer[convert2DIndicesTo1DIndex(y, x, DISPLAY_WIDTH)] = 1; // White
      } else {
        testBuffer[convert2DIndicesTo1DIndex(y, x, DISPLAY_WIDTH)] = 0; // Black
      }
    }
  }
  return testBuffer;
}
