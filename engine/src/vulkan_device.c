#include "vulkan_device.h"
#include "vulkan_instance.h"
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

// DEVICE
VkPhysicalDevice VULKAN_PHYSICAL_DEVICE;
VkPhysicalDeviceProperties2 VULKAN_PHYSICAL_DEVICE_PROPERTIES;
VkDevice VULKAN_DEVICE;

// QUEUES
uint32_t VULKAN_FAMILIES[VULKAN_FAMILY_COUNT];

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

uint32_t vulkan_device_verify_ext(const VkPhysicalDevice device, const char* const* requiredExts, const uint32_t requiredCount) {
    uint32_t propertyCount;
    vkEnumerateDeviceExtensionProperties(device, NULL, &propertyCount, NULL);
    VkExtensionProperties* extProps = malloc(sizeof(VkExtensionProperties) * propertyCount);
    vkEnumerateDeviceExtensionProperties(device, NULL, &propertyCount, extProps);

    uint32_t foundAll = 1;
    for (uint32_t j = 0; j < requiredCount; j++) {
        uint32_t found = 0;
        for (uint32_t i = 0; i < propertyCount; i++) {
            if (strcmp(requiredExts[j], extProps[i].extensionName) == 0) {
                found = 1;
                break;
            }
        }
        if (!found) {
            foundAll = 0;
            break;
        }
    }

    free(extProps);
    return foundAll;
}

uint32_t vulkan_device_get_memory_type(VkMemoryRequirements2* memReq2, VkMemoryPropertyFlags memFlags){
    uint32_t typeMask = memReq2->memoryRequirements.memoryTypeBits;

    VkPhysicalDeviceMemoryProperties2 memProps2 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2,
        .pNext = 0
    };

    vkGetPhysicalDeviceMemoryProperties2(VULKAN_PHYSICAL_DEVICE, &memProps2);

    VkPhysicalDeviceMemoryProperties memProps = memProps2.memoryProperties;

    for(uint32_t i = 0; i < memProps.memoryTypeCount; i++){
        if(typeMask & (1 << i) && (memProps.memoryTypes[i].propertyFlags & memFlags) == memFlags){
            return i;
        }
    }
}

void vulkan_device_show_extensions(VkPhysicalDevice device){
    uint32_t propertyCount;
    vkEnumerateDeviceExtensionProperties(device, NULL, &propertyCount, NULL);
    VkExtensionProperties* extProps = malloc(sizeof(VkExtensionProperties) * propertyCount);
    vkEnumerateDeviceExtensionProperties(device, NULL, &propertyCount, extProps);
    printf("here's device extensions :\n");
    for(int i = 0; i < propertyCount; i++){
        printf("\tname: %s, version: %d\n", extProps[i].extensionName, extProps[i].specVersion);
    }
}

void vulkan_device_init(){

    #define HARD_EXTENSION_COUNT 1
    const char* extProps[] = {
        "VK_KHR_swapchain"
    };

    uint32_t deviceCount;
    vkEnumeratePhysicalDevices(VULKAN_INSTANCE, &deviceCount, 0);
    VkPhysicalDevice* devices = malloc(sizeof(VkPhysicalDevice) * deviceCount);
    vkEnumeratePhysicalDevices(VULKAN_INSTANCE, &deviceCount, devices);

    printf("Choose one physical device by his ID: \n");
    for(int i = 0; i < deviceCount; i++){
        if(vulkan_device_verify_ext(devices[i], extProps, HARD_EXTENSION_COUNT)){
            VkPhysicalDeviceProperties2 props = {
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2,
                .properties = 0,
                .pNext = 0
            };
            vkGetPhysicalDeviceProperties2(devices[i], &props);
            printf("\t%d -  %s : APIversion: %d, DRIVERversion: %d, type: %s\n", 
            i, props.properties.deviceName, props.properties.apiVersion, props.properties.driverVersion, deviceTypeToString(&(props.properties.deviceType)));
        }
       
    }
    uint32_t deviceId;
    scanf("%" SCNu32, &deviceId);

    VULKAN_PHYSICAL_DEVICE = devices[deviceId];

    vulkan_device_show_extensions(VULKAN_PHYSICAL_DEVICE);

    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties2(devices[deviceId], &queueFamilyCount, 0);

    VkQueueFamilyProperties2* queueProps = malloc(sizeof(VkQueueFamilyProperties2) * queueFamilyCount);

    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        queueProps[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
        queueProps[i].pNext = 0;
    }

    vkGetPhysicalDeviceQueueFamilyProperties2(devices[deviceId], &queueFamilyCount, queueProps);

    VULKAN_FAMILIES[VULKAN_FAMILY_GRAPHICS] = 999, VULKAN_FAMILIES[VULKAN_FAMILY_TRANSFER] = 999; 
    float graphicPriorities[] = {1.0f}, transferPriorities[] = {1.0f};

    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        VkQueueFlags flags = queueProps[i].queueFamilyProperties.queueFlags;
        uint32_t count = queueProps[i].queueFamilyProperties.queueCount;
        if ((flags & VK_QUEUE_GRAPHICS_BIT) &&VULKAN_FAMILIES[VULKAN_FAMILY_GRAPHICS]== 999 && count)
           VULKAN_FAMILIES[VULKAN_FAMILY_GRAPHICS]= i;

        if ((flags & VK_QUEUE_TRANSFER_BIT) && !(flags & VK_QUEUE_GRAPHICS_BIT) && count &&VULKAN_FAMILIES[VULKAN_FAMILY_TRANSFER]== 999)
           VULKAN_FAMILIES[VULKAN_FAMILY_TRANSFER]= i;
    }

    assert(VULKAN_FAMILY_GRAPHICS != 999 &&VULKAN_FAMILIES[VULKAN_FAMILY_TRANSFER]!= 999 &&VULKAN_FAMILIES[VULKAN_FAMILY_GRAPHICS]!= VULKAN_FAMILY_TRANSFER);

    VkDeviceQueueCreateInfo queueInfos[] = {
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueCount = 1,
            .queueFamilyIndex = VULKAN_FAMILY_GRAPHICS,
            .pQueuePriorities = graphicPriorities,
            .flags = 0,
            .pNext = 0
        },
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueCount = 1,
            .queueFamilyIndex = VULKAN_FAMILY_TRANSFER,
            .pQueuePriorities = transferPriorities,
            .flags = 0,
            .pNext = 0
        }
    };

    VkPhysicalDeviceSynchronization2Features syncFeatures = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
        .synchronization2 = VK_TRUE,
        .pNext = 0
    };

    VkPhysicalDeviceFeatures2 supportedFeatures = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = &syncFeatures
    };

    vkGetPhysicalDeviceFeatures2(VULKAN_PHYSICAL_DEVICE, &supportedFeatures);

    VkDeviceCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .enabledExtensionCount = HARD_EXTENSION_COUNT,
        .ppEnabledExtensionNames = extProps,
        .pEnabledFeatures = 0,
        .queueCreateInfoCount = 2,
        .pQueueCreateInfos = queueInfos,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = 0,
        .flags = 0,
        .pNext = &supportedFeatures
    };
    
    if (vkCreateDevice(VULKAN_PHYSICAL_DEVICE, &info, 0, &VULKAN_DEVICE) != VK_SUCCESS){
        printf("device creation failed. code:%d\n", vkCreateDevice(VULKAN_PHYSICAL_DEVICE, &info, 0, &VULKAN_DEVICE));
        exit(1);
    }

    vkGetDeviceQueue(VULKAN_DEVICE, VULKAN_FAMILY_GRAPHICS, 0, &VULKAN_QUEUE_GRAPHICS);
    vkGetDeviceQueue(VULKAN_DEVICE, VULKAN_FAMILY_TRANSFER, 0, &VULKAN_QUEUE_TRANSFER);


}

void vulkan_device_cleanup(void){
    vkDeviceWaitIdle(VULKAN_DEVICE);
    vkDestroyDevice(VULKAN_DEVICE, 0);
}
