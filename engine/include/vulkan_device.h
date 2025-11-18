#pragma once
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include <volk.h>

// DEVICE
extern VkPhysicalDevice VULKAN_PHYSICAL_DEVICE;
extern VkPhysicalDeviceProperties2 VULKAN_PHYSICAL_DEVICE_PROPERTIES;
extern VkDevice VULKAN_DEVICE;

enum VULKAN_FAMILY{
    VULKAN_FAMILY_GRAPHICS,
    VULKAN_FAMILY_TRANSFER,
    VULKAN_FAMILY_COUNT
};

// QUEUES
extern uint32_t VULKAN_FAMILIES[VULKAN_FAMILY_COUNT];

extern VkQueue VULKAN_QUEUE_GRAPHICS;
extern VkQueue VULKAN_QUEUE_TRANSFER;



void vulkan_device_init();
void vulkan_device_cleanup(void);
uint32_t vulkan_device_get_memory_type(VkMemoryRequirements2* memReq2, VkMemoryPropertyFlags memFlags);
