#pragma once

#include "constants.h"
#include <array>
#include <filesystem>

class CPU {
public:
  CPU();
  void reset();
  bool loadROM(const std::filesystem::path &filepath);
  bool loadROM(const std::vector<Byte> &rom);
  void cycle();

  // --- Getters ---
  Word getOpcode() const;

  const std::array<Byte, MEMORY_SIZE> &getMemory() const;

  const std::array<Byte, NUM_REGISTERS> &getV() const;

  const Byte &getV(std::size_t pos) const;

  Word getI() const;

  Byte getDelayTimer() const;

  Byte getSoundTimer() const;

  Word getPC() const;

  Byte getSP() const;

  const std::array<Word, STACK_SIZE> &getStack() const;

  const std::array<Byte, DISPLAY_WIDTH * DISPLAY_HEIGHT> &getDisplay() const;

  const std::array<Byte, NUM_KEYS> &getKeyboard() const;

private:
  // --- Three steps of cycle() ---
  void fetch();
  void decode();
  void execute();

  // --- Dispatch table ---
  using Handler = void (CPU::*)();
  static const std::array<Handler, 16> _table;      // first nibble -> handler
  static const std::array<Handler, 2> _tableGroup0; // group 0
  static const std::array<Handler, 9> _tableGroup8; // group 8
  static const std::array<Handler, 2> _tableGroupE; // group E
  static const std::array<Handler, 9> _tableGroupF; // group F
  // -- First level handler --
  void group0();
  void group1();
  void group2();
  void group3();
  void group4();
  void group5();
  void group6();
  void group7();
  void group8();
  void group9();
  void groupA();
  void groupB();
  void groupC();
  void groupD();
  void groupE();
  void groupF();

  // -- Second level handler --
  /// @brief SYS addr - Jump to a machine code routine at nnn.
  void op0nnn();

  /// @brief CLS - Clear the display
  void op00E0();

  /// @brief JP addr - Jump to location nnn.
  void op1nnn();

  /// @brief LD Vx, byte - Set Vx = kk
  void op6xkk();

  /// @brief ADD Vx, byte - Set Vx = Vx + kk.
  void op7xkk();

  /// @brief LD I, addr - Set I = nnn.
  void opAnnn();

  /// @brief DRW Vx, Vy, nibble - Display n-byte sprite starting at memory
  /// location I at (Vx, Vy), set VF = collision
  void opDxyn();

private:
  // --- state ---
  std::array<Byte, MEMORY_SIZE> _memory{}; // RAM
  std::array<Byte, NUM_REGISTERS> _V{};    // Vx
  Word _I{};                               // index register
  Byte _delayTimer{};
  Byte _soundTimer{};
  Word _PC{PC_START_POS}; // Program counter
  Byte _SP{};             // Stack pointer
  std::array<Word, STACK_SIZE> _stack{};
  std::array<Byte, DISPLAY_WIDTH * DISPLAY_HEIGHT> _display{};
  std::array<Byte, NUM_KEYS> _keyboard{};

  // --- decoded opcode parts ---
  Word _opcode{}; // 2 bytes long instruction, stored in big endian order
  Word _nnn{};    // lowest 12 bits of the instruction
  Byte _n{};      // lowest 4 bits of the instruction
  Byte _x{};      // lowest 4 bits of the high byte of the instruction
  Byte _y{};      // highest 4 bits of the low byte of the instruction
  Byte _kk{};     // lowest 8 bits of the instruction
};