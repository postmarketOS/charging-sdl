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

#define UPTIME 5

void usage(char* appname)  {
    printf("Usage: %s [-tpc] [-f font]\n\
    -t: launch %s in test mode\n\
    -p: display battery capacity\n\
    -c: attempt to get current\n\
    -f: font to use\n", appname, appname);
}

#ifndef NDEBUG
    #define LOG(status, msg, ...) printf("[%s] ", status); printf(msg, ##__VA_ARGS__); printf("\n")
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
    double     current;
    int        is_charging;
    int        percent;
};

void update_bat_info(struct battery_device* dev) {
#ifdef USE_LIBBATTERY
    struct battery_info bat;
    if (battery_fill_info(&bat)) {
        dev->current = bat.current;
        if (!isfinite(bat.fraction)){
            dev->percent = -1;
        }else{
            dev->percent = (int)(bat.fraction * 100.0);
        }
        dev->is_charging = bat.state & CHARGING;
    }
#else
    SDL_PowerState state = SDL_GetPowerInfo(NULL, NULL);

    if ( !(state & SDL_POWERSTATE_UNKNOWN) ){
        SDL_GetPowerInfo(NULL, &dev->percent);
        dev->is_charging = !(state & SDL_POWERSTATE_CHARGING);
    }
#endif
}


int main (int argc, char** argv) {
    LOG("INFO", "charging-sdl version %s", CHARGING_SDL_VERSION);

    char flag_test = 0, flag_percent = 0, flag_current = 0;

    int screen_w = 480;
    int screen_h = 800;

    SDL_Window* window;
    SDL_Renderer* renderer;

    struct battery_device bat_info;
    SDL_Surface* battery_icon;
    SDL_Surface* lightning_icon;
    struct character_atlas* percent_atlas = NULL;
    char* flag_font = NULL;
    TTF_Font* font_struct = NULL;

    SDL_Rect is_charging_area = {
        .x = 0,
        .y = screen_w / 8 * 0.2,
        .w = screen_w / 8,
        .h = screen_w / 8
    };
    char opt;
    while ((opt = getopt(argc, argv, "tpcf:")) != -1) {
        switch (opt) {
            case 't': flag_test = 1; break;
            case 'p': flag_percent = 1; break;
            case 'c': flag_current = 1; break;
            case 'f': flag_font = optarg; break;
            default:
                usage(argv[0]);
                exit(1);
            }
    }
    if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO) < 0) {
        ERROR("failed to init SDL: %s", SDL_GetError());
        exit(1);
    }
    if ((flag_percent || flag_current) && flag_font)  {
        if (TTF_Init() < 0) {
            ERROR("failed to init SDL: %s", TTF_GetError());
            SDL_Quit();
            exit(1);
        }
    }

    if (flag_test) {
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


    LOG("INFO", "creating icon bitmaps");
    battery_icon = make_battery_icon(screen_w, screen_h);
    CHECK_CREATE_SUCCESS(battery_icon);

    lightning_icon = make_lightning_icon(is_charging_area.w, is_charging_area.h);
    CHECK_CREATE_SUCCESS(battery_icon);

    LOG("INFO", "creating textures from icons");
    SDL_Texture* battery_icon_texture = SDL_CreateTextureFromSurface(renderer, battery_icon);
    CHECK_CREATE_SUCCESS(battery_icon_texture);

    SDL_Texture* lightning_icon_texture = SDL_CreateTextureFromSurface(renderer, lightning_icon);
    CHECK_CREATE_SUCCESS(lightning_icon_texture);

    SDL_RenderClear(renderer);

    if (flag_percent || flag_current) {
        if (flag_font) {
            LOG("INFO", "using font %s", flag_font);
            if (flag_font == NULL) {
                ERROR("no font specified");
                TTF_Quit();
                SDL_Quit();
                exit(1);
            }
            SDL_Color color = {255, 255, 255};
            font_struct = TTF_OpenFont(flag_font, 256);
            if(!font_struct) {
                ERROR("failed to open font: %s\n", TTF_GetError());
                TTF_Quit();
                SDL_Quit();
                exit(1);
            }
            percent_atlas = create_character_atlas(renderer, "0123456789A.", color, font_struct);
            if (percent_atlas) {
                LOG("INFO", "successfully created percent/number text atlas");
            } else {
                ERROR("failed to create font: %s\n", TTF_GetError());
                TTF_Quit();
                SDL_Quit();
                exit(1);
            }
        } else {
            LOG("WARNING", "no font set, turning off text renderering");
            percent_atlas = NULL;
        }
    }
    SDL_Rect r;
    int running = 1;
    SDL_Event ev;
    Uint32 start = SDL_GetTicks();

    SDL_Rect battery_area;

    make_battery_rect(screen_w, screen_h, &battery_area);
    bat_info.current = -1.0f;
    bat_info.is_charging = 0;
    bat_info.percent = -1;

    Uint32 start_time = 0;
    Uint32 end_time = 0;

    char percent_text[4];
    char current_text[6];
    while (running) {
        SDL_RenderClear(renderer);

        update_bat_info(&bat_info);

        SDL_RenderCopy(renderer, battery_icon_texture, NULL, NULL);

        if (flag_percent && percent_atlas && bat_info.percent > 0) {
            sprintf(percent_text, "%d", bat_info.percent);
            if (percent_text[2]) {
                character_atlas_render_string(renderer, percent_atlas, percent_text,
                                              battery_area.w * 0.8,
                                              battery_area.w * 0.1 + battery_area.x,
                                              battery_area.y + battery_area.h / 2);
            }else{
                character_atlas_render_string(renderer, percent_atlas, percent_text,
                                              battery_area.w * 0.66 * 0.8,
                                              battery_area.w * 2.33 * 0.1 + battery_area.x,
                                              battery_area.y + battery_area.h / 2);
            }
        }
        if (bat_info.is_charging) {
            sprintf(current_text, "%2.1fA", bat_info.current);
            if ( flag_current && bat_info.current > 0 && percent_atlas){
                character_atlas_render_string(renderer, percent_atlas, current_text, is_charging_area.w * 1.2,
                    is_charging_area.w - is_charging_area.w * 0.05, is_charging_area.h + is_charging_area.h * 1.5 );
            }
            SDL_RenderCopy(renderer, lightning_icon_texture, NULL, &is_charging_area);
        }
        SDL_RenderPresent(renderer);
        if(flag_test) {
            while (SDL_PollEvent(&ev)) {
                switch (ev.type) {
                    case SDL_QUIT: running = 0; break;
                }
            }
        } else {
            SDL_Delay(1000);
            if (SDL_GetTicks() - start >= UPTIME * 1000) {
                running = 0;
            }
            SDL_RenderClear(renderer);
        }
    }

    if(percent_atlas) free_character_atlas(percent_atlas);
    if(font_struct) TTF_CloseFont(font_struct);
    SDL_FreeSurface(lightning_icon);
    SDL_FreeSurface(battery_icon);

    SDL_DestroyTexture(battery_icon_texture);
    SDL_DestroyTexture(lightning_icon_texture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    if(TTF_WasInit()) TTF_Quit();
    SDL_Quit();

}
