#include "../src/cpu.h"
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

TEST_F(CPUTest, CPUTest_DefaultInit_Test) {
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

TEST_F(CPUTest, CPUTest_Reset_Test) {
  cpu.reset();

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