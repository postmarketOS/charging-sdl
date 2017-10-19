SDL2_CFLAGS := $(shell sdl2-config --cflags)
SDL2_LIBS  := $(shell sdl2-config --libs)

CC       := gcc
CCFLAGS   := -g -I. $(SDL2_CFLAGS)

LIBS       := $(SDL2_LIBS) -lSDL2_ttf -lm

SOURCES    := ${wildcard *.c}
OBJECTS    := $(SOURCES:%.c=%.o)

all: charging_sdl

%.o: %.c
	@echo CC $<
	@$(CXX) -c -o $@ $< $(CCFLAGS)

charging_sdl: $(OBJECTS)
	@echo LD $@
	@$(CC) -o $@ $^ $(CCFLAGS) $(LIBS)

.PHONY: clean

clean:
	-rm -fv *.o charging_sdl
