#pragma once

#include "constants.h"
#include <array>

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

  std::array<Byte, MEMORY_SIZE> memory{}; // RAM
  std::array<Byte, NUM_REGISTERS> V{};    // Vx
  Word I{};                               // index register
  Byte delayTimer{};
  Byte soundTimer{};
  Word PC{PC_START_POS}; // Program counter
  Byte SP{};             // Stack pointer
  std::array<Word, STACK_SIZE> stack{};
  std::array<Byte, DISPLAY_WIDTH * DISPLAY_HEIGHT> display{};
  std::array<Byte, NUM_KEYS> keyboard{};
};