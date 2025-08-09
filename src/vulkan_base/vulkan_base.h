#include <vulkan/vulkan.h>

#define ARRAY_COUNT(array) (sizeof(array) / sizeof((array)[0]));

struct VulkanQueue {
    VkQueue queue;
    uint32_t familyIndex;
};

struct VulkanPipeline {
    VkPipeline pipeline;
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

VulkanPipeline createPipeline(VulkanContext* context, const char* vertextShaderFilename, const char* fragmentShaderFilename, VkRenderPass renderPass, uint32_t width, uint32_t height);
void destroyPipeline(VulkanContext* context, VulkanPipeline* pipeline);