#include <windows1.h>
#include <stdint.h>

#include "SDL.h"

SDL_Window* WINDOW_HANDLE;

void windows_input_handle_keys_up(SDL_Event e){
    switch(e.key.keysym.sym){
        // Lettres
        case SDLK_a: break;
        case SDLK_b: break;
        case SDLK_c: break;
        case SDLK_d: break;
        case SDLK_e: break;
        case SDLK_f: break;
        case SDLK_g: break;
        case SDLK_h: break;
        case SDLK_i: break;
        case SDLK_j: break;
        case SDLK_k: break;
        case SDLK_l: break;
        case SDLK_m: break;
        case SDLK_n: break;
        case SDLK_o: break;
        case SDLK_p: break;
        case SDLK_q: break;
        case SDLK_r: break;
        case SDLK_s: break;
        case SDLK_t: break;
        case SDLK_u: break;
        case SDLK_v: break;
        case SDLK_w: break;
        case SDLK_x: break;
        case SDLK_y: break;
        case SDLK_z: break;

        // Chiffres
        case SDLK_0: break;
        case SDLK_1: break;
        case SDLK_2: break;
        case SDLK_3: break;
        case SDLK_4: break;
        case SDLK_5: break;
        case SDLK_6: break;
        case SDLK_7: break;
        case SDLK_8: break;
        case SDLK_9: break;

        // Touches de contrôle
        case SDLK_RETURN: break;
        case SDLK_ESCAPE: break;
        case SDLK_BACKSPACE: break;
        case SDLK_TAB: break;
        case SDLK_SPACE: break;

        // Flèches
        case SDLK_UP: break;
        case SDLK_DOWN: break;
        case SDLK_LEFT: break;
        case SDLK_RIGHT: break;

        // Modificateurs
        case SDLK_LSHIFT: break;
        case SDLK_RSHIFT: break;
        case SDLK_LCTRL: break;
        case SDLK_RCTRL: break;
        case SDLK_LALT: break;
        case SDLK_RALT: break;

        // Fonctions
        case SDLK_F1: break;
        case SDLK_F2: break;
        case SDLK_F3: break;
        case SDLK_F4: break;
        case SDLK_F5: break;
        case SDLK_F6: break;
        case SDLK_F7: break;
        case SDLK_F8: break;
        case SDLK_F9: break;
        case SDLK_F10: break;
        case SDLK_F11: break;
        case SDLK_F12: break;

        default: break;
    }
}

void windows_input_handle_keys_down(SDL_Event e) {
    switch(e.key.keysym.sym){
        // Lettres
        case SDLK_a: break;
        case SDLK_b: break;
        case SDLK_c: break;
        case SDLK_d: break;
        case SDLK_e: break;
        case SDLK_f: break;
        case SDLK_g: break;
        case SDLK_h: break;
        case SDLK_i: break;
        case SDLK_j: break;
        case SDLK_k: break;
        case SDLK_l: break;
        case SDLK_m: break;
        case SDLK_n: break;
        case SDLK_o: break;
        case SDLK_p: break;
        case SDLK_q: break;
        case SDLK_r: break;
        case SDLK_s: break;
        case SDLK_t: break;
        case SDLK_u: break;
        case SDLK_v: break;
        case SDLK_w: break;
        case SDLK_x: break;
        case SDLK_y: break;
        case SDLK_z: break;

        // Chiffres
        case SDLK_0: break;
        case SDLK_1: break;
        case SDLK_2: break;
        case SDLK_3: break;
        case SDLK_4: break;
        case SDLK_5: break;
        case SDLK_6: break;
        case SDLK_7: break;
        case SDLK_8: break;
        case SDLK_9: break;

        // Touches de contrôle
        case SDLK_RETURN: break;
        case SDLK_ESCAPE: break;
        case SDLK_BACKSPACE: break;
        case SDLK_TAB: break;
        case SDLK_SPACE: break;

        // Flèches
        case SDLK_UP: break;
        case SDLK_DOWN: break;
        case SDLK_LEFT: break;
        case SDLK_RIGHT: break;

        // Modificateurs
        case SDLK_LSHIFT: break;
        case SDLK_RSHIFT: break;
        case SDLK_LCTRL: break;
        case SDLK_RCTRL: break;
        case SDLK_LALT: break;
        case SDLK_RALT: break;

        // Fonctions
        case SDLK_F1: break;
        case SDLK_F2: break;
        case SDLK_F3: break;
        case SDLK_F4: break;
        case SDLK_F5: break;
        case SDLK_F6: break;
        case SDLK_F7: break;
        case SDLK_F8: break;
        case SDLK_F9: break;
        case SDLK_F10: break;
        case SDLK_F11: break;
        case SDLK_F12: break;

        default: break;
    }
}

// ------------------- SOURIS -------------------
void windows_input_handle_motion(SDL_Event e) {
    int x = e.motion.x;
    int y = e.motion.y;
    int dx = e.motion.xrel;
    int dy = e.motion.yrel;
    printf("Souris déplacée: x=%d y=%d, dx=%d dy=%d\n", x, y, dx, dy);
}

void windows_input_handle_button_down(SDL_Event e) {
    switch(e.button.button){
        case SDL_BUTTON_LEFT:   printf("Clic gauche\n"); break;
        case SDL_BUTTON_RIGHT:  printf("Clic droit\n"); break;
        case SDL_BUTTON_MIDDLE: printf("Clic milieu\n"); break;
        case SDL_BUTTON_X1:     printf("Bouton X1\n"); break;
        case SDL_BUTTON_X2:     printf("Bouton X2\n"); break;
        default: break;
    }
}

void windows_input_handle_button_up(SDL_Event e) {
    switch(e.button.button){
        case SDL_BUTTON_LEFT:   printf("Relâche gauche\n"); break;
        case SDL_BUTTON_RIGHT:  printf("Relâche droit\n"); break;
        case SDL_BUTTON_MIDDLE: printf("Relâche milieu\n"); break;
        case SDL_BUTTON_X1:     printf("Relâche X1\n"); break;
        case SDL_BUTTON_X2:     printf("Relâche X2\n"); break;
        default: break;
    }
}



void windows_init(void) {
    WINDOW_HANDLE = SDL_CreateWindow("Blaze", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
}

void windows_cleanup(void) {
    SDL_DestroyWindow(WINDOW_HANDLE);
}

void windows_input_acquire(void* data) {
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_QUIT:
                *((uint32_t*)data) = 1;
                break;
            case SDL_KEYDOWN:
                windows_input_handle_keys_down(event);
                break;
            case SDL_KEYUP:
                windows_input_handle_keys_up(event);
                break;
            case SDL_MOUSEBUTTONDOWN:
                windows_input_handle_button_down(event);
                break;
            case SDL_MOUSEBUTTONUP:
                windows_input_handle_button_up(event);
                break;
            case SDL_MOUSEMOTION:
                windows_input_handle_motion(event);
                break;
            default:
                break;
        }
    }
}
