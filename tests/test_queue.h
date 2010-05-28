#include <gtest/gtest.h>
#include "../src/queue.h"

namespace {

using namespace tyrion;

TEST(QueueTest, Basic) {
  Queue<int> q;

  EXPECT_TRUE(q.empty());
  EXPECT_EQ(0, q.size());

  q.Push(10);

  EXPECT_FALSE(q.empty());
  EXPECT_EQ(1, q.size());

  EXPECT_EQ(10, q.Pop());

  EXPECT_TRUE(q.empty());
  EXPECT_EQ(0, q.size());

}

}  // namespace
