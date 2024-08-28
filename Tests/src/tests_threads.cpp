#include <gtest/gtest.h>
#include <Todo/Thread.hpp>

TEST(TestThreads, HandleConstructionDestruction) {
  todo::Thread* threadptr = new todo::Thread();
  EXPECT_TRUE(threadptr != nullptr) << "The Thread creation has failed.";

  delete threadptr;
  SUCCEED();
}