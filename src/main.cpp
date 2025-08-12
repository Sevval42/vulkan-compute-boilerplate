#include <iostream>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_beta.h>
#include "vulkan/vulkan_core.h"
#include "vulkan_base/vulkan_base.h"

VulkanContext* context;
VulkanDescriptorSet* descriptorSetInfo;
VulkanPipeline pipeline;
VulkanBuffer myBuffer;
int myData[] = {1, 2, 3, 4, 5};

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

    createBuffer(
        context, 
        &myBuffer, 
        sizeof(myData), 
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT , 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );
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

void runApplication() {
    VkCommandPool commandPool;
    {
        VkCommandPoolCreateInfo createInfo = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
        createInfo.queueFamilyIndex = context->computeQueue.familyIndex;
        if (vkCreateCommandPool(context->device, &createInfo, 0, &commandPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create command pool for running shaders");
        }
    }

    VkCommandBuffer commandBuffer;
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;
        if (vkAllocateCommandBuffers(context->device, &allocInfo, &commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffer");
        }
    }

    VkCommandBufferBeginInfo beginInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    vkCmdBindPipeline(
        commandBuffer, 
        VK_PIPELINE_BIND_POINT_COMPUTE, 
        pipeline.pipelines.front()
    );

    vkCmdBindDescriptorSets(
        commandBuffer, 
        VK_PIPELINE_BIND_POINT_COMPUTE, 
        pipeline.pipelineLayout, 
        0, 
        1, 
        &descriptorSetInfo->descriptorSet, 
        0, 
        0
    );

    vkCmdDispatch(commandBuffer, 5, 1, 1);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }

    VkSubmitInfo submitInfo = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    if (vkQueueSubmit(context->computeQueue.queue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit compute command buffer!");
    }

    vkQueueWaitIdle(context->computeQueue.queue);

    int data[5];
    getDataFromBufferWithStagingBuffer(context, &myBuffer, data, sizeof(myData));

    for (int i = 0; i < 5; i++) {
        std::cout << data[i] << ", ";
    }
    std::cout << std::endl;

    vkFreeCommandBuffers(context->device, commandPool, 1, &commandBuffer);
    vkDestroyCommandPool(context->device, commandPool, 0);

}


int main(int argc, char* argv[]) {
    initApplication();
    std::cout << "Hello World" << std::endl;
    runApplication();
    shutdownApplication();
    return 1;
}

