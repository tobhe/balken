enable_testing()

include(GoogleTest)

find_package(GTest REQUIRED)

add_executable(UnitTests
  testsuite.cc
  datamatrix_test.cc
  )
target_include_directories(UnitTests PRIVATE . ../src)
target_link_libraries(UnitTests GTest::GTest GTest::Main blaze)

gtest_discover_tests(UnitTests)

