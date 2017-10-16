#ifndef DRAW_H
#define DRAW_H

#include <SDL2/SDL.h>

/**
  create a battery icon to fit within a specific area
  @param w the width the battery must fit in
  @param h the height the battery must fit in
  @returns returns a battery icon, it's surface's width and height are equal to w and h
*/
SDL_Surface* make_battery_icon(int w, int h);    
#endif