#include "cpu.h"
#include "random.h"
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
  std::copy(std::cbegin(FONT_SET), std::cend(FONT_SET), std::begin(_memory));
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

const std::array<bool, NUM_KEYS> &CPU::getKeyboard() const { return _keyboard; }

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

  std::ifstream file{filepath, std::ios::binary};
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

void CPU::setKey(std::size_t key, bool pressed) {
  if (key < 0 || key > _keyboard.size()) {
    std::cerr << "Invalid key pressed: " << key << '\n';
    return;
  }
  _keyboard[key] = pressed;
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
  if (!_table[first4bit]) {
    std::cerr << "No dispatch available for " << static_cast<int>(first4bit)
              << '\n';
    return;
  }
  (this->*_table[first4bit])();
}

void CPU::updateTimers() {
  if (_delayTimer) {
    --_delayTimer;
  }
  if (_soundTimer) {
    --_soundTimer;
  }
}

const std::array<CPU::Handler, 16> CPU::_table = {
    &CPU::group0, &CPU::group1, &CPU::group2, &CPU::group3,
    &CPU::group4, &CPU::group5, &CPU::group6, &CPU::group7,
    &CPU::group8, &CPU::group9, &CPU::groupA, &CPU::groupB,
    &CPU::groupC, &CPU::groupD, &CPU::groupE, &CPU::groupF};

const std::array<CPU::Handler, 2> CPU::_tableGroup0 = {&CPU::op00E0,
                                                       &CPU::op00EE};

const std::array<CPU::Handler, 9> CPU::_tableGroup8 = {
    &CPU::op8xy0, &CPU::op8xy1, &CPU::op8xy2, &CPU::op8xy3, &CPU::op8xy4,
    &CPU::op8xy5, &CPU::op8xy6, &CPU::op8xy7, &CPU::op8xyE};

const std::array<CPU::Handler, 2> CPU::_tableGroupE = {&CPU::opEx9E,
                                                       &CPU::opExA1};

const std::array<CPU::Handler, 9> CPU::_tableGroupF = {
    &CPU::opFx07, &CPU::opFx0A, &CPU::opFx15, &CPU::opFx18, &CPU::opFx1E,
    &CPU::opFx29, &CPU::opFx33, &CPU::opFx55, &CPU::opFx65};

void CPU::group0() {
  Byte nibble{_n == 0x0E ? static_cast<Byte>(1) : _n};
  if (!_tableGroup0[nibble]) {
    std::cerr << "No dispatch available for " << static_cast<int>(nibble)
              << '\n';
    return;
  }
  (this->*_tableGroup0[nibble])();
}

void CPU::group1() { op1nnn(); }

void CPU::group2() { op2nnn(); }

void CPU::group3() { op3xkk(); }

void CPU::group4() { op4xkk(); }

void CPU::group5() { op5xy0(); }

void CPU::group6() { op6xkk(); }

void CPU::group7() { op7xkk(); }

void CPU::group8() {
  Byte nibble{_n == 0x0E ? static_cast<Byte>(8) : _n};
  if (!_tableGroup8[nibble]) {
    std::cerr << "No dispatch available for " << static_cast<int>(nibble)
              << '\n';
    return;
  }
  (this->*_tableGroup8[nibble])();
}

void CPU::group9() { op9xy0(); }

void CPU::groupA() { opAnnn(); }

void CPU::groupB() { opBnnn(); }

void CPU::groupC() { opCxkk(); }

void CPU::groupD() { opDxyn(); }

void CPU::groupE() {
  if (_kk == 0x9E) {
    (this->*_tableGroupE[0])();
  } else if (_kk == 0xA1) {
    (this->*_tableGroupE[1])();
  } else {
    std::cerr << "Instruction " << std::hex << "0xEx" << static_cast<int>(_kk)
              << " is not available\n";
  }
}

void CPU::groupF() {
  if (_kk == 0x07) {
    (this->*_tableGroupF[0])();
  } else if (_kk == 0x0A) {
    (this->*_tableGroupF[1])();
  } else if (_kk == 0x15) {
    (this->*_tableGroupF[2])();
  } else if (_kk == 0x18) {
    (this->*_tableGroupF[3])();
  } else if (_kk == 0x1E) {
    (this->*_tableGroupF[4])();
  } else if (_kk == 0x29) {
    (this->*_tableGroupF[5])();
  } else if (_kk == 0x33) {
    (this->*_tableGroupF[6])();
  } else if (_kk == 0x55) {
    (this->*_tableGroupF[7])();
  } else if (_kk == 0x65) {
    (this->*_tableGroupF[8])();
  } else {
    std::cerr << "Instruction " << std::hex << "0xFx" << static_cast<int>(_kk)
              << " is not available\n";
  }
}

void CPU::op00E0() { _display.fill(0); }

void CPU::op00EE() {
  if (_SP < 0) {
    std::cerr << "Underflow stack pointer: " << _SP << '\n';
    return;
  }
  _PC = _stack[_SP--];
}

void CPU::op1nnn() { _PC = _nnn; }

void CPU::op2nnn() {
  if (_SP > STACK_SIZE) {
    std::cerr << "Overflow stack pointer: " << _SP << '\n';
    return;
  }
  _stack[++_SP] = _PC;
  _PC = _nnn;
}

void CPU::op3xkk() {
  if (_V[_x] == _kk) {
    _PC += 2;
  }
}

void CPU::op4xkk() {
  if (_V[_x] != _kk) {
    _PC += 2;
  }
}

void CPU::op5xy0() {
  if (_V[_x] == _V[_y]) {
    _PC += 2;
  }
}

void CPU::op6xkk() { _V[_x] = _kk; }

void CPU::op7xkk() { _V[_x] += _kk; }

void CPU::op8xy0() { _V[_x] = _V[_y]; }

void CPU::op8xy1() { _V[_x] |= _V[_y]; }

void CPU::op8xy2() { _V[_x] &= _V[_y]; }

void CPU::op8xy3() { _V[_x] ^= _V[_y]; }

void CPU::op8xy4() {
  Word sum{static_cast<Word>(_V[_x] + _V[_y])};
  _V[_x] += _V[_y];
  _V.back() = sum > 255 ? 1 : 0;
}

void CPU::op8xy5() {
  _V.back() = _V[_x] > _V[_y] ? 1 : 0;
  _V[_x] -= _V[_y];
}

void CPU::op8xy6() {
  Byte lsb{static_cast<Byte>(_V[_x] & 0b00000001)};
  _V.back() = lsb ? 1 : 0;
  _V[_x] >>= 1;
}

void CPU::op8xy7() {
  _V.back() = _V[_y] > _V[_x] ? 1 : 0;
  _V[_x] = _V[_y] - _V[_x];
}

void CPU::op8xyE() {
  Byte msb{static_cast<Byte>(_V[_x] >> 7)};
  _V[_x] <<= 1;
  _V.back() = msb ? 1 : 0;
}

void CPU::op9xy0() {
  if (_V[_x] != _V[_y]) {
    _PC += 2;
  }
}

void CPU::opAnnn() { _I = _nnn; }

void CPU::opBnnn() { _PC = _nnn + _V.front(); }

void CPU::opCxkk() {
  int rand{Random::get(0, 255)};
  _V[_x] = static_cast<Byte>(rand) & _kk;
}

void CPU::opDxyn() {
  Byte startRow{_V[_y]};
  Byte startCol{_V[_x]};
  _V.back() = 0;
  for (std::size_t byteIdx{}; byteIdx < _n; ++byteIdx) {
    for (std::size_t bitIdx{}; bitIdx < 8; ++bitIdx) {
      Byte sprite{_memory[byteIdx + _I]};

      Byte bit{static_cast<Byte>((sprite >> (7 - bitIdx)) & 1)};
      Byte row{static_cast<Byte>((startRow + byteIdx) % DISPLAY_HEIGHT)};
      Byte col{static_cast<Byte>((startCol + bitIdx) % DISPLAY_WIDTH)};
      Byte &pixel{_display[convert2DIndicesTo1DIndex(row, col, DISPLAY_WIDTH)]};

      if (pixel && bit) {
        _V.back() = 1;
      }

      pixel ^= bit;
    }
  }
}

void CPU::opEx9E() {
  if (_keyboard[_V[_x]]) {
    _PC += 2;
  }
}

void CPU::opExA1() {
  if (!_keyboard[_V[_x]]) {
    _PC += 2;
  }
}

void CPU::opFx07() { _V[_x] = _delayTimer; }

void CPU::opFx0A() {
  for (auto pressed : _keyboard) {
    if (pressed) {
      _V[_x] = static_cast<Byte>(pressed);
      return;
    }
  }
  _PC -= 2; // no key pressed -> re-execute
}

void CPU::opFx15() { _delayTimer = _V[_x]; }

void CPU::opFx18() { _soundTimer = _V[_x]; }

void CPU::opFx1E() { _I += _V[_x]; }

void CPU::opFx29() { _I = 5 * _V[_x]; }

void CPU::opFx33() {
  Byte hundredsDigit{static_cast<Byte>(_V[_x] / 100)};
  Byte tensDigit{static_cast<Byte>((_V[_x] - hundredsDigit * 100) / 10)};
  Byte onesDigit{static_cast<Byte>(_V[_x] % 10)};
  _memory[_I] = hundredsDigit;
  _memory[_I + 1] = tensDigit;
  _memory[_I + 2] = onesDigit;
}

void CPU::opFx55() {
  std::copy(_V.begin(), _V.begin() + _x + 1, _memory.begin() + _I);
}

void CPU::opFx65() {
  std::copy(_memory.begin() + _I, _memory.begin() + _I + _x + 1, _V.begin());
}