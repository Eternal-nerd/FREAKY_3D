#pragma once

#include <vulkan/vulkan.h>

#include <string>
#include <vector>
#include <stdexcept>

#include "../types.h"
#include "../util.h"

class BaseElement {
public: 
	glm::vec2 getPosition();

    OverlayMode getMode();
    void setMode(OverlayMode mode);

protected:
    OverlayState* state_ = nullptr;
	OverlayElementState* elementState_ = nullptr;
	bool updateElementState_ = true;

    glm::vec2 position_ = {0.f, 0.f};

};
