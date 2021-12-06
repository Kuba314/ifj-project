CC = gcc
CXX = g++
CFLAGS = -std=c11 -Werror
CXXFLAGS = -std=c++11
CPPFLAGS = -Wall -Wextra -pedantic -Iinclude/ -g
EXECUTABLE = ifj21_compiler
TEST_EXECUTABLE = all_tests
TARGETS = $(TEST_EXECUTABLE) $(EXECUTABLE)
PACKED_PROJECT = xrozek02.tgz
DOC = dokumentace
DOC_DIR = doc
DEP_DIR = dep_dir

IS_IT_OK_SCRIPT = ./tests/is_it_ok.sh
LIB_OBJECTS = $(patsubst %.c, %.o, $(shell find . ! -name 'main.c' -type f -name '*.c'))

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

$(EXECUTABLE): $(LIB_OBJECTS) main.o
	gcc -o $@ $^ $(LDFLAGS)

main.o: main.c

# link test files with gtest
$(TEST_EXECUTABLE): $(TEST_OBJECTS) $(LIB_OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ -lstdc++ -lgtest -lgtest_main -lpthread

test: $(TEST_EXECUTABLE)
	./$(TEST_EXECUTABLE)

doc:
	doxygen Doxyfile
	cd $(DOC_DIR) && pdflatex $(DOC).tex && pdflatex $(DOC).tex

pack: clean doc
	mkdir -p $(DEP_DIR)
	cp $(ALL_SOURCE_FILES) $(ALL_HEADER_FILES) $(DOC_DIR)/$(DOC).pdf rozdeleni rozsireni Makefile $(DEP_DIR)/
	cd $(DEP_DIR) && tar -czf $(PACKED_PROJECT) *
	cp $(DEP_DIR)/$(PACKED_PROJECT) .

is_it_ok: pack
	mkdir -p $(DEP_DIR)/test
	cp $(IS_IT_OK_SCRIPT) ./$(DEP_DIR)
	chmod +x $(DEP_DIR)/is_it_ok.sh
	cd $(DEP_DIR) && ./is_it_ok.sh $(PACKED_PROJECT) test

clean:
	rm -rf $(TARGETS) $(ALL_OBJECTS) $(COV_REPORT_FILES)
	[ -f $(DOC_DIR) ] && cd $(DOC_DIR) && rm -f $(DOC).{aux,dvi,log,ps,out,toc,pdf} *.log || exit 0
	rm -rf $(DOC_DIR)/html $(DEP_DIR)
	rm -f $(DOC).pdf $(PACKED_PROJECT)
