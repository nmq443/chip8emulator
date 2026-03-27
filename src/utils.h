#pragma once

#include "constants.h"
#include <array>
#include <cstddef>

std::size_t convert2DIndicesTo1DIndex(std::size_t row, std::size_t col,
                                      std::size_t width);

template <typename T> void resetToDefault(T &t) { t = T{}; }

std::array<Byte, DISPLAY_WIDTH * DISPLAY_HEIGHT> getTestBuffer();