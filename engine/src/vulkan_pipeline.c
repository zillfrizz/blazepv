#include <vulkan_pipeline.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <vulkan_device.h>
#include <vulkan_swapchain.h>
#include <vulkan_renderpass.h>
#include <windows1.h>
#include <camera.h>
#include <file.h>

// PIPELINE
VkPipeline VULKAN_PIPELINE;
VkPipelineLayout VULKAN_PIPELINE_LAYOUT;
VkDescriptorPool VULKAN_PIPELINE_DESCRIPTOR_POOL;
VkDescriptorSetLayout VULKAN_PIPELINE_DESCRIPTOR_SET_LAYOUT;
VkDescriptorSet* VULKAN_PIPELINE_DESCRIPTOR_SETS;
VkPipelineCache VULKAN_PIPELINE_CACHE;

// SHADERS
VkShaderModule VULKAN_SHADER_VERTEX;
VkShaderModule VULKAN_SHADER_FRAGMENT;

// BUFFERS
VkBuffer VULKAN_BUFFER_VERTEX;
VkDeviceMemory VULKAN_BUFFER_VERTEX_MEMORY;


VkShaderModule create_shader_module(const char* cachePath, const char* srcPath) {
    uint32_t cacheSize;
    void* buffer = file_valid_check_fetch(cachePath, srcPath, &cacheSize);
    if (!buffer) {
        printf("commanD: glslangValidator -V %s -o %s\n", srcPath, cachePath);
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
    void* cacheBuffer = file_valid_check_fetch(path, src, &cacheSize);

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

VkDescriptorPool create_pipeline_descriptor_pool(void){
     VkDescriptorPoolSize descPoolSize = {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 3
    };

    VkDescriptorPoolCreateInfo descPoolInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .flags = 0,
        .pNext = 0,
        .maxSets = VULKAN_SWAPCHAIN_IMAGE_COUNT,
        .poolSizeCount = 1,
        .pPoolSizes = &descPoolSize  
    };

    VkDescriptorPool pool;
    vkCreateDescriptorPool(VULKAN_DEVICE, &descPoolInfo, 0, &pool);
    return pool;
}

VkDescriptorSetLayout create_pipeline_descriptor_set_layout(void){
    VkDescriptorSetLayoutBinding descSetLayoutBind = {
        .binding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .pImmutableSamplers = 0
    };

    VkDescriptorSetLayoutCreateInfo descLayoutInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .flags = 0,
        .pNext = 0,
        .bindingCount = 1,
        .pBindings = &descSetLayoutBind
    };

    VkDescriptorSetLayout layout;
    vkCreateDescriptorSetLayout(VULKAN_DEVICE, &descLayoutInfo, 0, &layout);
    return layout;
}

VkDescriptorSet* create_pipeline_descriptor_sets(VkDescriptorPool descPool,
     uint32_t setCount, VkDescriptorSetLayout* layouts){
    VkDescriptorSetAllocateInfo descSetAllocInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = 0,
        .descriptorPool = descPool,
        .descriptorSetCount = setCount,
        .pSetLayouts = &layouts,
    };

    VkDescriptorSet* sets = malloc(sizeof(VkDescriptorSet) * setCount);
    vkAllocateDescriptorSets(VULKAN_DEVICE, &descSetAllocInfo, sets);
    return sets;
}

void pipeline_init_descriptors(void){
    for(int i = 0; i < VULKAN_SWAPCHAIN_IMAGE_COUNT; i++){
        VkDescriptorBufferInfo bufferInfo = {
        .buffer = VULKAN_MATRIX_VIEW_BUFFERS[i],
        .offset = 0,
        .range = sizeof(UniformBufferObject)
    };

    VkWriteDescriptorSet descriptorWrite = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = descriptorSet,
        .dstBinding = 0,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = &bufferInfo
    };

    vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, NULL);
    }
}

VkPipelineLayout create_pipeline_layout(void) {



    VkPipelineLayoutCreateInfo layoutInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = 0,
        .setLayoutCount = 1,
        .pSetLayouts = &VULKAN_PIPELINE_DESCRIPTOR_SET_LAYOUT,
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
        .binding = 0, .stride = sizeof(VertexData), .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };

    VkVertexInputAttributeDescription attrDesc[] = {
        {.binding = 0, .location = 0, .format = VK_FORMAT_R32G32_SFLOAT, .offset = 0 },
        {.binding = 0, .location = 1, .format = VK_FORMAT_R32_UINT, .offset = sizeof(float) * 3}
    };

    VkPipelineVertexInputStateCreateInfo vertInputInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &bindDesc,
        .vertexAttributeDescriptionCount = 2,
        .pVertexAttributeDescriptions = attrDesc
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
        //.polygonMode = VK_POLYGON_MODE_FILL,
        .polygonMode = VK_POLYGON_MODE_LINE,
        //.cullMode = VK_CULL_MODE_BACK_BIT,
        .cullMode = VK_CULL_MODE_NONE,
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
        .size = sizeof(VertexData) * 36,
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

    vkMapMemory(VULKAN_DEVICE, VULKAN_BUFFER_VERTEX_MEMORY, 0, sizeof(VertexData) * 36, 0, &map);
    memcpy(map, data, sizeof(VertexData) * 36);
    vkUnmapMemory(VULKAN_DEVICE, VULKAN_BUFFER_VERTEX_MEMORY);
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
    VULKAN_PIPELINE_DESCRIPTOR_POOL = create_pipeline_descriptor_pool();
    VULKAN_PIPELINE_DESCRIPTOR_SET_LAYOUT = create_pipeline_descriptor_set_layout();
    VkDescriptorSetLayout* layouts = malloc(sizeof(VkDescriptorSetLayout) * VULKAN_SWAPCHAIN_IMAGE_COUNT);
    for(int i = 0; i < VULKAN_SWAPCHAIN_IMAGE_COUNT; i++)
        layouts[i] = VULKAN_PIPELINE_DESCRIPTOR_SET_LAYOUT;
    VULKAN_PIPELINE_DESCRIPTOR_SETS = create_pipeline_descriptor_sets(VULKAN_PIPELINE_DESCRIPTOR_POOL,
        VULKAN_SWAPCHAIN_IMAGE_COUNT, layouts);
    free(layouts);
    VULKAN_PIPELINE_LAYOUT = create_pipeline_layout();
    VULKAN_PIPELINE = create_graphics_pipeline(VULKAN_SHADER_VERTEX, VULKAN_SHADER_FRAGMENT, VULKAN_PIPELINE_LAYOUT, VULKAN_PIPELINE_CACHE);
    create_vertex_buffer(&VULKAN_BUFFER_VERTEX, &VULKAN_BUFFER_VERTEX_MEMORY);
    VertexData data[] = {
        // Face avant (Z = +0.5)
        {{-0.5f, -0.5f,  0.5f}, 0},
        {{ 0.5f, -0.5f,  0.5f}, 0},
        {{ 0.5f,  0.5f,  0.5f}, 0},

        {{-0.5f, -0.5f,  0.5f}, 0},
        {{ 0.5f,  0.5f,  0.5f}, 0},
        {{-0.5f,  0.5f,  0.5f}, 0},

        // Face arrière (Z = -0.5)
        {{-0.5f, -0.5f, -0.5f}, 1},
        {{ 0.5f,  0.5f, -0.5f}, 1},
        {{ 0.5f, -0.5f, -0.5f}, 1},

        {{-0.5f, -0.5f, -0.5f}, 1},
        {{-0.5f,  0.5f, -0.5f}, 1},
        {{ 0.5f,  0.5f, -0.5f}, 1},

        // Face gauche (X = -0.5)
        {{-0.5f, -0.5f, -0.5f}, 2},
        {{-0.5f, -0.5f,  0.5f}, 2},
        {{-0.5f,  0.5f,  0.5f}, 2},

        {{-0.5f, -0.5f, -0.5f}, 2},
        {{-0.5f,  0.5f,  0.5f}, 2},
        {{-0.5f,  0.5f, -0.5f}, 2},

        // Face droite (X = +0.5)
        {{ 0.5f, -0.5f, -0.5f}, 3},
        {{ 0.5f,  0.5f,  0.5f}, 3},
        {{ 0.5f, -0.5f,  0.5f}, 3},

        {{ 0.5f, -0.5f, -0.5f}, 3},
        {{ 0.5f,  0.5f, -0.5f}, 3},
        {{ 0.5f,  0.5f,  0.5f}, 3},

        // Face haut (Y = +0.5)
        {{-0.5f,  0.5f, -0.5f}, 4},
        {{-0.5f,  0.5f,  0.5f}, 4},
        {{ 0.5f,  0.5f,  0.5f}, 4},

        {{-0.5f,  0.5f, -0.5f}, 4},
        {{ 0.5f,  0.5f,  0.5f}, 4},
        {{ 0.5f,  0.5f, -0.5f}, 4},

        // Face bas (Y = -0.5)
        {{-0.5f, -0.5f, -0.5f}, 5},
        {{ 0.5f, -0.5f,  0.5f}, 5},
        {{-0.5f, -0.5f,  0.5f}, 5},

        {{-0.5f, -0.5f, -0.5f}, 5},
        {{ 0.5f, -0.5f, -0.5f}, 5},
        {{ 0.5f, -0.5f,  0.5f}, 5}
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
