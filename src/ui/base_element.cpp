#include "base_element.h"

/*-----------------------------------------------------------------------------
------------------------------GETTERS------------------------------------------
-----------------------------------------------------------------------------*/
glm::vec2 BaseElement::getPosition() {
    return { position_.x, position_.y };
}

OverlayMode BaseElement::getMode() {
    if (elementState_) {
        return elementState_->mode;
    }
    else {
        throw std::runtime_error("failed to access element mode due to nullptr. "); 
    }
}

void BaseElement::setMode(OverlayMode mode) {
    if (elementState_) {
        elementState_->mode = mode;
    }
    else {
        throw std::runtime_error("failed to set element mode due to nullptr. "); 
    }
}

glm::vec2 BaseElement::getScaledSize() {
    return { (sizePixels_.x / state_->extent.width) * state_->scale , (sizePixels_.y / state_->extent.height) * state_->scale };
}

bool BaseElement::isHovered() {
    // calculate boundaries
    float left = position_.x;
    float right = position_.x + ((sizePixels_.x / state_->extent.width) * state_->scale);
    float top = position_.y;
    float bottom = position_.y + ((sizePixels_.y / state_->extent.height) * state_->scale);

    return util::withinBorders(state_->mousePos, { left, right, top, bottom });
}

void BaseElement::setElementStateUpdate(bool state) {
    updateElementState_ = state;
}


/*-----------------------------------------------------------------------------
------------------------------CLEANUP------------------------------------------
-----------------------------------------------------------------------------*/
void BaseElement::cleanup() {
    util::log("ELEMENT BASE", "cleaning up overlay element resources");

    if (unique_ && elementState_) {
        delete elementState_;
    }
}
