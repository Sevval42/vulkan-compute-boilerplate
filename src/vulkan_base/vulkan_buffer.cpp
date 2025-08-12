#include "vulkan/vulkan_core.h"
#include "vulkan_base.h"
#include <cstring>
#include <stdexcept>

uint32_t findMemoryType(VulkanContext* context, uint32_t typeFilter, VkMemoryPropertyFlags memoryProperties) {
	VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
	vkGetPhysicalDeviceMemoryProperties(context->physicalDevice, &deviceMemoryProperties);

	for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; ++i) {
		// Check if required memory type is allowed
		if ((typeFilter & (1 << i)) != 0) {
			// Check if required properties are satisfied
			if ((deviceMemoryProperties.memoryTypes[i].propertyFlags & memoryProperties) == memoryProperties) {
				// Return this memory type index
				return i;
			}
		}
	}

	throw std::runtime_error("No matching avaialble memory type found");
}

void createBuffer(VulkanContext* context, VulkanBuffer* buffer, uint32_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties) {
    VkBufferCreateInfo createInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    createInfo.size = size;
    createInfo.usage = usage;
    vkCreateBuffer(context->device, &createInfo, 0, &buffer->buffer);

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(context->device, buffer->buffer, &memoryRequirements);
    uint32_t memoryIndex = findMemoryType(context, memoryRequirements.memoryTypeBits, memoryProperties);

    VkMemoryAllocateInfo allocateInfo = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = memoryIndex;

    vkAllocateMemory(context->device, &allocateInfo, 0, &buffer->memory);
    vkBindBufferMemory(context->device, buffer->buffer, buffer->memory, 0);
}

void copyBuffer(VulkanContext* context, VulkanBuffer* srcBuffer, VulkanBuffer* dstBuffer, size_t size) {

    VkCommandPool commandPool;
    {
        VkCommandPoolCreateInfo createInfo = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
        createInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        createInfo.queueFamilyIndex = context->computeQueue.familyIndex;
        if (vkCreateCommandPool(context->device, &createInfo, 0, &commandPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create command pool for copying buffers");
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
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion = {0, 0, size};
    vkCmdCopyBuffer(commandBuffer, srcBuffer->buffer, dstBuffer->buffer, 1, &copyRegion);
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(context->computeQueue.queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(context->computeQueue.queue);

    vkFreeCommandBuffers(context->device, commandPool, 1, &commandBuffer);
    vkDestroyCommandPool(context->device, commandPool, 0);
}

void uploadDataToBufferWithStagingBuffer(VulkanContext* context, VulkanBuffer* buffer, void* data, size_t size) {
    VulkanBuffer stagingBuffer;
    createBuffer(
        context, 
        &stagingBuffer, size, 
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    void* mapped;
    vkMapMemory(context->device, stagingBuffer.memory, 0, size, 0, &mapped);
    memcpy(mapped, data, size);
    vkUnmapMemory(context->device, stagingBuffer.memory);

    copyBuffer(context, &stagingBuffer, buffer, size);

    vkDestroyBuffer(context->device, stagingBuffer.buffer, 0);
    vkFreeMemory(context->device, stagingBuffer.memory, 0);
}

void getDataFromBufferWithStagingBuffer(VulkanContext* context, VulkanBuffer* buffer, void* data, size_t size) {
    VulkanBuffer stagingBuffer;
    createBuffer(
        context, 
        &stagingBuffer, 
        size, 
        VK_BUFFER_USAGE_TRANSFER_DST_BIT, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    void* mapped;
    

    copyBuffer(context, buffer, &stagingBuffer, size);

    vkMapMemory(context->device, stagingBuffer.memory, 0, size, 0, &mapped);
    memcpy(data, mapped, size);
    vkUnmapMemory(context->device, stagingBuffer.memory);

    vkDestroyBuffer(context->device, stagingBuffer.buffer, 0);
    vkFreeMemory(context->device, stagingBuffer.memory, 0);
}

void destroyBuffer(VulkanContext* context, VulkanBuffer* buffer) {
    vkDestroyBuffer(context->device, buffer->buffer, 0);
    vkFreeMemory(context->device, buffer->memory, 0);
}
