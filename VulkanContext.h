//
// Created by alex on 3/17/25.
//

// VulkanContext.h - New file
#ifndef VULKANCONTEXT_H
#define VULKANCONTEXT_H

#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>
#include <vector>
#include <string>

class VulkanContext {
public:
    VulkanContext();
    ~VulkanContext();

    bool initialize(SDL_Window* window);
    void cleanup();

    // Core Vulkan objects
    VkInstance instance = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkQueue computeQueue = VK_NULL_HANDLE;
    uint32_t computeQueueFamilyIndex = 0;

    // Check for ray tracing support
    bool hasRayTracingSupport() const;
};

#endif // VULKANCONTEXT_H
