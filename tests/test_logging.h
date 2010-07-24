#include <cstdio>
#include <gtest/gtest.h>
#include "../src/logging.h"

namespace {

using namespace std;
using namespace tyrion;

static const string static_file = "./assets/test_logging-file.log";

class LoggingTest : public ::testing::Test {
 protected:
  LoggingTest() : l(NULL) {
  }
  virtual ~LoggingTest() {
  }

  virtual void SetUp() {
    l = Logging::New();
  }

  virtual void TearDown() {
    if (l != NULL) {
      delete l;
      l = NULL;
    }
    remove(static_file.c_str());
  }
  Logging* l;
  DISALLOW_EVIL_CONSTRUCTORS(LoggingTest);
};

TEST_F(LoggingTest, Basic) {
  EXPECT_EQ(Logging::NONE, l->debug_level());
  EXPECT_EQ(Logging::NONE, l->file_level());
  EXPECT_EQ(Logging::NONE, l->lowest_level());

  l->Debug(Logging::CRITICAL);
  EXPECT_EQ(Logging::CRITICAL, l->debug_level());
  EXPECT_EQ(Logging::NONE, l->file_level());
  EXPECT_EQ(Logging::CRITICAL, l->lowest_level());

  l->File(static_file, Logging::INFO);
  EXPECT_EQ(Logging::CRITICAL, l->debug_level());
  EXPECT_EQ(Logging::INFO, l->file_level());
  EXPECT_EQ(Logging::INFO, l->lowest_level());
}

}  // namespace
