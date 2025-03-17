#pragma once

#include <SDL3/SDL.h>

#include <string>

#include "clock.h"
#include "util.h"
#include "gfx.h"
#include "camera.h"
#include "entity.h"

/*
The engine class pretty much just encapsulates everything. 
Keeps it all in one place.. I guess
*/
class Engine {
public:
	// public function to "start" engine
	void run();

	const std::string name_ = "ENGINE";

private:
	bool running_ = false;
	bool visible_ = true;

	// CLock
	Clock clock_;

	// Graphics
	Gfx gfx_;

	// Assets ptr
	Assets* assets_ = nullptr;

	// Player Camera
	Camera camera_;

	// Events
	SDL_Event event_;

	// entities (game world)
	std::vector<Entity> entities_;

	// 3 functions used to initialize, execute, and cleanup the engine
	void init();
	void loop();
	void cleanup();

	// world creation
	void generateWorld();

	// handle events/input
	void handleEvents();
	void handleInputEvent();

    // handles creating the pdated UBO for each frame
    void updateUBO();

    // draw game objects and preent image
    void renderScene();

};
