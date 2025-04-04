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

    overlay_ = &gfx_.getOverlay();

    float aspect = gfx_.getAspect();

    camera_.init(aspect);
    camBody_ = camera_.getBodyPtr();

    // generate/init game objects & world
    generateWorld();
    
    // get all rigid body ptrs from camera and game objects,
    // pass them to physics sim. and initialize model matrices
    sim_.init(clock_);

    // register camera rigid body and all entities
    sim_.registerBody(camBody_);
    for (auto entity : entities_) {
        sim_.registerBody(entity.getBodyPtr());
    }

}

/*-----------------------------------------------------------------------------
------------------------------MAIN-LOOP----------------------------------------
-----------------------------------------------------------------------------*/
void Engine::loop() {
	util::log(name_, "beginning the main loop");

	running_ = true;
	while (running_) {

		handleEvents();

        // step physics sim?
        sim_.stepSimulation();

        if (visible_) {
            camera_.update(gfx_.getAspect());
            updateUBO();
            updateOverlay();
            // RENDER stuff
            renderScene();
        }
	}
    gfx_.deviceWaitIdle();
}

/*-----------------------------------------------------------------------------
-----------------------------UPDATE-STUFF--------------------------------------
-----------------------------------------------------------------------------*/
void Engine::updateUBO() {
    float time = clock_.getProgramTime();
    
    // FIXME animations
    entities_[0].setOrientation(10*time, 20*time, 30*time);
    //entities_[1].setOrientation(10 * time, 20 * time, 30 * time);

    UniformBufferObject ubo{};

    // model transforms (obtined from each entity's rigid body) --=====<
    for (int i=0; i<entities_.size(); i++) {
        ubo.model[i] = entities_[i].getModelMat();
    } 

    // Camera matrices -------------------------=========<
    ubo.view = camera_.getViewMatrix();
    ubo.proj = camera_.getPerspectiveMatrix();

    // wait for frame to be ready
    gfx_.waitFrame();

    gfx_.mapUBO(ubo);
}

// TODO: step physics sim.

void Engine::updateOverlay() {
    overlay_->startUpdate();

    // do stuff here.

    overlay_->endUpdate();
}

/*-----------------------------------------------------------------------------
-----------------------------GRAPHICS------------------------------------------
-----------------------------------------------------------------------------*/
void Engine::renderScene() {
    VkCommandBuffer commandBuffer = gfx_.setupCommandBuffer();
    
    // draw entities
    for (int i = 0; i < entities_.size(); i++) {
        entities_[i].draw(commandBuffer);
    }

    // draw text
    overlay_->draw(commandBuffer);

    gfx_.submitCommandBuffer(commandBuffer);

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
            captureKeyState();
            handleKeyEvent();
            break;
        case SDL_EVENT_MOUSE_MOTION:
            handleMouseEvent();
            break;
            // MINIMIZE/MAXIMIZE EVENTS
        case SDL_EVENT_WINDOW_HIDDEN:
        case SDL_EVENT_WINDOW_MINIMIZED:
            util::log(name_, "window no longer visible");
            visible_ = false;
            break;
        case SDL_EVENT_WINDOW_SHOWN:
        case SDL_EVENT_WINDOW_MAXIMIZED:
        case SDL_EVENT_WINDOW_RESTORED:
            util::log(name_, "window is now visible");
            visible_ = true;
            break;
        default: break;
        }
    }
}

void Engine::captureKeyState() {
    // KEY INPUT
    bool down = event_.type == SDL_EVENT_KEY_DOWN;
    switch (event_.key.scancode) {
    case SDL_SCANCODE_ESCAPE:
        keys_.esc = down;
        break;
    case SDL_SCANCODE_P:
        keys_.p = down;
        break;
    case SDL_SCANCODE_W:
        keys_.w = down;
        break;
    case SDL_SCANCODE_A:
        keys_.a = down;
        break;
    case SDL_SCANCODE_S:
        keys_.s = down;
        break;
    case SDL_SCANCODE_D:
        keys_.d = down;
        break;
    case SDL_SCANCODE_SPACE:
        keys_.space = down;
        break;
    case SDL_SCANCODE_LSHIFT:
        keys_.shift = down;
        break;
    case SDL_SCANCODE_LCTRL:
        keys_.ctrl = down;
        break;
    case SDL_SCANCODE_N:
        keys_.n = down;
        break;
    default: break;
    }
}

void Engine::handleMouseEvent() {
    // MOUSE EVENTS
    if (!paused_) {
        if (event_.type == SDL_EVENT_MOUSE_MOTION) {
            camera_.incrementYaw((float)event_.motion.xrel / 250);
            camera_.incrementPitch((float)event_.motion.yrel / 250);
        }
    }
}

void Engine::handleKeyEvent() {
    if (keys_.esc) { togglePause(); }
    if (keys_.p && !keys_.shift) { gfx_.togglePolygonMode(); }
    if (keys_.p && keys_.shift) { overlay_->toggleWireframe(); }

    // TODO add physics sim that is synced with program time so that 
    // I can just update the camera's body's velocity here
    // and the sim will automatically update the position
    // CAMERA SHIT
    float sprintDiv = keys_.shift ? 1.f : 3.f;
    glm::vec3 initialVel = camera_.getBodyPtr()->getVelocity();
    // Z DIRECTION
    if (keys_.w) { initialVel.z = -NOCLIP_SPEED / sprintDiv; }
    if (keys_.s) { initialVel.z = NOCLIP_SPEED / sprintDiv; }
    if (keys_.w == keys_.s) { initialVel.z = 0; }
    // X DIRECTION
    if (keys_.a) { initialVel.x = -NOCLIP_SPEED / sprintDiv; }
    if (keys_.d) { initialVel.x = NOCLIP_SPEED / sprintDiv; }
    if (keys_.a == keys_.d) { initialVel.x = 0; }
    // Y DIRECTION
    if (keys_.ctrl) { initialVel.y = -NOCLIP_SPEED / sprintDiv; }
    if (keys_.space) { initialVel.y = NOCLIP_SPEED / sprintDiv; }
    if (keys_.ctrl == keys_.space) { initialVel.y = 0; }
    // UPDATE Velocity
    camera_.getBodyPtr()->setVelocity(initialVel);
}

void Engine::togglePause() {
    util::log(name_, "pausing");
    paused_ = !paused_;
    gfx_.toggleMouseMode(paused_);
    overlay_->toggleMenu();
}

/*-----------------------------------------------------------------------------
-----------------------------WORLD-GEN-----------------------------------------
-----------------------------------------------------------------------------*/
void Engine::generateWorld() {
    util::log(name_, "generating game world");

    // mclovin cube
    Entity e1;
    e1.init(0, assets_->getTexture("mclovin"), assets_->getMesh("cube"));
    e1.setPosition(0,0,-10);
    e1.setOrientation(0,0,180);
    entities_.push_back(e1);

    // skybox
    Entity e2;
    e2.init(1, assets_->getTexture("skybox"), assets_->getMesh("skybox"));
    e2.scale(1000.f, 1000.f, 1000.f);
    entities_.push_back(e2);

}

/*-----------------------------------------------------------------------------
------------------------------CLEANUP------------------------------------------
-----------------------------------------------------------------------------*/
void Engine::cleanup() {
	util::log(name_, "cleaning up");

	gfx_.cleanup();
}
