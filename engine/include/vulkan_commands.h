#pragma once
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include <volk.h>
#include <stdint.h>

extern VkCommandPool VULKAN_COMMAND_POOL_GRAPHICS;
extern VkCommandPool VULKAN_COMMAND_POOL_TRANSFER;

extern uint32_t VULKAN_COMMAND_BUFFERS_GRAPHICS_COUNT;
extern uint32_t VULKAN_COMMAND_BUFFERS_TRANSFER_COUNT;

extern VkCommandBuffer* VULKAN_COMMAND_BUFFERS_GRAPHICS;
extern VkCommandBuffer* VULKAN_COMMAND_BUFFERS_TRANSFER;

void vulkan_commands_init(void);
void vulkan_commands_cleanup(void);
void vulkan_commands_execute(void);
void vulkan_commands_graphics_records(void);