#include <agnostic/vulkan_memory.h>
#include <agnostic/vulkan_device.h>
#include <stdlib.h>

void vulkan_memory_ma_create(VkDevice device, VkPhysicalDevice physicalDevice, VulkanMA* ma){
    VkPhysicalDeviceMemoryProperties2 memProps;
    vulkan_device_physical_memory_properties_get(physicalDevice, &memProps);
    blaze_tab_create(&(*ma).heaps, memProps.memoryProperties.memoryTypeCount, sizeof(VulkanHeap));

    for(int i = 0; i < memProps.memoryProperties.memoryTypeCount; i++){
        vulkan_memory_heap_create(device, &((VulkanHeap*)(*ma).heaps.data)[i]);
    }
}

void vulkan_memory_heap_create(VkDevice device, VulkanHeap* heap){
    (*heap).handle = 0;
    blaze_bucket_allocator_create(&(*heap).allocator, 0);
}

void vulkan_memory_allocate(VkDevice device, VulkanHeap* heap, BzMemory* map, VkDeviceSize allocSize){
    if(blaze_bucket_allocate(&(*heap).allocator, allocSize, map) != BLAZE_RETURN_SUCCESS){
        
    }
}

void vulkan_memory_allocate_buffer(uint32_t memoryTypeId, VkBuffer* buffer, uint32_t offset);
void vulkan_memory_unallocate(uint32_t memoryTypeId, VkDeviceSize start, VkDeviceSize size);

void vulkan_buffer_create(VkBuffer* buffer, VkDeviceMemory* memory, uint32_t size, VkBufferUsageFlags usages, uint32_t familyCount, uint32_t* familyIds) {
    VkBufferCreateInfo bufferInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .queueFamilyIndexCount = familyCount,
        .pQueueFamilyIndices = familyIds,
        .usage = usages,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };

    vkCreateBuffer(VULKAN_DEVICE, &bufferInfo, 0, buffer);

    VkBufferMemoryRequirementsInfo2 memReqInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2,
        .buffer = *buffer
    };

    VkMemoryRequirements2 memReq = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2
    };

    vkGetBufferMemoryRequirements2(VULKAN_DEVICE, &memReqInfo, &memReq);

    VkMemoryAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memReq.memoryRequirements.size,
        .memoryTypeIndex = vulkan_device_get_memory_type(&memReq, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
    };

    vkAllocateMemory(VULKAN_DEVICE, &allocInfo, 0, memory);

    VkBindBufferMemoryInfo bindInfo = {
        .sType = VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_INFO,
        .buffer = *buffer,
        .memory = *memory,
        .memoryOffset = 0,
        .pNext = 0
    };

    vkBindBufferMemory2(VULKAN_DEVICE, 1, &bindInfo);
}

void vulkan_buffer_staged_create(uint32_t size, uint32_t dstCount, VkBuffer* dstBuffers, VkDeviceMemory* dstMemories, VkBufferUsageFlags dstUsages, 
    VkBuffer* srcBuffer, VkDeviceMemory* srcMemory, VkBufferUsageFlags srcUsages){
        vulkan_buffer_create(srcBuffer, srcMemory, size, srcUsages | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 1, VULKAN_FAMILIES[VULKAN_FAMILY_TRANSFER]);
        for(int i = 0; i < dstCount; i++)
            vulkan_buffer_create(&dstBuffers[i], &dstMemories[i], size, dstUsages | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 2, VULKAN_FAMILIES);
}

void* vulkan_buffer_map(uint32_t offset, uint32_t size, VkMemoryMapFlags flags, VkDeviceMemory* memory) {
    void* map;
    vkMapMemory(VULKAN_DEVICE, *memory, offset, size, flags, &map);
    return map;
}

void vulkan_buffer_unmap(VkDeviceMemory* memory) {
    vkUnmapMemory(VULKAN_DEVICE, memory);
}