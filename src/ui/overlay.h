#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <string>
#include <assert.h>

#include "../util.h"
#include "../types.h"
#include "../assets/assets.h"
#include "../assets/texture.h"
#include "element.h"

// Max. number of elements the text overlay buffer can hold
#define MAX_OVERLAY_ELEMENTS 2048

/*
class for 2d rendering (GUI/debug text)
*/
class Overlay {
public:
	void init(VkDevice device, VkPhysicalDevice physicalDevice, VkRenderPass renderpass, VkExtent2D extent, Assets& assets);

	// USED BY ENGINE:
	void beginTextUpdate();
	void addText(const std::string& text, float xPos, float yPos);
	void endTextUpdate();

	// FIXME
	void tester();

	// draw entire vertex buffer
	void draw(VkCommandBuffer commandBuffer);

	void updateExtent(VkExtent2D extent);

	void cleanup();

	const std::string name_ = "OVERLAY";

private:
	// access to vulkan
	VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
	VkDevice device_ = VK_NULL_HANDLE;
	VkRenderPass renderpass_ = VK_NULL_HANDLE;

	// extent used for scaling
	VkExtent2D swapChainExtent_;

	// Assets ptr
	Assets* assets_ = nullptr;
	int textureCount_ = 0;
    std::vector<Texture*> textures_{};
	void initTextures();

	// Vulkan stuff only used by this class
	VkBuffer vertexBuffer_;
	VkDeviceMemory vertexBufferMemory_;
	VkDescriptorPool descriptorPool_;
	VkDescriptorSetLayout descriptorSetLayout_;
	VkDescriptorSet descriptorSet_;
	VkPipelineLayout pipelineLayout_;
	VkPipelineCache pipelineCache_;
	VkPipeline pipeline_;
	void initDescriptors();
	void initPipeline();

	// memory mapped vertex buffer
	UIVertex* mapped_ = nullptr;
	
	// ui elements
	std::vector<Element> defaultElements_{};
	std::vector<Element> statElements_{};
	std::vector<Element> menuElements_{};
	std::vector<Element> inventoryElements_{};



};
