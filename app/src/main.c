#include "vulkan_instance.h"
#include "windows1.h"
#include "sdl_instance.h"
#include <process.h>
#include <windows.h>
#include <stdio.h>

int main(int argc, char** argv);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    return main(__argc, __argv);
}

void init(void){
    sdl_instance_init();
    windows_init();
    vulkan_instance_init();
}

int main(int argc, char** argv){
    init();
    uint32_t quit = 0;
    HANDLE thread;
    uint32_t threadId;
    //thread = _beginthreadex(0, 0, windows_input_acquire, &quit, 0, &threadId);
    while(!quit){
        windows_input_acquire(&quit);
    }
    return 0;
}