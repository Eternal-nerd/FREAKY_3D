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

void Assets::init(const TextureAccess access) {
	util::log(name, "initializing");

	// textures/images -------------------------------------------====================<
	textureAccess_ = access;
	for (int i = 0; i < textureCount_; i++) {
		Texture t;
		t.create(textureFilenames_[i], textureAccess_);
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

}

/*-----------------------------------------------------------------------------
------------------------------TEXTURES-----------------------------------------
-----------------------------------------------------------------------------*/
TextureDetails Assets::getTextureDetails(int index) {
	if (index < 0 || index >= textureCount_) {
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
------------------------------CLEANUP------------------------------------------
-----------------------------------------------------------------------------*/
void Assets::cleanup() {
	util::log(name, "cleaning up");

	// texture cleanup here
	for (auto t : textures_) {
		t.cleanup();
	}

	util::log(name, "destroying audio object");
	SDL_free(wavData_);

}