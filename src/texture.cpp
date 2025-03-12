#include "texture.h"

/*-----------------------------------------------------------------------------
------------------------------INITIALIZATION-----------------------------------
-----------------------------------------------------------------------------*/
void Texture::create(const std::string& filename, const TextureAccess& access) {
	util::log(name, "creating texture");

	textureAccess_ = access;

	filename_ = filename.c_str();

	createVkTexture();
}

/*-----------------------------------------------------------------------------
-----------------------------GETTERS-------------------------------------------
-----------------------------------------------------------------------------*/
const VkImageView& Texture::getTextureImageView() const { return textureImageView_; }
const VkSampler& Texture::getTextureSampler() const { return textureSampler_; }

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
		stagingBuffer, stagingBufferMemory, textureAccess_.device, textureAccess_.physicalDevice);

	void* data;
	vkMapMemory(textureAccess_.device, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(textureAccess_.device, stagingBufferMemory);

	stbi_image_free(pixels);

	util::createImage(
		texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage_, textureImageMemory_,
		textureAccess_.device, textureAccess_.physicalDevice);

	util::transitionImageLayout(textureImage_, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, 
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, textureAccess_.device, textureAccess_.commandPool, textureAccess_.graphicsQueue);
	imageLayout_ = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	util::copyBufferToImage(stagingBuffer, textureImage_, static_cast<uint32_t>(texWidth), 
		static_cast<uint32_t>(texHeight), textureAccess_.device, textureAccess_.commandPool, textureAccess_.graphicsQueue);
	util::transitionImageLayout(textureImage_, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, textureAccess_.device, textureAccess_.commandPool, textureAccess_.graphicsQueue);

	vkDestroyBuffer(textureAccess_.device, stagingBuffer, nullptr);
	vkFreeMemory(textureAccess_.device, stagingBufferMemory, nullptr);

	// TEXTURE IMAGE VIEW ------------------------------====<
	textureImageView_ = util::createImageView(textureImage_, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, textureAccess_.device);
	
	// TEXTURE IMAGE SAMPLER ------------------------------====<
	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(textureAccess_.physicalDevice, &properties);

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

	if (vkCreateSampler(textureAccess_.device, &samplerInfo, nullptr, &textureSampler_) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture sampler!");
	}
}


/*-----------------------------------------------------------------------------
------------------------------CLEANUP------------------------------------------
-----------------------------------------------------------------------------*/
void Texture::cleanup() {
	util::log(name, "cleaning up texture");

	// CLEANUP TEXTURES
	util::log(name, "destroying texture sampler");
	vkDestroySampler(textureAccess_.device, textureSampler_, nullptr);
	util::log(name, "destroying texture image view");
	vkDestroyImageView(textureAccess_.device, textureImageView_, nullptr);
	util::log(name, "destroying texture image");
	vkDestroyImage(textureAccess_.device, textureImage_, nullptr);
	util::log(name, "destroying texture image memory");
	vkFreeMemory(textureAccess_.device, textureImageMemory_, nullptr);
}