#include <gtest/gtest.h>

int main(int argc, char * argv[])
{
  // Init GoogleTest (strips gtest arguments from argv)
  ::testing::InitGoogleTest(&argc, argv);

  // Run Tests
  int ret = RUN_ALL_TESTS();

  return ret;
}
