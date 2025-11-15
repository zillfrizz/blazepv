#pragma once
#include <stdint.h>
struct SDL_Window;

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include <volk.h>
#include <stdint.h>

//WINDOWS
extern struct SDL_Window* WINDOW_HANDLE;
extern uint32_t WINDOW_HEIGHT;
extern uint32_t WINDOW_WIDTH;

//SURFACES
extern VkSurfaceKHR WINDOW_VULKAN_SURFACE;
extern VkSurfaceFormat2KHR WINDOW_VULKAN_SURFACE_FORMAT;
extern VkExtent2D WINDOW_VULKAN_SURFACE_EXTENT;
extern VkSurfaceCapabilities2KHR WINDOW_VULKAN_SURFACE_CAPABILITIES;

void windows_init(void);
void surface_init(void);
void surface_cleanup(void);
void windows_cleanup(void);
void windows_input_acquire(void* data);

