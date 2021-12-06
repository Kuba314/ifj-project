CC = gcc
CXX = g++
CFLAGS = -std=c11 -Werror
CXXFLAGS = -std=c++11
LDFLAGS = --coverage
CPPFLAGS = -Wall -Wextra -pedantic -Iinclude/ -g --coverage
TARGETS = all_tests
PACKED_PROJECT = xrozek02.tar.gz
DOC = dokumentace
DOC_DIR = doc
DEP_DIR = dep_dir

IS_IT_OK_SCRIPT = ./tests/is_it_ok.sh
EXECUTABLE = ifj21_compiler
LIB_OBJECTS = \
	src/utils/dynstring.o \
	src/utils/stack.o \
	src/utils/search_key.o \
	src/utils/binary_search_tree.o \
	src/utils/hashtablebase.o \
	src/utils/deque.o \
	src/parser-syn.o \
	src/parser-generated.o \
	src/parser-precedence.o \
	src/type.o \
	src/scanner.o \
	src/symtable.o \
	src/semantics.o

TEST_SOURCES = $(wildcard tests/*.cpp)
TEST_OBJECTS = $(patsubst %.cpp, %.o, $(TEST_SOURCES))
COV_REPORT_FILES = coverage/ $(shell find . -type f \( -name '*.gc??' -o -name '*.info' \))
ALL_OBJECTS = $(shell find . -type f -name '*.o')
ALL_SOURCE_FILES = $(shell find . -type f -name '*.c')
ALL_HEADER_FILES = $(shell find . -type f -name '*.h')
OBJ=$(SRC:.c=.o)

.PHONY: all test doc clean pack is_it_ok

vpath %.h include/
vpath %.c src/

# compiling in a folder with no directory structure
all: $(EXECUTABLE)

$(EXECUTABLE): $(LIB_OBJECTS) src/main.o
	gcc -o $@ $^ $(LDFLAGS)

src/main.o: src/main.c

# link test files with gtest
all_tests: $(TEST_OBJECTS) $(LIB_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ -lstdc++ -lgtest -lgtest_main -lpthread

test: all_tests
	./all_tests

doc:
	doxygen Doxyfile
	cd $(DOC_DIR) && pdflatex $(DOC).tex
	cd $(DOC_DIR) && pdflatex $(DOC).tex

coverage: test
	lcov --capture --output-file coverage.info -d . -q
	lcov --remove coverage.info '/usr/*' --output-file=coverage.info -q
	genhtml coverage.info --output-directory coverage -q

pack: clean doc
	mkdir -p $(DEP_DIR)
	cp $(ALL_SOURCE_FILES) $(ALL_HEADER_FILES) rozdeleni rozsireni Makefile $(DEP_DIR)/
	cd $(DEP_DIR) && tar -czf $(PACKED_PROJECT) *
	cp $(DEP_DIR)/$(PACKED_PROJECT) .

is_it_ok: pack
	cp $(IS_IT_OK_SCRIPT) ./$(DEP_DIR)
	chmod +x $(DEP_DIR)/is_it_ok.sh
	mkdir -p $(DEP_DIR)/test
	cd $(DEP_DIR) && ./is_it_ok.sh $(PACKED_PROJECT) test

clean:
	rm -rf $(TARGETS) $(ALL_OBJECTS) $(COV_REPORT_FILES)
	[ -f $(DOC_DIR) ] && cd $(DOC_DIR) && rm -f $(DOC).{aux,dvi,log,ps,out,toc,pdf} *.log || exit 0
	rm -rf $(DOC_DIR)/html $(DEP_DIR)
	rm -f $(DOC).pdf $(PACKED_PROJECT)
