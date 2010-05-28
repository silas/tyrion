#include <gtest/gtest.h>
#include "test_config.h"
#include "test_process.h"
#include "test_setting.h"
#include "test_queue.h"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
