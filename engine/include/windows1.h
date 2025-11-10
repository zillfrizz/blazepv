#pragma once
#include <stdint.h>
struct SDL_Window;

extern struct SDL_Window* WINDOW_HANDLE;
void windows_init(void);
void windows_cleanup(void);
void windows_input_acquire(void* data);

