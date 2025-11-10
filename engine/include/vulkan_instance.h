#pragma once

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include <volk.h>

extern VkInstance VULKAN_INSTANCE_HANDLE;

void vulkan_instance_init(void);
void vulkan_instance_cleanup(void);
void vulkan_instance_print_layers(void);
void vulkan_instance_print_extensions(void);