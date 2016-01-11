# CXX := g++-4.4
# CXX := g++
CXX = mpic++

DEBUG_OUTPUT := '"debug.log"'

DEBUG_CXX_FLAGS := -std=c++0x -Wall -Wextra -fstack-protector -ggdb -DDEBUG_OUTPUT=$(DEBUG_OUTPUT)
RELEASE_CXX_FLAGS := -std=c++0x -Wall -Wextra -fstack-protector -O3
CXX_FLAGS += -fsanitize=address -fsanitize=undefined -fno-sanitize-recover=address,undefined

LD_FLAGS := $(CXX_FLAGS) -pthread

BUILD_DIR := build
DEBUG_DIR := $(BUILD_DIR)/debug
RELEASE_DIR := $(BUILD_DIR)/release

DEBUG_TARGET := bin/debug
RELEASE_TARGET := bin/release

SOURCES := $(wildcard src/*.cxx) $(wildcard src/*/*.cxx)

DEBUG_OBJECTS := $(patsubst src/%.cxx,$(DEBUG_DIR)/%.o,$(SOURCES))
RELEASE_OBJECTS := $(patsubst src/%.cxx,$(RELEASE_DIR)/%.o,$(SOURCES))
OBJECTS := $(DEBUG_OBJECTS) $(RELEASE_OBJECTS)
OBJ_DIRS := $(sort $(dir $(OBJECTS) $(DEBUG_TARGET) $(RELEASE_TARGET)))

CONCURRENCIES := 1 2 3 4 5 8 27 64 200
TEST_DIR := out
TESTS := $(patsubst %,$(TEST_DIR)/%.out,$(CONCURRENCIES))

all: debug release

debug: $(DEBUG_TARGET)
release: $(RELEASE_TARGET)

run: run-release

run-debug: $(DEBUG_TARGET)
	mpirun $(DEBUG_TARGET)

run-release: $(RELEASE_TARGET)
	mpirun $(RELEASE_TARGET)

bench: $(RELEASE_TARGET)
	$(SHELL) bench.sh $(CONCURRENCIES)

test: $(TESTS)
	for i in $(TESTS); do\
		for j in $(TESTS); do\
			diff -q $$i $$j;\
		done;\
		break;\
	done

$(DEBUG_TARGET): $(DEBUG_OBJECTS)
	$(CXX) $^ $(LD_FLAGS) -o $@

$(RELEASE_TARGET): $(RELEASE_OBJECTS)
	$(CXX) $^ $(LD_FLAGS) -o $@

$(TESTS): | $(TEST_DIR)

$(TEST_DIR)/%.out: $(RELEASE_TARGET)
	$(SHELL) run.sh $(RELEASE_TARGET) $(notdir $(basename $@)) inputs/glider.csv '' 8 >$@

$(OBJECTS): | $(OBJ_DIRS)

$(OBJ_DIRS) $(TEST_DIR):
	mkdir -p $@

$(DEBUG_DIR)/%.o: src/%.cxx
	$(CXX) -c $< $(DEBUG_CXX_FLAGS) -o $@

$(RELEASE_DIR)/%.o: src/%.cxx
	$(CXX) -c $< $(RELEASE_CXX_FLAGS) -o $@

clean:
	rm -f $(wildcard $(OBJECTS) $(DEBUG_TARGET) $(RELEASE_TARGET) $(TESTS))

.PHONY: all debug release run clean run-release run-debug test bench

