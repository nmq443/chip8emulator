#pragma once

#include "constants.h"
#include <array>
#include <filesystem>
#include <vector>

class CPU {
public:
  CPU();
  void reset();
  bool loadROM(const std::filesystem::path &filepath);
  bool loadROM(const std::vector<Byte> &rom);
  void cycle();
  void setKey(std::size_t key, bool pressed);
  void updateTimers();

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

  const std::array<bool, NUM_KEYS> &getKeyboard() const;

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

  /// @brief CLS - Clear the display
  void op00E0();

  /// @brief RET - Return from a subroutine
  void op00EE();

  /// @brief JP addr - Jump to location nnn.
  void op1nnn();

  /// @brief CALL addr - Call subroutine at nnn.
  void op2nnn();

  /// @brief SE Vx, byte - Skip next instruction if Vx = kk
  void op3xkk();

  /// @brief SNE Vx, byte - Skip next instruction if Vx != kk.
  void op4xkk();

  /// @brief SE Vx, Vy - Skip next instruction if Vx = Vy
  void op5xy0();

  /// @brief LD Vx, byte - Set Vx = kk
  void op6xkk();

  /// @brief ADD Vx, byte - Set Vx = Vx + kk.
  void op7xkk();

  /// @brief LD Vx, Vy - Set Vx = Vy
  void op8xy0();

  /// @brief OR Vx, Vy - Set Vx = Vx OR Vy
  void op8xy1();

  /// @brief AND Vx, Vy - Set Vx = Vx AND Vy
  void op8xy2();

  /// @brief XOR Vx, Vy - Set Vx = Vx XOR Vy
  void op8xy3();

  /// @brief ADD Vx, Vy - Set Vx = Vx + Vy, set VF = carry
  void op8xy4();

  /// @brief SUB Vx, Vy - Set Vx = Vx - Vy, set VF = NOT borrow
  void op8xy5();

  /// @brief SHR Vx {, Vy} - Set Vx = Vx SHR 1
  void op8xy6();

  /// @brief SUBN Vx, Vy - Set Vx = Vy - Vx, set VF = NOT borrow
  void op8xy7();

  /// @brief SHL Vx {, Vy} - Set Vx = Vx SHL 1
  void op8xyE();

  /// @brief SNE Vx, Vy - Skip next instruction if Vx != Vy
  void op9xy0();

  /// @brief LD I, addr - Set I = nnn.
  void opAnnn();

  /// @brief JP V0, addr - Jump to location nnn + V0
  void opBnnn();

  /// @brief RND Vx, byte - Set Vx = random byte AND kk
  void opCxkk();

  /// @brief DRW Vx, Vy, nibble - Display n-byte sprite starting at memory
  /// location I at (Vx, Vy), set VF = collision
  void opDxyn();

  /// @brief SKP Vx - Skip next instruction if key with the value of Vx is
  /// pressed
  void opEx9E();

  /// @brief SKNP Vx - Skip next instruction if key with the value of Vx is not
  /// pressed
  void opExA1();

  /// @brief LD Vx, DT - Set Vx = delay timer value
  void opFx07();

  /// @brief LD Vx, K - Wait for a key press, store the value of the key in Vx
  void opFx0A();

  /// @brief LD DT, Vx - Set delay timer = Vx
  void opFx15();

  /// @brief LD ST, Vx - Set sound timer = Vx
  void opFx18();

  /// @brief ADD I, Vx - Set I = I + Vx
  void opFx1E();

  /// @brief LD F, Vx - Set I = location of sprite for digit V
  void opFx29();

  /// @brief LD B, Vx - Store BCD representation of Vx in memory locations I,
  /// I+1, and I+2
  void opFx33();

  /// @brief LD [I], Vx - Store registers V0 through Vx in memory starting at
  /// location I
  void opFx55();

  /// @brief LD Vx, [I] - Read registers V0 through Vx from memory starting at
  /// location I
  void opFx65();

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
  std::array<bool, NUM_KEYS> _keyboard{};

  // --- decoded opcode parts ---
  Word _opcode{}; // 2 bytes long instruction, stored in big endian order
  Word _nnn{};    // lowest 12 bits of the instruction
  Byte _n{};      // lowest 4 bits of the instruction
  Byte _x{};      // lowest 4 bits of the high byte of the instruction
  Byte _y{};      // highest 4 bits of the low byte of the instruction
  Byte _kk{};     // lowest 8 bits of the instruction
};