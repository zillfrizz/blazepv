#pragma once
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include <volk.h>
#include <stdint.h>

#include <blaze_bucket.h>

typedef struct VulkanHeap{
    VkDeviceMemory handle; // VULKAN MEMORY HANDLE
    BucketAllocation allocator; // BUCKET ALLOCATOR
} VulkanHeap;

typedef struct VulkanMA{
    BzTab heaps; // VULKAN HEAPS
} VulkanMA;

void vulkan_memory_ma_create(VkDevice device, VkPhysicalDevice physicalDevice, VulkanMA* ma);
void vulkan_memory_heap_create(VkDevice device, VulkanHeap* heap, VkDeviceSize heapSize);

void vulkan_memory_buffer_allocate(uint32_t heapId, VkBuffer* buffer, VkDeviceSize offset);
void vulkan_memory_unallocate(uint32_t heapId, VkDeviceSize start, VkDeviceSize size);