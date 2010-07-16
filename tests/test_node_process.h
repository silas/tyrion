#include <fcntl.h>
#include <gtest/gtest.h>
#include <string>
#include "../src/node_process.h"

namespace {

using namespace std;
using namespace tyrion;

class TestNodeProcess : public NodeProcess {
  public:
    TestNodeProcess(string command) : NodeProcess(command) {
      Setup();
    }
    TestNodeProcess(string command, bool system) : NodeProcess(command, system) {
      Setup();
    }
    TestNodeProcess(string command, bool system, int timeout)
        : NodeProcess(command, system, timeout) {
      Setup();
    }

    void Setup() {
      fcntl(outfd[0][0], F_SETFL, O_NONBLOCK);
      fcntl(outfd[1][0], F_SETFL, O_NONBLOCK);
    }

    string Read(NodeProcess::Type type) {
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

    string ReadAll(NodeProcess::Type type) {
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

TEST(NodeProcessTest, Close) {
  TestNodeProcess p1 = TestNodeProcess("tests/assets/test_process_close_code_true");
  p1.Run();
  EXPECT_EQ("", p1.ReadAll(NodeProcess::Stdout));
  EXPECT_EQ("", p1.ReadAll(NodeProcess::Stderr));
  EXPECT_EQ(0, p1.Close());

  TestNodeProcess p2 = TestNodeProcess("tests/assets/test_process_close_code_false");
  p2.Run();
  EXPECT_EQ("", p2.ReadAll(NodeProcess::Stdout));
  EXPECT_EQ("", p2.ReadAll(NodeProcess::Stderr));
  EXPECT_EQ(1, p2.Close());

  TestNodeProcess p3 = TestNodeProcess("true", true);
  p3.Run();
  EXPECT_EQ("", p3.ReadAll(NodeProcess::Stdout));
  EXPECT_EQ("", p3.ReadAll(NodeProcess::Stderr));
  EXPECT_EQ(0, p3.Close());

  TestNodeProcess p4 = TestNodeProcess("false", true);
  p4.Run();
  EXPECT_EQ("", p4.ReadAll(NodeProcess::Stdout));
  EXPECT_EQ("", p4.ReadAll(NodeProcess::Stderr));
  EXPECT_EQ(1, p4.Close());
}

TEST(NodeProcessTest, System) {
  TestNodeProcess p = TestNodeProcess("echo test", true);
  p.Run();
  EXPECT_EQ("test\n", p.ReadAll(NodeProcess::Stdout));
  EXPECT_EQ("", p.ReadAll(NodeProcess::Stderr));
  EXPECT_EQ(0, p.Close());
}

TEST(NodeProcessTest, Timeout) {
  TestNodeProcess p1 = TestNodeProcess("tests/assets/test_process_timeout", false, 1);
  p1.Run();
  EXPECT_EQ("test\n", p1.ReadAll(NodeProcess::Stdout));
  EXPECT_EQ("", p1.ReadAll(NodeProcess::Stderr));
  EXPECT_EQ(15, p1.Close());
  EXPECT_TRUE(p1.TimedOut());

  TestNodeProcess p2 = TestNodeProcess("echo test; sleep 2", true, 1);
  p2.Run();
  EXPECT_EQ("test\n", p2.ReadAll(NodeProcess::Stdout));
  EXPECT_EQ("", p2.ReadAll(NodeProcess::Stderr));
  EXPECT_EQ(15, p2.Close());
  EXPECT_TRUE(p2.TimedOut());
}

TEST(NodeProcessTest, Write) {
  TestNodeProcess p1 = TestNodeProcess("tests/assets/test_process_write");
  p1.Run();
  p1.Write("1+1\nquit()\n");
  EXPECT_EQ("2\n", p1.ReadAll(NodeProcess::Stdout));
  EXPECT_EQ("", p1.ReadAll(NodeProcess::Stderr));
  EXPECT_EQ(0, p1.Close());

  TestNodeProcess p2 = TestNodeProcess("bc -q", true);
  p2.Run();
  p2.Write("1+1\nquit()\n");
  EXPECT_EQ("2\n", p2.ReadAll(NodeProcess::Stdout));
  EXPECT_EQ("", p2.ReadAll(NodeProcess::Stderr));
  EXPECT_EQ(0, p2.Close());
}

}  // namespace
