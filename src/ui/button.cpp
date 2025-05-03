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
    
    // init member variables
    id_ = id;
    position_ = position;
    sizePixels_ = sizePixels;
    fontSize_ = fontSize;
    fontTexIndex_ = fontIndex;
	backgroundTexIndex_ = backgroundIndex;
    labelLength_ = (int)label.length();

    // init rectangle
    clickable_.init(*state_, elementState_, id_ + " clickable", position_, sizePixels_, { 0,0,1,1 }, backgroundTexIndex_);

    // init text - size of textbox should be same as the letters
    glm::vec2 labelBox = { fontSize_.x * label.length() + 0.1f, fontSize_.y };
    label_.init(*state_, elementState_, id_ + " text", label, {0.f,0.f}, labelBox, fontSize_, fontTexIndex_);

    // center label text
    centerLabel();

    // disable border
    label_.setBorder(false);

    // tell label to just update based off rectangle
    label_.setElementStateUpdate(false);
}


/*-----------------------------------------------------------------------------
------------------------------MAPPING-TO-BUFFER--------------------------------
-----------------------------------------------------------------------------*/
int Button::map(UIVertex* mapped, int overrideIndex) {
    int retVal = 0;
    // map rectangle
    retVal += clickable_.map(mapped, overrideIndex);
    mapped += retVal;
    // map text
    retVal += label_.map(mapped, overrideIndex);
    
    return retVal;

}

/*-----------------------------------------------------------------------------
--------------------------------UPDATES----------------------------------------
-----------------------------------------------------------------------------*/
void Button::setAction(void (*func)()) {
    action_ = func;
}

void Button::scale() {
    // scale rectangle
    clickable_.scale();

    // scale text
    label_.scale();

    centerLabel();
}

// NEXT 3 functions: need to reflect changes of clickable to the label
void Button::onMouseMove() {
    clickable_.onMouseMove();
    label_.onMouseMove();
    position_ = clickable_.getPosition();
}

void Button::onMouseButton() {
    clickable_.onMouseButton();
    label_.onMouseButton();
    
    if (!state_->mouseDown && elementState_->hovered) {
        util::log(name_, "executing " + id_ + " button function");
        action_();
    }
}

void Button::resetInteraction() {
    clickable_.resetInteraction();
    label_.resetInteraction();
}

/*-----------------------------------------------------------------------------
--------------------------------HELPER-----------------------------------------
-----------------------------------------------------------------------------*/
void Button::centerLabel() {
    // calculate center position for label
    float labelXPos = position_.x + ((((sizePixels_.x / state_->extent.width) * state_->scale) - ((labelLength_ * (fontSize_.x / state_->extent.width) * state_->scale))) / 2);
    float labelYPos = position_.y + ((((sizePixels_.y / state_->extent.height) * state_->scale) - ((fontSize_.y / state_->extent.height) * state_->scale)) / 2);

    label_.setPosition({ labelXPos, labelYPos });
}