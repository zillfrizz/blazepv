#include <vulkan_swapchain.h>
#include <vulkan_device.h>
#include <windows1.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <vulkan_renderpass.h>

// SWAPCHAIN
VkSwapchainKHR VULKAN_SWAPCHAIN;
VkExtent2D VULKAN_SWAPCHAIN_EXTENT;

// IMAGES
uint32_t VULKAN_SWAPCHAIN_IMAGE_COUNT;
VkImage* VULKAN_SWAPCHAIN_IMAGES;
VkImageView* VULKAN_SWAPCHAIN_IMAGE_VIEWS;
VkFramebuffer* VULKAN_FRAMEBUFFERS;

//SYNC
uint32_t VULKAN_SWAPCHAIN_IMAGE_INDEX;
VkFence* VULKAN_SWAPCHAIN_DB_FENCES;

const char* VkResultToString(VkResult res) {
    switch(res) {
        case VK_SUCCESS: return "VK_SUCCESS";
        case VK_NOT_READY: return "VK_NOT_READY";
        case VK_TIMEOUT: return "VK_TIMEOUT";
        case VK_EVENT_SET: return "VK_EVENT_SET";
        case VK_EVENT_RESET: return "VK_EVENT_RESET";
        case VK_INCOMPLETE: return "VK_INCOMPLETE";
        case VK_ERROR_OUT_OF_HOST_MEMORY: return "VK_ERROR_OUT_OF_HOST_MEMORY";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
        case VK_ERROR_INITIALIZATION_FAILED: return "VK_ERROR_INITIALIZATION_FAILED";
        case VK_ERROR_DEVICE_LOST: return "VK_ERROR_DEVICE_LOST";
        case VK_ERROR_MEMORY_MAP_FAILED: return "VK_ERROR_MEMORY_MAP_FAILED";
        case VK_ERROR_LAYER_NOT_PRESENT: return "VK_ERROR_LAYER_NOT_PRESENT";
        case VK_ERROR_EXTENSION_NOT_PRESENT: return "VK_ERROR_EXTENSION_NOT_PRESENT";
        case VK_ERROR_FEATURE_NOT_PRESENT: return "VK_ERROR_FEATURE_NOT_PRESENT";
        case VK_ERROR_INCOMPATIBLE_DRIVER: return "VK_ERROR_INCOMPATIBLE_DRIVER";
        case VK_ERROR_TOO_MANY_OBJECTS: return "VK_ERROR_TOO_MANY_OBJECTS";
        case VK_ERROR_FORMAT_NOT_SUPPORTED: return "VK_ERROR_FORMAT_NOT_SUPPORTED";
        case VK_ERROR_SURFACE_LOST_KHR: return "VK_ERROR_SURFACE_LOST_KHR";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
        default: return "UNKNOWN_ERROR";
    }
}

void vulkan_swapchain_framebuffers_init(void){
    VULKAN_FRAMEBUFFERS = malloc(sizeof(VkFramebuffer) * VULKAN_SWAPCHAIN_IMAGE_COUNT);
    VkFramebufferCreateInfo* framebuffers = malloc(sizeof(VkFramebufferCreateInfo) * VULKAN_SWAPCHAIN_IMAGE_COUNT);
    for(int i = 0; i < VULKAN_SWAPCHAIN_IMAGE_COUNT; i++){
        framebuffers[i].sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffers[i].renderPass = VULKAN_RENDERPASS;
        framebuffers[i].attachmentCount = 1;
        framebuffers[i].pAttachments = &VULKAN_SWAPCHAIN_IMAGE_VIEWS[i];
        framebuffers[i].layers = 1;
        framebuffers[i].width = VULKAN_SURFACE_EXTENT.width; 
        framebuffers[i].height = VULKAN_SURFACE_EXTENT.height;
        framebuffers[i].flags = 0;
        framebuffers[i].pNext = 0;
        if(vkCreateFramebuffer(VULKAN_DEVICE, &framebuffers[i], 0, &VULKAN_FRAMEBUFFERS[i]) != VK_SUCCESS){
            printf("can't create framebuffer(s).\n");
        }
    }
}

void vulkan_swapchain_imageviews_init(void){
    VULKAN_SWAPCHAIN_IMAGE_VIEWS = malloc(sizeof(VkImageView) * VULKAN_SWAPCHAIN_IMAGE_COUNT);
    vkGetSwapchainImagesKHR(VULKAN_DEVICE, VULKAN_SWAPCHAIN, &VULKAN_SWAPCHAIN_IMAGE_COUNT, VULKAN_SWAPCHAIN_IMAGES);
    for(int i = 0; i < VULKAN_SWAPCHAIN_IMAGE_COUNT; i++){
        VkImageViewCreateInfo viewInfos = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = VULKAN_SWAPCHAIN_IMAGES[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = VULKAN_SURFACE_FORMAT.surfaceFormat.format,
            .components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.a = VK_COMPONENT_SWIZZLE_IDENTITY,
            .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .subresourceRange.baseMipLevel = 0,
            .subresourceRange.levelCount = 1,
            .subresourceRange.baseArrayLayer = 0,
            .subresourceRange.layerCount = 1,
            .pNext = 0
        };
        if(vkCreateImageView(VULKAN_DEVICE, &viewInfos, 0, &VULKAN_SWAPCHAIN_IMAGE_VIEWS[i]) != VK_SUCCESS){
            printf("can't create swap chain image view(s)\n");
        }
    }
}

void vulkan_swapchain_fence_init(void){
    VULKAN_SWAPCHAIN_DB_FENCES = malloc(sizeof(VkFence) * 2);

    VkFenceCreateInfo fenceInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
        .pNext = 0
    };

    VkFenceCreateInfo fenceInfo2 = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = 0,
        .pNext = 0
    };

    if(vkCreateFence(VULKAN_DEVICE, &fenceInfo, 0, &VULKAN_SWAPCHAIN_DB_FENCES[0]) != VK_SUCCESS){
        printf("can't create swapchain fence.\n");
    }

    if(vkCreateFence(VULKAN_DEVICE, &fenceInfo2, 0, &VULKAN_SWAPCHAIN_DB_FENCES[1]) != VK_SUCCESS){
        printf("can't create swapchain fence.\n");
    }
}

void vulkan_swapchain_init(void){

    VkSwapchainCreateInfoKHR createInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = VULKAN_SURFACE,
        .imageFormat = VULKAN_SURFACE_FORMAT.surfaceFormat.format,
        .imageColorSpace = VULKAN_SURFACE_FORMAT.surfaceFormat.colorSpace,
        .minImageCount = 2,
        .imageExtent = VULKAN_SURFACE_EXTENT,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = 0,
        .preTransform = VULKAN_SURFACE_CAPABILITIES.surfaceCapabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = VK_PRESENT_MODE_FIFO_KHR,
        .clipped = VK_TRUE,
        .oldSwapchain = 0,
        .pNext = 0
    };

    VkResult result = vkCreateSwapchainKHR(VULKAN_DEVICE, &createInfo, NULL, &VULKAN_SWAPCHAIN);
    if (result != VK_SUCCESS) {
        printf("Can't create swap chain, VkResult = %s\n", VkResultToString(result));
    }

    vkGetSwapchainImagesKHR(VULKAN_DEVICE, VULKAN_SWAPCHAIN, &VULKAN_SWAPCHAIN_IMAGE_COUNT, 0);
    VULKAN_SWAPCHAIN_IMAGES = malloc(sizeof(VkImage) * VULKAN_SWAPCHAIN_IMAGE_COUNT);
    vulkan_swapchain_imageviews_init();
    vulkan_swapchain_framebuffers_init();
    vulkan_swapchain_fence_init();
}

void vulkan_swapchain_cleanup(void){
    for(int i = 0; i < VULKAN_SWAPCHAIN_IMAGE_COUNT; i++){
        vkDestroyFramebuffer(VULKAN_DEVICE, VULKAN_FRAMEBUFFERS[i], 0);
        vkDestroyImageView(VULKAN_DEVICE, VULKAN_SWAPCHAIN_IMAGE_VIEWS[i], 0);
    }
    vkDestroySwapchainKHR(VULKAN_DEVICE, VULKAN_SWAPCHAIN, 0);
}