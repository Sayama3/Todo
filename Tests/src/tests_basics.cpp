//
// Created by Sayama on 28/08/2024.
//

#include <gtest/gtest.h>
#include "Todo/Multithreading/TypePadder.hpp"

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);

  struct Vec3 {
    float x,y,z;
  };

  struct Vec4 {
    float x,y,z,w;
  };

  EXPECT_EQ(sizeof(Todo::ModPadder8<Vec3>), sizeof(Todo::ModPadder8<Vec4>));
}