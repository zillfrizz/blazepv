#pragma once
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include <volk.h>
#include <stdint.h>

#include <bz_tab.h>

typedef struct FreeZone{
    VkDeviceSize start;
    VkDeviceSize size;
} FreeZone;

typedef struct VulkanHeap{
    VkDeviceMemory handle;
    Tab frees;
    VkDeviceSize total;
} VulkanHeap;

void vulkan_memory_pool_create(VkInstance instance, VkDevice device, VkPhysicalDevice physicalDevice, VulkanHeap* heaps);
void vulkan_memory_heap_create(VulkanHeap* heap, VkDeviceSize heapSize, VkDeviceSize startSize);

void vulkan_memory_buffer_allocate(uint32_t heapId, VkBuffer* buffer, VkDeviceSize offset);
void vulkan_memory_unallocate(uint32_t heapId, VkDeviceSize start, VkDeviceSize size);