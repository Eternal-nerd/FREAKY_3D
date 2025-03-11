#pragma once

#include <string>
#include <vector>

#include "util.h"
#include "types.h"

/*
Class in charge of files/data on disk used by the engine
handles loading config, image, audio, and mesh data
*/
class Assets {
public:
	// needed for descriptor set layout
	void enumerateFiles();
	int getTextureCount();

	void init(VkDevice device, VkPhysicalDevice physicalDevice);



	std::string name = "ASSETS";

private:
	// references to devices
	VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
	VkDevice device_ = VK_NULL_HANDLE;

	// Textures
	int textureCount_ = 0;
	std::vector<std::string> textureFilenames_{};
	


	// Audio
	int audioCount_ = 0;
	std::vector<std::string> audioFilenames_{};


	// Mesh
	int meshCount_ = 0;
	std::vector<std::string> meshFilenames_{};

};