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

