#include <sdl_instance.h>
#include <SDL.h>


void sdl_instance_init(void){
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
}