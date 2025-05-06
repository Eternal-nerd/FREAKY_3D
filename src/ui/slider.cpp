#include "slider.h"

void Slider::init(OverlayState& state, OverlayElementState* elementState, const std::string& id, const std::string label, glm::vec2 limits, glm::vec2 position, glm::vec2 sizePixels, int fontIndex, int backgroundIndex, int knobIndex, int barIndex) {
    util::log(name_, "initializing overlay Slider");

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
    limits_ = limits;
    position_ = position;
    sizePixels_ = sizePixels;
    fontTexIndex_ = fontIndex;
    backgroundTexIndex_ = backgroundIndex;
    knobTexIndex_ = knobIndex;
    barTexIndex_ = barIndex;

    // initialize elements here
    // init background rect
    background_.init(*state_, elementState_, id_ + " background", position_, sizePixels_, { 0,0,1,1 }, backgroundTexIndex_);
    
    // init bar, set correct size here, position is set in 
    glm::vec2 barSize = { sizePixels_.x * 0.5f, sizePixels_.y * 0.05f };
    bar_.init(*state_, elementState_, id_ + " bar", position_, barSize, { 0,0,1,1 }, barTexIndex_);
    bar_.setElementStateUpdate(false);

    // init label
    glm::vec2 fontSize = { (sizePixels_.x * 0.25f * 0.75f) / label.length(), sizePixels_.y * 0.75f };
    glm::vec2 boxSize = { fontSize.x * label.length() + 0.1f, fontSize.y };
    label_.init(*state_, elementState_, id_ + " label", label, position_, boxSize, fontSize, fontTexIndex_);
    label_.setBorder(false);
    label_.setElementStateUpdate(false);

    // init reading
    fontSize = { (sizePixels_.x * 0.25f * 0.75f) / SLIDER_VALUE_RECTANGLE_COUNT, sizePixels_.y * 0.75f };
    boxSize = { fontSize.x * SLIDER_VALUE_RECTANGLE_COUNT + 0.1f, fontSize.y };
    reading_.init(*state_, elementState_, id_ + " reading", "testing", position_, boxSize, fontSize, fontTexIndex_);
    reading_.setBorder(false);
    reading_.setElementStateUpdate(false);

    // init knob, set correct size here, position is set in 
    glm::vec2 knobSize = { sizePixels_.y * 0.5f, sizePixels_.y * 0.5f };
    // IMPORTANT - know has its own element state
    knob_.init(*state_, nullptr, id_ + " knob", position_, knobSize, { 0,0,1,1 }, knobTexIndex_);
    // FIXME
    //knob_.setElementStateUpdate(false);

    // move elements to correct positions
    orientElements();
}


int Slider::map(UIVertex* mapped, int overrideIndex) {
    int retVal = 0;
    int offset = 0;
    // map background
    offset = background_.map(mapped, overrideIndex);
    mapped += offset;
    retVal += offset;
    // map bar
    offset = bar_.map(mapped, overrideIndex);
    mapped += offset;
    retVal += offset;
    // map knob
    offset = knob_.map(mapped, overrideIndex);
    mapped += offset;
    retVal += offset;
    // map label
    offset = label_.map(mapped, overrideIndex);
    mapped += offset;
    retVal += offset;
    // map reading
    offset = reading_.map(mapped, overrideIndex);
    mapped += offset;
    retVal += offset;
    
    return retVal;
}


void Slider::setAction(void (*func)()) {

}

/*-----------------------------------------------------------------------------
------------------------------UPDATES------------------------------------------
-----------------------------------------------------------------------------*/
void Slider::scale() {
    background_.scale();
    bar_.scale();
    knob_.scale();
    label_.scale();
    reading_.scale();
}

// SPECIAL! need to do different things if knob is hovered or just background rectangle
void Slider::onMouseMove() {
    if (!state_->mouseDown) {
        resetInteraction();
    }

    if (knob_.wasHovered() && !background_.toggled()) {
        if (!knob_.toggled()) {
            resetInteraction();
        }
        knob_.onMouseMove();
    }
    else {
        background_.onMouseMove();
        bar_.onMouseMove();
        label_.onMouseMove();
        reading_.onMouseMove();
        if (background_.wasHovered() && background_.toggled()) {
            // manually change knobs position
            float xDelta = state_->mousePos.x - state_->oldMousePos.x;
            float yDelta = state_->mousePos.y - state_->oldMousePos.y;
            knob_.setPosition({ knob_.getPosition().x + xDelta, knob_.getPosition().y + yDelta });
        }
    }
}

// SPECIAL! need to do different things if knob is hovered or just background rectangle
void Slider::onMouseButton() {
    if (knob_.wasHovered()) {
        knob_.onMouseButton();
    }
    else {
        background_.onMouseButton();
        bar_.onMouseButton();
        label_.onMouseButton();
        reading_.onMouseButton();
    }
}

void Slider::resetInteraction() {
    background_.resetInteraction();
    bar_.resetInteraction();
    knob_.resetInteraction();
    label_.resetInteraction();
    reading_.resetInteraction();
}

/*-----------------------------------------------------------------------------
--------------------------------HELPER-----------------------------------------
-----------------------------------------------------------------------------*/
void Slider::orientElements() {
    // move bar
    bar_.setPosition({ position_.x + (getScaledSize().x * 0.25f), position_.y + (getScaledSize().y - bar_.getScaledSize().y) / 2 });

    // move label
    label_.setPosition({ position_.x + (getScaledSize().x * 0.25f * 0.25f * 0.5f), position_.y + (getScaledSize().y - label_.getScaledSize().y) / 2 });

    // move reading
    reading_.setPosition({ (position_.x + getScaledSize().x) - (reading_.getScaledSize().x + (getScaledSize().x * 0.25f * 0.25f * 0.5f)), position_.y + (getScaledSize().y - reading_.getScaledSize().y) / 2 });

    // move knob
    knob_.setPosition({ position_.x + (getScaledSize().x - knob_.getScaledSize().x) / 2, position_.y + (getScaledSize().y - knob_.getScaledSize().y) / 2 });
}
