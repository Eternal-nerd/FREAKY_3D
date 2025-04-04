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
// FIXME make sure this is being used properly to set buffer size

/*
class for 2d rendering (GUI/debug text)
*/
class Overlay {
public:
	void init(VkDevice device, VkPhysicalDevice physicalDevice, VkRenderPass renderpass, VkExtent2D extent, Assets& assets);

	// API: call start update, then can call update textbox/element methods any amt of times, then, call end update
	void startUpdate();

	void updateTextBox(int index, const std::string& newText);
	//void updateElement(int index);

	void endUpdate();

	// draw entire vertex buffer
	void draw(VkCommandBuffer commandBuffer);

	void clearBuffer(VkCommandBuffer commandBuffer);

	void updateExtent(VkExtent2D extent);
	void toggleWireframe();
	void toggleMenu();

	void cleanup();

	const std::string name_ = "OVERLAY";

private:
	float scale_ = 1.f;

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
	VkBuffer vertexBuffer_ = VK_NULL_HANDLE;
	VkDeviceMemory vertexBufferMemory_ = VK_NULL_HANDLE;
	VkBuffer indexBuffer_ = VK_NULL_HANDLE;
	VkDeviceMemory indexBufferMemory_ = VK_NULL_HANDLE;
	VkDescriptorPool descriptorPool_;
	VkDescriptorSetLayout descriptorSetLayout_;
	VkDescriptorSet descriptorSet_;
	VkPipelineLayout pipelineLayout_;
	VkPipelineCache pipelineCache_;
	VkPipeline pipeline_;
	VkPolygonMode currentPolygonMode_ = VK_POLYGON_MODE_FILL;
	int wireframeIndex_ = -1;
	void initDescriptors();
	void initPipeline();

	// memory mapped vertex buffer
	UIVertex* vertexMapped_ = nullptr;
    uint32_t* indexMapped_ = nullptr;
	int indexCount_ = 0;
	int quadCount_ = 0;

	// ui elements
	bool menuShown_ = false;
	std::vector<Element> defaultElements_{};
	std::vector<Element> menuElements_{};
	void generateElements();

	// text boxes



	// EXTERNAL Vulka API function ptrs
	PFN_vkCmdSetPolygonModeEXT vkCmdSetPolygonModeEXT{ VK_NULL_HANDLE };

};
