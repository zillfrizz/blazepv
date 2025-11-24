#pragma once
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include <volk.h>
#include <stdint.h>

void vulkan_device_physical_memory_properties_get(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties2* out);