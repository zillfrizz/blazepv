#pragma once

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include <volk.h>
#include <stdint.h>

// SWAPCHAIN
extern VkSwapchainKHR VULKAN_SWAPCHAIN;
extern VkExtent2D VULKAN_SWAPCHAIN_EXTENT;

// IMAGES
extern uint32_t VULKAN_SWAPCHAIN_IMAGE_COUNT;
extern VkImage* VULKAN_SWAPCHAIN_IMAGES;
extern VkImage* VULKAN_SWAPCHAIN_DEPTH_IMAGES;
extern VkDeviceMemory* VULKAN_SWAPCHAIN_DEPTH_IMAGES_MEMORY;
extern VkImageView* VULKAN_FRAMEBUFFERS_IMAGE_VIEWS;
extern VkFramebuffer* VULKAN_FRAMEBUFFERS;

//SYNC
extern uint32_t VULKAN_SWAPCHAIN_IMAGE_INDEX;
extern VkFence* VULKAN_SWAPCHAIN_DB_FENCES;

void vulkan_swapchain_init(void);
void vulkan_swapchain_cleanup(void);