CXX := g++-4.4
CPP_FLAGS := -DDEBUG_OUTPUT='"debug.log"'
CXX_FLAGS := -std=c++0x -Wall -Wextra -fstack-protector -ggdb
# CXX_FLAGS := -std=c++0x -Wall -Wextra -fstack-protector -fsanitize=address -fsanitize=undefined -fno-sanitize-recover=address,undefined -ggdb
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
	$(CXX) -c $< $(CPP_FLAGS) $(CXX_FLAGS) -o $@

