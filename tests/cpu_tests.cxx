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

TEST_F(CPUTest, CPUTest_Opcode_00E0) {
  std::vector<Byte> rom = {0x00, 0xE0};
  EXPECT_TRUE(cpu.loadROM(rom));

  cpu.cycle();
  const auto &display{cpu.getDisplay()};
  EXPECT_TRUE(allZero(display));
}

TEST_F(CPUTest, CPUTest_Opcode_1nnn) {
  std::vector<Byte> rom = {0x12, 0x22};
  EXPECT_TRUE(cpu.loadROM(rom));

  cpu.cycle();
  Word PC{cpu.getPC()};
  EXPECT_EQ(PC, 0x0222);
}

TEST_F(CPUTest, CPUTest_Opcode_6xkk) {
  std::vector<Byte> rom = {0x61, 0x22}; // x = 1, kk = 22
  EXPECT_TRUE(cpu.loadROM(rom));

  cpu.cycle();
  Byte Vx{cpu.getV(1)};
  EXPECT_EQ(Vx, 0x22);
}

TEST_F(CPUTest, CPUTest_Opcode_7xkk) {
  std::vector<Byte> rom = {0x71, 0x22}; // x = 1, kk = 22
  EXPECT_TRUE(cpu.loadROM(rom));

  Byte VxBefore{cpu.getV(1)};
  cpu.cycle();
  Byte VxAfter{cpu.getV(1)};
  EXPECT_EQ(VxAfter, VxBefore + 0x22);
}

TEST_F(CPUTest, CPUTest_Opcode_Annn) {
  std::vector<Byte> rom = {0xA2, 0x22}; // nnn = 222
  EXPECT_TRUE(cpu.loadROM(rom));

  cpu.cycle();
  Word I{cpu.getI()};
  EXPECT_EQ(I, 0x0222);
}

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
  const auto &V{cpu.getV()};
  EXPECT_EQ(V.back(), 0);
}

TEST_F(CPUTest, CPUTest_Opcode_Dxyn_HasCollision) {
  std::vector<Byte> rom = {
      0x60, 0x00, // V0 = 0 (column)
      0x61, 0x00, // V1 = 0 (row)
      0xA2, 0x08, // Set I = 0x208
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
  const auto &V{cpu.getV()};
  EXPECT_EQ(V.back(), 1);
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
