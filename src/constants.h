#pragma once

#include <cstdint>

using Byte = uint8_t;
using Word = uint16_t;

constexpr int MEMORY_SIZE{4096};
constexpr int NUM_REGISTERS{16};
constexpr int STACK_SIZE{16};
constexpr int DISPLAY_WIDTH{64};
constexpr int DISPLAY_HEIGHT{32};
constexpr int NUM_KEYS{16};
constexpr Word PC_START_POS{0x200};
constexpr int DISPLAY_SCALE{10};
constexpr int FREQUENCY{60};