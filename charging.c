#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#ifdef USE_LIBBATTERY
#include <libbattery/battery.h>
#else
#include <SDL2/SDL_power.h>
#endif

#include <unistd.h>

#include <atlas.h>
#include <draw.h>
#define CHARGING_SDL_VERSION "0.1.0"

#define MODE_TEST      1
#define MODE_NOTEXT    2

#define UPTIME 5

#define DISPLAY_USAGE(appname) printf(\
"Usage: %s [-tn] [-f font]\n\
    -t: launch %s in test mode\n\
    -n: don't display battery capacity\n\
    -f: font to use\n\
", appname, appname)

#ifndef NDEBUG
    #define LOG(status, msg, ...) printf("[%s] ",status); printf(msg, ##__VA_ARGS__); printf("\n")
    #define ERROR(msg, ...) LOG("ERROR", msg, ##__VA_ARGS__)
#else
    #define LOG(status, msg, ...)
    #define ERROR(msg, ...) fprintf(stderr, "[ERROR] "); fprintf(stderr, msg, ##__VA_ARGS)
#endif


#define CHECK_CREATE_SUCCESS(obj) if (!obj) { \
    ERROR("failed to allocate object: %s", SDL_GetError());\
    SDL_Quit(); \
    if(TTF_WasInit()) TTF_Quit(); \
    exit(1); \
}
struct battery_device {
    double voltage;
    int    percent;
};

Uint32 update_bat_info(Uint32 dt, void* data) {
    struct battery_device* dev = data;
    
#ifdef USE_LIBBATTERY
    struct battery_info bat;
    if (battery_fill_info(&bat)) {
        dev->voltage = bat.voltage;
        dev->percent = bat.fraction;
    }else{
        dev->voltage = -1;
        dev->percent = -1;
    }
#else
    dev->voltage = -1.0;
    SDL_GetPowerInfo(NULL , &dev->percent);
#endif
    return dt;
}

int main (int argc, char** argv) {
    LOG("INFO", "charging-sdl version %s", CHARGING_SDL_VERSION);
    unsigned MODE = 0;
    int screen_w = 480;
    int screen_h = 800;
    int battery_percent;
    
    SDL_Window* window;
    SDL_Renderer* renderer;

    struct battery_device bat_info;
    SDL_Surface* battery_icon;
    struct character_atlas* percent_atlas;
    char* font = NULL;

    char opt;
    while ((opt = getopt(argc, argv, "tnf:")) != -1) {
        switch (opt) {
        case 't': MODE |= MODE_TEST; break;
        case 'n': MODE |= MODE_NOTEXT; break;
        case 'f': font = optarg; break;
        default:
            DISPLAY_USAGE(argv[0]);
            exit(1);
        }
    }
    if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        ERROR("failed to init SDL: %s", SDL_GetError());  
        exit(1);        
    }
    if ( !(MODE & MODE_NOTEXT) )  {
        if(font == NULL) {
            ERROR("no font set and no text mode has not been turned on");
            SDL_Quit();
            exit(1);
        }
        if (TTF_Init() < 0) {
            ERROR("failed to init SDL: %s", TTF_GetError());
            SDL_Quit();            
            exit(1);
        }
    }

    if (MODE & MODE_TEST) {
        LOG("INFO", "creating test window");
        window = SDL_CreateWindow("Charge - Test Mode", 
                                    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                    screen_w, screen_h, 0);
    } else {
        SDL_DisplayMode mode = { SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0};
        if(SDL_GetDisplayMode(0, 0, &mode) != 0){
          ERROR("error fetching display mode: %s", SDL_GetError());
          SDL_Quit();
          exit(1);
        }
        screen_w = mode.w;
        screen_h = mode.h;
        LOG("INFO", "creating window");
        window = SDL_CreateWindow("Charge", 
                                    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                    0, 0, SDL_WINDOW_FULLSCREEN | SDL_WINDOW_SHOWN);
    }
    CHECK_CREATE_SUCCESS(window);
    
    LOG("INFO", "creating general renderer");    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    CHECK_CREATE_SUCCESS(renderer);
    

    battery_icon = make_battery_icon(screen_w, screen_h);
    CHECK_CREATE_SUCCESS(battery_icon);
    

    LOG("INFO", "creating texture from icon");     
    SDL_Texture* battery_icon_texture = SDL_CreateTextureFromSurface(renderer, battery_icon);
    CHECK_CREATE_SUCCESS(battery_icon_texture);
    
    SDL_RenderClear(renderer);
    update_bat_info(0, &bat_info);
    
    if (!(MODE & MODE_NOTEXT)) {
        if ( bat_info.percent >= 0 && bat_info.percent <= 100) {
            LOG("INFO", "able to access battery");
            LOG("INFO", "current capacity: %d%%", bat_info.percent );
            LOG("INFO", "using font %s", font);
            if (font == NULL) {
                ERROR("no font specified");
                TTF_Quit();
                SDL_Quit();  
                exit(1);          
            }
            SDL_Color color = {255, 255, 255};
            TTF_Font* font_struct = TTF_OpenFont(font, 256);
            if(!font_struct) {
                ERROR("failed to open font: %s\n", TTF_GetError());
                TTF_Quit();
                SDL_Quit();
                exit(1);            
            }
            percent_atlas = create_character_atlas(renderer,"0123456789", color, font_struct);
            if (percent_atlas) {
                LOG("INFO", "successfully created percent/number text atlas");
            } else {
                ERROR("failed to create font: %s\n", TTF_GetError());
                TTF_Quit();
                SDL_Quit();
                exit(1);     
            }
        } else {
            LOG("WARNING", "unable to access battery, turning on no text mode");
            MODE |= MODE_NOTEXT;
        }
    }
    char percent_text[4];
    SDL_Rect r;
    int running = 1;
    SDL_Event ev;
    Uint32 start = SDL_GetTicks();

    SDL_Rect* battery_area = make_battery_rect(screen_w, screen_h);
    
    SDL_TimerID bat_timer = SDL_AddTimer(500, update_bat_info, (void*)&bat_info);    
    while (running) {
        SDL_RenderCopy(renderer, battery_icon_texture, NULL, NULL);
            
        if ( !(MODE & MODE_NOTEXT) ) {
            sprintf(percent_text, "%d", bat_info.percent );
            if (percent_text[2]) {
                character_atlas_render_string(renderer, percent_atlas, percent_text, battery_area->w * 0.8, 
                    battery_area->w * 0.1 + battery_area->x, battery_area->y + battery_area->h/2);
            }else{
                character_atlas_render_string(renderer, percent_atlas, percent_text, battery_area->w * 0.66 * 0.8, 
                    battery_area->w * 2.33 * 0.1 + battery_area->x, battery_area->y + battery_area->h/2);  
            }
        }
        SDL_RenderPresent(renderer);
        if(MODE & MODE_TEST) {
            if (SDL_WaitEvent(&ev)) {
                switch (ev.type) {
                    case SDL_QUIT: running = 0; break;
                }
            }
        } else {
            SDL_Delay(1000);            
            if (SDL_GetTicks() - start >= UPTIME * 1000) {
                running = 0;
            }
        }
    }

    if( !(MODE & MODE_NOTEXT)) {
        free_character_atlas(percent_atlas);
    }
    free(battery_area);
    SDL_DestroyTexture(battery_icon_texture);
    SDL_FreeSurface(battery_icon);
    SDL_DestroyRenderer(renderer);    
    SDL_DestroyWindow(window);
    SDL_RemoveTimer(bat_timer);
    SDL_Quit();
    TTF_Quit();
}