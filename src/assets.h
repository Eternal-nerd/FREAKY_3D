#pragma once

#include <string>

#include "util.h"


/*
Class in charge of files/data on disk used by the engine
handles loading config, image, audio, and mesh data
runs on background thread, so needs synchronization with
main thread
*/
class Assets {
public:

	void init();

	std::string name = "ASSETS";

private:


};