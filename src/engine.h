#pragma once

#include <SDL3/SDL.h>

#include <string>

#include "clock.h"
#include "util.h"
#include "gfx.h"
#include "camera.h"
#include "entity.h"
#include "phys/rigid_body.h"
#include "phys/simulation.h"

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
	bool paused_ = false;

	// FIXME FUN
	glm::vec3 cubeRotation_ = {10.f,20.f,30.f};

	// Clock
	Clock clock_;
	float fpsTime_ = 0.f;
	int loopsMeasured_ = 0;

	// Graphics
	Gfx gfx_;

	// Assets ptr
	Assets* assets_ = nullptr;

	// overlay ptr
	Overlay* overlay_ = nullptr;
	std::string fpsString_ = "";

	// Player Camera
	Camera camera_;
	RigidBody* camBody_ = nullptr;

	// Events
	SDL_Event event_;

	// keys state machine
	KeyStates keys_{};

	// entities (game world)
	std::vector<Entity> entities_;

	// physics
	Simulation sim_;

	// 3 functions used to initialize, execute, and cleanup the engine
	void init();
	void loop();
	void cleanup();

	// world creation
	void generateWorld();

	// handle events/input
	void handleEvents();
	void captureKeyState();
	void handleMouseEvent();
	void handleKeyEvent();
	void togglePause();

    // handles creating the pdated UBO for each frame
    void updateUBO();

	// handles updating text
	void updateOverlay();

    // draw game objects and preent image
    void renderScene();
};
