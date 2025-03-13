#pragma once

#include <string>
#include <vector>

#include "util.h"
#include "types.h"
#include "texture.h"
#include "mesh.h"

/*
Class in charge of files/data on disk used by the engine
handles loading config, image, audio, and mesh data
*/
class Assets {
public:
	// needed for descriptor set layout
	void enumerateFiles();
	int getTextureCount();

	void init(const GfxAccess access);

	TextureDetails getTextureDetails(int index);

	void playSound(int soundIndex);

	void cleanup();

	const std::string name_ = "ASSETS";

private:
	// vk access
	GfxAccess access_;

	// Textures
	int textureFileCount_ = 0;
	std::vector<std::string> textureFilenames_{};
	std::vector<Texture> textures_{};

	// Audio
	int audioFileCount_ = 0;
	std::vector<std::string> audioFilenames_{};
	SDL_AudioStream* stream_ = NULL;
	Uint8* wavData_ = NULL;
	Uint32 wavDataLen_ = 0;
	SDL_AudioSpec audioSpec_;

	// Models / Meshs
	int modelFileCount_ = 0;
	std::vector<std::string> modelFilenames_{};
	std::vector<Mesh> meshs_{};
	void loadModels();
	void generateMeshs();


};
