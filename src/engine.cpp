#include "engine.h"

#include <thread>

/*-----------------------------------------------------------------------------
-----------------------------ONLY-PUBLIC-METHOD--------------------------------
-----------------------------------------------------------------------------*/
void Engine::run() {
	util::log(name_, "running engine");
	init();
	loop();
	cleanup();
}

/*-----------------------------------------------------------------------------
------------------------------INITIALIZATION-----------------------------------
-----------------------------------------------------------------------------*/
void Engine::init() {
    util::log(name_, "initializing");

    clock_.init();

    gfx_.init();

    assets_ = &gfx_.getAssets();

    float aspect = gfx_.getAspect();

    camera_.init(aspect);

    // generate/init game objects & world
    generateWorld();

    // get all rigid body ptrs from camera and game objects,
    // pass them to physics sim. and initialize model matrices

}

/*-----------------------------------------------------------------------------
------------------------------MAIN-LOOP----------------------------------------
-----------------------------------------------------------------------------*/
void Engine::loop() {
	util::log(name_, "beginning the main loop");

	running_ = true;
	while (running_) {

		handleEvents();

        if (visible_) {
            camera_.update(gfx_.getAspect());
            updateUBO();
            // RENDER stuff
            renderScene();
        }
	}
}

/*-----------------------------------------------------------------------------
-----------------------------UPDATE-STUFF--------------------------------------
-----------------------------------------------------------------------------*/
void Engine::updateUBO() {

}

// TODO: step physics sim.

/*-----------------------------------------------------------------------------
-----------------------------GRAPHICS------------------------------------------
-----------------------------------------------------------------------------*/
void Engine::renderScene() {
    
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
-----------------------------WORLD-GEN-----------------------------------------
-----------------------------------------------------------------------------*/
void Engine::generateWorld() {
    util::log(name_, "generating game world");

    // mclovin cube
    Entity e1;
    RigidBodyProperties p1;
    e1.init(0, assets_->getTexture("mclovin"), assets_->getMesh("cube"), p1);
    entities_.push_back(e1);

}

/*-----------------------------------------------------------------------------
------------------------------CLEANUP------------------------------------------
-----------------------------------------------------------------------------*/
void Engine::cleanup() {
	util::log(name_, "cleaning up");

	gfx_.cleanup();
}
