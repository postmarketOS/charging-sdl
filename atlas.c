#include <atlas.h>

void free_character_atlas (struct character_atlas* atlas) {
    free(atlas->shapes);
    SDL_DestroyTexture(atlas->image);
}

struct character_atlas* create_character_atlas(SDL_Renderer* renderer, const char* characters, SDL_Color color,  TTF_Font* font) {
    int numchars = strlen(characters);

    struct character_atlas* atlas = (struct character_atlas*)malloc(sizeof(struct character_atlas));
    atlas->shapes = (SDL_Rect*)malloc(sizeof(SDL_Rect) * numchars);
    SDL_Surface** surf = (SDL_Surface**)malloc(sizeof(SDL_Surface*) * numchars);

    int w = 0;
    int h = 0;
    for(int i = 0; i < numchars; ++i){
        surf[i] = TTF_RenderGlyph_Solid(font, characters[i], color);
        if (!surf[i]) {
            free(atlas->shapes);
            free(atlas);
            return NULL;
        }
        atlas->shapes[i].x = w;
        atlas->shapes[i].y = 0;
        atlas->shapes[i].w = surf[i]->w;
        atlas->shapes[i].h = surf[i]->h;
        
        w += surf[i]->w;
        if (surf[i]->h > h) {
            h = surf[i]->h;
        }

    }
    
    SDL_Surface* all_in_one;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    all_in_one = SDL_CreateRGBSurface(0, w, h, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
    all_in_one = SDL_CreateRGBSurface(0, w, h, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif


    for(int j = 0; j < numchars; ++j){
        SDL_Rect r = atlas->shapes[j];
        if (SDL_BlitSurface(surf[j], NULL, all_in_one, &r)) {
            free(atlas->shapes);
            free(atlas);
            return NULL;
        }
        SDL_FreeSurface(surf[j]);
    }
    atlas->image = SDL_CreateTextureFromSurface(renderer, all_in_one);
    SDL_FreeSurface(all_in_one);

    return atlas;
}