CXX := g++-4.4
# CXX := g++

DEBUG_OUTPUT := '"debug.log"'

CXX_FLAGS := -std=c++0x -Wall -Wextra -fstack-protector -ggdb
# CXX_FLAGS += -fsanitize=address -fsanitize=undefined -fno-sanitize-recover=address,undefined

LD_FLAGS := $(CXX_FLAGS) -pthread

SOURCES = $(wildcard src/*.cxx)
OBJECTS = $(patsubst src/%.cxx,build/%.o,$(SOURCES))

TARGET := ./main

all: $(TARGET)

run: $(TARGET)
	$(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $^ $(LD_FLAGS) -o $@

build/output.o: src/output.cxx
	$(CXX) -c $< -DDEBUG_OUTPUT=$(DEBUG_OUTPUT) $(CXX_FLAGS) -o $@

build/%.o: src/%.cxx
	$(CXX) -c $< $(CXX_FLAGS) -o $@

