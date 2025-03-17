//
// Created by alex on 3/17/25.
//

// VulkanRenderer.cpp
#include "VulkanRenderer.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <glm/glm.hpp>

// Structure to hold camera data that will be passed to the shader
struct CameraData {
    glm::vec3 position;
    glm::vec3 forward;
    glm::vec3 right;
    glm::vec3 up;
};

// Structure to hold triangle data
struct VulkanTriangle {
    glm::vec3 v0, v1, v2;
    glm::vec3 color;
    glm::vec3 emission;
    int materialIndex;
};

// Structure to hold sphere data
struct VulkanSphere {
    glm::vec3 center;
    float radius;
    glm::vec3 color;
    glm::vec3 emission;
    int materialIndex;
};

VulkanRenderer::VulkanRenderer(VulkanContext& context, uint32_t width, uint32_t height)
    : context(context), width(width), height(height) {
}

VulkanRenderer::~VulkanRenderer() {
    // Clean up Vulkan resources
    VkDevice device = context.device;

    // Destroy pipeline resources
    if (computePipeline != VK_NULL_HANDLE)
        vkDestroyPipeline(device, computePipeline, nullptr);
    if (pipelineLayout != VK_NULL_HANDLE)
        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    if (descriptorSetLayout != VK_NULL_HANDLE)
        vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
    if (descriptorPool != VK_NULL_HANDLE)
        vkDestroyDescriptorPool(device, descriptorPool, nullptr);
    if (computeShaderModule != VK_NULL_HANDLE)
        vkDestroyShaderModule(device, computeShaderModule, nullptr);

    // Destroy buffers
    if (outputBuffer.buffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(device, outputBuffer.buffer, nullptr);
        vkFreeMemory(device, outputBuffer.memory, nullptr);
    }

    if (cameraBuffer.buffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(device, cameraBuffer.buffer, nullptr);
        vkFreeMemory(device, cameraBuffer.memory, nullptr);
    }

    // Destroy scene buffers
    if (sceneBuffers.triangleBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(device, sceneBuffers.triangleBuffer, nullptr);
        vkFreeMemory(device, sceneBuffers.triangleMemory, nullptr);
    }

    if (sceneBuffers.sphereBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(device, sceneBuffers.sphereBuffer, nullptr);
        vkFreeMemory(device, sceneBuffers.sphereMemory, nullptr);
    }

    if (sceneBuffers.materialBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(device, sceneBuffers.materialBuffer, nullptr);
        vkFreeMemory(device, sceneBuffers.materialMemory, nullptr);
    }

    // Destroy command pool
    if (commandPool != VK_NULL_HANDLE)
        vkDestroyCommandPool(device, commandPool, nullptr);
}

bool VulkanRenderer::initialize() {
    VkDevice device = context.device;

    // Create command pool
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = context.computeQueueFamilyIndex;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        std::cerr << "Failed to create command pool" << std::endl;
        return false;
    }

    // Create compute pipeline
    if (!createComputePipeline()) {
        std::cerr << "Failed to create compute pipeline" << std::endl;
        return false;
    }

    // Create descriptor sets
    if (!createDescriptorSets()) {
        std::cerr << "Failed to create descriptor sets" << std::endl;
        return false;
    }

    // Create buffers
    if (!createBuffers()) {
        std::cerr << "Failed to create buffers" << std::endl;
        return false;
    }

    // Create command buffer
    createCommandBuffer();

    std::cout << "VulkanRenderer initialized successfully" << std::endl;
    return true;
}

bool VulkanRenderer::createComputePipeline() {
    VkDevice device = context.device;

    // For now, we'll use a simple compute shader that just fills the output with a color
    // In the future, we'll implement a more complex shader for ray tracing

    // Load shader code (we'll implement this later)
    std::vector<char> shaderCode = readFile("shaders/simple_compute.spv");
    computeShaderModule = createShaderModule(shaderCode);

    // Create descriptor set layout
    VkDescriptorSetLayoutBinding outputBinding = {};
    outputBinding.binding = 0;
    outputBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    outputBinding.descriptorCount = 1;
    outputBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorSetLayoutBinding cameraBinding = {};
    cameraBinding.binding = 1;
    cameraBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    cameraBinding.descriptorCount = 1;
    cameraBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    std::vector<VkDescriptorSetLayoutBinding> bindings = {outputBinding, cameraBinding};

    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        return false;
    }

    // Create pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        return false;
    }

    // Create compute pipeline
    VkComputePipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipelineInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    pipelineInfo.stage.module = computeShaderModule;
    pipelineInfo.stage.pName = "main";
    pipelineInfo.layout = pipelineLayout;

    if (vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &computePipeline) != VK_SUCCESS) {
        return false;
    }

    return true;
}

bool VulkanRenderer::createDescriptorSets() {
    VkDevice device = context.device;

    // Create descriptor pool
    std::vector<VkDescriptorPoolSize> poolSizes = {
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 }
    };

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = 1;

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        return false;
    }

    // Allocate descriptor set
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &descriptorSetLayout;

    if (vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet) != VK_SUCCESS) {
        return false;
    }

    return true;
}

bool VulkanRenderer::createBuffers() {
    VkDevice device = context.device;

    // Create output buffer
    VkDeviceSize outputBufferSize = sizeof(uint32_t) * width * height;
    if (!createBuffer(outputBufferSize,
                     VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     outputBuffer.buffer, outputBuffer.memory)) {
        return false;
    }

    // Create camera buffer
    VkDeviceSize cameraBufferSize = sizeof(CameraData);
    if (!createBuffer(cameraBufferSize,
                     VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     cameraBuffer.buffer, cameraBuffer.memory)) {
        return false;
    }

    // Create initial descriptor set updates
    std::vector<VkWriteDescriptorSet> descriptorWrites;

    VkDescriptorBufferInfo outputBufferInfo = {};
    outputBufferInfo.buffer = outputBuffer.buffer;
    outputBufferInfo.offset = 0;
    outputBufferInfo.range = outputBufferSize;

    VkWriteDescriptorSet outputWrite = {};
    outputWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    outputWrite.dstSet = descriptorSet;
    outputWrite.dstBinding = 0;
    outputWrite.dstArrayElement = 0;
    outputWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    outputWrite.descriptorCount = 1;
    outputWrite.pBufferInfo = &outputBufferInfo;
    descriptorWrites.push_back(outputWrite);

    VkDescriptorBufferInfo cameraBufferInfo = {};
    cameraBufferInfo.buffer = cameraBuffer.buffer;
    cameraBufferInfo.offset = 0;
    cameraBufferInfo.range = cameraBufferSize;

    VkWriteDescriptorSet cameraWrite = {};
    cameraWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    cameraWrite.dstSet = descriptorSet;
    cameraWrite.dstBinding = 1;
    cameraWrite.dstArrayElement = 0;
    cameraWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    cameraWrite.descriptorCount = 1;
    cameraWrite.pBufferInfo = &cameraBufferInfo;
    descriptorWrites.push_back(cameraWrite);

    vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

    return true;
}

void VulkanRenderer::createCommandBuffer() {
    VkDevice device = context.device;

    // Allocate command buffer
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);
}

void VulkanRenderer::render(const Camera& camera) {
    VkDevice device = context.device;

    // Update camera buffer
    CameraData cameraData;
    cameraData.position = camera.getPosition();
    cameraData.forward = camera.getForward();
    cameraData.right = camera.getRight();
    cameraData.up = camera.getUp();

    void* data;
    vkMapMemory(device, cameraBuffer.memory, 0, sizeof(CameraData), 0, &data);
    memcpy(data, &cameraData, sizeof(CameraData));
    vkUnmapMemory(device, cameraBuffer.memory);

    // Record command buffer
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

    // Dispatch compute work groups
    uint32_t groupSizeX = 16;
    uint32_t groupSizeY = 16;
    vkCmdDispatch(commandBuffer, (width + groupSizeX - 1) / groupSizeX, (height + groupSizeY - 1) / groupSizeY, 1);

    vkEndCommandBuffer(commandBuffer);

    // Submit command buffer
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(context.computeQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(context.computeQueue);
}

bool VulkanRenderer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
    VkDevice device = context.device;

    // Create buffer
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        return false;
    }

    // Get memory requirements
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    // Allocate memory
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        vkDestroyBuffer(device, buffer, nullptr);
        return false;
    }

    // Bind memory to buffer
    vkBindBufferMemory(device, buffer, bufferMemory, 0);

    return true;
}

void VulkanRenderer::setupScene(const std::vector<Triangle>& triangles, const std::vector<Sphere>& spheres, const std::vector<Material>& materials) {
    VkDevice device = context.device;

    // Convert triangles to VulkanTriangles
    std::vector<VulkanTriangle> vulkanTriangles(triangles.size());
    for (size_t i = 0; i < triangles.size(); i++) {
        vulkanTriangles[i].v0 = triangles[i].v0;
        vulkanTriangles[i].v1 = triangles[i].v1;
        vulkanTriangles[i].v2 = triangles[i].v2;
        vulkanTriangles[i].color = triangles[i].color;
        vulkanTriangles[i].emission = triangles[i].emission;
        vulkanTriangles[i].materialIndex = triangles[i].materialIndex;
    }

    // Convert spheres to VulkanSpheres
    std::vector<VulkanSphere> vulkanSpheres(spheres.size());
    for (size_t i = 0; i < spheres.size(); i++) {
        vulkanSpheres[i].center = spheres[i].center;
        vulkanSpheres[i].radius = spheres[i].radius;
        vulkanSpheres[i].color = spheres[i].color;
        vulkanSpheres[i].emission = spheres[i].emission;
        vulkanSpheres[i].materialIndex = spheres[i].materialIndex;
    }

    // Create triangle buffer
    VkDeviceSize triangleBufferSize = sizeof(VulkanTriangle) * vulkanTriangles.size();
    if (triangleBufferSize > 0) {
        if (!createBuffer(triangleBufferSize,
                         VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         sceneBuffers.triangleBuffer, sceneBuffers.triangleMemory)) {
            throw std::runtime_error("Failed to create triangle buffer");
        }

        // Copy data to triangle buffer
        void* data;
        vkMapMemory(device, sceneBuffers.triangleMemory, 0, triangleBufferSize, 0, &data);
        memcpy(data, vulkanTriangles.data(), triangleBufferSize);
        vkUnmapMemory(device, sceneBuffers.triangleMemory);
    }

    // Create sphere buffer
    VkDeviceSize sphereBufferSize = sizeof(VulkanSphere) * vulkanSpheres.size();
    if (sphereBufferSize > 0) {
        if (!createBuffer(sphereBufferSize,
                         VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         sceneBuffers.sphereBuffer, sceneBuffers.sphereMemory)) {
            throw std::runtime_error("Failed to create sphere buffer");
        }

        // Copy data to sphere buffer
        void* data;
        vkMapMemory(device, sceneBuffers.sphereMemory, 0, sphereBufferSize, 0, &data);
        memcpy(data, vulkanSpheres.data(), sphereBufferSize);
        vkUnmapMemory(device, sceneBuffers.sphereMemory);
    }

    // Create material buffer
    VkDeviceSize materialBufferSize = sizeof(Material) * materials.size();
    if (materialBufferSize > 0) {
        if (!createBuffer(materialBufferSize,
                         VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         sceneBuffers.materialBuffer, sceneBuffers.materialMemory)) {
            throw std::runtime_error("Failed to create material buffer");
        }

        // Copy data to material buffer
        void* data;
        vkMapMemory(device, sceneBuffers.materialMemory, 0, materialBufferSize, 0, &data);
        memcpy(data, materials.data(), materialBufferSize);
        vkUnmapMemory(device, sceneBuffers.materialMemory);
    }

    // Update descriptor set with scene buffers
    std::vector<VkWriteDescriptorSet> descriptorWrites;

    if (triangleBufferSize > 0) {
        VkDescriptorBufferInfo triangleBufferInfo = {};
        triangleBufferInfo.buffer = sceneBuffers.triangleBuffer;
        triangleBufferInfo.offset = 0;
        triangleBufferInfo.range = triangleBufferSize;

        VkWriteDescriptorSet triangleWrite = {};
        triangleWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        triangleWrite.dstSet = descriptorSet;
        triangleWrite.dstBinding = 2;
        triangleWrite.dstArrayElement = 0;
        triangleWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        triangleWrite.descriptorCount = 1;
        triangleWrite.pBufferInfo = &triangleBufferInfo;
        descriptorWrites.push_back(triangleWrite);
    }

    if (sphereBufferSize > 0) {
        VkDescriptorBufferInfo sphereBufferInfo = {};
        sphereBufferInfo.buffer = sceneBuffers.sphereBuffer;
        sphereBufferInfo.offset = 0;
        sphereBufferInfo.range = sphereBufferSize;

        VkWriteDescriptorSet sphereWrite = {};
        sphereWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        sphereWrite.dstSet = descriptorSet;
        sphereWrite.dstBinding = 3;
        sphereWrite.dstArrayElement = 0;
        sphereWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        sphereWrite.descriptorCount = 1;
        sphereWrite.pBufferInfo = &sphereBufferInfo;
        descriptorWrites.push_back(sphereWrite);
    }

    if (materialBufferSize > 0) {
        VkDescriptorBufferInfo materialBufferInfo = {};
        materialBufferInfo.buffer = sceneBuffers.materialBuffer;
        materialBufferInfo.offset = 0;
        materialBufferInfo.range = materialBufferSize;

        VkWriteDescriptorSet materialWrite = {};
        materialWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        materialWrite.dstSet = descriptorSet;
        materialWrite.dstBinding = 4;
        materialWrite.dstArrayElement = 0;
        materialWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        materialWrite.descriptorCount = 1;
        materialWrite.pBufferInfo = &materialBufferInfo;
        descriptorWrites.push_back(materialWrite);
    }

    if (!descriptorWrites.empty()) {
        vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}

uint32_t VulkanRenderer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(context.physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type");
}

std::vector<char> VulkanRenderer::readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

VkShaderModule VulkanRenderer::createShaderModule(const std::vector<char>& code) {
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(context.device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create shader module");
    }

    return shaderModule;
}

std::vector<uint32_t> VulkanRenderer::getPixelData() {
    std::vector<uint32_t> pixels(width * height);

    void* data;
    vkMapMemory(context.device, outputBuffer.memory, 0, sizeof(uint32_t) * width * height, 0, &data);
    memcpy(pixels.data(), data, sizeof(uint32_t) * width * height);
    vkUnmapMemory(context.device, outputBuffer.memory);

    return pixels;
}

void VulkanRenderer::resize(uint32_t newWidth, uint32_t newHeight) {
    // Only resize if dimensions have changed
    if (width == newWidth && height == newHeight) {
        return;
    }

    // Update dimensions
    width = newWidth;
    height = newHeight;

    VkDevice device = context.device;

    // Clean up old output buffer
    if (outputBuffer.buffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(device, outputBuffer.buffer, nullptr);
        vkFreeMemory(device, outputBuffer.memory, nullptr);
        outputBuffer.buffer = VK_NULL_HANDLE;
        outputBuffer.memory = VK_NULL_HANDLE;
    }

    // Create new output buffer
    VkDeviceSize outputBufferSize = sizeof(uint32_t) * width * height;
    if (!createBuffer(outputBufferSize,
                     VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     outputBuffer.buffer, outputBuffer.memory)) {
        throw std::runtime_error("Failed to create output buffer");
    }

    // Update descriptor set
    VkDescriptorBufferInfo outputBufferInfo = {};
    outputBufferInfo.buffer = outputBuffer.buffer;
    outputBufferInfo.offset = 0;
    outputBufferInfo.range = outputBufferSize;

    VkWriteDescriptorSet outputWrite = {};
    outputWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    outputWrite.dstSet = descriptorSet;
    outputWrite.dstBinding = 0;
    outputWrite.dstArrayElement = 0;
    outputWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    outputWrite.descriptorCount = 1;
    outputWrite.pBufferInfo = &outputBufferInfo;

    vkUpdateDescriptorSets(device, 1, &outputWrite, 0, nullptr);
}

void VulkanRenderer::setRenderParameters(const RenderParameters& params) {
    VkDevice device = context.device;

    // Create uniform buffer for render parameters if it doesn't exist yet
    if (renderParamsBuffer.buffer == VK_NULL_HANDLE) {
        VkDeviceSize bufferSize = sizeof(RenderParameters);
        if (!createBuffer(bufferSize,
                         VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         renderParamsBuffer.buffer, renderParamsBuffer.memory)) {
            throw std::runtime_error("Failed to create render parameters buffer");
                         }

        // Update descriptor set
        VkDescriptorBufferInfo paramsBufferInfo = {};
        paramsBufferInfo.buffer = renderParamsBuffer.buffer;
        paramsBufferInfo.offset = 0;
        paramsBufferInfo.range = bufferSize;

        VkWriteDescriptorSet paramsWrite = {};
        paramsWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        paramsWrite.dstSet = descriptorSet;
        paramsWrite.dstBinding = 5;  // Adjust binding based on your descriptor set layout
        paramsWrite.dstArrayElement = 0;
        paramsWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        paramsWrite.descriptorCount = 1;
        paramsWrite.pBufferInfo = &paramsBufferInfo;

        vkUpdateDescriptorSets(device, 1, &paramsWrite, 0, nullptr);
    }

    // Update buffer with new parameters
    void* data;
    vkMapMemory(device, renderParamsBuffer.memory, 0, sizeof(RenderParameters), 0, &data);
    memcpy(data, &params, sizeof(RenderParameters));
    vkUnmapMemory(device, renderParamsBuffer.memory);
}

void VulkanRenderer::waitForCompletion() {
    vkQueueWaitIdle(context.computeQueue);
}

void VulkanRenderer::updateSceneObject(uint32_t objectId, const SceneObject& object) {
    // Implementation depends on your scene structure
    // This is just a placeholder for updating a specific object in the scene

    // Example: Update a sphere
    if (object.type == SceneObjectType::Sphere && objectId < sceneData.spheres.size()) {
        VulkanSphere vulkanSphere;
        vulkanSphere.center = object.sphere.center;
        vulkanSphere.radius = object.sphere.radius;
        vulkanSphere.color = object.sphere.color;
        vulkanSphere.emission = object.sphere.emission;
        vulkanSphere.materialIndex = object.sphere.materialIndex;

        void* data;
        vkMapMemory(context.device, sceneBuffers.sphereMemory,
                   objectId * sizeof(VulkanSphere), sizeof(VulkanSphere), 0, &data);
        memcpy(data, &vulkanSphere, sizeof(VulkanSphere));
        vkUnmapMemory(context.device, sceneBuffers.sphereMemory);
    }
    // Add similar implementation for triangles and other object types
}

void VulkanRenderer::clearScene() {
    VkDevice device = context.device;

    // Clean up existing scene buffers
    if (sceneBuffers.triangleBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(device, sceneBuffers.triangleBuffer, nullptr);
        vkFreeMemory(device, sceneBuffers.triangleMemory, nullptr);
        sceneBuffers.triangleBuffer = VK_NULL_HANDLE;
        sceneBuffers.triangleMemory = VK_NULL_HANDLE;
    }

    if (sceneBuffers.sphereBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(device, sceneBuffers.sphereBuffer, nullptr);
        vkFreeMemory(device, sceneBuffers.sphereMemory, nullptr);
        sceneBuffers.sphereBuffer = VK_NULL_HANDLE;
        sceneBuffers.sphereMemory = VK_NULL_HANDLE;
    }

    if (sceneBuffers.materialBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(device, sceneBuffers.materialBuffer, nullptr);
        vkFreeMemory(device, sceneBuffers.materialMemory, nullptr);
        sceneBuffers.materialBuffer = VK_NULL_HANDLE;
        sceneBuffers.materialMemory = VK_NULL_HANDLE;
    }

    // Clear scene data
    sceneData.triangles.clear();
    sceneData.spheres.clear();
    sceneData.materials.clear();
}

bool VulkanRenderer::createAccelerationStructures() {
    // Check if the device supports ray tracing
    if (!context.rayTracingSupported) {
        std::cout << "Ray tracing not supported on this device, skipping acceleration structure creation" << std::endl;
        return false;
    }

    VkDevice device = context.device;

    // Create bottom-level acceleration structure for triangles
    if (!sceneData.triangles.empty()) {
        // Implementation of acceleration structure creation for triangles
        // This is a placeholder - actual implementation depends on Vulkan ray tracing extensions

        // Example pseudocode:
        /*
        VkAccelerationStructureGeometryKHR triangleGeometry = {};
        triangleGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
        triangleGeometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
        triangleGeometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
        triangleGeometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
        triangleGeometry.geometry.triangles.vertexData.deviceAddress = getBufferDeviceAddress(sceneBuffers.triangleBuffer);
        triangleGeometry.geometry.triangles.vertexStride = sizeof(VulkanTriangle);
        triangleGeometry.geometry.triangles.maxVertex = sceneData.triangles.size() * 3;
        triangleGeometry.geometry.triangles.indexType = VK_INDEX_TYPE_NONE_KHR;
        */
    }

    // Create bottom-level acceleration structure for spheres
    if (!sceneData.spheres.empty()) {
        // Implementation of acceleration structure creation for spheres
        // This is a placeholder - actual implementation depends on Vulkan ray tracing extensions or custom AABB implementation
    }

    // Create top-level acceleration structure
    // This is a placeholder - actual implementation depends on Vulkan ray tracing extensions

    return true;
}

void VulkanRenderer::renderToImage(const std::string& outputPath) {
    // Render the scene
    Camera camera;  // Use default camera or get from somewhere
    render(camera);

    // Get pixel data
    std::vector<uint32_t> pixels = getPixelData();

    // Save to image file
    // This is a placeholder - implement based on your image saving library
    // Example using stb_image_write (you would need to include this library)
    /*
    stbi_write_png(outputPath.c_str(), width, height, 4, pixels.data(), width * 4);
    */

    std::cout << "Image saved to " << outputPath << std::endl;
}

void VulkanRenderer::renderMultipleFrames(uint32_t frameCount, const std::string& outputPrefix) {
    for (uint32_t i = 0; i < frameCount; i++) {
        // Update scene or camera if needed
        Camera camera;  // Get or update camera

        // Render frame
        render(camera);

        // Save frame
        std::string outputPath = outputPrefix + "_" + std::to_string(i) + ".png";
        std::vector<uint32_t> pixels = getPixelData();

        // Save to image file (placeholder)
        std::cout << "Frame " << i << " saved to " << outputPath << std::endl;
    }
}