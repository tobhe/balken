cmake_minimum_required(VERSION 3.8)

project(balken)

set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)
add_compile_options (-Wall -Wextra -Wpedantic -fPIC -Wfatal-errors)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_PREFIX_PATH
  $CMAKE_PREFIX_PATH
  ${PROJECT_SOURCE_DIR}/cmake-modules
)

set(CMAKE_MODULE_PATH
  $CMAKE_MODULE_PATH
  ${PROJECT_SOURCE_DIR}/cmake-modules
)

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(example)
add_subdirectory(thirdparty/blaze)
