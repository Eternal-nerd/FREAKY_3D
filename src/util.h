#pragma once

#include "../lib/tiny_obj_loader.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include <string>
#include <vector>
#include <mutex>

#include "types.h"

/*
The util namespace should include all kinds of helper functions
Logging, Math, File IO, Vulkan helpers?, etc.
*/
namespace util {
	// loggin
	void log(const std::string& src, const std::string& msg);

	// Vulkan/SDL device/instance selection/creation
	std::vector<const char*> getRequiredExtensions();
	bool checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice, const std::vector<const char*>& deviceExtensions);
	bool isDeviceSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, const std::vector<const char*>& deviceExtensions);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

	// DEBUGGING
	bool checkValidationLayerSupport(const std::vector<const char*>& validationLayers);
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

	// SWAPCHAIN details
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, SDL_Window* window);

	// Depth
	VkFormat findDepthFormat(const VkPhysicalDevice& physicalDevice);
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features, const VkPhysicalDevice& physicalDevice);

	// Image shit
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkDevice& device);
	void createImage(uint32_t width, uint32_t height, VkFormat format,
		VkImageTiling tiling, VkImageUsageFlags usage,
		VkMemoryPropertyFlags properties, VkImage& image,
		VkDeviceMemory& imageMemory, VkDevice& device, 
		VkPhysicalDevice& physicalDevice);
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue);

	// BUffers/memory stuff
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, const VkPhysicalDevice& physicalDevice);
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory, const VkDevice& device, const VkPhysicalDevice& physicalDevice);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkPhysicalDevice physicalDevice, VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue);
	
	// COMMAND BUFFERS
	VkCommandBuffer beginSingleTimeCommands(VkDevice device, VkCommandPool commandPool);
	void endSingleTimeCommands(VkCommandBuffer commandBuffer, VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue);

	// File IO
	MeshData getObjData(const std::string obj_filename);
	std::vector<char> readFile(const std::string& filename);

	// SHADERS
	VkShaderModule createShaderModule(const std::vector<char>& code, const VkDevice& device);

	//MISC
	void checkNPos(std::string::size_type t);
}