#pragma once

#include <vulkan/vulkan.h>

#include "../lib/stb_image.h"

#include <string>

#include "util.h"
#include "types.h"

class Texture {
public:
	void create(const std::string& filename, const TextureAccess& access);

	const VkImageView& getTextureImageView() const;
	const VkSampler& getTextureSampler() const;

	void cleanup();

	std::string name = "TEXTURE";

private:
	// references to vk stuff
	TextureAccess textureAccess_;

	// name of picture file
	const char* filename_;

	// TEXTURE STUFF
	VkImage textureImage_ = VK_NULL_HANDLE;
	VkDeviceMemory textureImageMemory_ = VK_NULL_HANDLE;
	VkImageView textureImageView_ = VK_NULL_HANDLE;
	VkSampler textureSampler_ = VK_NULL_HANDLE;
	VkImageLayout imageLayout_;
	VkDescriptorImageInfo descriptor_;

	void createVkTexture();

};