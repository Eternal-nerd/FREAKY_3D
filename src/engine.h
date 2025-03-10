#pragma once

#include <SDL3/SDL.h>

#include <string>

#include "clock.h"
#include "util.h"
#include "gfx.h"
#include "assets.h"

/*
The engine class pretty much just encapsulates everything. 
Keeps it all in one place.. I guess
*/
class Engine {
public:
	// public function to "start" engine
	void run();

	std::string name = "ENGINE";

private:
	bool running_ = false;
	bool visible_ = true;

  // Asset manager
  Assets assets_; 

	// clock
	Clock clock_;

	// Graphics
	Gfx gfx_;

	// Events
	SDL_Event event_;

	// 3 functions used to initialize, execute, and cleanup the engine
	void init();
	void loop();
	void cleanup();

	// handle events/input
	void handleEvents();
	void handleInputEvent();
	

};
