CXX := g++-4.4
CXX := g++

DEBUG_OUTPUT := '"debug.log"'

DEBUG_CXX_FLAGS := -std=c++0x -Wall -Wextra -fstack-protector -ggdb -DDEBUG_OUTPUT=$(DEBUG_OUTPUT)
RELEASE_CXX_FLAGS := -std=c++0x -Wall -Wextra -fstack-protector -O3
# CXX_FLAGS += -fsanitize=address -fsanitize=undefined -fno-sanitize-recover=address,undefined

LD_FLAGS := $(CXX_FLAGS) -pthread

BUILD_DIR := build
DEBUG_DIR := $(BUILD_DIR)/debug
RELEASE_DIR := $(BUILD_DIR)/release

DEBUG_TARGET := bin/debug
RELEASE_TARGET := bin/release

SOURCES := $(wildcard src/*.cxx) $(wildcard src/tiles/*.cxx)

DEBUG_OBJECTS := $(patsubst src/%.cxx,$(DEBUG_DIR)/%.o,$(SOURCES))
RELEASE_OBJECTS := $(patsubst src/%.cxx,$(RELEASE_DIR)/%.o,$(SOURCES))
OBJECTS := $(DEBUG_OBJECTS) $(RELEASE_OBJECTS)
OBJ_DIRS := $(sort $(dir $(OBJECTS) $(DEBUG_TARGET) $(RELEASE_TARGET)))

all: debug release

debug: $(DEBUG_TARGET)
release: $(RELEASE_TARGET)

run: $(DEBUG_TARGET)
	$(DEBUG_TARGET)

$(DEBUG_TARGET): $(DEBUG_OBJECTS)
	$(CXX) $^ $(LD_FLAGS) -o $@

$(RELEASE_TARGET): $(RELEASE_OBJECTS)
	$(CXX) $^ $(LD_FLAGS) -o $@

$(OBJECTS): | $(OBJ_DIRS)

$(OBJ_DIRS):
	mkdir -p $@

$(DEBUG_DIR)/%.o: src/%.cxx
	$(CXX) -c $< $(DEBUG_CXX_FLAGS) -o $@

$(RELEASE_DIR)/%.o: src/%.cxx
	$(CXX) -c $< $(RELEASE_CXX_FLAGS) -o $@

clean:
	rm -f $(OBJECTS) $(DEBUG_TARGET) $(RELEASE_TARGET)

.PHONY: all debug release run clean

