#include "utils.h"

std::size_t convert2DIndicesTo1DIndex(std::size_t row, std::size_t col,
                                      std::size_t width) {
  return row * width + col;
}
