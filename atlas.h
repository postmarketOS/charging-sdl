#ifndef ATLAS_H
#define ATLAS_H

#include <SDL2/SDL_ttf.h>

typedef struct character_atlas character_atlas_t;


struct glyph_metrics {
  int minx;
  int miny;
  int maxx;
  int maxy;
  int advance;
};

// chacter atlas item is an character in a character atlas. 
// it stores the glyph metrics (reported by SDL_TTF)
// as well as the character's position on an atlas's texture  
struct character_atlas_item {
  struct glyph_metrics metrics;
  SDL_Rect             bitmap;
  Uint16               character;
};

struct character_atlas {
    SDL_Texture*                 image;
    struct character_atlas_item* items;
    int                          num_items;
    
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

/**
  renderer a string using characters from an atlas
  @param renderer to render to
  @param atlas atlas to pull characters from
  @param color the color of the glyphs (the background of each glyph is transparent)
  @param font the font used to draw the characters
*/
int character_atlas_render_string(SDL_Renderer* renderer,struct character_atlas* atlas, const char* str, int w, int x, int y);
#endif