#include <vulkan_pipeline.h>
#include <windows.h>
#include <stdlib.h>
#include <io.h>
#include <stdio.h>
#include <sys/stat.h>
#include <vulkan_device.h>
#include <vulkan_swapchain.h>
#include <vulkan_renderpass.h>
#include <windows1.h>

#define PIPELINE_SRC_PATH "D:\\Dev\\blazepv\\engine\\src\\vulkan_pipeline.c"
#define PIPELINE_PATH "D:\\Dev\\blazepv\\engine\\cache\\pipeline.bin"
#define SHADER_VERTEX_SRC_PATH "D:\\Dev\\blazepv\\engine\\assets\\shaders\\shader.vert"
#define SHADER_VERTEX_PATH "D:\\Dev\\blazepv\\engine\\cache\\shader.vert.spv"
#define SHADER_FRAGMENT_SRC_PATH "D:\\Dev\\blazepv\\engine\\assets\\shaders\\shader.frag"
#define SHADER_FRAGMENT_PATH "D:\\Dev\\blazepv\\engine\\cache\\shader.frag.spv"

// PIPELINE
VkPipeline VULKAN_PIPELINE;
VkPipelineLayout VULKAN_PIPELINE_LAYOUT;
VkPipelineCache VULKAN_PIPELINE_CACHE;

// SHADERS
VkShaderModule VULKAN_SHADER_VERTEX;
VkShaderModule VULKAN_SHADER_FRAGMENT;

// BUFFERS
VkBuffer VULKAN_BUFFER_VERTEX;
VkDeviceMemory VULKAN_BUFFER_VERTEX_MEMORY;


/* 
   UTILS
*/
void file_push(const char* filePath, void* data, uint32_t size) {
    FILE* f = fopen(filePath, "wb");
    fwrite(data, 1, size, f);
    fclose(f);
    free(data);
}

void* file_fetch(const char* filePath, uint32_t* aSize) {
    FILE* f = fopen(filePath, "rb");
    fseek(f, 0, SEEK_END);
    *aSize = ftell(f);
    rewind(f);
    void* buffer = malloc(*aSize);
    *aSize = fread(buffer, 1, *aSize, f);
    fclose(f);
    return buffer;
}

void* file_check(const char* filePath, const char* srcPath, uint32_t* aSize) {
    struct stat fileStats, sourceStats;
    if (stat(filePath, &fileStats) || stat(srcPath, &sourceStats)) return 0;
    if (fileStats.st_mtime < sourceStats.st_mtime) return 0;
    return file_fetch(filePath, aSize);
}

VkShaderModule create_shader_module(const char* cachePath, const char* srcPath) {
    uint32_t cacheSize;
    void* buffer = file_check(cachePath, srcPath, &cacheSize);
    if (!buffer) {
        printf("command: glslangValidator -V %s -o %s\n", srcPath, cachePath);
        char cmd[512];
        sprintf(cmd, "glslangValidator -V %s -o %s", srcPath, cachePath);
        system(cmd);
        buffer = file_fetch(cachePath, &cacheSize);
    }

    VkShaderModuleCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = cacheSize,
        .pCode = buffer,
    };

    VkShaderModule module;
    vkCreateShaderModule(VULKAN_DEVICE, &createInfo, 0, &module);
    free(buffer);
    return module;
}

VkPipelineCache create_pipeline_cache(const char* path, const char* src) {
    uint32_t cacheSize;
    void* cacheBuffer = file_check(path, src, &cacheSize);

    VkPipelineCacheCreateInfo cacheInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
        .initialDataSize = cacheBuffer ? cacheSize : 0,
        .pInitialData = cacheBuffer,
    };

    VkPipelineCache cache;
    vkCreatePipelineCache(VULKAN_DEVICE, &cacheInfo, 0, &cache);
    free(cacheBuffer);
    return cache;
}

VkPipelineLayout create_pipeline_layout(void) {

    VkPushConstantRange constantInfo = {
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .offset = 0,
        .size = 64
    };

    VkPipelineLayoutCreateInfo layoutInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges = &constantInfo,
        .setLayoutCount = 0,
        .pSetLayouts = 0,
        .flags = 0,
        .pNext = 0
    };

    VkPipelineLayout layout;
    vkCreatePipelineLayout(VULKAN_DEVICE, &layoutInfo, 0, &layout);
    return layout;
}

VkPipeline create_graphics_pipeline(VkShaderModule vert, VkShaderModule frag, VkPipelineLayout layout, VkPipelineCache cache) {
    /*
        SHADERS
    */
    VkPipelineShaderStageCreateInfo vertStage = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vert,
        .pName = "main"
    };

    VkPipelineShaderStageCreateInfo fragStage = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = frag,
        .pName = "main"
    };

    VkPipelineShaderStageCreateInfo stages[] = { vertStage, fragStage };

    /*
        VERTEX INPUT
    */

    VkVertexInputBindingDescription bindDesc = {
        .binding = 0, .stride = sizeof(float) * 2, .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    };

    VkVertexInputAttributeDescription attrDesc = {
        .binding = 0, .location = 0, .format = VK_FORMAT_R32G32_SFLOAT, .offset = 0
    };

    VkPipelineVertexInputStateCreateInfo vertInputInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &bindDesc,
        .vertexAttributeDescriptionCount = 1,
        .pVertexAttributeDescriptions = &attrDesc
    };

    VkPipelineInputAssemblyStateCreateInfo assemblyInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
    };

    /*
        VIEW
    */

    VkViewport viewport = {
        .x = 0.0f, .y = 0.0f,
        .width = VULKAN_SURFACE_EXTENT.width,
        .height = VULKAN_SURFACE_EXTENT.height,
        .minDepth = 0.0f, .maxDepth = 1.0f
    };

    VkRect2D scissor = { .offset = {0, 0}, .extent = VULKAN_SURFACE_EXTENT };

    VkPipelineViewportStateCreateInfo viewportInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1, .pViewports = &viewport,
        .scissorCount = 1, .pScissors = &scissor
    };

    /*
        RASTER
    */

    VkPipelineRasterizationStateCreateInfo rasterInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .lineWidth = 1.0f
    };

    VkPipelineMultisampleStateCreateInfo sampleInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT
    };

    VkPipelineColorBlendAttachmentState blendAttachment = {
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
    };

    VkPipelineColorBlendStateCreateInfo blendInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .attachmentCount = 1, .pAttachments = &blendAttachment
    };

    /*
        PIPELINE
    */

    VkGraphicsPipelineCreateInfo pipelineInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2,
        .pStages = stages,
        .pVertexInputState = &vertInputInfo,
        .pInputAssemblyState = &assemblyInfo,
        .pViewportState = &viewportInfo,
        .pRasterizationState = &rasterInfo,
        .pMultisampleState = &sampleInfo,
        .pColorBlendState = &blendInfo,
        .layout = layout,
        .renderPass = VULKAN_RENDERPASS,
        .subpass = 0
    };

    VkPipeline pipeline;
    if (vkCreateGraphicsPipelines(VULKAN_DEVICE, cache, 1, &pipelineInfo, 0, &pipeline) != VK_SUCCESS) {
        printf("Can't create graphics pipeline.\n");
    }

    return pipeline;
}

void create_vertex_buffer(VkBuffer* buffer, VkDeviceMemory* memory) {
    VkBufferCreateInfo bufferInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = sizeof(float) * 6,
        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };

    vkCreateBuffer(VULKAN_DEVICE, &bufferInfo, 0, buffer);

    VkBufferMemoryRequirementsInfo2 memReqInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2,
        .buffer = *buffer
    };

    VkMemoryRequirements2 memReq = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2
    };

    vkGetBufferMemoryRequirements2(VULKAN_DEVICE, &memReqInfo, &memReq);

    VkMemoryAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memReq.memoryRequirements.size,
        .memoryTypeIndex = vulkan_device_get_memory_type(&memReq, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
    };

    vkAllocateMemory(VULKAN_DEVICE, &allocInfo, 0, memory);

    VkBindBufferMemoryInfo bindInfo = {
        .sType = VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_INFO,
        .buffer = VULKAN_BUFFER_VERTEX,
        .memory = VULKAN_BUFFER_VERTEX_MEMORY,
        .memoryOffset = 0,
        .pNext = 0
    };

    vkBindBufferMemory2(VULKAN_DEVICE, 1, &bindInfo);
}

void fill_vertex_buffer(void* data) {
    void* map;

    VkMemoryMapInfo mapInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_MAP_INFO,
        .memory = VULKAN_BUFFER_VERTEX_MEMORY,
        .offset = 0,
        .size = VK_WHOLE_SIZE,
        .flags = 0,
        .pNext = 0
    };

    VkMemoryUnmapInfo unmapInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_UNMAP_INFO,
        .memory = VULKAN_BUFFER_VERTEX_MEMORY,
        .flags = 0,
        .pNext = 0
    };

    vkMapMemory2(VULKAN_DEVICE, &mapInfo, &map);
    memcpy(map, data, sizeof(float) * 6);
    vkUnmapMemory2(VULKAN_DEVICE, &unmapInfo);
}

/*
    PUBLIC
*/ 

void vulkan_vertex_buffer_transfer(VkBuffer buffer, uint32_t transferSize, uint32_t transferOffset, void* pTransferData){

}

void vulkan_pipeline_init(void) {
    VULKAN_SHADER_VERTEX = create_shader_module(SHADER_VERTEX_PATH, SHADER_VERTEX_SRC_PATH);
    VULKAN_SHADER_FRAGMENT = create_shader_module(SHADER_FRAGMENT_PATH, SHADER_FRAGMENT_SRC_PATH);
    VULKAN_PIPELINE_CACHE = create_pipeline_cache(PIPELINE_PATH, PIPELINE_SRC_PATH);
    VULKAN_PIPELINE_LAYOUT = create_pipeline_layout();
    VULKAN_PIPELINE = create_graphics_pipeline(VULKAN_SHADER_VERTEX, VULKAN_SHADER_FRAGMENT, VULKAN_PIPELINE_LAYOUT, VULKAN_PIPELINE_CACHE);
    create_vertex_buffer(&VULKAN_BUFFER_VERTEX, &VULKAN_BUFFER_VERTEX_MEMORY);
    float data[] = {
        0.0f, -1.0f,
        -1.0f, 1.0f,
        1.0f, 1.0f
    };
    fill_vertex_buffer(data);
}

void vulkan_pipeline_cleanup(void) {
    vkDestroyPipeline(VULKAN_DEVICE, VULKAN_PIPELINE, 0);
    vkDestroyPipelineLayout(VULKAN_DEVICE, VULKAN_PIPELINE_LAYOUT, 0);
    vkDestroyShaderModule(VULKAN_DEVICE, VULKAN_SHADER_VERTEX, 0);
    vkDestroyShaderModule(VULKAN_DEVICE, VULKAN_SHADER_FRAGMENT, 0);

    size_t dataSize = 0;
    vkGetPipelineCacheData(VULKAN_DEVICE, VULKAN_PIPELINE_CACHE, &dataSize, 0);
    void* data = malloc(dataSize);
    vkGetPipelineCacheData(VULKAN_DEVICE, VULKAN_PIPELINE_CACHE, &dataSize, data);
    file_push(PIPELINE_PATH, data, dataSize);

    vkDestroyPipelineCache(VULKAN_DEVICE, VULKAN_PIPELINE_CACHE, 0);
    vkDestroyBuffer(VULKAN_DEVICE, VULKAN_BUFFER_VERTEX, 0);
    vkFreeMemory(VULKAN_DEVICE, VULKAN_BUFFER_VERTEX_MEMORY, 0);
}
