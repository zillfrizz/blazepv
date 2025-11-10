#include "vulkan_device.h"
#include "vulkan_instance.h"
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

VkPhysicalDevice VULKAN_PHYSICAL_DEVICE_HANDLE;
VkPhysicalDeviceProperties2 VULKAN_PHYSICAL_DEVICE_PROPERTIES;
VkDevice VULKAN_DEVICE_HANDLE;

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

    VkDeviceCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .enabledExtensionCount = extCount,
        .ppEnabledExtensionNames = extProps,
        .pEnabledFeatures = 0,
        .queueCreateInfoCount = 0,
        .pQueueCreateInfos = 0,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = 0,
        .flags = 0,
        .pNext = 0
    };
    
    vkCreateDevice(devices[deviceId], );
}

void vulkan_device_cleanup(void){
}

void vulkan_device_show_extensions(void);