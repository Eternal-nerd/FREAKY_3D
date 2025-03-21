#pragma once

#include <vulkan/vulkan.h>

/*
class for 2d rendering (GUI/debug text)
*/
class Overlay {
public:
	void init(VkDevice device, VkPhysicalDevice physicalDevice, VkRenderPass renderpass);


private:
	// access to vulkan
	VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
	VkDevice device_ = VK_NULL_HANDLE;
	VkRenderPass renderpass_ = VK_NULL_HANDLE;

	// extent used for scaling
	VkExtent2D swapChainExtent_;

	// Vulkan stuff only used by this class
	VkBuffer vertexBuffer_;
	VkDeviceMemory vertexBufferMemory_;
	VkDescriptorPool descriptorPool_;
	VkDescriptorSetLayout descriptorSetLayout_;
	VkDescriptorSet descriptorSet_;
	VkPipelineLayout pipelineLayout_;
	VkPipelineCache pipelineCache_;
	VkPipeline pipeline_;
};