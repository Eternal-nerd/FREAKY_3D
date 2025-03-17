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

	// HELPERs
	Texture& getTexture(const std::string name);
	Mesh& getMesh(const std::string name);

	void playSound(int soundIndex);

	void cleanup();

	const std::string name_ = "ASSETS";

private:
	// vk access
	GfxAccess access_;

	// Textures
	std::vector<std::string> textureFilenames_{};
	std::vector<Texture> textures_{};
	std::vector<std::string> textureNames_{};

	// Audio
	std::vector<std::string> audioFilenames_{};
	SDL_AudioStream* stream_ = NULL;
	Uint8* wavData_ = NULL;
	Uint32 wavDataLen_ = 0;
	SDL_AudioSpec audioSpec_;

	// Models / Meshs
	std::vector<std::string> meshFilenames_{};
	std::vector<Mesh> meshs_{};
	std::vector<std::string> meshNames_{};
	void loadModels();
	void generateMeshs();


};
