#include <vulkan_instance.h>
#include <stdio.h>
#include <stdlib.h>
#include "tab.h"
#include "string.h"
#include "vulkan_device.h"

// Définition des méthodes ici

VkInstance VULKAN_INSTANCE_HANDLE;

void vulkan_instance_init(void){
    volkInitialize();

    #ifndef NDEBUG
    vulkan_instance_print_layers();
    vulkan_instance_print_extensions();
    #endif

    uint32_t apiVersion;
    vkEnumerateInstanceVersion(&apiVersion);

    VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .apiVersion = apiVersion,
        .pEngineName = "BlazeEngine",
        .engineVersion = VK_MAKE_VERSION(1,0,0),
        .pApplicationName = "Blaze",
        .applicationVersion = VK_MAKE_VERSION(0,0,0),
        .pNext = 0
    };

    const char* validationLayers[] = {
        "VK_LAYER_KHRONOS_validation"
    };

    const char* extensions[] = {
        "VK_KHR_surface",
        "VK_KHR_win32_surface"
    };

    VkInstanceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = 1,
        .ppEnabledLayerNames = validationLayers,
        .enabledExtensionCount = 2,
        .ppEnabledExtensionNames = extensions,
        .flags = 0,
        .pNext = 0
    };

    vkCreateInstance(&createInfo, 0, &VULKAN_INSTANCE_HANDLE);
    volkLoadInstance(VULKAN_INSTANCE_HANDLE);
}

void vulkan_instance_cleanup(void){
    vulkan_device_cleanup();
    vkDestroyInstance(VULKAN_INSTANCE_HANDLE, 0);
}

void vulkan_instance_print_layers(void){
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, 0);
    VkLayerProperties* layers = malloc(sizeof(VkLayerProperties) * layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, layers);

    for (int i = 0; i < layerCount; i++) { 
        printf("Layer: %s, Description: %s, Extensions: \n", layers[i].layerName, layers[i].description);
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(layers[i].layerName, &extensionCount, 0);
        VkExtensionProperties* extensions = malloc(sizeof(VkExtensionProperties) * extensionCount);
        vkEnumerateInstanceExtensionProperties(layers[i].layerName, &extensionCount, extensions);

        for(int j = 0; j < extensionCount; j++) { 
            printf("\tExtension: %s, Version: %d\n", extensions[j].extensionName, extensions[j].specVersion);
        }

        printf("\n");
    }
}

void vulkan_instance_print_extensions(void){
    uint32_t extensionCount;
    vkEnumerateInstanceExtensionProperties(0, &extensionCount, 0);
    VkExtensionProperties* extensions = malloc(sizeof(VkExtensionProperties) * extensionCount);
    vkEnumerateInstanceExtensionProperties(0, &extensionCount, extensions);

    for(int i = 0; i < extensionCount; i++){
        printf("Extension: %s, Version: %d\n", extensions[i].extensionName, extensions[i].specVersion);
    }
}
