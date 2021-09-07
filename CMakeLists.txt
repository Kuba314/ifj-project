cmake_minimum_required(VERSION 3.10)
project(myproj VERSION 0.1 LANGUAGES CXX C)

# compiler flags
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_FLAGS "-Werror -Wall -Wextra -pedantic")

# add include/ directory
include_directories(${CMAKE_SOURCE_DIR}/include)

# fetch gtest
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/955c7f837efad184ec63e771c42542d37545eaef.zip
)
FetchContent_MakeAvailable(googletest)
enable_testing()

include(GoogleTest)

# add test files and link them
file(GLOB_RECURSE TEST_FILES tests/*.cpp)
add_executable(run_tests ${TEST_FILES})
target_link_libraries(run_tests gtest_main)
gtest_discover_tests(run_tests)
