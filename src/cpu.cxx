#include "cpu.h"
#include "utils.h"
#include <fstream>
#include <iostream>

CPU::CPU() {
  std::copy(std::cbegin(FONT_SET), std::cend(FONT_SET), std::begin(_memory));
}

void CPU::reset() {
  resetToDefault(_opcode);
  resetToDefault(_memory);
  resetToDefault(_V);
  resetToDefault(_I);
  resetToDefault(_delayTimer);
  resetToDefault(_soundTimer);
  resetToDefault(_SP);
  resetToDefault(_stack);
  resetToDefault(_display);
  resetToDefault(_keyboard);

  resetToDefault(_nnn);
  resetToDefault(_x);
  resetToDefault(_y);
  resetToDefault(_n);
  resetToDefault(_kk);

  _PC = PC_START_POS;
}

Word CPU::getOpcode() const { return _opcode; }

const std::array<Byte, MEMORY_SIZE> &CPU::getMemory() const { return _memory; }

const std::array<Byte, NUM_REGISTERS> &CPU::getV() const { return _V; }

const Byte &CPU::getV(std::size_t pos) const { return _V[pos]; };

Word CPU::getI() const { return _I; }

Byte CPU::getDelayTimer() const { return _delayTimer; }

Byte CPU::getSoundTimer() const { return _soundTimer; }

Word CPU::getPC() const { return _PC; }

Byte CPU::getSP() const { return _SP; }

const std::array<Word, STACK_SIZE> &CPU::getStack() const { return _stack; }

const std::array<Byte, DISPLAY_WIDTH * DISPLAY_HEIGHT> &
CPU::getDisplay() const {
  return _display;
}

const std::array<Byte, NUM_KEYS> &CPU::getKeyboard() const { return _keyboard; }

bool CPU::loadROM(const std::filesystem::path &filepath) {
  if (!std::filesystem::exists(filepath)) {
    std::cerr << "File " << filepath << " does not exist!\n";
    return false;
  }

  std::uintmax_t fileSize{std::filesystem::file_size(filepath)};
  if (fileSize > ROM_SIZE_MAX) {
    std::cerr << "ROM file size is too large for chip 8 memory!\n";
    return false;
  }

  std::ifstream file{filepath};
  if (!file.is_open()) {
    std::cerr << "Can not open file " << filepath << '\n';
    return false;
  }

  std::vector<char> buffer(fileSize);
  if (!file.read(buffer.data(), fileSize)) {
    std::cerr << "Can't read file!\n";
    return false;
  }
  std::copy(buffer.begin(), buffer.end(), _memory.begin() + PC_START_POS);
  return true;
}

bool CPU::loadROM(const std::vector<Byte> &rom) {
  if (rom.size() > ROM_SIZE_MAX) {
    std::cerr << "ROM file size is too large for chip 8 memory!\n";
    return false;
  }
  std::copy(rom.begin(), rom.end(), _memory.begin() + PC_START_POS);
  return true;
}

void CPU::cycle() {
  fetch();
  decode();
  execute();
}

void CPU::fetch() {
  if (_PC + 1 < _memory.size()) {
    _opcode = (_memory[_PC] << 8) | _memory[_PC + 1];
    _PC += 2;
  }
}

void CPU::decode() {
  _nnn = _opcode & 0x0FFF;
  _n = _opcode & 0x000F;
  _x = (_opcode & 0x0F00) >> 8;
  _y = (_opcode & 0x00F0) >> 4;
  _kk = _opcode & 0x00FF;
}

void CPU::execute() {
  Byte first4bit{static_cast<Byte>((_opcode & 0xF000) >> 12)};
  (this->*_table[first4bit])();
}

const std::array<CPU::Handler, 16> CPU::_table = {
    &CPU::group0, &CPU::group1, &CPU::group2, &CPU::group3,
    &CPU::group4, &CPU::group5, &CPU::group6, &CPU::group7,
    &CPU::group8, &CPU::group9, &CPU::groupA, &CPU::groupB,
    &CPU::groupC, &CPU::groupD, &CPU::groupE, &CPU::groupF};

const std::array<CPU::Handler, 2> CPU::_tableGroup0 = {&CPU::op0nnn,
                                                       &CPU::op00E0};

const std::array<CPU::Handler, 9> CPU::_tableGroup8 = {};

const std::array<CPU::Handler, 2> CPU::_tableGroupE = {};

const std::array<CPU::Handler, 9> CPU::_tableGroupF = {};

void CPU::group0() {
  if (_kk == 0xE0) {
    (this->*_tableGroup0[1])();
  } else if (_kk == _nnn) {
    (this->*_tableGroup0[0])();
  } else {
    std::cerr << "Instruction " << std::hex << "0x00" << static_cast<int>(_kk)
              << " is not available\n";
  }
}

void CPU::group1() { op1nnn(); }

void CPU::group2() {}

void CPU::group3() {}

void CPU::group4() {}

void CPU::group5() {}

void CPU::group6() { op6xkk(); }

void CPU::group7() { op7xkk(); }

void CPU::group8() {}

void CPU::group9() {}

void CPU::groupA() { opAnnn(); }

void CPU::groupB() {}

void CPU::groupC() {}

void CPU::groupD() { opDxyn(); }

void CPU::groupE() {}

void CPU::groupF() {}

void CPU::op0nnn() {}

void CPU::op00E0() {
  for (auto &pixel : _display) {
    pixel = {};
  }
}

void CPU::op1nnn() {
  Word nnn{static_cast<Word>(_opcode & 0x0FFF)};
  _PC = nnn;
}

void CPU::op6xkk() { _V[_x] = _kk; }

void CPU::op7xkk() { _V[_x] += _kk; }

void CPU::opAnnn() { _I = _nnn; }

void CPU::opDxyn() {
  Byte startRow{_V[_y]};
  Byte startCol{_V[_x]};
  for (std::size_t byteIdx{}; byteIdx < _n; ++byteIdx) {
    for (std::size_t bitIdx{}; bitIdx < 8; ++bitIdx) {
      Byte sprite{_memory[byteIdx + _I]};

      Byte bit{static_cast<Byte>((sprite >> (7 - bitIdx)) & 1)};
      Byte row{(startRow + byteIdx) % DISPLAY_HEIGHT};
      Byte col{(startCol + bitIdx) % DISPLAY_WIDTH};
      Byte &pixel{_display[convert2DIndicesTo1DIndex(row, col, DISPLAY_WIDTH)]};

      if (pixel && !(pixel ^ bit)) {
        _V.back() = 1;
      }

      pixel ^= bit;
    }
  }
}