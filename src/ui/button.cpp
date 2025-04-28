#include "button.h"

/*-----------------------------------------------------------------------------
------------------------------INITIALIZATION-----------------------------------
-----------------------------------------------------------------------------*/
void Button::init(OverlayState& state, OverlayElementState* elementState, const std::string& id, const std::string label, glm::vec2 position, glm::vec2 sizePixels, glm::vec2 fontSize, int fontIndex, int backgroundIndex) {
    util::log(name_, "initializing overlay button");
    
    state_ = &state;
    elementState_ = elementState;
    
    if (elementState_ == nullptr) {
		unique_ = true;
		elementState_ = new OverlayElementState;
		elementState_->dragged = false;
		elementState_->hovered = false;
		elementState_->interaction = 0;
		elementState_->movable = true; // FIXME

	}
	else {
		unique_ = false;
	}


}


/*-----------------------------------------------------------------------------
------------------------------MAPPING-TO-BUFFER--------------------------------
-----------------------------------------------------------------------------*/
int Button::map(UIVertex* mapped, int overrideIndex) {
	return 0;
}

int Button::mapLines(UIVertex* mapped) {
	return 0;
}

/*-----------------------------------------------------------------------------
--------------------------------UPDATES----------------------------------------
-----------------------------------------------------------------------------*/
void Button::scale() {

}

void Button::onMouseMove() {

}

void Button::onMouseButton() {

}

void Button::resetInteraction() {

}

/*-----------------------------------------------------------------------------
------------------------------CLEANUP------------------------------------------
-----------------------------------------------------------------------------*/
void Button::cleanup() {
    util::log(name_, "cleaning up Button resources");

	if (unique_) {
		delete elementState_;
	}
}
