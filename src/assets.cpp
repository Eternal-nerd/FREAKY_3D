#include "assets.h"

#include <thread>
#include <chrono>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

/*-----------------------------------------------------------------------------
------------------------------INITIALIZATION-----------------------------------
-----------------------------------------------------------------------------*/
void Assets::enumerateFiles() {
	util::log(name, "enumerating asset filenames");

	fs::path toCheck = "../assets";

	if (!fs::is_directory(toCheck)) {
		throw std::runtime_error("could not find the assets/ directory");
	}

	for (auto const& entry : fs::recursive_directory_iterator(toCheck)) {
		if (entry.path().has_extension()) {
			util::log(name, "found file: " + entry.path().generic_string());
			fs::path ext = entry.path().extension();
			if (ext == ".jpg" || ext == ".png") {
				textureFilenames_.push_back(entry.path().generic_string());
				textureCount_++;
			}
			if (ext == ".wav") {
				audioFilenames_.push_back(entry.path().generic_string());
				audioCount_++;
			}
			if (ext == ".obj") {
				meshFilenames_.push_back(entry.path().generic_string());
				meshCount_++;
			}
		}
	}
}

int Assets::getTextureCount() {
	return textureCount_;
}

void Assets::init(VkDevice device, VkPhysicalDevice physicalDevice) {
	util::log(name, "initializing");

	device_ = device;
	physicalDevice_ = physicalDevice;	


	// TESTING 
	for (int i=0; i<10; i++) {
		std::string s = "Count: " + std::to_string(i);
		util::log(name, s);
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
  
}

/*-----------------------------------------------------------------------------
------------------------------CLEANUP------------------------------------------
-----------------------------------------------------------------------------*/