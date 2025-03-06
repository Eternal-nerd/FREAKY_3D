#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include <string>
#include <vector>

#include "util.h"

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

const uint32_t WIDTH = 1600;
const uint32_t HEIGHT = 800;

const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

/*
The Gfx class will encapsulate/insulate the Vulkan API. 
There are some wrapper classes that will also be used to abstract
Vulkan objects, all should be encapsulated by this class tho.
*/
class Gfx {
public: 

	void init();

	void render();

	void cleanup();

	std::string name = "GFX";

private:
	// SDL Shit
	SDL_Window* window_ = nullptr;

	// Vulkan DEVICE Stuff ------------------------==<
	VkDebugUtilsMessengerEXT debugMessenger_ = VK_NULL_HANDLE;
	VkInstance instance_ = VK_NULL_HANDLE;
	VkSurfaceKHR surface_ = VK_NULL_HANDLE;
	VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
	VkDevice device_ = VK_NULL_HANDLE;
	VkQueue graphicsQueue_ = VK_NULL_HANDLE;
	VkQueue presentQueue_ = VK_NULL_HANDLE;
	void createDevice();


	// Vulkan Renderpass ------------------------==<
	VkRenderPass renderPass_ = VK_NULL_HANDLE;
	void createRenderPass();

	// Vulkan Swapchain -----------------------===<
	VkSwapchainKHR swapChain_ = VK_NULL_HANDLE;
	std::vector<VkImage> swapChainImages_;
	VkFormat swapChainImageFormat_;
	VkExtent2D swapChainExtent_;
	std::vector<VkImageView> swapChainImageViews_;
	std::vector<VkFramebuffer> swapChainFramebuffers_;
	VkImage depthImage_ = VK_NULL_HANDLE;
	VkDeviceMemory depthImageMemory_ = VK_NULL_HANDLE;
	VkImageView depthImageView_ = VK_NULL_HANDLE;
	void createSwapchain();
	void recreateSwapchain();
	void cleanupSwapchain();

	// Vulkan descriptor layout & pipeline --------------------===<
	VkDescriptorSetLayout descriptorSetLayout_ = VK_NULL_HANDLE;
	VkPipelineLayout pipelineLayout_ = VK_NULL_HANDLE;
	VkPipeline graphicsPipeline_ = VK_NULL_HANDLE;
	//void createDescriptorSetLayout();
	//void createGraphicsPipeline(VkPolygonMode mode = VK_POLYGON_MODE_FILL);
	//void recreatePipeline(VkPolygonMode mode);





};