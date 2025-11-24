#include <vulkan_pipeline.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <vulkan_device.h>
#include <vulkan_swapchain.h>
#include <vulkan_renderpass.h>
#include <windows1.h>
#include <camera.h>
#include <bz_file.h>
#include <config.h>

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
VkBuffer* VULKAN_BUFFERS_VERTEX;
VkDeviceMemory* VULKAN_BUFFERS_VERTEX_MEMORY;

VkBuffer VULKAN_BUFFER_VERTEX_STAGING;
VkDeviceMemory VULKAN_BUFFER_VERTEX_MEMORY_STAGING;

VkBuffer* VULKAN_BUFFERS_INSTANCE;
VkDeviceMemory* VULKAN_BUFFERS_INSTANCE_MEMORY;

VkBuffer VULKAN_BUFFER_INSTANCE_STAGING;
VkDeviceMemory VULKAN_BUFFER_INSTANCE_MEMORY_STAGING;

// COMPILE/LOAD A SHADER MODULE
VkShaderModule vulkan_pipeline_shader_module_load(const char* cachePath, const char* srcPath) {
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

// LOAD PIPELINE CACHE
VkPipelineCache vulkan_pipeline_cache_load(const char* path, const char* src) {
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

// CREATE A DESCRIPTOR POOL
VkDescriptorPool vulkan_pipeline_descriptor_pool_create(const uint32_t typeCount, const VkDescriptorType* types, const uint32_t* counts, const uint32_t maxSets){
    VkDescriptorPoolSize* sizes = malloc(sizeof(VkDescriptorPoolSize) * typeCount);
    for(int i = 0; i < typeCount; i++){
        sizes[i].type = types[i];
        sizes[i].descriptorCount = counts[i];
    }

    VkDescriptorPoolCreateInfo descPoolInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .flags = 0,
        .pNext = 0,
        .maxSets = maxSets,
        .poolSizeCount = typeCount,
        .pPoolSizes = sizes 
    };

    VkDescriptorPool pool;
    vkCreateDescriptorPool(VULKAN_DEVICE, &descPoolInfo, 0, &pool);
    return pool;
}

// CREATE A DESCRIPTOR SET LAYOUT
VkDescriptorSetLayout vulkan_pipeline_descriptor_set_layout_create(uint32_t bindCount, VkDescriptorSetLayoutBinding* binds){
    VkDescriptorSetLayoutCreateInfo descLayoutInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .flags = 0,
        .pNext = 0,
        .bindingCount = bindCount,
        .pBindings = binds
    };

    VkDescriptorSetLayout layout;
    vkCreateDescriptorSetLayout(VULKAN_DEVICE, &descLayoutInfo, 0, &layout);
    return layout;
}

// CREATE DESCRIPTOR SETS
VkDescriptorSet* vulkan_pipeline_descriptor_sets_create(VkDescriptorPool descPool, uint32_t setCount, VkDescriptorSetLayout* layouts){
    VkDescriptorSetAllocateInfo descSetAllocInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = 0,
        .descriptorPool = descPool,
        .descriptorSetCount = setCount,
        .pSetLayouts = layouts,
    };

    VkDescriptorSet* sets = malloc(sizeof(VkDescriptorSet) * setCount);
    vkAllocateDescriptorSets(VULKAN_DEVICE, &descSetAllocInfo, sets);
    return sets;
}

// WRITE ON DESCRIPTORS SETS
void vulkan_pipeline_descriptors_sets_write(uint32_t count, VkBuffer* srcBuffers, VkDeviceSize* srcOffsets, VkDeviceSize* srcRanges, 
    VkDescriptorSet* dstDescriptorSets, uint32_t* dstBindings, uint32_t* dstDescriptorCounts, VkDescriptorType* dstTypes){

    VkDescriptorBufferInfo* bufferInfos = malloc(sizeof(VkDescriptorBufferInfo) * count);
    VkWriteDescriptorSet* writeInfos = malloc(sizeof(VkWriteDescriptorSet) * count);

    for(int i = 0; i < VULKAN_SWAPCHAIN_IMAGE_COUNT; i++){
        VkDescriptorBufferInfo bufferInfo = {
            .buffer = srcBuffers[i],
            .offset = srcOffsets[i],
            .range = srcRanges[i]
        };
        bufferInfos[i] = bufferInfo;

        VkWriteDescriptorSet writeInfo = {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = dstDescriptorSets[i],
            .dstBinding = dstBindings[i],
            .dstArrayElement = 0,
            .descriptorCount = dstDescriptorCounts[i],
            .descriptorType = dstTypes[i],
            .pBufferInfo = &bufferInfos[i]
        };
        writeInfos[i] = writeInfo;
    }

    vkUpdateDescriptorSets(VULKAN_DEVICE, count, &writeInfos, 0, 0);
}

// CREATE PIPELINE LAYOUT
VkPipelineLayout vulkan_pipeline_layout_create(uint32_t rangeCount, VkPushConstantRange* ranges, uint32_t layoutCount, VkDescriptorSetLayout* layouts) {

    VkPipelineLayoutCreateInfo layoutInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .flags = 0,
        .pNext = 0,
        .pushConstantRangeCount = rangeCount,
        .pPushConstantRanges = ranges,
        .setLayoutCount = layoutCount,
        .pSetLayouts = layouts
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

    VkVertexInputBindingDescription bindDesc[] = {
        {.binding = 0, .stride = sizeof(VertexData), .inputRate = VK_VERTEX_INPUT_RATE_VERTEX},
        {.binding = 1, .stride = sizeof(InstanceData), .inputRate = VK_VERTEX_INPUT_RATE_INSTANCE}
    };

    VkVertexInputAttributeDescription attrDesc[] = {
        // VERTEX
        {.binding = 0, .location = 0, .format = VK_FORMAT_R32G32B32_SFLOAT, .offset = 0 },
        {.binding = 0, .location = 1, .format = VK_FORMAT_R32_UINT, .offset = sizeof(float) * 3},
        // INSTANCE
        {.binding = 1, .location = 0, .format = VK_FORMAT_R32G32B32A32_SFLOAT, .offset = 0},
        {.binding = 1, .location = 1, .format = VK_FORMAT_R32G32B32A32_SFLOAT, .offset = sizeof(vec4)},
        {.binding = 1, .location = 2, .format = VK_FORMAT_R32G32B32A32_SFLOAT, .offset = sizeof(vec4) * 2},
        {.binding = 1, .location = 3, .format = VK_FORMAT_R32G32B32A32_SFLOAT, .offset = sizeof(vec4) * 3}
    };

    VkPipelineVertexInputStateCreateInfo vertInputInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .flags = 0,
        .pNext = 0,
        .vertexBindingDescriptionCount = 2,
        .pVertexBindingDescriptions = bindDesc,
        .vertexAttributeDescriptionCount = 6,
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
        .depthClampEnable = VK_FALSE,
        .depthBiasEnable = VK_FALSE,
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

    VkPipelineDepthStencilStateCreateInfo depthInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .flags = 0,
        .pNext = 0,
        .depthTestEnable = VK_TRUE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp = VK_COMPARE_OP_LESS,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable = VK_FALSE
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
        .pDepthStencilState = &depthInfo,
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

void create_vertex_buffer(VkBuffer* buffer, VkDeviceMemory* memory, uint32_t size) {
    VkBufferCreateInfo bufferInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .queueFamilyIndexCount = 2,
        .pQueueFamilyIndices = VULKAN_FAMILIES,
        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
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
        .memoryTypeIndex = vulkan_device_get_memory_type(&memReq, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
    };

    vkAllocateMemory(VULKAN_DEVICE, &allocInfo, 0, memory);

    VkBindBufferMemoryInfo bindInfo = {
        .sType = VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_INFO,
        .buffer = *buffer,
        .memory = *memory,
        .memoryOffset = 0,
        .pNext = 0
    };

    vkBindBufferMemory2(VULKAN_DEVICE, 1, &bindInfo);
}

void create_vertex_staging_buffer(VkBuffer* stagingBuffer, VkDeviceMemory* stagingMemory, uint32_t size){
    VkBufferCreateInfo bufferInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .flags = 0,
        .pNext = 0,
        .size = size,
        .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &VULKAN_FAMILIES[VULKAN_FAMILY_TRANSFER],
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };

    vkCreateBuffer(VULKAN_DEVICE, &bufferInfo, 0, stagingBuffer);

    VkBufferMemoryRequirementsInfo2 memReqInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2,
        .buffer = *stagingBuffer
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

    vkAllocateMemory(VULKAN_DEVICE, &allocInfo, 0, stagingMemory);

    VkBindBufferMemoryInfo bindInfo = {
        .sType = VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_INFO,
        .buffer = *stagingBuffer,
        .memory = *stagingMemory,
        .memoryOffset = 0,
        .pNext = 0
    };

    vkBindBufferMemory2(VULKAN_DEVICE, 1, &bindInfo);
}

void fill_vertex_buffer(void* data, uint32_t size, VkBuffer* stagingBuffer, VkDeviceMemory* stagingMemory, VkBuffer* vertexBuffer, VkDeviceMemory* vertexMemory) {
    void* map;

    vkMapMemory(VULKAN_DEVICE, *stagingMemory, 0, size * 36, 0, &map);
    memcpy(map, data, size * 36);
    vkUnmapMemory(VULKAN_DEVICE, *stagingMemory);


}

/*
    PUBLIC
*/ 

// 
void vulkan_vertex_buffer_transfer(VkBuffer buffer, uint32_t transferSize, uint32_t transferOffset, void* pTransferData){

}

// COMPILE/LOAD SHADERS
void vulkan_pipeline_shaders_init(void){
    VULKAN_SHADER_VERTEX = create_shader_module(ENGINE_DIR"/cache/shader.vert.spv", ENGINE_DIR"/assets/shaders/shader.vert");
    VULKAN_SHADER_FRAGMENT = create_shader_module(ENGINE_DIR"/cache/shader.frag.spv", ENGINE_DIR"/assets/shaders/shader.frag");
}

// LOAD CACHE
void vulkan_pipeline_cache_init(void){
    VULKAN_PIPELINE_CACHE = create_pipeline_cache(ENGINE_DIR"/src/vulkan_pipeline.c", ENGINE_DIR"/assets/pipelines/vulkan_pipeline.bin");
}

// CREATE PIPELINE DESCRIPTORS SETS LAYOUT, DESCRIPTOR POOL, DESCRIPTOR SETS & DESCRIPTOR SET LAYOUT
void vulkan_pipeline_description_init(void){
    // LAYOUTS
    VkDescriptorSetLayoutBinding descSetLayoutBind = {
        .binding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .pImmutableSamplers = 0
    };

    VULKAN_PIPELINE_DESCRIPTOR_SET_LAYOUT = vulkan_pipeline_descriptor_set_layout_create(1, &descSetLayoutBind);
    VkDescriptorSetLayout* layouts = malloc(sizeof(VkDescriptorSetLayout) * VULKAN_SWAPCHAIN_IMAGE_COUNT);
    for(int i = 0; i < VULKAN_SWAPCHAIN_IMAGE_COUNT; i++)
        layouts[i] = VULKAN_PIPELINE_DESCRIPTOR_SET_LAYOUT;
    
    // POOL
    VkDescriptorType types[] = {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER};
    uint32_t counts[] = {VULKAN_SWAPCHAIN_IMAGE_COUNT};
    VULKAN_PIPELINE_DESCRIPTOR_POOL = create_pipeline_descriptor_pool(1, types, counts, VULKAN_SWAPCHAIN_IMAGE_COUNT);

    // SETS

    VULKAN_PIPELINE_DESCRIPTOR_SETS = create_pipeline_descriptor_sets(VULKAN_PIPELINE_DESCRIPTOR_POOL, VULKAN_SWAPCHAIN_IMAGE_COUNT, layouts);
    free(layouts);

    // INITIALISATION WRITE ON SETS
    pipeline_init_descriptors();

    // PIPELINE LAYOUT
    VULKAN_PIPELINE_LAYOUT = create_pipeline_layout();
}

// CREATE AND FILL VERTEX BUFFERS WITH INITIAL VALUES
void vulkan_pipeline_vertex_buffers_init(void){
    create_vertex_staging_buffer(&VULKAN_BUFFER_VERTEX_MEMORY_STAGING, &VULKAN_BUFFER_VERTEX_MEMORY_STAGING, sizeof(VertexData) * 36);
    create_vertex_buffer(&VULKAN_BUFFERS_VERTEX, &VULKAN_BUFFERS_VERTEX_MEMORY, sizeof(VertexData) * 36);
    create_vertex_staging_buffer(&VULKAN_BUFFER_INSTANCE_STAGING, &VULKAN_BUFFER_INSTANCE_MEMORY_STAGING, sizeof(InstanceData) * 3);
    create_vertex_buffer(&VULKAN_BUFFERS_INSTANCE, &VULKAN_BUFFERS_INSTANCE_MEMORY, sizeof(InstanceData) * 3);

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

    InstanceData initial;
    glm_mat4_identity(initial.modelMat);

    InstanceData initials[] = {initial, initial, initial};

    for(int i = 0; i < VULKAN_SWAPCHAIN_IMAGE_COUNT; i++){
        fill_vertex_buffer(data, sizeof(VertexData) * 36, &VULKAN_BUFFER_VERTEX_STAGING, &VULKAN_BUFFER_VERTEX_MEMORY_STAGING,
            &VULKAN_BUFFERS_VERTEX[i], &VULKAN_BUFFERS_VERTEX_MEMORY[i]
        );
        fill_vertex_buffer(initials, sizeof(InstanceData) * 3, &VULKAN_BUFFER_INSTANCE_STAGING, &VULKAN_BUFFER_INSTANCE_MEMORY_STAGING,
            &VULKAN_BUFFERS_INSTANCE[i], &VULKAN_BUFFERS_INSTANCE_MEMORY[i]
        );
    }
}

// MAIN INIT FUNCTION (SHADERS -> CACHE -> DESCRIPTION -> VERTEX BUFFERS -> PIPELINE)
void vulkan_pipeline_init(void) {
    vulkan_pipeline_shaders_init();
    vulkan_pipeline_cache_init();
    vulkan_pipeline_description_init();
    vulkan_pipeline_vertex_buffers_init();
    VULKAN_PIPELINE = create_graphics_pipeline(VULKAN_SHADER_VERTEX, VULKAN_SHADER_FRAGMENT, VULKAN_PIPELINE_LAYOUT, VULKAN_PIPELINE_CACHE);
}

void vulkan_pipeline_cleanup(void) {
    //PIPELINE
    vkDestroyPipeline(VULKAN_DEVICE, VULKAN_PIPELINE, 0);

    // PIPELINE LAYOUT
    vkDestroyPipelineLayout(VULKAN_DEVICE, VULKAN_PIPELINE_LAYOUT, 0);

    // SHADER MODULES
    vkDestroyShaderModule(VULKAN_DEVICE, VULKAN_SHADER_VERTEX, 0);
    vkDestroyShaderModule(VULKAN_DEVICE, VULKAN_SHADER_FRAGMENT, 0);

    // DESCRIPTORS
    vkFreeDescriptorSets(VULKAN_DEVICE, VULKAN_PIPELINE_DESCRIPTOR_POOL, VULKAN_SWAPCHAIN_IMAGE_COUNT, VULKAN_PIPELINE_DESCRIPTOR_SETS);
    vkDestroyDescriptorPool(VULKAN_DEVICE, VULKAN_PIPELINE_DESCRIPTOR_POOL, 0);
    vkDestroyDescriptorSetLayout(VULKAN_DEVICE, VULKAN_PIPELINE_DESCRIPTOR_SET_LAYOUT, 0);

    // PIPELINE CACHE
    size_t dataSize = 0;
    vkGetPipelineCacheData(VULKAN_DEVICE, VULKAN_PIPELINE_CACHE, &dataSize, 0);
    void* data = malloc(dataSize);
    vkGetPipelineCacheData(VULKAN_DEVICE, VULKAN_PIPELINE_CACHE, &dataSize, data);
    file_push(ENGINE_DIR"/assets/pipelines/vulkan_pipeline.bin", data, dataSize);
    free(data);
    vkDestroyPipelineCache(VULKAN_DEVICE, VULKAN_PIPELINE_CACHE, 0);

    // INPUT BUFFERS
    vkDestroyBuffer(VULKAN_DEVICE, VULKAN_BUFFER_VERTEX_STAGING, 0);
    vkFreeMemory(VULKAN_DEVICE, VULKAN_BUFFER_VERTEX_MEMORY_STAGING, 0);
    vkDestroyBuffer(VULKAN_DEVICE, VULKAN_BUFFER_INSTANCE_STAGING, 0);
    vkFreeMemory(VULKAN_DEVICE, VULKAN_BUFFER_INSTANCE_MEMORY_STAGING, 0);

    for(int i = 0; i < VULKAN_SWAPCHAIN_IMAGE_COUNT; i++){
        vkDestroyBuffer(VULKAN_DEVICE, VULKAN_BUFFERS_VERTEX[i], 0);
        vkFreeMemory(VULKAN_DEVICE, VULKAN_BUFFERS_VERTEX_MEMORY[i], 0);
        vkDestroyBuffer(VULKAN_DEVICE, VULKAN_BUFFERS_INSTANCE[i], 0);
        vkFreeMemory(VULKAN_DEVICE, VULKAN_BUFFERS_INSTANCE_MEMORY[i], 0);
    }
    
}
