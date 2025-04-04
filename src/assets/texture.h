#pragma once

#include <vulkan/vulkan.h>

#include "../../lib/stb_image.h"

#include <string>

#include "../util.h"
#include "../types.h"

class Texture {
public:
	void create(int index, const std::string& filename, VkPhysicalDevice physicalDevice, VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue);

	const VkImageView& getTextureImageView() const;
	const VkSampler& getTextureSampler() const;
	int getIndex();

	void cleanup();

	const std::string name_ = "TEXTURE";

private:
	// index of texture in list
	int index_ = -1;

	// references to vk stuff
	VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
	VkDevice device_ = VK_NULL_HANDLE;
	VkCommandPool commandPool_ = VK_NULL_HANDLE;
	VkQueue graphicsQueue_ = VK_NULL_HANDLE;

	// name of picture file
	const char* filename_;

	// TEXTURE STUFF
	VkImage textureImage_ = VK_NULL_HANDLE;
	VkDeviceMemory textureImageMemory_ = VK_NULL_HANDLE;
	VkImageView textureImageView_ = VK_NULL_HANDLE;
	VkSampler textureSampler_ = VK_NULL_HANDLE;
	VkImageLayout imageLayout_;

	void createVkTexture();

};