#include "engine.h"

#include <thread>

/*-----------------------------------------------------------------------------
-----------------------------ONLY-PUBLIC-METHOD--------------------------------
-----------------------------------------------------------------------------*/
void Engine::run() {
	util::log(name, "running engine");
	init();
	loop();
	cleanup();
}

/*-----------------------------------------------------------------------------
------------------------------INITIALIZATION-----------------------------------
-----------------------------------------------------------------------------*/
void Engine::init() {
	util::log(name, "initializing");

  // Kick off asset pipeline on background thread
  std::thread asset_thread(&Assets::init, &assets_);

  clock_.init();

	gfx_.init();

  asset_thread.join();

}

/*-----------------------------------------------------------------------------
------------------------------MAIN-LOOP----------------------------------------
-----------------------------------------------------------------------------*/
void Engine::loop() {
	util::log(name, "beginning the main loop");

	running_ = true;
	while (running_) {

		handleEvents();

        if (visible_) {
            // RENDER stuff
            gfx_.render();
        }

	}

}

/*-----------------------------------------------------------------------------
------------------------------INPUT--------------------------------------------
-----------------------------------------------------------------------------*/
void Engine::handleEvents() {
    // Poll events (inputs)
    while (SDL_PollEvent(&event_)) {
        switch (event_.type) {
            // X OUT OF WINDOW
        case SDL_EVENT_QUIT:
            running_ = false;
            break;
            // Input EVENTS
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
        case SDL_EVENT_MOUSE_MOTION:
            handleInputEvent();
            break;
            // MINIMIZE/MAXIMIZE EVENTS
        case SDL_EVENT_WINDOW_HIDDEN:
        case SDL_EVENT_WINDOW_MINIMIZED:
            visible_ = false;
            break;
        case SDL_EVENT_WINDOW_SHOWN:
        case SDL_EVENT_WINDOW_MAXIMIZED:
        case SDL_EVENT_WINDOW_RESTORED:
            visible_ = true;
            break;
        default: break;
        }
    }
}

void Engine::handleInputEvent() {
    if (event_.key.scancode == SDL_SCANCODE_ESCAPE) { 
        running_ = false; 
    }

    if (event_.type != SDL_EVENT_MOUSE_MOTION) {
        // update keystates struct
        bool down = event_.type == SDL_EVENT_KEY_DOWN;
        switch (event_.key.scancode) {
        case SDL_SCANCODE_W:
            //keys_.w = down;
            break;
        }
    }
}

/*-----------------------------------------------------------------------------
------------------------------CLEANUP------------------------------------------
-----------------------------------------------------------------------------*/
void Engine::cleanup() {
	util::log(name, "cleaning up");

	gfx_.cleanup();
}
