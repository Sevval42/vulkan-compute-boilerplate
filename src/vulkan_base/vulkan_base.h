#include "vulkan/vulkan_core.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <unordered_map>

#define ARRAY_COUNT(array) (sizeof(array) / sizeof((array)[0]));

struct ivec3 {
    int x;
    int y;
    int z;
};

struct VulkanQueue {
    VkQueue queue;
    uint32_t familyIndex;
};

struct VulkanContext {
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkPhysicalDeviceProperties physicalDeviceProperties;
    VkDevice device;
    VulkanQueue computeQueue;
};

struct VulkanBuffer {
    VkBuffer buffer;
    VkDeviceMemory memory;
};

struct VulkanDescriptorBufferInfo {
    VkDescriptorBufferInfo bufferInfo;
    VkDescriptorImageInfo imageInfo;
    enum class Type {
        BUFFER,
        IMAGE,
    } type;
};

struct VulkanDescriptorSet {
    uint32_t layoutCount;
    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
    VkDescriptorSetLayout descriptorSetLayout;
    std::unordered_map<VkDescriptorType, uint32_t> descriptorTypeCount;
    VkDescriptorPool descriptorPool;
    VkDescriptorSet descriptorSet;
    std::vector<VulkanDescriptorBufferInfo> buffers;
    void addBufferAndData(
        VulkanContext* context, 
        VulkanBuffer* buffer, 
        void* data, uint32_t size, 
        VkBufferUsageFlags usage, 
        VkMemoryPropertyFlags memoryProperties
    );
};

struct VulkanPipeline {
    std::vector<VkPipeline> pipelines;
    std::vector<ivec3> dispatchSizes;
    VkPipelineLayout pipelineLayout;
};

VulkanContext* initVulkan(uint32_t extensionCount, const char** extensions, uint32_t deviceExtensionCount, const char** deviceExtensions);
void exitVulkan(VulkanContext* context);

void createBuffer(VulkanContext* context, VulkanBuffer* buffer, uint32_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties);
void uploadDataToBufferWithStagingBuffer(VulkanContext* context, VulkanBuffer* buffer, void* data, size_t size);
void getDataFromBufferWithStagingBuffer(VulkanContext* context, VulkanBuffer* buffer, void* data, size_t size);
void destroyBuffer(VulkanContext* context, VulkanBuffer* buffer);

VulkanDescriptorSet* initDescriptorSet();
void addDescriptorSetLayout(VulkanDescriptorSet* descriptorSet, VkDescriptorType descriptorType);
void createDescriptorSet(VulkanContext* context, VulkanDescriptorSet* descriptorSet);
void fillDescriptorSet(VulkanContext* context, VulkanDescriptorSet* descriptorSet);
void destroyDescriptorSet(VulkanContext* context, VulkanDescriptorSet* descriptorSet);

VulkanPipeline createPipeline(VulkanContext* context, std::vector<const char*> computeShaderFilenames, std::vector<ivec3> dispatches, VulkanDescriptorSet* descriptorSet);
void destroyPipeline(VulkanContext* context, VulkanPipeline* pipeline);
