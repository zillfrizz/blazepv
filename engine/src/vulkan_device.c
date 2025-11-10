#include "vulkan_device.h"
#include "vulkan_instance.h"
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>

VkPhysicalDevice VULKAN_PHYSICAL_DEVICE_HANDLE;
VkPhysicalDeviceProperties2 VULKAN_PHYSICAL_DEVICE_PROPERTIES;
VkDevice VULKAN_DEVICE_HANDLE;
VkQueue VULKAN_QUEUE_GRAPHICS;
VkQueue VULKAN_QUEUE_TRANSFER;

const char* deviceTypeToString(const VkPhysicalDeviceType* type) {
    switch(*type) {
        case VK_PHYSICAL_DEVICE_TYPE_OTHER: return "Other";
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: return "Integrated GPU";
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: return "Discrete GPU";
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: return "Virtual GPU";
        case VK_PHYSICAL_DEVICE_TYPE_CPU: return "CPU";
        default: return "Unknown";
    }
}

uint32_t vulkan_device_verify_ext(const VkPhysicalDevice* device, const char* const* someExtProps, const uint32_t someExtCount){
    uint32_t propertyCount;
    vkEnumerateDeviceExtensionProperties(device, 0, &propertyCount, 0);
    VkExtensionProperties* extProps = malloc(sizeof(VkExtensionProperties) * propertyCount);
    vkEnumerateDeviceExtensionProperties(device, 0, &propertyCount, extProps);
    for(int i = 0; i < propertyCount; i++){
        uint32_t found = 0;
        for(int j = 0; j < someExtCount; j++){
            if(strcmp(someExtProps[j], extProps[i].extensionName)){
                found = 1;
            }
        }
        if(!found) return 0;
    }
    return 1;
}

void vulkan_device_init(const char* const* extProps, const uint32_t extCount){
    uint32_t deviceCount;
    vkEnumeratePhysicalDevices(VULKAN_INSTANCE_HANDLE, &deviceCount, 0);
    VkPhysicalDevice* devices = malloc(sizeof(VkPhysicalDevice) * deviceCount);
    vkEnumeratePhysicalDevices(VULKAN_INSTANCE_HANDLE, &deviceCount, devices);

    printf("Choose one physical device by his ID: \n");
    for(int i = 0; i < deviceCount; i++){
        if(vulkan_device_verify_ext(&devices[i], extProps, extCount)){
             VkPhysicalDeviceProperties2* props = malloc(sizeof(VkPhysicalDeviceProperties2));
            vkGetPhysicalDeviceProperties2(VULKAN_INSTANCE_HANDLE, &props);
            printf("\t%d -  %s : APIversion: %s, DRIVERversion: %s, type: %s\n", 
            i, props->properties.deviceName, props->properties.apiVersion, props->properties.driverVersion, deviceTypeToString(&props->properties.deviceType));
        }
       
    }
    uint32_t deviceId;
    scanf("%" SCNu32, &deviceId);

    uint32_t queueCount;
    vkGetPhysicalDeviceQueueFamilyProperties2(devices[deviceId], &queueCount, 0);
    VkQueueFamilyProperties2* queueProps = malloc(sizeof(VkQueueFamilyProperties2) * queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties2(devices[deviceId], &queueCount, queueProps);

    uint32_t graphicId = 0, transferId = 0; 
    float graphicPriorities[] = {1.0f}, transferPriorities[] = {1.0f};

    for(int i = 0; i < queueCount; i++){
        if(queueProps[queueCount].queueFamilyProperties.queueCount >= 1 & queueProps[queueCount].queueFamilyProperties.queueFlags == VK_QUEUE_GRAPHICS_BIT)
            graphicId = i;
        if(queueProps[queueCount].queueFamilyProperties.queueCount >= 1 & queueProps[queueCount].queueFamilyProperties.queueFlags == VK_QUEUE_TRANSFER_BIT)
            transferId = i;
    }

    assert(graphicId && transferId);

    VkDeviceQueueCreateInfo queueInfos[] = {
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueCount = 1,
            .queueFamilyIndex = graphicId,
            .pQueuePriorities = graphicPriorities,
            .flags = 0,
            .pNext = 0
        },
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueCount = 1,
            .queueFamilyIndex = transferId,
            .pQueuePriorities = transferPriorities,
            .flags = 0,
            .pNext = 0
        }
    };

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(VULKAN_PHYSICAL_DEVICE_HANDLE, &supportedFeatures);

    VkPhysicalDeviceFeatures enabledFeatures = {
    };

    VkDeviceCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .enabledExtensionCount = extCount,
        .ppEnabledExtensionNames = extProps,
        .pEnabledFeatures = &enabledFeatures,
        .queueCreateInfoCount = 2,
        .pQueueCreateInfos = queueInfos,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = 0,
        .flags = 0,
        .pNext = 0
    };
    
    if (vkCreateDevice(devices[deviceId], &info, 0, VULKAN_PHYSICAL_DEVICE_HANDLE) != VK_SUCCESS){
        printf("device creation failed.\n");
    }

    vkGetDeviceQueue(VULKAN_DEVICE_HANDLE, graphicId, 0, &VULKAN_QUEUE_GRAPHICS);
    vkGetDeviceQueue(VULKAN_DEVICE_HANDLE, transferId, 0, &VULKAN_QUEUE_TRANSFER);
}

void vulkan_device_cleanup(void){
    vkDeviceWaitIdle(VULKAN_DEVICE_HANDLE);
    vkDestroyDevice(VULKAN_DEVICE_HANDLE, 0);
}

void vulkan_device_show_extensions(void){

}