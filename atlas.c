#include <atlas.h>

void free_character_atlas (struct character_atlas* atlas) {
    free(atlas->items);
    SDL_DestroyTexture(atlas->image);
    free(atlas);
}

struct character_atlas* create_character_atlas(SDL_Renderer* renderer, const char* characters, SDL_Color color,  TTF_Font* font) {
    int numchars = strlen(characters);

    struct character_atlas* atlas = (struct character_atlas*)malloc(sizeof(struct character_atlas));
    atlas->items = (struct character_atlas_item*)malloc(sizeof(struct character_atlas_item) * numchars);
    SDL_Surface* surf[numchars];
    atlas->num_items = 0;
    int w = 0;
    int h = 0;
    for(int i = 0; i < numchars; ++i){
        surf[i] = TTF_RenderGlyph_Solid(font, characters[i], color);
        if (!surf[i]) {
            free(atlas->items);
            free(atlas);
            return NULL;
        }
        atlas->items[i].bitmap.x = w;
        atlas->items[i].bitmap.y = 0;
        atlas->items[i].bitmap.w = surf[i]->w;
        atlas->items[i].bitmap.h = surf[i]->h;

        atlas->items[i].character = characters[i];
        TTF_GlyphMetrics(font, characters[i], &atlas->items[i].metrics.minx,
            &atlas->items[i].metrics.maxx,  &atlas->items[i].metrics.miny,
            &atlas->items[i].metrics.maxy,  &atlas->items[i].metrics.advance);

        w += surf[i]->w;
        if (surf[i]->h > h) {
            h = surf[i]->h;
        }
        atlas->num_items++;
    }

    SDL_Surface* all_in_one;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    all_in_one = SDL_CreateRGBSurface(0, w, h, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
    all_in_one = SDL_CreateRGBSurface(0, w, h, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif


    for(int j = 0; j < numchars; ++j){
        SDL_Rect r = atlas->items[j].bitmap;
        if (SDL_BlitSurface(surf[j], NULL, all_in_one, &r)) {
            free(atlas->items);
            free(atlas);
            return NULL;
        }
        SDL_FreeSurface(surf[j]);
    }
    atlas->image = SDL_CreateTextureFromSurface(renderer, all_in_one);
    SDL_FreeSurface(all_in_one);

    return atlas;
}

static inline struct character_atlas_item* find_char (struct character_atlas* atlas, Uint16 c) {
    for(int i = 0; i < atlas->num_items; ++i) {
        if (atlas->items[i].character == c) {
            return atlas->items + i;
        }
    }
    return NULL;
}
int character_atlas_render_string(SDL_Renderer* renderer,struct character_atlas* atlas, const char* str, int w, int x, int y) {
    int len = strlen(str);
    int maxy = 0;
    struct character_atlas_item* items[len];
    int str_normal_width = 0;
    for(int i = 0; i < len; ++i) {
        items[i] = find_char(atlas, str[i]);
        if(!items[i]) {
            return 1;
        }
        str_normal_width += items[i]->metrics.advance;
        if (maxy < items[i]->bitmap.h){
            maxy = items[i]->bitmap.h;
        }
    }
    //how much do we have to change the letter's size by?
    double str_width_factor = (double)w/(double)str_normal_width;
    SDL_Rect r;
    r.x = x;
    r.y = y - maxy/2;
    for(int i = 0; i < len; ++i) {
        r.w = items[i]->bitmap.w * str_width_factor;
        r.h = items[i]->bitmap.h * str_width_factor;

        SDL_RenderCopy(renderer, atlas->image, &items[i]->bitmap, &r);
        r.x += items[i]->metrics.advance * str_width_factor;
    }
    return 0;
}
