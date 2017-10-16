#ifndef ATLAS_H
#define ATLAS_H

#include <SDL2/SDL_ttf.h>

typedef struct character_atlas character_atlas_t;



struct character_atlas {
    SDL_Texture* image;
    //TODO store glyph metrics
    SDL_Rect*    shapes;
};

/**
  free a character atlas
  @param atlas to free
*/
void free_character_atlas (struct character_atlas* atlas);

/**
  create a new character atlas for a set of characters
  @param renderer used to create the atlas's texture
  @param characters characters to include in the atlas
  @param color the color of the glyphs (the background of each glyph is transparent)
  @param font the font used to draw the characters
*/
struct character_atlas* create_character_atlas(SDL_Renderer* renderer, const char* characters, SDL_Color color,  TTF_Font* font);    

//TODO add rendering functions
#endif