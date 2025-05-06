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

    glm::vec2 getScaledSize();

    bool wasHovered();
    bool toggled();

    void cleanup(); 

protected:
    bool unique_ = false;

    OverlayState* state_ = nullptr;
	OverlayElementState* elementState_ = nullptr;

    glm::vec2 position_ = {0.f, 0.f};
    glm::vec2 sizePixels_ = { 0.f, 0.f };
};
