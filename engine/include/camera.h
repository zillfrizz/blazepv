#pragma once
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include <volk.h>
#include <cglm/cglm.h>
#include <cglm/mat4.h>
#include <stdint.h>

extern mat4 VULKAN_MATRIX_VIEW;

extern VkBuffer VULKAN_MATRIX_VIEW_BUFFER_STAGING;
extern VkDeviceMemory* VULKAN_MATRIX_VIEW_BUFFERS_MEMORY;
extern VkBuffer* VULKAN_MATRIX_VIEW_BUFFERS;
extern VkDeviceMemory VULKAN_MATRIX_VIEW_BUFFER_STAGING_MEMORY;
extern VkCommandBuffer* VULKAN_MATRIX_VIEW_BUFFERS_COMMAND;


void view_matrices_update();
void view_matrices_init();
void view_matrices_buffers_fill();