#pragma once

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include <volk.h>
#include <stdint.h>

// PIPELINE
extern VkPipeline VULKAN_PIPELINE;
extern VkPipelineLayout VULKAN_PIPELINE_LAYOUT;
extern VkPipelineCache VULKAN_PIPELINE_CACHE;

// SHADERS
extern VkShaderModule VULKAN_SHADER_VERTEX;
extern VkShaderModule VULKAN_SHADER_FRAGMENT;

// BUFFERS
extern VkBuffer VULKAN_BUFFER_VERTEX;
extern VkDeviceMemory VULKAN_BUFFER_VERTEX_MEMORY;


void vulkan_pipeline_init(void);
void vulkan_pipeline_cleanup(void);
void vulkan_vertex_buffer_transfer(VkBuffer buffer, uint32_t transferSize, uint32_t transferOffset, void* pTransferData);