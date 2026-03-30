#include "../src/cpu.h"
#include "../src/utils.h"
#include <algorithm>
#include <gtest/gtest.h>

class CPUTest : public ::testing::Test {
protected:
  CPU cpu{};
};

template <typename T> bool allZero(const T &container) {
  return std::all_of(std::cbegin(container), std::cend(container),
                     [](auto i) { return i == 0; });
}

TEST_F(CPUTest, CPUTest_DefaultInit) {
  EXPECT_EQ(cpu.getOpcode(), 0);
  EXPECT_EQ(cpu.getDelayTimer(), 0);
  EXPECT_EQ(cpu.getSoundTimer(), 0);
  EXPECT_EQ(cpu.getI(), 0);
  EXPECT_EQ(cpu.getPC(), PC_START_POS);
  EXPECT_EQ(cpu.getSP(), 0);

  const auto &display = cpu.getDisplay();
  const auto &keyboard = cpu.getKeyboard();
  const auto &stack = cpu.getStack();
  const auto &V = cpu.getV();

  EXPECT_TRUE(allZero(display));
  EXPECT_TRUE(allZero(keyboard));
  EXPECT_TRUE(allZero(stack));
  EXPECT_TRUE(allZero(V));
}

TEST_F(CPUTest, CPUTest_Reset) {
  cpu.reset();

  EXPECT_EQ(cpu.getOpcode(), 0);
  EXPECT_EQ(cpu.getDelayTimer(), 0);
  EXPECT_EQ(cpu.getSoundTimer(), 0);
  EXPECT_EQ(cpu.getI(), 0);
  EXPECT_EQ(cpu.getPC(), PC_START_POS);
  EXPECT_EQ(cpu.getSP(), 0);

  const auto &display{cpu.getDisplay()};
  const auto &keyboard{cpu.getKeyboard()};
  const auto &stack{cpu.getStack()};
  const auto &V{cpu.getV()};

  EXPECT_TRUE(allZero(display));
  EXPECT_TRUE(allZero(keyboard));
  EXPECT_TRUE(allZero(stack));
  EXPECT_TRUE(allZero(V));
}

TEST_F(CPUTest, CPUTest_LoadROM) {
  EXPECT_TRUE(cpu.loadROM("rom/IBM Logo.ch8"));
  const auto &memory{cpu.getMemory()};
  EXPECT_EQ(memory[PC_START_POS], 0x00);
  EXPECT_EQ(memory[PC_START_POS + 1], 0xE0);
  EXPECT_EQ(memory[PC_START_POS + 2], 0xA2);
  EXPECT_EQ(memory[PC_START_POS + 3], 0x2A);
  EXPECT_EQ(memory[PC_START_POS + 4], 0x60);
  EXPECT_EQ(memory[PC_START_POS + 5], 0x0C);
  EXPECT_EQ(memory[PC_START_POS + 6], 0x61);
  EXPECT_EQ(memory[PC_START_POS + 7], 0x08);
  EXPECT_EQ(memory[PC_START_POS + 8], 0xD0);
  EXPECT_EQ(memory[PC_START_POS + 9], 0x1F);
}

// --- Group 0 ---
TEST_F(CPUTest, CPUTest_Opcode_00E0) {
  std::vector<Byte> rom = {0x00, 0xE0};
  EXPECT_TRUE(cpu.loadROM(rom));

  cpu.cycle();
  const auto &display{cpu.getDisplay()};
  EXPECT_TRUE(allZero(display));
}

TEST_F(CPUTest, CPUTest_Opcode_00EE) {
  std::vector<Byte> rom = {0x00, 0xEE};
  EXPECT_TRUE(cpu.loadROM(rom));

  cpu.cycle();
  Word PC{cpu.getPC()};
  EXPECT_EQ(PC, 0);
}

// --- Group 1 ---
TEST_F(CPUTest, CPUTest_Opcode_1nnn) {
  std::vector<Byte> rom = {0x12, 0x22}; // nnn = 222
  EXPECT_TRUE(cpu.loadROM(rom));

  cpu.cycle();
  Word PC{cpu.getPC()};
  EXPECT_EQ(PC, 0x0222);
}

// --- Group 2 ---
TEST_F(CPUTest, CPUTest_Opcode_2nnn) {
  std::vector<Byte> rom = {0x22, 0x22}; // nnn = 222
  EXPECT_TRUE(cpu.loadROM(rom));

  Word PC{cpu.getPC()};
  cpu.cycle();
  Word PCAfterCycle{cpu.getPC()};
  const auto &stack{cpu.getStack()};
  Byte SP{cpu.getSP()};
  EXPECT_EQ(PC + 2,
            stack[SP]); // +2 is because after fetching, PC is incremented by 2
  EXPECT_EQ(PCAfterCycle, 0x0222);
}

// --- Group 3 ---
TEST_F(CPUTest, CPUTest_Opcode_3xkk) {
  std::vector<Byte> rom = {0x32, 0x00}; // x = 2, kk = 00
  EXPECT_TRUE(cpu.loadROM(rom));

  Word PCBeforeCycle{cpu.getPC()};
  cpu.cycle();
  Word PCAfterCycle{cpu.getPC()};
  EXPECT_EQ(cpu.getV(2), 0x00);
  EXPECT_EQ(PCBeforeCycle + 2 + 2,
            PCAfterCycle); // first + 2 is after fetching, second + 2 is
                           // skipping next instruction
}

// --- Group 4 ---
TEST_F(CPUTest, CPUTest_Opcode_4xkk) {
  std::vector<Byte> rom = {0x42, 0x01}; // x = 2, kk = 01
  EXPECT_TRUE(cpu.loadROM(rom));

  Word PCBeforeCycle{cpu.getPC()};
  cpu.cycle();
  Word PCAfterCycle{cpu.getPC()};
  EXPECT_NE(cpu.getV(2), 0x01);
  EXPECT_EQ(PCBeforeCycle + 2 + 2,
            PCAfterCycle); // first + 2 is after fetching, second + 2 is
                           // skipping next instruction
}

// --- Group 5 ---
TEST_F(CPUTest, CPUTest_Opcode_5xy0) {
  std::vector<Byte> rom = {0x51, 0x20}; // x = 1, y = 2
  EXPECT_TRUE(cpu.loadROM(rom));

  Word PCBeforeCycle{cpu.getPC()};
  cpu.cycle();
  Word PCAfterCycle{cpu.getPC()};
  EXPECT_EQ(cpu.getV(1), cpu.getV(2));
  EXPECT_EQ(PCBeforeCycle + 2 + 2,
            PCAfterCycle); // first + 2 is after fetching, second + 2 is
                           // skipping next instruction
}

// --- Group 6 ---
TEST_F(CPUTest, CPUTest_Opcode_6xkk) {
  std::vector<Byte> rom = {0x61, 0x22}; // x = 1, kk = 22
  EXPECT_TRUE(cpu.loadROM(rom));

  cpu.cycle();
  Byte Vx{cpu.getV(1)};
  EXPECT_EQ(Vx, 0x22);
}

// --- Group 7 ---
TEST_F(CPUTest, CPUTest_Opcode_7xkk) {
  std::vector<Byte> rom = {0x71, 0x22}; // x = 1, kk = 22
  EXPECT_TRUE(cpu.loadROM(rom));

  Byte VxBefore{cpu.getV(1)};
  cpu.cycle();
  Byte VxAfter{cpu.getV(1)};
  EXPECT_EQ(VxAfter, VxBefore + 0x22);
}

// --- Group 8 ---
TEST_F(CPUTest, CPUTest_Opcode_8xy0) {
  std::vector<Byte> rom = {
      0x61, 0x42, // V1 = 0x42
      0x62, 0x55, // V2 = 0x55
      0x81, 0x20  // V1 = V2
  };
  EXPECT_TRUE(cpu.loadROM(rom));

  cpu.cycle(); // V1 = 0x42
  cpu.cycle(); // V2 = 0x55
  cpu.cycle(); // 8120: V1 = V2
  EXPECT_EQ(cpu.getV(1), 0x55);
}

TEST_F(CPUTest, CPUTest_Opcode_8xy1) {
  std::vector<Byte> rom = {
      0x61, 0x0F, // V1 = 0x0F
      0x62, 0xF0, // V2 = 0xF0
      0x81, 0x21  // V1 = V1 | V2
  };
  EXPECT_TRUE(cpu.loadROM(rom));

  cpu.cycle(); // V1 = 0x0F
  cpu.cycle(); // V2 = 0xF0
  cpu.cycle(); // 8121: V1 = V1 | V2 = 0xFF
  EXPECT_EQ(cpu.getV(1), 0xFF);
}

TEST_F(CPUTest, CPUTest_Opcode_8xy2) {
  std::vector<Byte> rom = {
      0x61, 0xFF, // V1 = 0xFF
      0x62, 0x0F, // V2 = 0x0F
      0x81, 0x22  // V1 = V1 & V2
  };
  EXPECT_TRUE(cpu.loadROM(rom));

  cpu.cycle(); // V1 = 0xFF
  cpu.cycle(); // V2 = 0x0F
  cpu.cycle(); // 8122: V1 = V1 & V2
  EXPECT_EQ(cpu.getV(1), 0xFF & 0x0F);
}

TEST_F(CPUTest, CPUTest_Opcode_8xy3) {
  std::vector<Byte> rom = {
      0x61, 0xFF, // V1 = 0xFF
      0x62, 0x0F, // V2 = 0x0F
      0x81, 0x23  // V1 = V1 ^ V2
  };
  EXPECT_TRUE(cpu.loadROM(rom));

  cpu.cycle(); // V1 = 0xFF
  cpu.cycle(); // V2 = 0x0F
  cpu.cycle(); // 8123: V1 = V1 ^ V2
  EXPECT_EQ(cpu.getV(1), 0xFF ^ 0x0F);
}

TEST_F(CPUTest, CPUTest_Opcode_8xy4_NoCarry) {
  std::vector<Byte> rom = {
      0x61, 0x10, // V1 = 0x10
      0x62, 0x20, // V2 = 0x20
      0x81, 0x24  // V1 = V1 + V2
  };
  EXPECT_TRUE(cpu.loadROM(rom));

  cpu.cycle(); // V1 = 0x10
  cpu.cycle(); // V2 = 0x20
  cpu.cycle(); // 8124: V1 = 0x30, VF = 0
  EXPECT_EQ(cpu.getV(1), 0x30);
  EXPECT_EQ(cpu.getV().back(), 0);
}

TEST_F(CPUTest, CPUTest_Opcode_8xy4_Carry) {
  std::vector<Byte> rom = {
      0x61, 0xFF, // V1 = 0xFF
      0x62, 0x01, // V2 = 0x01
      0x81, 0x24  // V1 = V1 + V2
  };
  EXPECT_TRUE(cpu.loadROM(rom));

  cpu.cycle(); // V1 = 0xFF
  cpu.cycle(); // V2 = 0x01
  cpu.cycle(); // 8124: V1 = (0xFF + 0x01) & 0xFF = 0x00, VF = 1
  EXPECT_EQ(cpu.getV(1), 0x00);
  EXPECT_EQ(cpu.getV().back(), 1);
}

TEST_F(CPUTest, CPUTest_Opcode_8xy5_NoBorrow) {
  std::vector<Byte> rom = {
      0x61, 0x50, // V1 = 0x50
      0x62, 0x20, // V2 = 0x20
      0x81, 0x25  // V1 = V1 - V2
  };
  EXPECT_TRUE(cpu.loadROM(rom));

  cpu.cycle(); // V1 = 0x50
  cpu.cycle(); // V2 = 0x20
  cpu.cycle(); // 8125: V1 = 0x30, VF = 1 (no borrow, Vx >= Vy, VF = NOT borrow)
  EXPECT_EQ(cpu.getV(1), 0x30);
  EXPECT_EQ(cpu.getV().back(), 1);
}

TEST_F(CPUTest, CPUTest_Opcode_8xy5_Borrow) {
  std::vector<Byte> rom = {
      0x61, 0x10, // V1 = 0x10
      0x62, 0x20, // V2 = 0x20
      0x81, 0x25  // V1 = V1 - V2
  };
  EXPECT_TRUE(cpu.loadROM(rom));

  cpu.cycle(); // V1 = 0x10
  cpu.cycle(); // V2 = 0x20
  cpu.cycle(); // 8125: V1 = 0xF0 (wrapped), VF = 0 (borrow, Vx < Vy,  VF = NOT
               // borrow)
  EXPECT_EQ(cpu.getV(1), 0xF0);
  EXPECT_EQ(cpu.getV().back(), 0);
}

TEST_F(CPUTest, CPUTest_Opcode_8xy6_LSBOne) {
  std::vector<Byte> rom = {
      0x61, 0x03, // V1 = 0x03 (binary: 00000011)
      0x81, 0x06  // V1 = V1 >> 1
  };
  EXPECT_TRUE(cpu.loadROM(rom));

  cpu.cycle(); // V1 = 0x03
  cpu.cycle(); // 8106: V1 = 0x01, VF = 1 (LSB was 1)
  EXPECT_EQ(cpu.getV(1), 0x01);
  EXPECT_EQ(cpu.getV().back(), 1);
}

TEST_F(CPUTest, CPUTest_Opcode_8xy6_LSBZero) {
  std::vector<Byte> rom = {
      0x61, 0x04, // V1 = 0x04 (binary: 00000100)
      0x81, 0x06  // V1 = V1 >> 1
  };
  EXPECT_TRUE(cpu.loadROM(rom));

  cpu.cycle(); // V1 = 0x04
  cpu.cycle(); // 8106: V1 = 0x02, VF = 0 (LSB was 0)
  EXPECT_EQ(cpu.getV(1), 0x02);
  EXPECT_EQ(cpu.getV().back(), 0);
}

TEST_F(CPUTest, CPUTest_Opcode_8xy7_NoBorrow) {
  std::vector<Byte> rom = {
      0x61, 0x20, // V1 = 0x20 (Vx)
      0x62, 0x50, // V2 = 0x50 (Vy)
      0x81, 0x27  // V1 = V2 - V1
  };
  EXPECT_TRUE(cpu.loadROM(rom));

  cpu.cycle(); // V1 = 0x20
  cpu.cycle(); // V2 = 0x50
  cpu.cycle(); // 8127: V1 = 0x30, VF = 1 (no borrow, Vy >= Vx)
  EXPECT_EQ(cpu.getV(1), 0x30);
  EXPECT_EQ(cpu.getV().back(), 1);
}

TEST_F(CPUTest, CPUTest_Opcode_8xy7_Borrow) {
  std::vector<Byte> rom = {
      0x61, 0x50, // V1 = 0x50 (Vx)
      0x62, 0x20, // V2 = 0x20 (Vy)
      0x81, 0x27  // V1 = V2 - V1
  };
  EXPECT_TRUE(cpu.loadROM(rom));

  cpu.cycle(); // V1 = 0x50
  cpu.cycle(); // V2 = 0x20
  cpu.cycle(); // 8127: V1 = 0xD0 (wrapped), VF = 0 (borrow, Vy < Vx)
  EXPECT_EQ(cpu.getV(1), 0xD0);
  EXPECT_EQ(cpu.getV().back(), 0);
}

TEST_F(CPUTest, CPUTest_Opcode_8xyE_MSBOne) {
  std::vector<Byte> rom = {
      0x61, 0x80, // V1 = 0x80 (binary: 10000000)
      0x81, 0x0E  // V1 = V1 << 1
  };
  EXPECT_TRUE(cpu.loadROM(rom));

  cpu.cycle(); // V1 = 0x80
  cpu.cycle(); // 810E: V1 = 0x00, VF = 1 (MSB was 1)
  EXPECT_EQ(cpu.getV(1), 0x00);
  EXPECT_EQ(cpu.getV().back(), 1);
}

TEST_F(CPUTest, CPUTest_Opcode_8xyE_MSBZero) {
  std::vector<Byte> rom = {
      0x61, 0x40, // V1 = 0x40 (binary: 01000000)
      0x81, 0x0E  // V1 = V1 << 1
  };
  EXPECT_TRUE(cpu.loadROM(rom));

  cpu.cycle(); // V1 = 0x40
  cpu.cycle(); // 810E: V1 = 0x80, VF = 0 (MSB was 0)
  EXPECT_EQ(cpu.getV(1), 0x80);
  EXPECT_EQ(cpu.getV().back(), 0);
}

// --- Group 9 ---
TEST_F(CPUTest, CPUTest_Opcode_9xy0) {
  std::vector<Byte> rom = {
      0x61, 0x50, // V1 = 0x50 (Vx)
      0x62, 0x20, // V2 = 0x20 (Vy)
      0x91, 0x20  // 9120: skip next instruction if V1 != V2
  };
  EXPECT_TRUE(cpu.loadROM(rom));

  Word PCBeforeCycle{cpu.getPC()};
  cpu.cycle(); // 6150, pc += 2
  cpu.cycle(); // 6220, pc += 2
  cpu.cycle(); // 9120, pc += 2
  Word PCAfterCycle{cpu.getPC()};
  EXPECT_NE(cpu.getV(1), cpu.getV(2));
  EXPECT_EQ(PCBeforeCycle + 2 * 4,
            PCAfterCycle); // pc += 2 * 3, then +2 to skip next instruction
}

// --- Group A ---
TEST_F(CPUTest, CPUTest_Opcode_Annn) {
  std::vector<Byte> rom = {0xA2, 0x22}; // nnn = 222
  EXPECT_TRUE(cpu.loadROM(rom));

  cpu.cycle();
  Word I{cpu.getI()};
  EXPECT_EQ(I, 0x0222);
}

// --- Group D ---
TEST_F(CPUTest, CPUTest_Opcode_Dxyn_DrawSpriteToDisplay) {
  std::vector<Byte> rom = {
      0x60, 0x00, // V0 = 0 (column)
      0x61, 0x00, // V1 = 0 (row)
      0xA2, 0x08, // Set I = 0x208
      0xD0, 0x11, // Draw 1 row sprite start from (V0, V1)
      0xF0        // 0xF0 - top 4 pixels on
  };

  EXPECT_TRUE(cpu.loadROM(rom));
  cpu.cycle(); // V0 = 0
  cpu.cycle(); // V1 = 0
  cpu.cycle(); // I = 0x208
  cpu.cycle(); // D011

  const auto &display{cpu.getDisplay()};
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(0, 0, DISPLAY_WIDTH)], 1);
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(0, 1, DISPLAY_WIDTH)], 1);
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(0, 2, DISPLAY_WIDTH)], 1);
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(0, 3, DISPLAY_WIDTH)], 1);
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(0, 4, DISPLAY_WIDTH)], 0);
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(0, 5, DISPLAY_WIDTH)], 0);
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(0, 6, DISPLAY_WIDTH)], 0);
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(0, 7, DISPLAY_WIDTH)], 0);
}

TEST_F(CPUTest, CPUTest_Opcode_Dxyn_NoCollision) {
  std::vector<Byte> rom = {
      0x60, 0x00, // V0 = 0 (column)
      0x61, 0x00, // V1 = 0 (row)
      0xA2, 0x08, // Set I = 0x208
      0xD0, 0x11, // Draw 1 row sprite start from (V0, V1)
      0xF0        // 0xF0 - top 4 pixels on
  };

  EXPECT_TRUE(cpu.loadROM(rom));
  cpu.cycle(); // V0 = 0
  cpu.cycle(); // V1 = 0
  cpu.cycle(); // I = 0x208
  cpu.cycle(); // D011

  const auto &display{cpu.getDisplay()};
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(0, 0, DISPLAY_WIDTH)], 1);
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(0, 1, DISPLAY_WIDTH)], 1);
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(0, 2, DISPLAY_WIDTH)], 1);
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(0, 3, DISPLAY_WIDTH)], 1);
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(0, 4, DISPLAY_WIDTH)], 0);
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(0, 5, DISPLAY_WIDTH)], 0);
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(0, 6, DISPLAY_WIDTH)], 0);
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(0, 7, DISPLAY_WIDTH)], 0);
  EXPECT_EQ(cpu.getV().back(), 0);
}

TEST_F(CPUTest, CPUTest_Opcode_Dxyn_HasCollision) {
  std::vector<Byte> rom = {
      0x60, 0x00, // V0 = 0 (column)
      0x61, 0x00, // V1 = 0 (row)
      0xA2, 0x0A, // Set I = 0x20A
      0xD0, 0x11, // First draw: Draw 1 row sprite start from (V0, V1)
      0xD0, 0x11, // Second draw: Draw at same position -> collision
      0xF0, 0x00  // 0xF0 - top 4 pixels on
  };

  EXPECT_TRUE(cpu.loadROM(rom));
  cpu.cycle(); // V0 = 0
  cpu.cycle(); // V1 = 0
  cpu.cycle(); // I = 0x208
  cpu.cycle(); // D011 first draw
  cpu.cycle(); // D011 second draw

  const auto &display{cpu.getDisplay()};
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(0, 0, DISPLAY_WIDTH)], 0);
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(0, 1, DISPLAY_WIDTH)], 0);
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(0, 2, DISPLAY_WIDTH)], 0);
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(0, 3, DISPLAY_WIDTH)], 0);
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(0, 4, DISPLAY_WIDTH)], 0);
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(0, 5, DISPLAY_WIDTH)], 0);
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(0, 6, DISPLAY_WIDTH)], 0);
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(0, 7, DISPLAY_WIDTH)], 0);
  EXPECT_EQ(cpu.getV().back(), 1);
}

TEST_F(CPUTest, CPUTest_Opcode_Dxyn_HorizontalWraping) {
  std::vector<Byte> rom = {
      0x60, 0x3E, // V0 = 62 (column)
      0x61, 0x00, // V1 = 0 (row)
      0xA2, 0x08, // Set I = 0x208
      0xD0, 0x11, // Draw 1 row sprite start from (V0, V1)
      0xF0        // 0xF0 - top 4 pixels on
  };

  EXPECT_TRUE(cpu.loadROM(rom));
  cpu.cycle(); // V0 = 0
  cpu.cycle(); // V1 = 0
  cpu.cycle(); // I = 0x208
  cpu.cycle(); // D011

  const auto &display{cpu.getDisplay()};
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(0, 62, DISPLAY_WIDTH)], 1);
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(0, 63, DISPLAY_WIDTH)], 1);
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(0, 0, DISPLAY_WIDTH)], 1);
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(0, 1, DISPLAY_WIDTH)], 1);
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(0, 4, DISPLAY_WIDTH)], 0);
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(0, 5, DISPLAY_WIDTH)], 0);
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(0, 6, DISPLAY_WIDTH)], 0);
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(0, 7, DISPLAY_WIDTH)], 0);
}

TEST_F(CPUTest, CPUTest_Opcode_Dxyn_VerticalWraping) {
  std::vector<Byte> rom = {
      0x60, 0x00, // V0 = 0 (column)
      0x61, 0x1E, // V1 = 30 (row)
      0xA2, 0x08, // Set I = 0x208
      0xD0, 0x15, // Draw 5 row sprite start from (V0, V1)
      0xF0,       // 11110000
      0x10,       // 00010000
      0xF0,       // 11110000
      0x10,       // 00010000
      0xF0,       // 11110000
  };

  EXPECT_TRUE(cpu.loadROM(rom));
  cpu.cycle(); // V0 = 0
  cpu.cycle(); // V1 = 0
  cpu.cycle(); // I = 0x208
  cpu.cycle(); // D015

  const auto &display{cpu.getDisplay()};
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(30, 0, DISPLAY_WIDTH)], 1);
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(31, 1, DISPLAY_WIDTH)], 0);
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(0, 1, DISPLAY_WIDTH)], 1);
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(1, 1, DISPLAY_WIDTH)], 0);
  EXPECT_EQ(display[convert2DIndicesTo1DIndex(2, 1, DISPLAY_WIDTH)], 1);
}

// --- Group E ---
TEST_F(CPUTest, CPUTest_Opcode_Ex9E) {
  std::vector<Byte> rom = {
      0x61, 0x02, // V1 = 0x02 (Vx)
      0xE1, 0x9E, // skip next instruction if key at V1 is pressed
      0x61, 0x50, // V1 = 0x50 (Vx)
      0x62, 0x20, // V2 = 0x20 (Vy)
  };
  EXPECT_TRUE(cpu.loadROM(rom));
  cpu.setKey(2, true);
  cpu.cycle(); // 6102
  cpu.cycle(); // E19E
  cpu.cycle(); // 6220 instead of 6150 (6150 is skipped)
  EXPECT_NE(cpu.getV(1), 0x50);
  EXPECT_EQ(cpu.getV(1), 0x02);
  EXPECT_EQ(cpu.getV(2), 0x20);
}

TEST_F(CPUTest, CPUTest_Opcode_ExA1) {
  std::vector<Byte> rom = {
      0x61, 0x02, // V1 = 0x02 (Vx)
      0xE1, 0xA1, // skip next instruction if key at V1 is not pressed
      0x61, 0x50, // V1 = 0x50 (Vx)
      0x62, 0x20, // V2 = 0x20 (Vy)
  };
  EXPECT_TRUE(cpu.loadROM(rom));
  // cpu.setKey(2, true); -> don't press key at V1 anymore
  cpu.cycle(); // 6102
  cpu.cycle(); // E19E
  cpu.cycle(); // 6220 instead of 6150 (6150 is skipped)
  EXPECT_NE(cpu.getV(1), 0x50);
  EXPECT_EQ(cpu.getV(1), 0x02);
  EXPECT_EQ(cpu.getV(2), 0x20);
}

// --- Group F ---
TEST_F(CPUTest, CPUTest_Opcode_Fx07) {
  std::vector<Byte> rom = {
      0x61, 0x02, // V1 = 0x02 (Vx)
      0xF1, 0x07  // Set V1 = delay timer
  };
  EXPECT_TRUE(cpu.loadROM(rom));
  cpu.cycle();
  cpu.cycle();
  EXPECT_EQ(cpu.getV(1), cpu.getDelayTimer());
}

TEST_F(CPUTest, CPUTest_Opcode_Fx0A) {
  std::vector<Byte> rom = {
      0x61, 0x02, // V1 = 0x02 (Vx)
      0xF1, 0x0A  // F10A
  };
  EXPECT_TRUE(cpu.loadROM(rom));
  cpu.cycle();
  cpu.setKey(2, true);
  cpu.cycle();
  EXPECT_EQ(cpu.getV(1), 1);
}

TEST_F(CPUTest, CPUTest_Opcode_Fx15) {
  std::vector<Byte> rom = {
      0x61, 0x02, // V1 = 0x02 (Vx)
      0xF1, 0x15  // Set delay timer = V1
  };
  EXPECT_TRUE(cpu.loadROM(rom));
  cpu.cycle();
  cpu.cycle();
  EXPECT_EQ(cpu.getV(1), cpu.getDelayTimer());
}

TEST_F(CPUTest, CPUTest_Opcode_Fx18) {
  std::vector<Byte> rom = {
      0x61, 0x02, // V1 = 0x02 (Vx)
      0xF1, 0x18  // Set sound timer = V1
  };
  EXPECT_TRUE(cpu.loadROM(rom));
  cpu.cycle();
  cpu.cycle();
  EXPECT_EQ(cpu.getV(1), cpu.getSoundTimer());
}

TEST_F(CPUTest, CPUTest_Opcode_Fx1E) {
  std::vector<Byte> rom = {
      0x61, 0x02, // V1 = 0x02 (Vx)
      0xF1, 0x1E  // Set I = I + V1
  };
  Word IBefore{cpu.getI()};
  EXPECT_TRUE(cpu.loadROM(rom));
  cpu.cycle();
  cpu.cycle();
  Word IAfter{cpu.getI()};
  EXPECT_EQ(IBefore + 0x02, IAfter);
}

TEST_F(CPUTest, CPUTest_Opcode_Fx29) {
  std::vector<Byte> rom = {
      0x61, 0x02, // V1 = 0x02 (Vx)
      0xF1, 0x29  // Set I = Vx * 5
  };
  Word IBefore{cpu.getI()};
  EXPECT_TRUE(cpu.loadROM(rom));
  cpu.cycle();
  cpu.cycle();
  Word IAfter{cpu.getI()};
  EXPECT_EQ(IBefore + (5 * 0x02), IAfter);
}

TEST_F(CPUTest, CPUTest_Opcode_Fx33) {
  std::vector<Byte> rom = {
      0x61, 0xFF, // V1 = 0xFF (Vx)
      0xF1, 0x33  // Save BCD of Vx to memory at I, I + 1, I + 2
  };
  EXPECT_TRUE(cpu.loadROM(rom));
  cpu.cycle();
  cpu.cycle();
  EXPECT_EQ(cpu.getMemory()[cpu.getI()], 2);
  EXPECT_EQ(cpu.getMemory()[cpu.getI() + 1], 5);
  EXPECT_EQ(cpu.getMemory()[cpu.getI() + 2], 5);
}

TEST_F(CPUTest, CPUTest_Opcode_Fx55) {
  std::vector<Byte> rom = {
      0x61, 0xFF, // V1 = 0xFF
      0x62, 0xF0, // V2 = 0xF0
      0x63, 0x0F, // V2 = 0x0F
      0xF3, 0x55  // Save registers from V0->Vx to memory
  };
  EXPECT_TRUE(cpu.loadROM(rom));
  cpu.cycle();
  cpu.cycle();
  cpu.cycle();
  cpu.cycle();
  for (std::size_t i{}; i <= 3; ++i) {
    EXPECT_EQ(cpu.getMemory()[cpu.getI() + i], cpu.getV(i));
  }
}

TEST_F(CPUTest, CPUTest_Opcode_Fx65) {
  std::vector<Byte> rom = {
      0x61, 0xFF, // V1 = 0xFF
      0x62, 0xF0, // V2 = 0xF0
      0x63, 0x0F, // V2 = 0x0F
      0xF3, 0x65  // Read registers to V0->Vx from memory
  };
  EXPECT_TRUE(cpu.loadROM(rom));
  cpu.cycle();
  cpu.cycle();
  cpu.cycle();
  cpu.cycle();
  for (std::size_t i{}; i <= 3; ++i) {
    EXPECT_EQ(cpu.getMemory()[cpu.getI() + i], cpu.getV(i));
  }
}