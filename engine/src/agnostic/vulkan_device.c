#include <agnostic/vulkan_device.h>

void vulkan_device_physical_memory_properties_get(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties2* out){
    vkGetPhysicalDeviceMemoryProperties2(physicalDevice, out);
}
