#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <string>
#include <assert.h>
#include <unordered_map>

#include "../util.h"
#include "../types.h"
#include "../config.h"
#include "../assets/assets.h"
#include "../assets/texture.h"
#include "crosshair.h"
#include "rectangle.h"
#include "container.h"
#include "text.h"
#include "button.h"

// Max. number of elements the text overlay buffer can hold
#define MAX_OVERLAY_QUADS 2048
#define MAX_OVERLAY_LINES 100

/*
class for 2d rendering (GUI/debug text)
*/
class Overlay {
public:
	void init(VkDevice device, VkPhysicalDevice physicalDevice, VkRenderPass renderpass, VkExtent2D extent, Assets& assets);

	OverlayUpdates getUpdates();
	void resetUpdates();

	// API: call start update, then can call update textbox/element methods any amt of times, then, call end update
	void update();

	void updateTextBox(const std::string& label, const std::string& newText);

	// draw entire vertex buffer
	void draw(VkCommandBuffer commandBuffer);

	// input events
	void toggleMenu();
	void toggleWireframe();
	void mouseButtonTrigger(bool state);
	void updateMousePosition(float xPos, float yPos);
	void updateExtent(VkExtent2D extent);

	void cleanup();

	const std::string name_ = "OVERLAY";

private:
	Config config_;
	
	// extent is in here
	OverlayState state_;

	OverlayUpdates updates_;

	// access to vulkan
	VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
	VkDevice device_ = VK_NULL_HANDLE;
	VkRenderPass renderpass_ = VK_NULL_HANDLE;

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
	int wireframeIndex_ = 0;
	void initDescriptors();
	void initPipeline();

	// memory mapped vertex buffer
	UIVertex* vertexMapped_ = nullptr;
    uint32_t* indexMapped_ = nullptr;
	int indexCount_ = 0;
	int quadCount_ = 0;

	// ELEMENTS
	std::vector<Rectangle> rectangles_{};
	std::vector<Text> text_{};
	std::vector<Button> buttons_{};

	// Actual elements that will exist:
	// textbox, slider, button, 

	void generateElements();

	// utility
	OverlayMode getMode(const std::string& modeString);

	// UPDATES
	void handleInputUpdates();
	//void 

	// UTILITY:



	// debug drawing lines, also used for crosshair, also border of containers
	// memory mapped vertex buffer
	Crosshair crosshair_;
	VkBuffer lineVertexBuffer_ = VK_NULL_HANDLE;
	VkDeviceMemory lineVertexBufferMemory_ = VK_NULL_HANDLE;
	UIVertex* lineVertexMapped_ = nullptr;
	int linePointCount_ = 0;



	// EXTERNAL Vulkan API function ptrs
	PFN_vkCmdSetPolygonModeEXT vkCmdSetPolygonModeEXT{ VK_NULL_HANDLE };

};
