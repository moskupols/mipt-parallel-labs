CXX := g++
CXX_FLAGS := -std=c++03 -Wall -Wextra -fstack-protector -fsanitize=address -fsanitize=undefined -ggdb
LD_FLAGS := $(CXX_FLAGS) -pthread

SOURCES = $(wildcard src/*.cxx)
OBJECTS = $(patsubst src/%.cxx,build/%.o,$(SOURCES))

TARGET := ./main

all: $(TARGET)

run: $(TARGET)
	$(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $^ $(LD_FLAGS) -o $@

build/%.o: src/%.cxx
	$(CXX) -c $< $(CXX_FLAGS) -o $@

