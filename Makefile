CXX := g++-4.4
# CXX := g++

DEBUG_OUTPUT := '"debug.log"'

DEBUG_CXX_FLAGS := -std=c++0x -Wall -Wextra -fstack-protector -ggdb -DDEBUG_OUTPUT=$(DEBUG_OUTPUT)
RELEASE_CXX_FLAGS := -std=c++0x -Wall -Wextra -fstack-protector -O3
# CXX_FLAGS += -fsanitize=address -fsanitize=undefined -fno-sanitize-recover=address,undefined

LD_FLAGS := $(CXX_FLAGS) -pthread

SOURCES = $(wildcard src/*.cxx) $(wildcard src/tiles/*.cxx)
DEBUG_OBJECTS = $(patsubst src/%.cxx,build/debug/%.o,$(SOURCES))
RELEASE_OBJECTS = $(patsubst src/%.cxx,build/release/%.o,$(SOURCES))

debug: build/debug/main
release: build/release/main

run: build/debug/main
	build/debug/main

build/debug/main: $(DEBUG_OBJECTS)
	$(CXX) $^ $(LD_FLAGS) -o $@

build/release/main: $(RELEASE_OBJECTS)
	$(CXX) $^ $(LD_FLAGS) -o $@

build/debug/%.o: src/%.cxx
	$(CXX) -c $< $(DEBUG_CXX_FLAGS) -o $@

build/release/%.o: src/%.cxx
	$(CXX) -c $< $(RELEASE_CXX_FLAGS) -o $@


