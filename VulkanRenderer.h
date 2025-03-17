//
// Created by alex on 3/17/25.
//

// VulkanRenderer.h
#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "VulkanContext.h"
#include "Scene.h"

class VulkanRenderer {
public:
    VulkanRenderer(VulkanContext& context, uint32_t width, uint32_t height);
    ~VulkanRenderer();

    // Initialize the renderer
    bool initialize();

    // Render frame using Vulkan compute
    void renderFrame(const Scene& scene,
                    const glm::vec3& camPos,
                    const glm::vec3& forward,
                    const glm::vec3& right,
                    const glm::vec3& up,
                    uint32_t* outputPixels);

    // Create and update scene buffers
    bool createSceneBuffers(const Scene& scene);
    void updateCameraBuffer(const glm::vec3& camPos,
                           const glm::vec3& forward,
                           const glm::vec3& right,
                           const glm::vec3& up);

private:
    // Reference to the Vulkan context
    VulkanContext& context;

    // Dimensions
    uint32_t width;
    uint32_t height;

    // Vulkan resources
    VkCommandPool commandPool = VK_NULL_HANDLE;
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

    // Compute pipeline
    VkPipeline computePipeline = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
    VkShaderModule computeShaderModule = VK_NULL_HANDLE;

    // Buffers
    struct {
        VkBuffer buffer = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;
    } outputBuffer;

    struct {
        VkBuffer buffer = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;
    } cameraBuffer;

    struct {
        VkBuffer triangleBuffer = VK_NULL_HANDLE;
        VkDeviceMemory triangleMemory = VK_NULL_HANDLE;
        VkBuffer sphereBuffer = VK_NULL_HANDLE;
        VkDeviceMemory sphereMemory = VK_NULL_HANDLE;
        VkBuffer materialBuffer = VK_NULL_HANDLE;
        VkDeviceMemory materialMemory = VK_NULL_HANDLE;
    } sceneBuffers;

    // Private methods
    bool createComputePipeline();
    bool createDescriptorSets();
    bool createBuffers();
    void createCommandBuffer();

    // Helper methods
    bool createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                     VkMemoryPropertyFlags properties,
                     VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    VkShaderModule createShaderModule(const std::vector<char>& code);
    std::vector<char> readFile(const std::string& filename);
};

#endif // VULKAN_RENDERER_H
