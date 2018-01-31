SDL2_CFLAGS := $(shell sdl2-config --cflags)
SDL2_LIBS  := $(shell sdl2-config --libs)

CC       := gcc
CCFLAGS   := -g -I. $(SDL2_CFLAGS)

LIBS       := $(SDL2_LIBS) -lSDL2_ttf -lm $(if $(LIBBATTERY),-lbattery)

SOURCES    := ${wildcard *.c}
OBJECTS    := $(SOURCES:%.c=%.o)

all: charging_sdl

%.o: %.c
	@echo CC $<
	@$(CC) -c -o $@ $< $(CCFLAGS) $(if $(LIBBATTERY),-DUSE_LIBBATTERY) $(if $(OLED),-DOLED_SCREEN)

charging_sdl: $(OBJECTS)
	@echo LD $@
	@$(CC) -o $@ $^ $(CCFLAGS) $(LIBS)

.PHONY: clean

clean:
	-rm -fv *.o charging_sdl
