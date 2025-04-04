#pragma once

#include <vulkan/vulkan.h>

#include <string>

#include "../types.h"
#include "../util.h"

/*
Class for managing mesh data used to make vulkan vertex/index buffers
Could provide some vertex processing here 
*/
class Mesh {
public:

	void init(MeshData data, VkPhysicalDevice physicalDevice, VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue);

	void setTextureIndex(int texIndex);

	// BINDING
	void bindVertexBuffer(VkCommandBuffer commandBuffer);
	void bindIndexBuffer(VkCommandBuffer commandBuffer);

	// GETTERS
	int getIndexCount();

	void cleanup();

	const std::string name_ = "MESH";
private:
	MeshData data_;

	// references to vk stuff
	VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
	VkDevice device_ = VK_NULL_HANDLE;
	VkCommandPool commandPool_ = VK_NULL_HANDLE;
	VkQueue graphicsQueue_ = VK_NULL_HANDLE;

	VkBuffer vertexBuffer_ = VK_NULL_HANDLE;
	VkDeviceMemory vertexBufferMemory_ = VK_NULL_HANDLE;
	VkBuffer indexBuffer_ = VK_NULL_HANDLE;
	VkDeviceMemory indexBufferMemory_ = VK_NULL_HANDLE;

	void createVertexBuffer();
	void createIndexBuffer();


};