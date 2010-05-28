#include <gtest/gtest.h>
#include <string>
#include "../src/process.h"

namespace {

using namespace std;
using namespace tyrion;

TEST(ProcessTest, CloseCode) {
  Process p1 = Process("true", true);
  p1.Run();
  EXPECT_EQ(0, p1.Close());

  Process p2 = Process("false", true);
  p2.Run();
  EXPECT_EQ(1, p2.Close());
}

TEST(ProcessTest, System) {
  Process p = Process("echo test", true);
  p.Run();
  EXPECT_EQ("test\n", p.ReadAll(Process::Stdout));
  EXPECT_EQ("", p.ReadAll(Process::Stderr));
  EXPECT_EQ(0, p.Close());
}

TEST(ProcessTest, Write) {
  Process p = Process("assets/test_process_write");
  p.Run();
  p.Write("1+1\nquit()\n");
  EXPECT_EQ("2\n", p.ReadAll(Process::Stdout));
  EXPECT_EQ("", p.ReadAll(Process::Stderr));
  EXPECT_EQ(0, p.Close());
}

}  // namespace
