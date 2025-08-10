#include <iostream>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_beta.h>
#include "vulkan/vulkan_core.h"
#include "vulkan_base/vulkan_base.h"

VulkanContext* context;
VulkanDescriptorSet* descriptorSetInfo;
VulkanPipeline pipeline;
VulkanBuffer myBuffer;


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

    descriptorSetInfo = initDescriptorSet();

    addDescriptorSetLayout(descriptorSetInfo, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
    createDescriptorSet(context, descriptorSetInfo);

    std::vector<const char*> computeShaders;
    computeShaders.push_back("../shaders/test.spv");

    pipeline = createPipeline(context, computeShaders, descriptorSetInfo);

    int myData[] = {1, 2, 3, 4, 5};

    createBuffer(context, &myBuffer, sizeof(myData), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    uploadDataToBufferWithStagingBuffer(context, &myBuffer, myData, sizeof(myData));
    

    VkDescriptorBufferInfo myBufferInfo = {};
    myBufferInfo.buffer = myBuffer.buffer;
    myBufferInfo.offset = 0;
    myBufferInfo.range = sizeof(myData);

    std::vector<void*> allBuffers = {
        &myBufferInfo,
    };
    
    fillDescriptorSet(context, descriptorSetInfo, allBuffers);
}

void shutdownApplication() {
    vkDeviceWaitIdle(context->device);
    
    destroyBuffer(context, &myBuffer);
    destroyPipeline(context, &pipeline);
    destroyDescriptorSet(context, descriptorSetInfo);
    
    exitVulkan(context);
}


int main(int argc, char* argv[]) {
    initApplication();
    std::cout << "Hello World" << std::endl;
    shutdownApplication();
    return 1;
}

