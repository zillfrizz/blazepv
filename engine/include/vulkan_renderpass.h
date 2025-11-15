#pragma once

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include <volk.h>
#include <stdint.h>

extern VkRenderPass VULKAN_RENDERPASS;

void vulkan_renderpass_init(void);
void vulkan_renderpass_cleanup(void);