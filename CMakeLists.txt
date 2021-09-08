cmake_minimum_required(VERSION 3.10)
project(IFJ_project VERSION 0.1 LANGUAGES CXX C)

set(PROJECT_TEST_NAME run_tests)

set(CMAKE_BUILD_TYPE Debug)

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

# add coverage target
if(CMAKE_COMPILER_IS_GNUCXX)
    list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules")
    include(CodeCoverage)
    append_coverage_compiler_flags()

    # exclude system header files, google deps and tests
    set(COVERAGE_LCOV_EXCLUDES "/usr/include/*" "build/_deps/*" "tests/*")
    SETUP_TARGET_FOR_COVERAGE_LCOV(
        NAME coverage
        EXECUTABLE ${PROJECT_TEST_NAME}
        DEPENDENCIES ${PROJECT_TEST_NAME})
endif()


# add test files and link them
file(GLOB_RECURSE TEST_FILES tests/*.cpp)
add_executable(${PROJECT_TEST_NAME}
    ${TEST_FILES}
)
target_link_libraries(${PROJECT_TEST_NAME} gtest_main)
gtest_discover_tests(${PROJECT_TEST_NAME})
