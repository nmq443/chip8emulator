#include "cpu.h"
#include "utils.h"

void CPU::reset() noexcept {
  resetToDefault(opcode);
  resetToDefault(memory);
  resetToDefault(V);
  resetToDefault(I);
  resetToDefault(delayTimer);
  resetToDefault(soundTimer);
  resetToDefault(SP);
  resetToDefault(stack);
  resetToDefault(display);
  resetToDefault(keyboard);

  PC = PC_START_POS;
}

Word CPU::getOpcode() const noexcept { return opcode; }

const std::array<Byte, MEMORY_SIZE> &CPU::getMemory() const noexcept {
  return memory;
}

const std::array<Byte, NUM_REGISTERS> &CPU::getV() const noexcept { return V; }

Word CPU::getI() const noexcept { return I; }

Byte CPU::getDelayTimer() const noexcept { return delayTimer; }

Byte CPU::getSoundTimer() const noexcept { return soundTimer; }

Word CPU::getPC() const noexcept { return PC; }

Byte CPU::getSP() const noexcept { return SP; }

const std::array<Word, STACK_SIZE> &CPU::getStack() const noexcept {
  return stack;
}

const std::array<Byte, DISPLAY_WIDTH * DISPLAY_HEIGHT> &
CPU::getDisplay() const noexcept {
  return display;
}

const std::array<Byte, NUM_KEYS> &CPU::getKeyboard() const noexcept {
  return keyboard;
}
