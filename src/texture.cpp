#include "texture.h"

/*-----------------------------------------------------------------------------
------------------------------INITIALIZATION-----------------------------------
-----------------------------------------------------------------------------*/
void Texture::create(int index, const std::string& filename, const GfxAccess& access) {
	util::log(name_, "creating texture");

	index_ = index;

	access_ = access;

	filename_ = filename.c_str();

	createVkTexture();
}

/*-----------------------------------------------------------------------------
-----------------------------GETTERS-------------------------------------------
-----------------------------------------------------------------------------*/
const VkImageView& Texture::getTextureImageView() const { return textureImageView_; }
const VkSampler& Texture::getTextureSampler() const { return textureSampler_; }
int Texture::getIndex() { return index_; }

/*-----------------------------------------------------------------------------
------------------------------VK-TEXTURE----------------------------------
-----------------------------------------------------------------------------*/
void Texture::createVkTexture() {
	// TEXTURE IMAGE ------------------------------====<
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(filename_, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = texWidth * texHeight * 4;

	if (!pixels) {
		throw std::runtime_error("failed to load texture image!");
	}

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	util::createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer, stagingBufferMemory, access_.device, access_.physicalDevice);

	void* data;
	vkMapMemory(access_.device, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(access_.device, stagingBufferMemory);

	stbi_image_free(pixels);

	util::createImage(
		texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage_, textureImageMemory_,
		access_.device, access_.physicalDevice);

	util::transitionImageLayout(textureImage_, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, 
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, access_.device, access_.commandPool, access_.graphicsQueue);
	imageLayout_ = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	util::copyBufferToImage(stagingBuffer, textureImage_, static_cast<uint32_t>(texWidth), 
		static_cast<uint32_t>(texHeight), access_.device, access_.commandPool, access_.graphicsQueue);
	util::transitionImageLayout(textureImage_, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, access_.device, access_.commandPool, access_.graphicsQueue);

	vkDestroyBuffer(access_.device, stagingBuffer, nullptr);
	vkFreeMemory(access_.device, stagingBufferMemory, nullptr);

	// TEXTURE IMAGE VIEW ------------------------------====<
	textureImageView_ = util::createImageView(textureImage_, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, access_.device);
	
	// TEXTURE IMAGE SAMPLER ------------------------------====<
	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(access_.physicalDevice, &properties);

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_FALSE;
	samplerInfo.maxAnisotropy =	properties.limits.maxSamplerAnisotropy; // FIXME -> 1.0f
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

	if (vkCreateSampler(access_.device, &samplerInfo, nullptr, &textureSampler_) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture sampler!");
	}
}


/*-----------------------------------------------------------------------------
------------------------------CLEANUP------------------------------------------
-----------------------------------------------------------------------------*/
void Texture::cleanup() {
	util::log(name_, "cleaning up texture");

	// CLEANUP TEXTURES
	util::log(name_, "destroying texture sampler");
	vkDestroySampler(access_.device, textureSampler_, nullptr);
	util::log(name_, "destroying texture image view");
	vkDestroyImageView(access_.device, textureImageView_, nullptr);
	util::log(name_, "destroying texture image");
	vkDestroyImage(access_.device, textureImage_, nullptr);
	util::log(name_, "destroying texture image memory");
	vkFreeMemory(access_.device, textureImageMemory_, nullptr);
}