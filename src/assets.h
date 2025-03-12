#pragma once

#include <string>
#include <vector>

#include "util.h"
#include "types.h"
#include "texture.h"

/*
Class in charge of files/data on disk used by the engine
handles loading config, image, audio, and mesh data
*/
class Assets {
public:
	// needed for descriptor set layout
	void enumerateFiles();
	int getTextureCount();

	void init(const TextureAccess access);

	TextureDetails getTextureDetails(int index);

	void playSound(int soundIndex);

	void cleanup();

	std::string name = "ASSETS";

private:
	// references to vk stuff
	TextureAccess textureAccess_;

	// Textures
	int textureCount_ = 0;
	std::vector<std::string> textureFilenames_{};
	std::vector<Texture> textures_{};

	// Audio
	int audioCount_ = 0;
	std::vector<std::string> audioFilenames_{};
	SDL_AudioStream* stream_ = NULL;
	Uint8* wavData_ = NULL;
	Uint32 wavDataLen_ = 0;
	SDL_AudioSpec audioSpec_;

	// Mesh
	int meshCount_ = 0;
	std::vector<std::string> meshFilenames_{};

};