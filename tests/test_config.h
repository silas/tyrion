#include <gtest/gtest.h>
#include "../src/config.h"

namespace {

using namespace tyrion;

TEST(ConfigTest, Basic) {
  Config c("tests/assets/test.ini");
  EXPECT_FALSE(c.ParseError());

  // string
  EXPECT_TRUE(c.Has("string", "space"));
  EXPECT_EQ("hello world", c.Get("string", "space", "fail"));
  EXPECT_FALSE(c.Has("string", "fail"));
  EXPECT_EQ("default", c.Get("string", "fail", "default"));

  // int
  EXPECT_TRUE(c.Has("int", "one"));
  EXPECT_EQ(1, c.GetInt("int", "one", 0));
  EXPECT_FALSE(c.Has("int", "fail"));
  EXPECT_EQ(1, c.GetInt("int", "fail", 1));
}

TEST(ConfigTest, NoFile) {
  Config c("tests/assets/file-which-does-not-exist");
  EXPECT_TRUE(c.ParseError());
}

}  // namespace
