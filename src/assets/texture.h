#pragma once

#include <vulkan/vulkan.h>

#include "../../lib/stb_image.h"

#include <string>

#include "../util.h"
#include "../types.h"

class Texture {
public:
	void create(int index, const std::string& filename, const GfxAccess& access);

	const VkImageView& getTextureImageView() const;
	const VkSampler& getTextureSampler() const;
	int getIndex();

	void cleanup();

	const std::string name_ = "TEXTURE";

private:
	// index of texture in list
	int index_ = -1;

	// references to vk stuff
	GfxAccess access_;

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