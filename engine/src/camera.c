#include "camera.h"
#include <math.h>
#include <vulkan_commands.h>
#include <vulkan_device.h>
#include <vulkan_swapchain.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

float value;
mat4 VULKAN_MATRIX_VIEW;

VkBuffer* VULKAN_MATRIX_VIEW_BUFFERS;
VkDeviceMemory* VULKAN_MATRIX_VIEW_BUFFERS_MEMORY;

VkBuffer VULKAN_MATRIX_VIEW_BUFFER_STAGING;
VkDeviceMemory VULKAN_MATRIX_VIEW_BUFFER_STAGING_MEMORY;

void view_matrices_update(){
    value = fmodf(value * 1.1f, 2.0f);
    glm_mat4_identity(VULKAN_MATRIX_VIEW);
    glm_mat4_scale(VULKAN_MATRIX_VIEW, value);
}

void view_matrices_staging_buffer_init(){
    VkBufferCreateInfo bufferInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = 0,
        .flags = 0,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &VULKAN_FAMILIES[VULKAN_FAMILY_TRANSFER],
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .size = 64,
        .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT
    };

    if(vkCreateBuffer(VULKAN_DEVICE, &bufferInfo, 0, &VULKAN_MATRIX_VIEW_BUFFER_STAGING) != VK_SUCCESS){
        printf("can't create view matrix staging buffer\n");
    }

    VkBufferMemoryRequirementsInfo2 bufferMemReq = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2,
        .pNext = 0,
        .buffer = VULKAN_MATRIX_VIEW_BUFFER_STAGING
    };

    VkMemoryRequirements2 memReq = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2,
        .pNext = 0
    };
    vkGetBufferMemoryRequirements2(VULKAN_DEVICE, &bufferMemReq, &memReq);

    VkMemoryAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = 0,
        .allocationSize = 64,
        .memoryTypeIndex = vulkan_device_get_memory_type(&memReq, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
    };

    if(vkAllocateMemory(VULKAN_DEVICE, &allocInfo, 0, &VULKAN_MATRIX_VIEW_BUFFER_STAGING_MEMORY) != VK_SUCCESS){
        printf("can't allocate view matrix staging memory\n");
    }

    VkBindBufferMemoryInfo bindInfo = {
        .sType = VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_INFO,
        .pNext = 0,
        .buffer = VULKAN_MATRIX_VIEW_BUFFER_STAGING,
        .memory = VULKAN_MATRIX_VIEW_BUFFER_STAGING_MEMORY,
        .memoryOffset = 0
    };

    if(vkBindBufferMemory2(VULKAN_DEVICE, 1, &bindInfo) != VK_SUCCESS){
        printf("can't bind view matrix staging buffer\n");
    }
}

void view_matrices_buffers_init(){
    VULKAN_MATRIX_VIEW_BUFFERS = malloc(sizeof(VkBuffer) * VULKAN_SWAPCHAIN_IMAGE_COUNT);
    VULKAN_MATRIX_VIEW_BUFFERS_MEMORY = malloc(sizeof(VkDeviceMemory) * VULKAN_SWAPCHAIN_IMAGE_COUNT);

    VkBufferCreateInfo bufferInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = 0,
        .flags = 0,
        .queueFamilyIndexCount = 2,
        .pQueueFamilyIndices = VULKAN_FAMILIES,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .size = 64,
        .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
    };

    for(int i = 0; i < VULKAN_SWAPCHAIN_IMAGE_COUNT; i++){
        if(vkCreateBuffer(VULKAN_DEVICE, &bufferInfo, 0, &VULKAN_MATRIX_VIEW_BUFFERS[i]) != VK_SUCCESS){
            printf("can't create view matrix buffer\n");
        }

        VkBufferMemoryRequirementsInfo2 bufferMemReq = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2,
            .pNext = 0,
            .buffer = VULKAN_MATRIX_VIEW_BUFFERS[i]
        };

        VkMemoryRequirements2 memReq = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2,
            .pNext = 0
        };
        vkGetBufferMemoryRequirements2(VULKAN_DEVICE, &bufferMemReq, &memReq);

        VkMemoryAllocateInfo allocInfo = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext = 0,
            .allocationSize = 64,
            .memoryTypeIndex = vulkan_device_get_memory_type(&memReq, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
        };

        if(vkAllocateMemory(VULKAN_DEVICE, &allocInfo, 0, &VULKAN_MATRIX_VIEW_BUFFERS_MEMORY[i]) != VK_SUCCESS){
            printf("can't allocate view matrix memory\n");
        }

        VkBindBufferMemoryInfo bindInfo = {
            .sType = VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_INFO,
            .pNext = 0,
            .buffer = VULKAN_MATRIX_VIEW_BUFFERS[i],
            .memory = VULKAN_MATRIX_VIEW_BUFFERS_MEMORY[i],
            .memoryOffset = 0
        };

        if(vkBindBufferMemory2(VULKAN_DEVICE, 1, &bindInfo) != VK_SUCCESS){
            printf("can't bind view matrix buffer\n");
        }
    }
}

void view_matrices_init(){
    glm_mat4_identity(VULKAN_MATRIX_VIEW);
    view_matrices_buffers_init();
    view_matrices_staging_buffer_init();
}

void view_matrices_buffers_fill(){
        VkFence tmpFence;

        VkFenceCreateInfo fenceInfo = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = 0,
            .flags = 0
        };

        if(vkCreateFence(VULKAN_DEVICE, &fenceInfo, 0, &tmpFence) != VK_SUCCESS){
            printf("can't create matrix tmp fence\n");
        }

        VkMemoryMapInfo mapInfo = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_MAP_INFO,
            .pNext = 0,
            .flags = 0,
            .memory = VULKAN_MATRIX_VIEW_BUFFER_STAGING_MEMORY,
            .offset = 0,
            .size = 64
        };

        VkMemoryUnmapInfo unmapInfo = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_UNMAP_INFO,
            .pNext = 0,
            .flags = 0,
            .memory = VULKAN_MATRIX_VIEW_BUFFER_STAGING_MEMORY
        };

    for(int i = 0; i < VULKAN_SWAPCHAIN_IMAGE_COUNT; i++){
        void* map;
        vkMapMemory(VULKAN_DEVICE, VULKAN_MATRIX_VIEW_BUFFER_STAGING_MEMORY,
        0, 64, 0, &map);
        memcpy(map, VULKAN_MATRIX_VIEW, 64);
        vkUnmapMemory(VULKAN_DEVICE, VULKAN_MATRIX_VIEW_BUFFER_STAGING_MEMORY);
        vulkan_commands_transfer_view_matrix_init(i, tmpFence);
    }
}
