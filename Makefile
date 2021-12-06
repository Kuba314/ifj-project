CC = gcc
CXX = g++
CFLAGS = -std=c11 -Werror
LDFLAGS = --coverage
CXXFLAGS = -std=c++11
CPPFLAGS = -Wall -Wextra -pedantic -Iinclude/ --coverage -g
TARGETS = all_tests
PACK = xrozek02
DOC = dokumentace
DOC_DIR = doc
IS_IT_OK_DIR = is_it_ok_dir
IS_IT_OK_SCRIPT = ./tests/is_it_ok.sh
LIB_OBJECTS = $(patsubst %.c, %.o, $(shell find src/ -type f -name '*.c'))

TEST_SOURCES = $(wildcard tests/*.cpp)
TEST_OBJECTS = $(patsubst %.cpp, %.o, $(TEST_SOURCES))

COV_REPORT_FILES = coverage/ $(shell find . -type f \( -name '*.gc??' -o -name '*.info' \))
ALL_OBJECTS = $(shell find . -type f -name '*.o')

.PHONY: all test doc clean pack clean_pack is_it_ok clean_is_it_ok

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
	cd $(DOC_DIR) && pdflatex dokumentace.tex
	cd $(DOC_DIR) && pdflatex dokumentace.tex
	cp $(DOC_DIR)/$(DOC).pdf $(DOC).pdf

# collect *.gcda files, remove system includes and generate coverage html report
# TODO: don't put --coverage into relase binaries, compile own binary with this flag
coverage: test
	lcov --capture --output-file coverage.info -d . -q
	lcov --remove coverage.info '/usr/*' --output-file=coverage.info -q
	genhtml coverage.info --output-directory coverage -q

$(PACK).tgz: doc
	tar -czf $@ *.h *.c rozdeleni rozdeleni $^

is_it_ok: clean_is_it_ok $(PACK).tgz
	chmod +x $(IS_IT_OK_SCRIPT)
	$(IS_IT_OK_SCRIPT) $(PACK).tgz $(IS_IT_OK_DIR)

clean_is_it_ok:
	rm -rf $(IS_IT_OK_DIR)

clean_pack:
	rm -f $(PACK).tgz

clean:
	rm -rf $(TARGETS) $(ALL_OBJECTS) $(COV_REPORT_FILES)
	cd $(DOC_DIR) && rm -f 	$(DOC).aux $(DOC).dvi $(DOC).log $(DOC).ps $(DOC).out $(DOC).toc $(DOC).pdf *.log
	cd $(DOC_DIR) && rm -rf html/
	rm -f $(DOC).pdf
