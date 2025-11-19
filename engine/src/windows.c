#include <windows1.h>
#include <stdint.h>

#include <SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan_instance.h>
#include <vulkan_device.h>
#include <camera.h>

//WINDOWS
struct SDL_Window* WINDOW_HANDLE;
uint32_t WINDOW_WIDTH = 800;
uint32_t WINDOW_HEIGHT = 600;

//SURFACES
VkSurfaceKHR VULKAN_SURFACE;
VkSurfaceFormat2KHR VULKAN_SURFACE_FORMAT;
VkExtent2D VULKAN_SURFACE_EXTENT;
VkSurfaceCapabilities2KHR VULKAN_SURFACE_CAPABILITIES;

const char* vkFormatToString(VkFormat format) {
    switch (format) {
        case VK_FORMAT_B8G8R8A8_SRGB: return "VK_FORMAT_B8G8R8A8_SRGB";
        case VK_FORMAT_R8G8B8A8_UNORM: return "VK_FORMAT_R8G8B8A8_UNORM";
        // ajouter tous les formats que tu veux
        default: return "UNKNOWN_FORMAT";
    }
}

const char* vkColorSpaceToString(VkColorSpaceKHR cs) {
    switch (cs) {
        case VK_COLOR_SPACE_SRGB_NONLINEAR_KHR: return "SRGB_NONLINEAR";
        default: return "UNKNOWN_COLORSPACE";
    }
}

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

void windows_input_handle_motion(SDL_Event e) {
    int x = e.motion.x;
    int y = e.motion.y;
    int dx = e.motion.xrel;
    int dy = e.motion.yrel;
    printf("Souris déplacée: x=%d y=%d, dx=%d dy=%d\n", x, y, dx, dy);
    view_matrices_update();
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
    #ifdef SDL_VIDEO_VULKAN
        printf("SDL Vulkan is supported\n");
    #else
        printf("SDL Vulkan is NOT supported\n");
    #endif

    WINDOW_HANDLE = SDL_CreateWindow("Blaze", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_VULKAN);
    if (!WINDOW_HANDLE) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        exit(1);
    }
}

void surface_vulkan_init_format_extent(void){

    VkPhysicalDeviceSurfaceInfo2KHR surfaceInfo = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR,
        .surface = VULKAN_SURFACE,
        .pNext = 0
    };

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormats2KHR(VULKAN_PHYSICAL_DEVICE, &surfaceInfo, &formatCount, 0);
    VkSurfaceFormat2KHR* formats = malloc(sizeof(VkSurfaceFormat2KHR) * formatCount); 
    for(int i = 0; i < formatCount; i++){
        formats[i].sType = VK_STRUCTURE_TYPE_SURFACE_FORMAT_2_KHR;
        formats[i].pNext = 0;
    }
    vkGetPhysicalDeviceSurfaceFormats2KHR(VULKAN_PHYSICAL_DEVICE, &surfaceInfo, &formatCount, formats);

    printf("choose one format by id:\n");
    for(int i = 0; i < formatCount; i++){
        VkImageFormatProperties props;
        if(!vkGetPhysicalDeviceImageFormatProperties(
            VULKAN_PHYSICAL_DEVICE,
            formats[i].surfaceFormat.format,
            VK_IMAGE_TYPE_2D,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            0,
            &props
        ))
            printf("\t%d, %s, %s\n", i, vkColorSpaceToString(formats[i].surfaceFormat.colorSpace), vkFormatToString(formats[i].surfaceFormat.format));
    }
    uint32_t formatId;
    scanf("%" SCNu32, &formatId);
    printf("\n");
    VULKAN_SURFACE_FORMAT = formats[formatId];
        
    VULKAN_SURFACE_CAPABILITIES.sType = VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_KHR;
    VULKAN_SURFACE_CAPABILITIES.pNext = 0;
    vkGetPhysicalDeviceSurfaceCapabilities2KHR(VULKAN_PHYSICAL_DEVICE, &surfaceInfo, &VULKAN_SURFACE_CAPABILITIES);
    VULKAN_SURFACE_EXTENT = VULKAN_SURFACE_CAPABILITIES.surfaceCapabilities.currentExtent;
    #define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
    if(VULKAN_SURFACE_CAPABILITIES.surfaceCapabilities.currentExtent.height != UINT32_MAX){
        VULKAN_SURFACE_EXTENT.height = 
        CLAMP(
            WINDOW_HEIGHT, VULKAN_SURFACE_CAPABILITIES.surfaceCapabilities.minImageExtent.height, 
            VULKAN_SURFACE_CAPABILITIES.surfaceCapabilities.maxImageExtent.height
        );
        VULKAN_SURFACE_EXTENT.width = 
        CLAMP(
            WINDOW_HEIGHT, VULKAN_SURFACE_CAPABILITIES.surfaceCapabilities.minImageExtent.width, 
            VULKAN_SURFACE_CAPABILITIES.surfaceCapabilities.maxImageExtent.width
        );
    }
}

void surface_init(void){
    if(!SDL_Vulkan_CreateSurface(WINDOW_HANDLE, VULKAN_INSTANCE, &VULKAN_SURFACE)){
        fprintf(stderr, "Erreur SDL: %s\n", SDL_GetError());
        exit(1);
    }

    surface_vulkan_init_format_extent();
    
}

void windows_cleanup(void) {
    SDL_DestroyWindow(WINDOW_HANDLE);
}

void surface_cleanup(void){
    vkDestroySurfaceKHR(VULKAN_INSTANCE, VULKAN_SURFACE, 0);
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
