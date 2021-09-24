CC = gcc
CXX = g++
CFLAGS = -std=c11
LDFLAGS = --coverage
CXXFLAGS = -std=c++11
CPPFLAGS = -Werror -Wall -Wextra -pedantic --coverage
TARGETS = all_tests

TEST_SOURCES = $(wildcard tests/*.cpp)
TEST_OBJECTS = $(patsubst %.cpp, %.o, $(TEST_SOURCES))

.PHONY: all test clean

vpath %.h include/
vpath %.c src/

all: all_tests

# link test files with gtest
all_tests: $(TEST_OBJECTS)
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
	rm -f $(TARGETS) *.o tests/*.o tests/*.gc?? *.info
