#include <fcntl.h>
#include <gtest/gtest.h>
#include <string>
#include "../src/process.h"

namespace {

using namespace std;
using namespace tyrion;

class TestProcess : public Process {
  public:
    TestProcess(string command) : Process(command) {
      Setup();
    }
    TestProcess(string command, bool system) : Process(command, system) {
      Setup();
    }
    TestProcess(string command, bool system, int timeout)
        : Process(command, system, timeout) {
      Setup();
    }

    void Setup() {
      fcntl(outfd[0][0], F_SETFL, O_NONBLOCK);
      fcntl(outfd[1][0], F_SETFL, O_NONBLOCK);
    }

    string Read(Process::Type type) {
      char input[1024];
      int rc = read(outfd[type][0], input, 1023);

      if (rc == 0) {
        outfdeof[type] = true;
      } else if (rc == -1) {
        usleep(100000);
      } else {
        input[rc] = 0;
        return string(input);
      }

      return ""; 
    }

    string ReadAll(Process::Type type) {
      string data;
      string buffer;

      while(!outfdeof[type] && !TimedOut()) {
        buffer = Read(type);

        if (!buffer.empty())
          data += buffer;
      }

      return data;
    }
};

TEST(ProcessTest, Close) {
  TestProcess p1 = TestProcess("assets/test_process_close_code_true");
  p1.Run();
  EXPECT_EQ("", p1.ReadAll(Process::Stdout));
  EXPECT_EQ("", p1.ReadAll(Process::Stderr));
  EXPECT_EQ(0, p1.Close());

  TestProcess p2 = TestProcess("assets/test_process_close_code_false");
  p2.Run();
  EXPECT_EQ("", p2.ReadAll(Process::Stdout));
  EXPECT_EQ("", p2.ReadAll(Process::Stderr));
  EXPECT_EQ(1, p2.Close());

  TestProcess p3 = TestProcess("true", true);
  p3.Run();
  EXPECT_EQ("", p3.ReadAll(Process::Stdout));
  EXPECT_EQ("", p3.ReadAll(Process::Stderr));
  EXPECT_EQ(0, p3.Close());

  TestProcess p4 = TestProcess("false", true);
  p4.Run();
  EXPECT_EQ("", p4.ReadAll(Process::Stdout));
  EXPECT_EQ("", p4.ReadAll(Process::Stderr));
  EXPECT_EQ(1, p4.Close());
}

TEST(ProcessTest, System) {
  TestProcess p = TestProcess("echo test", true);
  p.Run();
  EXPECT_EQ("test\n", p.ReadAll(Process::Stdout));
  EXPECT_EQ("", p.ReadAll(Process::Stderr));
  EXPECT_EQ(0, p.Close());
}

TEST(ProcessTest, Timeout) {
  TestProcess p1 = TestProcess("assets/test_process_timeout", false, 1);
  p1.Run();
  EXPECT_EQ("test\n", p1.ReadAll(Process::Stdout));
  EXPECT_EQ("", p1.ReadAll(Process::Stderr));
  EXPECT_EQ(15, p1.Close());
  EXPECT_TRUE(p1.TimedOut());

  TestProcess p2 = TestProcess("echo test; sleep 4", true, 2);
  p2.Run();
  EXPECT_EQ("test\n", p2.ReadAll(Process::Stdout));
  EXPECT_EQ("", p2.ReadAll(Process::Stderr));
  EXPECT_EQ(15, p2.Close());
  EXPECT_TRUE(p2.TimedOut());
}

TEST(ProcessTest, Write) {
  TestProcess p1 = TestProcess("assets/test_process_write");
  p1.Run();
  p1.Write("1+1\nquit()\n");
  EXPECT_EQ("2\n", p1.ReadAll(Process::Stdout));
  EXPECT_EQ("", p1.ReadAll(Process::Stderr));
  EXPECT_EQ(0, p1.Close());

  TestProcess p2 = TestProcess("bc -q", true);
  p2.Run();
  p2.Write("1+1\nquit()\n");
  EXPECT_EQ("2\n", p2.ReadAll(Process::Stdout));
  EXPECT_EQ("", p2.ReadAll(Process::Stderr));
  EXPECT_EQ(0, p2.Close());
}

}  // namespace
