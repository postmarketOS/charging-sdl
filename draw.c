#include <draw.h>

#define FETCH_PIXEL(s, x, y) ((Uint32*) ((Uint8*) (s)->pixels + ( (y) * (s)->w + (x) ) * ((s)->format->BytesPerPixel)))

static inline Uint32* fetch_pixel(SDL_Surface* surf, int x, int y) {
    return ((Uint32*) ((Uint8*)surf->pixels + ( y * surf->w + x ) * surf->format->BytesPerPixel));
}
static inline void flip_coords(int * x, int * y, int * x1, int * y1) {
    int tmpx = *x;
    int tmpy = *y;

    *x = *x1;
    *y = *y1;

    *x1 = tmpx;
    *y1 = tmpy;
}

SDL_Rect* make_battery_rect(int w, int h, SDL_Rect* bat_rect) {
    if (w > h) {
        bat_rect->h = h/1.5;
        bat_rect->w = bat_rect->h/2;
    }else{
        bat_rect->w = w/1.5;
        bat_rect->h = bat_rect->w*2;
    }
    bat_rect->x = (w - bat_rect->w) / 2;
    bat_rect->y = (h - bat_rect->h) / 2;
    return bat_rect;
}

SDL_Surface* make_battery_icon(int w, int h) {
    SDL_Surface* surf;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    surf = SDL_CreateRGBSurface(0, w, h, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
    surf = SDL_CreateRGBSurface(0, w, h, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif
    SDL_Rect bat_rect;
    make_battery_rect(w, h, &bat_rect);

    SDL_FillRect(surf, NULL, SDL_MapRGBA(surf->format, 0, 0, 0, 255));
    SDL_FillRect(surf, &bat_rect, SDL_MapRGBA(surf->format, 255, 255, 255, 255));

    int margin = bat_rect.h * 0.05;

    bat_rect.x += bat_rect.h * 0.05;
    bat_rect.y += bat_rect.h * 0.05;
    bat_rect.w -= bat_rect.h * 0.1;
    bat_rect.h -= bat_rect.h * 0.1;
    SDL_FillRect(surf, &bat_rect, SDL_MapRGBA(surf->format, 0, 0, 0, 255));

    bat_rect.y += bat_rect.h * 0.9;
    bat_rect.h = bat_rect.h * 0.1;
    SDL_FillRect(surf, &bat_rect, SDL_MapRGBA(surf->format, 255, 0, 0, 255));

    bat_rect.y -= bat_rect.h * 10.5;
    bat_rect.x += bat_rect.w * 0.2;

    bat_rect.w -= bat_rect.w * 0.4;
    SDL_FillRect(surf, &bat_rect, SDL_MapRGBA(surf->format, 255, 255, 255, 255));
    return surf;
}
SDL_Surface* make_lightning_icon(int w, int h){
    SDL_Surface* surf;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    surf = SDL_CreateRGBSurface(0, w, h, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
    surf = SDL_CreateRGBSurface(0, w, h, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif
    int offset_x = 0;
    if(w > h) {
        offset_x = (w - h/2)/2;
        w = h/2;
    }

    SDL_FillRect(surf, NULL, SDL_MapRGBA(surf->format, 0, 0, 0, 0));
    SDL_Point outline[7] = {
        {offset_x + w * 0.4, 0},
        {offset_x + w * 0.2, h/2},
        {offset_x + w * 0.4, h/2},
        {offset_x + w * 0.3, h},
        {offset_x + w * 0.8, h/3},
        {offset_x + w * 0.6, h/3},
        {offset_x + w * 0.8, 0},
    };

    fill_polygon(surf, SDL_MapRGBA(surf->format, 255, 255, 255, 255), outline, 7);
    return surf;
}

int draw_line(SDL_Surface* surf, Uint32 c, int x, int y, int x1, int y1) {
    if ( (x > x1 && y >= y1) || (y > y1 && x < x1)) {
        flip_coords(&x, &y, &x1, &y1);
    }
    double cur_x = x;
    double cur_y = y;

    Uint32* cur_pix = NULL;

    int steps;
    if(x1 - x > y1 - y) {
        steps = abs(x1 - x);
    }else{
        steps = abs(y1 - y);
    }
    double change_x = (x1-x)/(double)steps;
    double change_y = (y1-y)/(double)steps;
    for( int i = 0; i < steps; ++i ){
        cur_pix = FETCH_PIXEL(surf, (int)cur_x, (int)cur_y);
        (*cur_pix) = c;

        cur_x += change_x;
        cur_y += change_y;
    }
    return 0;
}

int fill_polygon(SDL_Surface* surf, Uint32 color, SDL_Point * points, int numpoints) {
    int max_x = -1;
    int max_y = -1;
    int min_y = surf->h;
    int min_x = surf->w;

    for (int l = 0; l < numpoints; ++l) {
        if (points[l].x > max_x) max_x = points[l].x;
        if (points[l].x < min_x) min_x = points[l].x;
        if (points[l].y > max_y) max_y = points[l].y;
        if (points[l].y < min_y) min_y = points[l].y;

        if (l != numpoints-1) {
            if (draw_line(surf, color, points[l].x, points[l].y, points[l+1].x, points[l+1].y)) return 1;
        }else{
            if (draw_line(surf, color, points[l].x, points[l].y, points[0].x, points[0].y)) return 1;
        }
    }
    int flip = 0;
    Uint32* pix;
    for (int y = min_y; y < max_y; ++y) {
        flip = 0;
        for(int x = min_x; x < max_x; ++x){
            pix = FETCH_PIXEL(surf, x, y);
            if (*pix == color) {
                flip += 1;
                pix = FETCH_PIXEL(surf, ++x, y);
                while (*pix == color) {
                    pix = FETCH_PIXEL(surf, ++x, y);
                }
                int dx = x;
                while (*pix != color) {
                    if(dx >= max_x) break;
                    pix = FETCH_PIXEL(surf, ++dx, y);
                }
                if(dx >= max_x) break;
                else pix = FETCH_PIXEL(surf, x, y);

            }

            if (flip % 2 != 0) *pix = color;
        }
    }
    return 0;
}
