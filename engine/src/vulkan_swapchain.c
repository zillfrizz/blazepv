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
VkImage* VULKAN_SWAPCHAIN_DEPTH_IMAGES;
VkDeviceMemory* VULKAN_SWAPCHAIN_DEPTH_IMAGES_MEMORY;
VkImageView* VULKAN_FRAMEBUFFERS_IMAGE_VIEWS;
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
    for(int i = 0; i < VULKAN_SWAPCHAIN_IMAGE_COUNT; i++){
        VkFramebufferCreateInfo framebufferInfo = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = VULKAN_RENDERPASS,
            .attachmentCount = 2,
            .pAttachments = &VULKAN_FRAMEBUFFERS_IMAGE_VIEWS[i * 2],
            .layers = 1,
            .width = VULKAN_SURFACE_EXTENT.width,
            .height = VULKAN_SURFACE_EXTENT.height,
            .flags = 0,
            .pNext = 0
        };
        
        if(vkCreateFramebuffer(VULKAN_DEVICE, &framebufferInfo, 0, &VULKAN_FRAMEBUFFERS[i]) != VK_SUCCESS){
            printf("can't create framebuffer(s).\n");
        }
    }
}

void vulkan_swapchain_imageviews_init(void){
    VULKAN_FRAMEBUFFERS_IMAGE_VIEWS = malloc(sizeof(VkImageView) * VULKAN_SWAPCHAIN_IMAGE_COUNT * 2);
    for(int i = 0; i < VULKAN_SWAPCHAIN_IMAGE_COUNT; i++){
        VkImageViewCreateInfo colorViewInfos = {
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
        VkImageViewCreateInfo depthViewInfos = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = VULKAN_SWAPCHAIN_DEPTH_IMAGES[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = VULKAN_SURFACE_FORMAT_DEPTH,
            .components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.a = VK_COMPONENT_SWIZZLE_IDENTITY,
            .subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
            .subresourceRange.baseMipLevel = 0,
            .subresourceRange.levelCount = 1,
            .subresourceRange.baseArrayLayer = 0,
            .subresourceRange.layerCount = 1,
            .pNext = 0
        };

        if(vkCreateImageView(VULKAN_DEVICE, &colorViewInfos, 0, &VULKAN_FRAMEBUFFERS_IMAGE_VIEWS[i * 2]) != VK_SUCCESS){
            printf("can't create swap chain color image view(s)\n");
        }
        if(vkCreateImageView(VULKAN_DEVICE, &depthViewInfos, 0, &VULKAN_FRAMEBUFFERS_IMAGE_VIEWS[(i * 2) + 1]) != VK_SUCCESS){
            printf("can't create swap chain depth image view(s)\n");
        }
    }
}

void vulkan_swapchain_depth_images_init(void){
    VkImageCreateInfo imageInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .flags = 0,
        .pNext = 0,
        .imageType = VK_IMAGE_TYPE_2D,
        .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &VULKAN_FAMILIES[VULKAN_FAMILY_GRAPHICS],
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .extent = {VULKAN_SURFACE_EXTENT.width, VULKAN_SURFACE_EXTENT.height, 1},
        .format = VULKAN_SURFACE_FORMAT_DEPTH,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .arrayLayers = 1,
        .mipLevels = 1,
        .tiling = VK_IMAGE_TILING_OPTIMAL
    };

    VkBindImageMemoryInfo bindInfos[VULKAN_SWAPCHAIN_IMAGE_COUNT];

    for(int i = 0; i < VULKAN_SWAPCHAIN_IMAGE_COUNT; i++){
        vkCreateImage(VULKAN_DEVICE, &imageInfo, 0, &VULKAN_SWAPCHAIN_DEPTH_IMAGES[i]);

        VkImageMemoryRequirementsInfo2 reqInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2,
            .pNext = 0,
            .image = VULKAN_SWAPCHAIN_DEPTH_IMAGES[i]
        };

        VkMemoryRequirements2 memReqs = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2,
            .pNext = 0
        };

        vkGetImageMemoryRequirements2(VULKAN_DEVICE, &reqInfo, &memReqs);

        VkMemoryAllocateInfo allocInfo = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext = 0,
            .allocationSize = memReqs.memoryRequirements.size,
            .memoryTypeIndex = vulkan_device_get_memory_type(&memReqs, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
        };

        vkAllocateMemory(VULKAN_DEVICE, &allocInfo, 0, &VULKAN_SWAPCHAIN_DEPTH_IMAGES_MEMORY[i]);

        VkBindImageMemoryInfo bindInfo = {
            .sType = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO,
            .pNext = 0,
            .image = VULKAN_SWAPCHAIN_DEPTH_IMAGES[i],
            .memory = VULKAN_SWAPCHAIN_DEPTH_IMAGES_MEMORY[i],
            .memoryOffset = 0,
        };

        bindInfos[i] = bindInfo;
    }
        

    vkBindImageMemory2(VULKAN_DEVICE, VULKAN_SWAPCHAIN_IMAGE_COUNT, bindInfos);
}

void vulkan_swapchain_fence_init(void){
    VULKAN_SWAPCHAIN_DB_FENCES = malloc(sizeof(VkFence) * 3);

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
        printf("can't create swapchain fence 1.\n");
    }

    if(vkCreateFence(VULKAN_DEVICE, &fenceInfo2, 0, &VULKAN_SWAPCHAIN_DB_FENCES[1]) != VK_SUCCESS){
        printf("can't create swapchain fence 2.\n");
    }

    if(vkCreateFence(VULKAN_DEVICE, &fenceInfo, 0, &VULKAN_SWAPCHAIN_DB_FENCES[2]) != VK_SUCCESS){
        printf("can't create swapchain fence 3.\n");
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
        //.presentMode = VK_PRESENT_MODE_FIFO_KHR,
        .presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR,
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
    VULKAN_SWAPCHAIN_DEPTH_IMAGES = malloc(sizeof(VkImage) * VULKAN_SWAPCHAIN_IMAGE_COUNT);
    VULKAN_SWAPCHAIN_DEPTH_IMAGES_MEMORY = malloc(sizeof(VkDeviceMemory) * VULKAN_SWAPCHAIN_IMAGE_COUNT);
    vkGetSwapchainImagesKHR(VULKAN_DEVICE, VULKAN_SWAPCHAIN, &VULKAN_SWAPCHAIN_IMAGE_COUNT, VULKAN_SWAPCHAIN_IMAGES);
    vulkan_swapchain_depth_images_init();
    vulkan_swapchain_imageviews_init();
    vulkan_swapchain_framebuffers_init();
    vulkan_swapchain_fence_init();
}

void vulkan_swapchain_cleanup(void){
    for(int i = 0; i < VULKAN_SWAPCHAIN_IMAGE_COUNT; i++)
        vkDestroyFramebuffer(VULKAN_DEVICE, VULKAN_FRAMEBUFFERS[i], 0);
    for(int i = 0; i < VULKAN_SWAPCHAIN_IMAGE_COUNT * 2; i++)
        vkDestroyImageView(VULKAN_DEVICE, VULKAN_FRAMEBUFFERS_IMAGE_VIEWS[i], 0);
    vkDestroySwapchainKHR(VULKAN_DEVICE, VULKAN_SWAPCHAIN, 0);
}