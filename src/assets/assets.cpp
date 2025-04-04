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
	util::log(name_, "enumerating asset filenames");

	fs::path toCheck = "../res";

	if (!fs::is_directory(toCheck)) {
		throw std::runtime_error("could not find the res/ directory");
	}

	for (auto const& entry : fs::recursive_directory_iterator(toCheck)) {
		if (entry.path().has_extension()) {
			util::log(name_, "found file: " + entry.path().generic_string());
			fs::path ext = entry.path().extension();
			if (ext == ".jpg" || ext == ".png") {
				textureFilenames_.push_back(entry.path().generic_string());
				textureNames_.push_back(entry.path().stem().generic_string());
			}
			if (ext == ".wav") {
				audioFilenames_.push_back(entry.path().generic_string());
			}
			if (ext == ".obj") {
				meshFilenames_.push_back(entry.path().generic_string());
				meshNames_.push_back(entry.path().stem().generic_string());
			}
		}
	}
}

int Assets::getTextureCount() {
	return static_cast<int>(textureFilenames_.size());
}

void Assets::init(VkPhysicalDevice physicalDevice, VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue) {
	util::log(name_, "initializing");

	// textures/images -------------------------------------------====================<
	physicalDevice_ = physicalDevice;
	device_ = device;
	commandPool_ = commandPool;
	graphicsQueue_ = graphicsQueue;


	for (int i = 0; i < textureFilenames_.size(); i++) {
		Texture t;
		t.create(i, textureFilenames_[i], physicalDevice_, device_, commandPool_, graphicsQueue_);
		textures_.push_back(t);
	}

	// Audio -------------------------------------------====================<
	// for now, create SDL audio stream on init, and set audio spec to first file in list..
	if (!SDL_LoadWAV(audioFilenames_[0].c_str(), &audioSpec_, &wavData_, &wavDataLen_)) {
		throw std::runtime_error("failed to load .WAV file: " + audioFilenames_[0]);
	}

	// Create our audio stream in the same format as the .wav file. It'll convert to what the audio hardware wants.
	stream_ = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audioSpec_, NULL, NULL);
	if (!stream_) {
		SDL_Log("Couldn't create audio stream: %s", SDL_GetError());
		throw std::runtime_error("Failed to create SDL audio stream! ");
	}

	// lower volume
	SDL_SetAudioStreamGain(stream_, 0.1f);

	// SDL_OpenAudioDeviceStream starts the device paused. You have to tell it to start!
	SDL_ResumeAudioStreamDevice(stream_);

	// MESHSSS   -------------------------------------------====================<
	loadModels();
	generateMeshs();
}

/*-----------------------------------------------------------------------------
------------------------------TEXTURES-----------------------------------------
-----------------------------------------------------------------------------*/
TextureDetails Assets::getTextureDetails(int index) {
	if (index < 0 || index >= textures_.size()) {
		throw std::runtime_error("texture index out of bounds");
	}

	TextureDetails d;
	d.textureImageView = textures_[index].getTextureImageView();
	d.textureSampler = textures_[index].getTextureSampler();
	return d;
}

/*-----------------------------------------------------------------------------
------------------------------PLAY-SOUNDS--------------------------------------
-----------------------------------------------------------------------------*/
void Assets::playSound(int soundIndex) {
	// free PCM data previously loaded into buffer
	SDL_free(wavData_);

	if (!SDL_LoadWAV(audioFilenames_[soundIndex].c_str(), &audioSpec_, &wavData_, &wavDataLen_)) {
		throw std::runtime_error("Failed to load .WAV file: " + audioFilenames_[soundIndex]);
	}

	// clear audiostream incase of past sounds still replaying:
	// FIXMEEEEE need to add overlapping sounds
	SDL_ClearAudioStream(stream_);

	// feed more data to the stream. It will queue at the end, and trickle out as the hardware needs more data. 
	SDL_PutAudioStreamData(stream_, wavData_, wavDataLen_);
}

/*-----------------------------------------------------------------------------
------------------------------MESHS--------------------------------------------
-----------------------------------------------------------------------------*/
void Assets::loadModels() {
	util::log(name_, "loading models");
	for (int i = 0; i < meshFilenames_.size(); i++) {
		Mesh m;
		m.init(util::getObjData(meshFilenames_[i]), physicalDevice_, device_, commandPool_, graphicsQueue_);
		meshs_.push_back(m);
	}

}

void Assets::generateMeshs() {
	util::log(name_, "generating additional meshs");

	// do this by reading a file? map editor? 
	// shitty way for now

	// skybox mesh
	Mesh skybox;
	skybox.init(Geometry::InvertedTexturedCube::getMeshData(), physicalDevice_, device_, commandPool_, graphicsQueue_);
	meshs_.push_back(skybox);
	meshNames_.push_back("skybox");


	// FLOOR mesh
	Mesh floor;
	floor.init(Geometry::Plane::getMeshData(), physicalDevice_, device_, commandPool_, graphicsQueue_);
	meshs_.push_back(floor);
	meshNames_.push_back("floor");

	// cube mesh
	Mesh cube;
	cube.init(Geometry::TexturedCube::getMeshData(), physicalDevice_, device_, commandPool_, graphicsQueue_);
	meshs_.push_back(cube);
	meshNames_.push_back("cube");
}

/*-----------------------------------------------------------------------------
------------------------------HELPERS------------------------------------------
-----------------------------------------------------------------------------*/
Texture& Assets::getTexture(const std::string name) {
	util::log(name_, "searching for texture: " + name);

	for (int i = 0; i < textureNames_.size(); i++) {
		if (textureNames_[i] == name) {
			return textures_[i];
		}
	}
	throw std::runtime_error("failed to find texture: " + name);
}

int Assets::getTextureIndex(const std::string name) {
	util::log(name_, "searching for texture: " + name);

	for (int i = 0; i < textureNames_.size(); i++) {
		if (textureNames_[i] == name) {
			return i;
		}
	}
	throw std::runtime_error("failed to find texture: " + name);
}

Mesh& Assets::getMesh(const std::string name) {
	util::log(name_, "searching for mesh: " + name);

	for (int i = 0; i < meshNames_.size(); i++) {
		if (meshNames_[i] == name) {
			return meshs_[i];
		}
	}
	throw std::runtime_error("failed to find mesh: " + name);
}

/*-----------------------------------------------------------------------------
------------------------------CLEANUP------------------------------------------
-----------------------------------------------------------------------------*/
void Assets::cleanup() {
	util::log(name_, "cleaning up");

	// texture cleanup here
	for (auto t : textures_) {
		t.cleanup();
	}

	// cleanup meshs (buffers)
	for (auto m : meshs_) {
		m.cleanup();
	}

	util::log(name_, "destroying audio object");
	SDL_free(wavData_);

}