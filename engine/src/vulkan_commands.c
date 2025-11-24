#include <vulkan_commands.h>
#include <vulkan_device.h>
#include <vulkan_renderpass.h>
#include <vulkan_swapchain.h>
#include <vulkan_pipeline.h>
#include <windows1.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <camera.h>

/*
    BUFFERS
*/
VkCommandPool VULKAN_COMMAND_POOL_GRAPHICS;
VkCommandPool VULKAN_COMMAND_POOL_TRANSFER;

#define VULKAN_COMMAND_BUFFERS_FRAME_GRAPHICS_COUNT 1
VkCommandBuffer* VULKAN_COMMAND_BUFFERS_FRAMERATE_GRAPHICS;
#define VULKAN_COMMAND_BUFFERS_FRAME_TRANSFER_COUNT 2
VkCommandBuffer* VULKAN_COMMAND_BUFFERS_FRAMERATE_TRANSFER;

/*
    LOCAL
*/

// ALLOCATION

VkCommandPool vulkan_commands_create_pool(VkDevice device, uint32_t familyIndex) {
    VkCommandPool pool;

    VkCommandPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .queueFamilyIndex = familyIndex,
        .flags = 0,
        .pNext = 0
    };

    if (vkCreateCommandPool(device, &poolInfo, 0, &pool) != VK_SUCCESS) {
        printf("Failed to create command pool.\n");
        return VK_NULL_HANDLE;
    }

    return pool;
}

VkCommandBuffer* vulkan_commands_allocate_buffers(VkDevice device, VkCommandPool pool, uint32_t count) {
    VkCommandBuffer* buffers = malloc(sizeof(VkCommandBuffer) * count);

    VkCommandBufferAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = count,
        .pNext = 0
    };

    if (vkAllocateCommandBuffers(device, &allocInfo, buffers) != VK_SUCCESS) {
        printf("Failed to allocate command buffers.\n");
        free(buffers);
        return 0;
    }

    return buffers;
}

// ON CMD BEGIN

void vulkan_commands_begin_buffer(VkCommandBuffer buffer) {
    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = 0,
        .pInheritanceInfo = 0,
        .pNext = 0
    };

    vkBeginCommandBuffer(buffer, &beginInfo);
}

void vulkan_commands_begin_renderpass(VkCommandBuffer buffer, VkRenderPass renderPass, VkFramebuffer framebuffer, VkExtent2D extent) {
    VkClearValue colorClearValue = { 
        .color = { {0.0f, 0.0f, 0.0f, 1.0f} }
    };

    VkClearValue depthClearValue = { 
        .depthStencil.depth = 1.0f,
        .depthStencil.stencil = 0
    };

    VkClearValue clearValues[] = {colorClearValue, depthClearValue};

    VkRenderPassBeginInfo renderInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = renderPass,
        .framebuffer = framebuffer,
        .renderArea.offset = {0, 0},
        .renderArea.extent = extent,
        .clearValueCount = 2,
        .pClearValues = clearValues,
        .pNext = 0
    };

    vkCmdBeginRenderPass(buffer, &renderInfo, VK_SUBPASS_CONTENTS_INLINE);
}

// CMD RECORD

void vulkan_commands_graphics_record(uint32_t frameId) {
    VkCommandBuffer cmd = VULKAN_COMMAND_BUFFERS_FRAMERATE_GRAPHICS[frameId];
    vulkan_commands_begin_buffer(cmd);

        VkBufferMemoryBarrier acquireBarrier = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
            .pNext = NULL,
            .srcAccessMask = 0,
            .dstAccessMask = VK_ACCESS_UNIFORM_READ_BIT, 
            .srcQueueFamilyIndex = VULKAN_FAMILY_TRANSFER,
            .dstQueueFamilyIndex = VULKAN_FAMILY_GRAPHICS,
            .buffer = VULKAN_MATRIX_VIEW_BUFFERS[frameId],
            .offset = 0,
            .size = VK_WHOLE_SIZE
        };

        vkCmdPipelineBarrier(
            cmd,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, 
            0, 
            0, NULL,
            1, &acquireBarrier, 
            0, NULL
        );

        vulkan_commands_begin_renderpass(cmd, VULKAN_RENDERPASS, VULKAN_FRAMEBUFFERS[frameId], VULKAN_SURFACE_EXTENT);
            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, VULKAN_PIPELINE);
            vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, VULKAN_PIPELINE_LAYOUT, 
                0, 1, &VULKAN_PIPELINE_DESCRIPTOR_SETS[frameId], 0, 0);
            VkDeviceSize size = sizeof(VertexData) * 36;
            VkDeviceSize offset = 0;
            vkCmdBindVertexBuffers2(cmd, 0, 1, &VULKAN_BUFFERS_VERTEX[frameId], &offset, &size, 0);
            vkCmdDraw(cmd, 36, 1, 0, 0);
     vkCmdEndRenderPass(cmd);
    if (vkEndCommandBuffer(cmd) != VK_SUCCESS) {
        printf("Failed to end command buffer.\n");
    }
}

void vulkan_commands_staging_copy_record(VkCommandBuffer* cmd, VkBuffer* srcBuffer, VkBuffer* dstBuffer, uint32_t srcOffset, uint32_t dstOffset, uint32_t size){
     VkBufferCopy copyInfo = {
        .srcOffset = srcOffset,
        .dstOffset = dstOffset,
        .size = size
    };

    vulkan_commands_begin_buffer(cmd);
        vkCmdCopyBuffer(cmd, *srcBuffer, *dstBuffer, 1, &copyInfo);

        VkBufferMemoryBarrier releaseBarrier = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
            .pNext = 0,
            .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
            .dstAccessMask = 0,
            .srcQueueFamilyIndex = VULKAN_FAMILY_TRANSFER,
            .dstQueueFamilyIndex = VULKAN_FAMILY_GRAPHICS,
            .buffer = *dstBuffer,
            .offset = dstOffset,
            .size = size
        };

        vkCmdPipelineBarrier(
            cmd,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 
            0, 
            0, 0, 
            1, &releaseBarrier, 
            0, 0 
        );

    vkEndCommandBuffer(cmd);
}

void vulkan_commands_graphics_records(){
    for(int i = 0; i < VULKAN_SWAPCHAIN_IMAGE_COUNT; i++){
        vulkan_commands_graphics_record(i);
    }
}

void vulkan_commands_transfer_records(){
    for(int i = 0; i < VULKAN_SWAPCHAIN_IMAGE_COUNT; i++)
        vulkan_commands_staging_copy_record(VULKAN_COMMAND_BUFFERS_TRANSFER[i], &VULKAN_MATRIX_VIEW_BUFFER_STAGING, &VULKAN_MATRIX_VIEW_BUFFERS[i],
            0, 0, 64
        );
        vulkan_commands_staging_copy_record(VULKAN_COMMAND_BUFFERS_TRANSFER[VULKAN_SWAPCHAIN_IMAGE_COUNT], &VULKAN_BUFFER_VERTEX_STAGING, &VULKAN_BUFFER_VERTEX,
            0, 0, sizeof(VertexData) * 36
        );
        vulkan_commands_staging_copy_record(VULKAN_COMMAND_BUFFERS_TRANSFER[VULKAN_SWAPCHAIN_IMAGE_COUNT + 1], &VULKAN_BUFFER_INSTANCE_STAGING, &VULKAN_BUFFER_INSTANCE,
            0, 0, sizeof(InstanceData) * 3
        );
}

// PUBLIC INIT

void vulkan_commands_transfer_view_matrix_init(uint32_t bufferId, VkFence signaledFence){
    VkCommandBufferSubmitInfo cmdSubmitInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
        .pNext = 0,
        .commandBuffer = VULKAN_COMMAND_BUFFERS_TRANSFER[bufferId],
        .deviceMask = 0x1
    };

    VkSubmitInfo2 submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
        .pNext = 0,
        .flags = 0,
        .commandBufferInfoCount = 1,
        .pCommandBufferInfos = &cmdSubmitInfo,
        .waitSemaphoreInfoCount = 0,
        .pWaitSemaphoreInfos = 0,
        .signalSemaphoreInfoCount = 0,
        .pSignalSemaphoreInfos = 0
    };

    vkQueueSubmit2(VULKAN_QUEUE_TRANSFER, 1, &submitInfo, signaledFence);
    vkWaitForFences(VULKAN_DEVICE, 1, &signaledFence, VK_TRUE, UINT64_MAX);
    vkResetFences(VULKAN_DEVICE, 1, &signaledFence);
}

void vulkan_commands_init(void) {
    VULKAN_COMMAND_POOL_GRAPHICS = vulkan_commands_create_pool(VULKAN_DEVICE, VULKAN_FAMILY_GRAPHICS);
    VULKAN_COMMAND_POOL_TRANSFER = vulkan_commands_create_pool(VULKAN_DEVICE, VULKAN_FAMILY_TRANSFER);

    if (VULKAN_COMMAND_POOL_GRAPHICS == VK_NULL_HANDLE || VULKAN_COMMAND_POOL_TRANSFER == VK_NULL_HANDLE) {
        printf("Command pools creation failed.\n");
        return;
    }

    VULKAN_COMMAND_BUFFERS_FRAMERATE_GRAPHICS = 
    vulkan_commands_allocate_buffers(VULKAN_DEVICE, VULKAN_COMMAND_POOL_GRAPHICS, VULKAN_SWAPCHAIN_IMAGE_COUNT * VULKAN_COMMAND_BUFFERS_FRAME_GRAPHICS_COUNT);

    VULKAN_COMMAND_BUFFERS_FRAMERATE_TRANSFER = 
    vulkan_commands_allocate_buffers(VULKAN_DEVICE, VULKAN_COMMAND_POOL_TRANSFER, VULKAN_SWAPCHAIN_IMAGE_COUNT * VULKAN_COMMAND_BUFFERS_FRAME_TRANSFER_COUNT);

    if (!VULKAN_COMMAND_BUFFERS_FRAMERATE_GRAPHICS || !VULKAN_COMMAND_BUFFERS_FRAMERATE_TRANSFER) {
        printf("Command buffer allocation failed.\n");
    }

    vulkan_commands_graphics_records();
    vulkan_commands_transfer_records();
}

// EXECUTE

void vulkan_commands_graphics_execute(){
     VkCommandBufferSubmitInfo bufferSubmitInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
        .commandBuffer = VULKAN_COMMAND_BUFFERS_GRAPHICS[VULKAN_SWAPCHAIN_IMAGE_INDEX],
        .deviceMask = 0x1,
        .pNext = 0
    };

    VkSubmitInfo2 submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
        .commandBufferInfoCount = 1,
        .pCommandBufferInfos = &bufferSubmitInfo,
        .signalSemaphoreInfoCount = 0,
        .pSignalSemaphoreInfos = 0,
        .waitSemaphoreInfoCount = 0,
        .pWaitSemaphoreInfos = 0,
        .flags = 0,
        .pNext = 0
    };

    vkQueueSubmit2(VULKAN_QUEUE_GRAPHICS, 1, &submitInfo, VULKAN_SWAPCHAIN_DB_FENCES[0]);

        VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 0,
        .pWaitSemaphores = NULL,
        .swapchainCount = 1,
        .pSwapchains = &VULKAN_SWAPCHAIN,
        .pImageIndices = &VULKAN_SWAPCHAIN_IMAGE_INDEX,
        .pResults = NULL
    };

    vkQueuePresentKHR(VULKAN_QUEUE_GRAPHICS, &presentInfo);
}

void vulkan_commands_transfer_execute(uint32_t frameId){
    VkCommandBufferSubmitInfo cmdSubmitInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
        .pNext = 0,
        .commandBuffer = cmd,
        .deviceMask = 0x1
    };

    VkSubmitInfo2 submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
        .pNext = 0,
        .flags = 0,
        .commandBufferInfoCount = 1,
        .pCommandBufferInfos = &cmdSubmitInfo,
        .waitSemaphoreInfoCount = 0,
        .pWaitSemaphoreInfos = 0,
        .signalSemaphoreInfoCount = 0,
        .pSignalSemaphoreInfos = 0
    };

    vkQueueSubmit2(VULKAN_QUEUE_TRANSFER, 1, &submitInfo, VULKAN_SWAPCHAIN_DB_FENCES[2]);
}

void vulkan_commands_execute() {   
    vkAcquireNextImageKHR(VULKAN_DEVICE, VULKAN_SWAPCHAIN, UINT64_MAX, VK_NULL_HANDLE, VULKAN_SWAPCHAIN_DB_FENCES[1], &VULKAN_SWAPCHAIN_IMAGE_INDEX);
    vkWaitForFences(VULKAN_DEVICE, 3, VULKAN_SWAPCHAIN_DB_FENCES, VK_TRUE, UINT64_MAX);
    vkResetFences(VULKAN_DEVICE, 3, VULKAN_SWAPCHAIN_DB_FENCES);

    vulkan_commands_transfer_view_matrix_execute((VULKAN_SWAPCHAIN_IMAGE_INDEX + 1) % VULKAN_SWAPCHAIN_IMAGE_COUNT);
    vulkan_commands_graphics_execute();   
}

// CLEANUP

void vulkan_commands_cleanup(void) {
    vkFreeCommandBuffers(VULKAN_DEVICE, VULKAN_COMMAND_POOL_GRAPHICS, VULKAN_COMMAND_BUFFERS_GRAPHICS_COUNT, VULKAN_COMMAND_BUFFERS_GRAPHICS);
    vkDestroyCommandPool(VULKAN_DEVICE, VULKAN_COMMAND_POOL_GRAPHICS, 0);
    vkFreeCommandBuffers(VULKAN_DEVICE, VULKAN_COMMAND_POOL_TRANSFER, VULKAN_COMMAND_BUFFERS_TRANSFER_COUNT, VULKAN_COMMAND_BUFFERS_TRANSFER);
    vkDestroyCommandPool(VULKAN_DEVICE, VULKAN_COMMAND_POOL_TRANSFER, 0);
    free(VULKAN_COMMAND_BUFFERS_GRAPHICS);
    free(VULKAN_COMMAND_BUFFERS_TRANSFER);
    VULKAN_COMMAND_BUFFERS_GRAPHICS = 0;
    VULKAN_COMMAND_BUFFERS_TRANSFER = 0;
}

