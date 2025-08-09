#include <iostream>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_beta.h>
#include "vulkan/vulkan_core.h"
#include "vulkan_base/vulkan_base.h"

VulkanContext* context;


void initApplication() {

    const char* instanceExtensions[] = {
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
        VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
    };
    uint32_t instanceExtensionsCount = ARRAY_COUNT(instanceExtensions);

    const char* deviceExtensions[] = {
        VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME,
    };
    uint32_t deviceExtensionsCount = ARRAY_COUNT(deviceExtensions);

    context = initVulkan(
        instanceExtensionsCount,
        instanceExtensions, 
        deviceExtensionsCount, 
        deviceExtensions
    );
}

void shutdownApplication() {
    vkDeviceWaitIdle(context->device);
    
    
    
    exitVulkan(context);
}


int main(int argc, char* argv[]) {
    initApplication();
    std::cout << "Hello World" << std::endl;
    shutdownApplication();
    return 1;
}

