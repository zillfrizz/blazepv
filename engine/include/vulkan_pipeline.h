#pragma once

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include <volk.h>
#include <stdint.h>
#include <cglm/cglm.h>

// PIPELINE
extern VkPipeline VULKAN_PIPELINE;
extern VkPipelineLayout VULKAN_PIPELINE_LAYOUT;
extern VkDescriptorPool VULKAN_PIPELINE_DESCRIPTOR_POOL;
extern VkDescriptorSetLayout VULKAN_PIPELINE_DESCRIPTOR_SET_LAYOUT;
extern VkDescriptorSet* VULKAN_PIPELINE_DESCRIPTOR_SETS;
extern VkPipelineCache VULKAN_PIPELINE_CACHE;

// SHADERS
extern VkShaderModule VULKAN_SHADER_VERTEX;
extern VkShaderModule VULKAN_SHADER_FRAGMENT;

// BUFFERS
extern VkBuffer* VULKAN_BUFFERS_VERTEX;
extern VkDeviceMemory* VULKAN_BUFFERS_VERTEX_MEMORY;

extern VkBuffer VULKAN_BUFFER_VERTEX_STAGING;
extern VkDeviceMemory VULKAN_BUFFER_VERTEX_MEMORY_STAGING;

extern VkBuffer* VULKAN_BUFFERS_INSTANCE;
extern VkDeviceMemory* VULKAN_BUFFERS_INSTANCE_MEMORY;

extern VkBuffer VULKAN_BUFFER_INSTANCE_STAGING;
extern VkDeviceMemory VULKAN_BUFFER_INSTANCE_MEMORY_STAGING;

typedef struct VertexData{
    float coords[3];
    uint32_t faceId;
} VertexData;

typedef struct InstanceData{
    mat4 modelMat;
} InstanceData;


void vulkan_pipeline_init(void);
void vulkan_pipeline_cleanup(void);
void vulkan_vertex_buffer_transfer(VkBuffer buffer, uint32_t transferSize, uint32_t transferOffset, void* pTransferData);