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
    for (auto &entity : entities_) {
        sim_.registerBody(entity.getBodyPtr());
    }

}

/*-----------------------------------------------------------------------------
------------------------------MAIN-LOOP----------------------------------------
-----------------------------------------------------------------------------*/
void Engine::loop() {
	util::log(name_, "beginning the main loop");

    // FIXME
    std::string testt = "abcdefghijklmnopqrstuvwxyz";
    int idx = 1;
    bool upward = true;
    // END FIXME

	running_ = true;
	while (running_) {
        float frameStart = clock_.getProgramTime();

		handleEvents();

        // step physics sim?
        sim_.stepSimulation();

        if (visible_) {
            camera_.update(gfx_.getAspect());
            updateUBO();
            updateOverlay();
            // RENDER stuff, mesure frametime
            renderScene();
        }

        // limit fps if wanted
        //std::this_thread::sleep_for(std::chrono::milliseconds(50));

        // measure FPS here
        float frameEnd = clock_.getProgramTime();
        fpsTime_ += frameEnd - frameStart;
        loopsMeasured_++;

        if (loopsMeasured_ > FPS_MEASURE_INTERVAL) {
            float fps = 1 / (fpsTime_ / FPS_MEASURE_INTERVAL);

            fpsString_ = "FPS: " + std::to_string(fps);
            overlay_->updateTextBox("FPS", fpsString_);

            fpsTime_ = 0.f;
            loopsMeasured_ = 0;

            // FIXME
            overlay_->updateTextBox("testing", testt.substr(0, idx));
            if (upward) {
                idx++;
                upward = !(idx == testt.length());
            }
            else {
                idx--;
                upward = idx == 0;
            }
            // END FIXME
        }
	}
    
    gfx_.deviceWaitIdle();
}

/*-----------------------------------------------------------------------------
-----------------------------UPDATE-STUFF--------------------------------------
-----------------------------------------------------------------------------*/
// TODO: consider moving data to heap?
void Engine::updateUBO() {
    float time = clock_.getProgramTime();
    
    // FIXME animations
    entities_[0].setOrientation(cubeRotation_.x*time, cubeRotation_.y*time, cubeRotation_ .z*time);
    //entities_[3].setOrientation(270.f, 0.f, 10.f * time);

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
    // need to check overlay for updates
    if (overlay_->updates_.unpause) {
        togglePause();
    }
    if (overlay_->updates_.quit) {
        util::log(name_, "quitting program (onClick)");
        running_ = false;
    }
    if (overlay_->updates_.fov != camera_.config_.fovy) {
        camera_.config_.fovy = overlay_->updates_.fov;
    }
    if (overlay_->updates_.polygonToggle) {
        gfx_.togglePolygonMode();
    }

    glm::vec3 camPos = camera_.getBodyPtr()->getPosition();
    std::string camPosStr = "Cam Pos: [" + std::to_string(camPos.x).substr(0,5) + ", " + std::to_string(camPos.y).substr(0,5) + ", " + std::to_string(camPos.z).substr(0,5) + "]";
    overlay_->updateTextBox("CameraPos", camPosStr);

    // need to reset updates ??? do we?
    overlay_->resetUpdates();

    overlay_->update();
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
-----------------------------WORLD-GEN-----------------------------------------
-----------------------------------------------------------------------------*/
// TODO: make from config file
void Engine::generateWorld() {
    util::log(name_, "generating game world");

    // mclovin cube
    Entity cube;
    cube.init(0, assets_->getTexture("mclovin"), assets_->getMesh("cube"));
    cube.setPosition(0.f, 2.f, -5.f);
    cube.setOrientation(0.f, 0.f, 180.f);
    entities_.push_back(cube);

    // skybox
    Entity skybox;
    skybox.init(1, assets_->getTexture("skybox"), assets_->getMesh("skybox"));
    skybox.scale(1000.f, 1000.f, 1000.f);
    entities_.push_back(skybox);

    // Floor
    Entity floor;
    floor.init(2, assets_->getTexture("grass"), assets_->getMesh("floor"));
    floor.setOrientation(270.f, 0.f, 0.f);
    floor.scale(100.f, 100.f, 0.f);
    entities_.push_back(floor);

    // viking room model
    Entity viking;
    viking.init(3, assets_->getTexture("viking_room"), assets_->getMesh("viking_room"));
    viking.setPosition(20.f, 0.1f, 0.f);
    viking.setOrientation(270.f, 0.f, 180.f);
    viking.scale(3.f, 3.f, 3.f);
    entities_.push_back(viking);

    // house
    Entity house;
    house.init(4, assets_->getTexture("wood"), assets_->getMesh("house"));
    house.setPosition(-30.f, 0.f, 0.f);
    house.scale(0.1f, 0.1f, 0.1f);
    entities_.push_back(house);

    // font texture 
    Entity font;
    font.init(5, assets_->getTexture("fontgrid"), assets_->getMesh("square"));
    font.setPosition(-5.f, 2.f, -10.f);
    font.scale(3.f, 3.f, 0.f);
    entities_.push_back(font);
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
        case SDL_EVENT_MOUSE_BUTTON_UP:
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
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
    else {
        float x = 0.f;
        float y = 0.f;
        bool down = true;
        SDL_MouseButtonFlags state = SDL_GetMouseState(&x, &y);

        /*switch (state) {
            case SDL_BUTTON_MIDDLE:
            util::log(name_, "case SDL_BUTTON_MIDDLE");
            break;
        case SDL_BUTTON_LEFT:
            util::log(name_, "case SDL_BUTTON_LEFT");
            break;
        case SDL_BUTTON_RIGHT:
            util::log(name_, "case SDL_BUTTON_RIGHT"); // NOT WORKING?!?!?
            break;
        }*/

        switch (event_.type) {
        case SDL_EVENT_MOUSE_MOTION:
            overlay_->updateMousePosition(x, y);
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            down = false;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            overlay_->mouseButtonTrigger(down);
            break;
        }
    }
}

void Engine::handleKeyEvent() {
    if (keys_.esc) { togglePause(); }
    if (keys_.p && !keys_.shift) { gfx_.togglePolygonMode(); }
    if (keys_.p && keys_.shift) { overlay_->toggleWireframe(); }

    // only capture movement input while unpaused
    if (!paused_) {
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
}

void Engine::togglePause() {
    if (paused_) {
        util::log(name_, "unpausing");
    }
    else {
        util::log(name_, "pausing");
    }
    paused_ = !paused_;
    gfx_.toggleMouseMode(paused_);
    overlay_->toggleMenu();
}

/*-----------------------------------------------------------------------------
------------------------------CLEANUP------------------------------------------
-----------------------------------------------------------------------------*/
void Engine::cleanup() {
	util::log(name_, "cleaning up");

	gfx_.cleanup();
}
