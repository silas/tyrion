#include <gtest/gtest.h>
#include <string>
#include "../src/process.h"

namespace {

using namespace std;
using namespace tyrion;

TEST(ProcessTest, Close) {
  Process p1 = Process("assets/test_process_close_code_true");
  p1.Run();
  EXPECT_EQ("", p1.ReadAll(Process::Stdout));
  EXPECT_EQ("", p1.ReadAll(Process::Stderr));
  EXPECT_EQ(0, p1.Close());

  Process p2 = Process("assets/test_process_close_code_false");
  p2.Run();
  EXPECT_EQ("", p2.ReadAll(Process::Stdout));
  EXPECT_EQ("", p2.ReadAll(Process::Stderr));
  EXPECT_EQ(1, p2.Close());

  Process p3 = Process("true", true);
  p3.Run();
  EXPECT_EQ("", p3.ReadAll(Process::Stdout));
  EXPECT_EQ("", p3.ReadAll(Process::Stderr));
  EXPECT_EQ(0, p3.Close());

  Process p4 = Process("false", true);
  p4.Run();
  EXPECT_EQ("", p4.ReadAll(Process::Stdout));
  EXPECT_EQ("", p4.ReadAll(Process::Stderr));
  EXPECT_EQ(1, p4.Close());
}

TEST(ProcessTest, System) {
  Process p = Process("echo test", true);
  p.Run();
  EXPECT_EQ("test\n", p.ReadAll(Process::Stdout));
  EXPECT_EQ("", p.ReadAll(Process::Stderr));
  EXPECT_EQ(0, p.Close());
}

TEST(ProcessTest, Timeout) {
  Process p1 = Process("assets/test_process_timeout", false, 1);
  p1.Run();
  EXPECT_EQ("test\n", p1.ReadAll(Process::Stdout));
  EXPECT_EQ("", p1.ReadAll(Process::Stderr));
  EXPECT_EQ(15, p1.Close());
  EXPECT_TRUE(p1.TimedOut());

  Process p2 = Process("echo test; sleep 2", true, 1);
  p2.Run();
  EXPECT_EQ("test\n", p2.ReadAll(Process::Stdout));
  EXPECT_EQ("", p2.ReadAll(Process::Stderr));
  EXPECT_EQ(15, p2.Close());
  EXPECT_TRUE(p2.TimedOut());
}

TEST(ProcessTest, Write) {
  Process p1 = Process("assets/test_process_write");
  p1.Run();
  p1.Write("1+1\nquit()\n");
  EXPECT_EQ("2\n", p1.ReadAll(Process::Stdout));
  EXPECT_EQ("", p1.ReadAll(Process::Stderr));
  EXPECT_EQ(0, p1.Close());

  Process p2 = Process("bc -q", true);
  p2.Run();
  p2.Write("1+1\nquit()\n");
  EXPECT_EQ("2\n", p2.ReadAll(Process::Stdout));
  EXPECT_EQ("", p2.ReadAll(Process::Stderr));
  EXPECT_EQ(0, p2.Close());
}

}  // namespace
