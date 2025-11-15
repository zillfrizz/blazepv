#include <vulkan_renderpass.h>
#include <vulkan_device.h>
#include <windows1.h>
#include <stdio.h>
#include <stdlib.h>

VkRenderPass VULKAN_RENDERPASS;

void vulkan_renderpass_init(void){
    VkAttachmentDescription2 attachInfos = {
        .sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        .format = WINDOW_VULKAN_SURFACE_FORMAT.surfaceFormat.format,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .flags = 0,
        .pNext = 0
    };

    VkAttachmentReference2 references = {
        .sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .attachment = 0,
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .pNext = 0
    };

    VkSubpassDescription2 subpass = {
        .sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2,
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &references,
        .pResolveAttachments = 0,
        .pDepthStencilAttachment = 0,
        .inputAttachmentCount = 0,
        .pInputAttachments = 0,
        .viewMask = 0,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = 0,
        .flags = 0,
        .pNext = 0
    };
    
    VkRenderPassCreateInfo2 renderInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2,
        .attachmentCount = 1,
        .pAttachments = &attachInfos,
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 0,
        .pDependencies = 0,
        .correlatedViewMaskCount = 0,
        .pCorrelatedViewMasks = 0,
        .flags = 0,
        .pNext = 0
    };

    if(vkCreateRenderPass2(VULKAN_DEVICE, &renderInfo, 0, &VULKAN_RENDERPASS) != VK_SUCCESS){
        printf("can't create renderpass.\n");
    }

}

void vulkan_renderpass_cleanup(void){
    vkDestroyRenderPass(VULKAN_DEVICE, VULKAN_RENDERPASS, 0);
}