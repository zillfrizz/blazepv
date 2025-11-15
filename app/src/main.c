#include <process.h>
#include <windows.h>
#include <stdio.h>
#include <SDL.h>
#include <windows1.h>
#include <vulkan_instance.h>
#include <vulkan_swapchain.h>
#include <vulkan_renderpass.h>
#include <vulkan_pipeline.h>
#include <vulkan_commands.h>
#include <vulkan_device.h>

int main(int argc, char** argv);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    return main(__argc, __argv);
}

void init(void){
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    windows_init();
    vulkan_instance_init();
    vulkan_device_init();
    surface_init();
    vulkan_renderpass_init();
    vulkan_swapchain_init();
    vulkan_pipeline_init();
    vulkan_commands_init();
}

void cleanup(void){
    vulkan_commands_cleanup();
    vulkan_pipeline_cleanup();
    vulkan_swapchain_cleanup();
    vulkan_renderpass_cleanup();
    surface_cleanup();
    vulkan_device_cleanup();
    vulkan_instance_cleanup();
    windows_cleanup();
}

int main(int argc, char** argv){
    init();
    uint32_t quit = 0;
    HANDLE thread;
    uint32_t threadId;
    //thread = _beginthreadex(0, 0, windows_input_acquire, &quit, 0, &threadId);
    while(!quit){
        windows_input_acquire(&quit);
        vulkan_commands_execute();
    }
    return 0;
}