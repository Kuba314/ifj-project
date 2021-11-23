CC = gcc
CXX = g++
CFLAGS = -std=c11 -Werror
LDFLAGS = --coverage
CXXFLAGS = -std=c++11
CPPFLAGS = -Wall -Wextra -pedantic -Iinclude/ --coverage
TARGETS = all_tests
LIB_OBJECTS = \
	src/utils/dynstring.o \
	src/utils/stack.o \
	src/utils/search_key.o \
	src/utils/binary_search_tree.o \
	src/utils/hashtablebase.o \
	src/utils/deque.o \
	src/parser-syn.o \
	src/parser-generated.o \
	src/type.o \
	src/scanner.o \
	src/symtable.o

TEST_SOURCES = $(wildcard tests/*.cpp)
TEST_OBJECTS = $(patsubst %.cpp, %.o, $(TEST_SOURCES))

COV_REPORT_FILES = coverage/ $(shell find . -type f \( -name '*.gc??' -o -name '*.info' \))
ALL_OBJECTS = $(shell find . -type f -name '*.o')

.PHONY: all test doc clean

vpath %.h include/
vpath %.c src/

all: all_tests

# link test files with gtest
all_tests: $(TEST_OBJECTS) $(LIB_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS) -lstdc++ -lgtest -lgtest_main -lpthread -lm

test: all_tests
	./all_tests

doc:
	doxygen Doxyfile

# collect *.gcda files, remove system includes and generate coverage html report
# TODO: don't put --coverage into relase binaries, compile own binary with this flag
coverage: test
	lcov --capture --output-file coverage.info -d . -q
	lcov --remove coverage.info '/usr/*' --output-file=coverage.info -q
	genhtml coverage.info --output-directory coverage -q

clean:
	rm -rf $(TARGETS) $(ALL_OBJECTS) $(COV_REPORT_FILES)

