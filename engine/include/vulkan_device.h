#pragma once
#include <vulkan/vulkan.h>

extern VkPhysicalDevice VULKAN_PHYSICAL_DEVICE_HANDLE;
extern VkPhysicalDeviceProperties2 VULKAN_PHYSICAL_DEVICE_PROPERTIES;
extern VkDevice VULKAN_DEVICE_HANDLE;
extern VkQueue VULKAN_QUEUE_GRAPHICS;
extern VkQueue VULKAN_QUEUE_TRANSFER;

void vulkan_device_init(const char* const* extProps, const uint32_t extCount);
void vulkan_device_cleanup(void);
void vulkan_device_show_extensions(void);
