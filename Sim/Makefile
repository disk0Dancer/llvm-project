CXX      ?= c++
CXXFLAGS ?= -O2 -std=c++17 -Wall -Wextra

SDL2_CFLAGS := $(shell sdl2-config --cflags 2>/dev/null)
SDL2_LIBS   := $(shell sdl2-config --libs   2>/dev/null)
ifneq ($(strip $(SDL2_LIBS)),)
  SDL_DEFINES := -DGRAPH_SIM_SDL
endif

graph-sim: graph-sim.cpp
	$(CXX) $(CXXFLAGS) $(SDL_DEFINES) $(SDL2_CFLAGS) -o $@ $< $(SDL2_LIBS)

clean:
	rm -f graph-sim *.o out.ppm

.PHONY: clean
