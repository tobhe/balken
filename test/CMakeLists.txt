enable_testing()

include(GoogleTest)

find_package(GTest REQUIRED)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost COMPONENTS gil)

if(Boost_FOUND)
  add_executable (test
    test.cc
    )
  target_include_directories(test
    ${Boost_INCLUDE_DIRS}
    .)
  target_link_libraries(test
    ${Boost_LIBRARIES}
    balken
    )

endif()

find_package( blaze )
if( blaze_FOUND )
  add_executable(UnitTests
    testsuite.cc
    mser_test.cc
    )
  target_include_directories(UnitTests PRIVATE .)
  target_link_libraries(UnitTests GTest::GTest GTest::Main balken blaze::blaze)
endif()

gtest_discover_tests(UnitTests)

