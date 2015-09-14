CXX := g++
CXX_FLAGS := -Wall -Wextra -fstack-protector -fsanitize=address -fsanitize=undefined -ggdb
LD_FLAGS := -pthread

SOURCES = $(wildcard src/*.cxx)
OBJECTS = $(patsubst src/%.cxx,build/%.o,$(SOURCES))

all: $(OBJECTS)

build/%.o: src/%.cxx
	$(CXX) -c $< $(CXX_FLAGS) -o $@

