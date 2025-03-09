#include "assets.h"

#include <thread>
#include <chrono>


void Assets::init() {
	util::log(name, "initializing");
	
	// EXECUTES ON BACKGROUND THREAD!!!!!!!!!!!!!!!!!!!!!!!!!!  ~~~----=========<
	// TODO: enumerate asset filenames in vectors (image, audio, mesh, etc.) that are listed in config file
	// save the count of the each filetype and signal that its ready
	// the counts will be shared data
	// can reformat data after the signal, then 
	// create command pool to create vulkan objects (textures)
	// see ( Cmdr::beginSingleTimeCommands() - 3D_MODEL_LOADER)
  
  // TESTING 
  for (int i=0; i<10; i++) {
    std::string s = "Count: " + std::to_string(i);
    util::log(name, s);    
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  
}
