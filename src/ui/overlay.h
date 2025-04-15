#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <string>
#include <assert.h>

#include "../util.h"
#include "../types.h"
#include "../assets/assets.h"
#include "../assets/texture.h"
#include "text.h"
#include "rectangle.h"

// Max. number of elements the text overlay buffer can hold
#define MAX_OVERLAY_QUADS 2048
#define MAX_OVERLAY_LINES 100
// FIXME make sure this is being used properly to set buffer size

/*
class for 2d rendering (GUI/debug text)
*/
class Overlay {
public:
	void init(VkDevice device, VkPhysicalDevice physicalDevice, VkRenderPass renderpass, VkExtent2D extent, Assets& assets);

	OverlayUpdates getUpdates();
	void resetUpdates();

	// API: call start update, then can call update textbox/element methods any amt of times, then, call end update
	void startUpdate();

	void updateMousePosition(float xPos, float yPos);

	void updateTextBox(int index, const std::string& newText);
	//void updateElement(int index);
	bool checkTextBoxMessage(int index, const std::string& compareString);

	void endUpdate();

	// draw entire vertex buffer
	void draw(VkCommandBuffer commandBuffer);

	void updateExtent(VkExtent2D extent);
	void toggleWireframe();
	void toggleMenu();
	void mouseButtonTrigger(bool state);

	void cleanup();

	const std::string name_ = "OVERLAY";

private:
	bool mouseDown_ = false;
	bool mouseRelease_ = false;

	// tracked mouse position [0,1]
	glm::vec2 mousePos_ = { 0.f,0.f };

	bool menuShown_ = false;

	OverlayUpdates updates_;

	// access to vulkan
	VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
	VkDevice device_ = VK_NULL_HANDLE;
	VkRenderPass renderpass_ = VK_NULL_HANDLE;

	// extent used for scaling
	VkExtent2D extent_;
	bool initialized_ = false;

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

	// FIXME TESTING
	//std::vector<>;
	Text testText_;
	Rectangle testRect_;
	std::vector<Rectangle> rectangles_{};
	std::vector<Text> text_{};
	//std::vector<>

	void generateElements();


	// UPDATES
	void handleInputUpdates();

	// UTILITY:



	// debug drawing lines, also used for crosshair
	// memory mapped vertex buffer
	VkBuffer lineVertexBuffer_ = VK_NULL_HANDLE;
	VkDeviceMemory lineVertexBufferMemory_ = VK_NULL_HANDLE;
	UIVertex* lineVertexMapped_ = nullptr;
	int lineCount_ = 0;
	void mapLines();



	// EXTERNAL Vulkan API function ptrs
	PFN_vkCmdSetPolygonModeEXT vkCmdSetPolygonModeEXT{ VK_NULL_HANDLE };

};
