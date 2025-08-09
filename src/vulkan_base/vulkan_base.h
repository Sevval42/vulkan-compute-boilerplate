#include "vulkan/vulkan_core.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <unordered_map>

#define ARRAY_COUNT(array) (sizeof(array) / sizeof((array)[0]));

struct VulkanQueue {
    VkQueue queue;
    uint32_t familyIndex;
};

struct VulkanDescriptorSet {
    uint32_t layoutCount;
    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
    VkDescriptorSetLayout descriptorSetLayout;
    std::unordered_map<VkDescriptorType, uint32_t> descriptorTypeCount;
    VkDescriptorPool descriptorPool;
    VkDescriptorSet descriptorSet;
};

struct VulkanPipeline {
    std::vector<VkPipeline> pipelines;
    VkPipelineLayout pipelineLayout;
};

struct VulkanContext {
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkPhysicalDeviceProperties physicalDeviceProperties;
    VkDevice device;
    VulkanQueue computeQueue;
};

VulkanContext* initVulkan(uint32_t extensionCount, const char** extensions, uint32_t deviceExtensionCount, const char** deviceExtensions);
void exitVulkan(VulkanContext* context);

VkRenderPass createRenderPass(VulkanContext* context, VkFormat format);
void destroyRenderPass(VulkanContext* context, VkRenderPass renderPass);

VulkanDescriptorSet* initDescriptorSet();
void addDescriptorSetLayout(VulkanDescriptorSet* descriptorSet, VkDescriptorType descriptorType);
void createDescriptorSet(VulkanContext* context, VulkanDescriptorSet* descriptorSet);
void fillDescriptorSet(VulkanContext* context, VulkanDescriptorSet* descriptorSet, std::vector<void*>& buffers);
void destroyDescriptorSet(VulkanContext* context, VulkanDescriptorSet* descriptorSet);

VulkanPipeline createPipeline(VulkanContext* context, std::vector<const char*> computeShaderFilenames, VulkanDescriptorSet* descriptorSet);
void destroyPipeline(VulkanContext* context, VulkanPipeline* pipeline);