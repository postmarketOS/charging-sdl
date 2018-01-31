#ifndef DRAW_H
#define DRAW_H

#include <SDL2/SDL.h>
#include <math.h>

/**
  draw line draws a line from (x,y) to (x1, y1)
  @parma surf the surface to draw on
  @param c color to draw the line
  @param x x position of the starting point
  @param y y position of the starting point
  @param x1 x position of the ending point
  @param y1 y position of the ending point
*/
int draw_line(SDL_Surface* surf, Uint32 c, int x, int y, int x1, int y1);

/**
  create a battery icon to fit within a specific area
  @param w the width the battery must fit in
  @param h the height the battery must fit in
  @returns returns a battery icon, its surface's width and height are equal to w and h
*/
SDL_Surface* make_battery_icon(int w, int h);

/**
  create a lightning bolt icon, the icon will fit within w and h without being stretched
  @param w the width the bolt must fit in
  @param h the height the bolt must fit in
  @returns returns a lightning icon, its surface's width and height are equal to w and h
*/
SDL_Surface* make_lightning_icon(int w, int h);

/**
  get the rectangle for the batteries 'body' that fits within specific bounds
  @param w the width the battery must fit in
  @param h the height the battery must fit in
  @param bat_rect a pointer to the rectangle to fill
  @returns returns the rectangle for the batteries body
*/
SDL_Rect* make_battery_rect(int w, int h, SDL_Rect* bat_rect);

/**
  create a small square, that will move around the screen, to prevent burn-in's
  @param h the height of the screen
  @param oled_rect a pointer to the rectangle to fill
  @returns returns a square, its side length's equal to the height divided by 25
*/
SDL_Rect* make_oled_rect(int h, SDL_Rect* oled_rect);

/**
  place the burn-in prevention square to a random place within the screen
  @param w the width of the screen
  @param h the height of the screen
  @param rect a pointer to the rectangle to move
*/
void move_oled_rect(int w, int h, SDL_Rect* rect);

/**
  fill_polygon fills in the space within a set of points
  @parma surf the surface to draw on
  @param color the color the polygon should be
  @param points list of points that make up the polygon
  @param numpoints the number of points in `points`
  @returns 1 on failure, 0 on success
*/
int fill_polygon(SDL_Surface* surf, Uint32 color, SDL_Point * points, int numpoints);

#endif
