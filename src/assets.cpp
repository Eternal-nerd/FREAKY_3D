#include "assets.h"

void Assets::init() {
	util::log(name, "initializing");
	
	// EXECUTES ON BACKGROUND THREAD!!!!!!!!!!!!!!!!!!!!!!!!!!  ~~~----=========<
	// TODO: enumerate asset filenames in vectors (image, audio, mesh, etc.) that are listed in config file
	// save the count of the each filetype and signal that its ready
	// the counts will be shared data
	// can reformat data after the signal, then 
	// create command pool to create vulkan objects (textures)
	// see ( Cmdr::beginSingleTimeCommands() - 3D_MODEL_LOADER)
}