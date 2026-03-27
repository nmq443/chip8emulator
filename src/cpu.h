#pragma once

#include <array>
#include <cstdint>

using Byte = uint8_t;
using Word = uint16_t;

constexpr int MEMORY_SIZE = 4096;
constexpr int NUM_REGISTERS = 16;
constexpr int STACK_SIZE = 16;
constexpr int DISPLAY_WIDTH = 64;
constexpr int DISPLAY_HEIGHT = 32;
constexpr int NUM_KEYS = 32;
constexpr Word PC_START_POS = 0x200;

class CPU {
public:
  explicit CPU() = default;
  void reset() noexcept;
  Word getOpcode() const noexcept;
  const std::array<Byte, MEMORY_SIZE> &getMemory() const noexcept;
  const std::array<Byte, NUM_REGISTERS> &getV() const noexcept;
  Word getI() const noexcept;
  Byte getDelayTimer() const noexcept;
  Byte getSoundTimer() const noexcept;
  Word getPC() const noexcept;
  Byte getSP() const noexcept;
  const std::array<Word, STACK_SIZE> &getStack() const noexcept;
  const std::array<Byte, DISPLAY_WIDTH * DISPLAY_HEIGHT> &
  getDisplay() const noexcept;
  const std::array<Byte, NUM_KEYS> &getKeyboard() const noexcept;

private:
  Word opcode;

  std::array<Byte, MEMORY_SIZE> memory; // RAM
  std::array<Byte, NUM_REGISTERS> V;    // Vx
  Word I;                               // index register
  Byte delayTimer;
  Byte soundTimer;
  Word PC = PC_START_POS; // Program counter
  Byte SP;                // Stack pointer
  std::array<Word, STACK_SIZE> stack;
  std::array<Byte, DISPLAY_WIDTH * DISPLAY_HEIGHT> display;
  std::array<Byte, NUM_KEYS> keyboard;
};