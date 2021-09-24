CC = gcc
CXX = g++
CFLAGS = -std=c11
LDFLAGS = --coverage
CXXFLAGS = -std=c++11
CPPFLAGS = -Werror -Wall -Wextra -pedantic -Iinclude/ --coverage
TARGETS = all_tests
LIB_OBJECTS =

TEST_SOURCES = $(wildcard tests/*.cpp)
TEST_OBJECTS = $(patsubst %.cpp, %.o, $(TEST_SOURCES))

COV_REPORT_FILES = coverage/ $(shell find . -type f \( -name '*.gc??' -o -name '*.info' \))
ALL_OBJECTS = $(shell find . -type f -name '*.o')

.PHONY: all test clean

vpath %.h include/
vpath %.c src/

all: all_tests

# link test files with gtest
all_tests: $(TEST_OBJECTS) $(LIB_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS) -lstdc++ -lgtest -lgtest_main

test: all_tests
	./all_tests

# collect *.gcda files, remove system includes and generate coverage html report
# TODO: don't put --coverage into relase binaries, compile own binary with this flag
coverage: test
	lcov --capture --output-file coverage.info -d . -q
	lcov --remove coverage.info '/usr/*' --output-file=coverage.info -q
	genhtml coverage.info --output-directory coverage -q

clean:
	rm -rf $(TARGETS) $(ALL_OBJECTS) $(COV_REPORT_FILES)
