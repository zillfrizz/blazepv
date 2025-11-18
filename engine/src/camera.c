#include "camera.h"
#include <math.h>
#include <vulkan_commands.h>
#include <vulkan_swapchain.h>
#include <vulkan_device.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

float value;
mat4 VULKAN_MATRIX_VIEW;

VkBuffer* VULKAN_MATRIX_VIEW_BUFFERS;
VkDeviceMemory* VULKAN_MATRIX_VIEW_BUFFERS_MEMORY;

VkBuffer VULKAN_MATRIX_VIEW_BUFFER_STAGING;
VkDeviceMemory VULKAN_MATRIX_VIEW_BUFFER_MEMORY_STAGING;

void view_matrices_buffers_init(){
    VULKAN_MATRIX_VIEW_BUFFERS = malloc(sizeof(VkBuffer) * VULKAN_SWAPCHAIN_IMAGE_COUNT);
    VULKAN_MATRIX_VIEW_BUFFERS_MEMORY = malloc(sizeof(VkDeviceMemory) * VULKAN_SWAPCHAIN_IMAGE_COUNT);
    VkBufferCreateInfo bufferInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = 0,
        .flags = 0,
        .size = 64,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 2,
        .pQueueFamilyIndices = VULKAN_FAMILIES,
        .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
    };

    for(int i = 0; i < VULKAN_SWAPCHAIN_IMAGE_COUNT; i++){
        vkCreateBuffer(VULKAN_DEVICE, &bufferInfo, 0, &VULKAN_MATRIX_VIEW_BUFFERS[i]);

        VkBufferMemoryRequirementsInfo2 bufferReqInfo = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2,
            .pNext = 0,
            .buffer = VULKAN_MATRIX_VIEW_BUFFERS[i]
        };

        VkMemoryRequirements2 reqInfo = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2,
            .pNext = 0
        };

        vkGetBufferMemoryRequirements2(VULKAN_DEVICE, &bufferReqInfo, &reqInfo);

        VkMemoryAllocateInfo allocInfo = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext = 0,
            .allocationSize = 64,
            .memoryTypeIndex = vulkan_device_get_memory_type(&reqInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
        };

        vkAllocateMemory(VULKAN_DEVICE, &allocInfo, 0, &VULKAN_MATRIX_VIEW_BUFFERS_MEMORY[i]);

        VkBindBufferMemoryInfo bindInfo = {
            .sType = VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_INFO,
            .pNext = 0,
            .buffer = VULKAN_MATRIX_VIEW_BUFFERS[i],
            .memory = VULKAN_MATRIX_VIEW_BUFFERS_MEMORY[i],
            .memoryOffset = 0
        };

        vkBindBufferMemory2(VULKAN_DEVICE, 1, &bindInfo);
    }
}

void view_matrices_buffers_staging_init(){
    VkBufferCreateInfo bufferInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = 0,
        .flags = 0,
        .size = 64,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &VULKAN_FAMILIES[VULKAN_FAMILY_TRANSFER],
        .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT
    };

    vkCreateBuffer(VULKAN_DEVICE, &bufferInfo, 0, &VULKAN_MATRIX_VIEW_BUFFER_STAGING);

    VkBufferMemoryRequirementsInfo2 bufferReqInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2,
        .pNext = 0,
        .buffer = VULKAN_MATRIX_VIEW_BUFFER_STAGING
    };

    VkMemoryRequirements2 reqInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2,
        .pNext = 0
    };

    vkGetBufferMemoryRequirements2(VULKAN_DEVICE, &bufferReqInfo, &reqInfo);

    VkMemoryAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = 0,
        .allocationSize = 64,
        .memoryTypeIndex = vulkan_device_get_memory_type(&reqInfo, 
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
    };

    vkAllocateMemory(VULKAN_DEVICE, &allocInfo, 0, &VULKAN_MATRIX_VIEW_BUFFER_MEMORY_STAGING);

    VkBindBufferMemoryInfo bindInfo = {
        .sType = VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_INFO,
        .pNext = 0,
        .buffer = VULKAN_MATRIX_VIEW_BUFFER_STAGING,
        .memory = VULKAN_MATRIX_VIEW_BUFFER_MEMORY_STAGING,
        .memoryOffset = 0
    };

    vkBindBufferMemory2(VULKAN_DEVICE, 1, &bindInfo);
}

void view_matrices_update(){
    value = fmodf(value * 1.1f, 2.0f);
    glm_mat4_identity(VULKAN_MATRIX_VIEW);
    glm_mat4_scale(VULKAN_MATRIX_VIEW, value);
    vulkan_commands_graphics_records();
}

void view_matrices_init(void){
    view_matrices_buffers_init();
    view_matrices_buffers_staging_init();
}

void view_matrices_buffer_fill(uint32_t bufferId, VkFence fence){
    void* data;
    vkMapMemory(VULKAN_DEVICE, VULKAN_MATRIX_VIEW_BUFFER_MEMORY_STAGING, 0, 64, 0, &data);
    memcpy(data, VULKAN_MATRIX_VIEW, 64);
    vkUnmapMemory(VULKAN_DEVICE, VULKAN_MATRIX_VIEW_BUFFER_MEMORY_STAGING);
    free(data);
    vulkan_commands_matrix_view_init(bufferId, fence);
}

void view_matrices_buffers_fill(){
    VkFence tmpFence;
    VkFenceCreateInfo fenceInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = 0,
        .pNext = 0
    };
    vkCreateFence(VULKAN_DEVICE, &fenceInfo, 0, &tmpFence);
    for(int i = 0; i < VULKAN_SWAPCHAIN_IMAGE_COUNT; i++){
        view_matrices_buffer_fill(i, tmpFence);
    }
    vkDestroyFence(VULKAN_DEVICE, tmpFence, 0);
}
