#include <gtest/gtest.h>
#include "../src/settings.h"

namespace {

using namespace tyrion;

TEST(SettingTest, Basic) {
  Settings s("tests/assets/test.ini");

  EXPECT_FALSE(s.HasError());

  // string
  EXPECT_TRUE(s.Has("string", "space"));
  EXPECT_EQ("hello world", s.Get("string", "space", "fail"));
  EXPECT_FALSE(s.Has("string", "fail"));
  EXPECT_EQ("", s.Get("string", "fail"));
  EXPECT_EQ("default", s.Get("string", "fail", "default"));

  // int
  EXPECT_TRUE(s.Has("int", "one"));
  EXPECT_EQ(1, s.GetInt("int", "one", 0));
  EXPECT_FALSE(s.Has("int", "fail"));
  EXPECT_EQ(0, s.GetInt("int", "fail"));
  EXPECT_EQ(1, s.GetInt("int", "fail", 1));

  // bool
  EXPECT_TRUE(s.Has("bool", "true"));
  EXPECT_EQ(true, s.GetBool("bool", "true", false));
  EXPECT_TRUE(s.Has("bool", "false"));
  EXPECT_EQ(false, s.GetBool("bool", "false", true));
  EXPECT_FALSE(s.Has("bool", "fail"));
  EXPECT_EQ(false, s.GetBool("bool", "fail"));
  EXPECT_EQ(true, s.GetBool("bool", "fail", true));
}

}  // namespace
