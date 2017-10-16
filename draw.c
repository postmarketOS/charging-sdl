#include <draw.h>

SDL_Surface* make_battery_icon(int w, int h) {
    SDL_Surface* surf;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    surf = SDL_CreateRGBSurface(0, w, h, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
    surf = SDL_CreateRGBSurface(0, w, h, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif
    SDL_Rect bat_rect = *make_battery_rect(w, h);    

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