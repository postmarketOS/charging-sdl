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
    draw_line(surf, SDL_MapRGBA(surf->format, 255, 0, 0, 255), 0, 0, w, h);
    return surf;
}

SDL_Surface* make_lightning_icon(int w, int h){
    
}

int draw_line(SDL_Surface* surf, Uint32 c, int x, int y, int x1, int y1) {
    double mag = sqrt(pow((double)x1-(double)x,2.0f)+ pow((double)y1-(double)y, 2.0f));
    if(mag == 0){
        return 1;
    }
    double change_x = (x1-x)/mag;
    double change_y = (y1-y)/mag;
    double cur_x = x;
    double cur_y = y;

    Uint32* cur_pix = NULL;
    while(floor(cur_x) != x1 || floor(cur_y) != y1){
        cur_pix = (Uint32*) ((Uint8*)surf->pixels + ((long)cur_y * surf->w + (long)cur_x) * 4);
        *cur_pix = c;
        
        cur_x += change_x;
        cur_y += change_y;
    }
}