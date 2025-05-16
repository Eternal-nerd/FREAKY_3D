#include "slider.h"

void Slider::init(OverlayState& state, OverlayElementState* elementState, const std::string& id, const std::string label, glm::vec2 limits, float initValue, glm::vec2 position, glm::vec2 sizePixels, int fontIndex, int backgroundIndex, int knobIndex, int barIndex) {
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
    currentValue_ = initValue;
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
    reading_.init(*state_, elementState_, id_ + " reading", "test.", position_, boxSize, fontSize, fontTexIndex_);
    reading_.updateMessage(std::to_string(currentValue_).substr(0, SLIDER_VALUE_RECTANGLE_COUNT));
    reading_.setBorder(false);
    reading_.setElementStateUpdate(false);

    // init knob, set correct size here, position is set in 
    glm::vec2 knobSize = { sizePixels_.y * 0.5f, sizePixels_.y * 0.5f };
    // IMPORTANT - know has its own element state
    knobState_ = new OverlayElementState;
    knobState_->dragged = false;
    knobState_->hovered = false;
    knobState_->interaction = 0;
    knobState_->movable = true;
    knob_.init(*state_, knobState_, id_ + " knob", position_, knobSize, { 0,0,1,1 }, knobTexIndex_);

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


void Slider::setAction(void (*func)(float)) {
    action_ = func;
    // initialize updates
    action_(currentValue_);
}

float Slider::getValue() {
    return currentValue_;
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
    orientElements();
}

// SPECIAL! need to do different things if knob is hovered or just background rectangle
void Slider::onMouseMove() {
    // state_->mousePos is a new val, get relative by state_->mousePos - state_->oldMousePos_
    float xDelta = state_->mousePos.x - state_->oldMousePos.x;
    float yDelta = state_->mousePos.y - state_->oldMousePos.y;
    if (knobState_->dragged) {
        // need to set boundaries for knob position
        float leftBound = position_.x + (getScaledSize().x * 0.25f);
        float rightBound = position_.x + (getScaledSize().x - (getScaledSize().x * 0.25f) - knob_.getScaledSize().x);
        if (knob_.getPosition().x + xDelta > leftBound && knob_.getPosition().x + xDelta < rightBound) {
            knob_.setPosition({ knob_.getPosition().x + xDelta, knob_.getPosition().y });
            knobMove();
        }

    }
    else if (elementState_->dragged) {
        background_.setPosition({ background_.getPosition().x + xDelta, background_.getPosition().y + yDelta });
        bar_.setPosition({ bar_.getPosition().x + xDelta, bar_.getPosition().y + yDelta });
        label_.setPosition({ label_.getPosition().x + xDelta, label_.getPosition().y + yDelta });
        reading_.setPosition({ reading_.getPosition().x + xDelta, reading_.getPosition().y + yDelta });
        knob_.setPosition({ knob_.getPosition().x + xDelta, knob_.getPosition().y + yDelta });
    }   
    else {
        if (knob_.isHovered()) {
            knobState_->hovered = true;
            elementState_->hovered = false;
        }
        else {
            knobState_->hovered = false;
            elementState_->hovered = background_.isHovered();
        }
    }

    background_.updateInteraction();
    bar_.updateInteraction();
    label_.updateInteraction();
    reading_.updateInteraction();
    knob_.updateInteraction();

    position_ = background_.getPosition();
}

// SPECIAL! need to do different things if knob is hovered or just background rectangle
void Slider::onMouseButton() {
    if (state_->mouseDown) {
        if (knob_.isHovered()) {
            knobState_->dragged = true;
        }
        else {
            elementState_->dragged = background_.isHovered();
        }
    }
    else {
        if (knobState_->dragged) {
            util::log(name_, "set value to: " + std::to_string(currentValue_));
        }
        resetInteraction();
    }

    background_.updateInteraction();
    bar_.updateInteraction();
    label_.updateInteraction();
    reading_.updateInteraction();
    knob_.updateInteraction();
}

void Slider::resetInteraction() {
    background_.resetInteraction();
    bar_.resetInteraction();
    label_.resetInteraction();
    reading_.resetInteraction();
    knob_.resetInteraction();
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
    // get x offset
    float percentFull = (currentValue_ - limits_.x) / (limits_.y - limits_.x);
    knob_.setPosition({ position_.x + (getScaledSize().x * 0.25f) + (((getScaledSize().x * 0.5f) - knob_.getScaledSize().x) * percentFull), position_.y + (getScaledSize().y - knob_.getScaledSize().y) / 2});
}

void Slider::knobMove() {
    // need to set boundaries for knob position
    float leftBound = position_.x + (getScaledSize().x * 0.25f);
    float rightBound = position_.x + (getScaledSize().x - (getScaledSize().x * 0.25f) - knob_.getScaledSize().x);

    float percentFull = (knob_.getPosition().x - leftBound) / (rightBound - leftBound);

    currentValue_ = limits_.x + ((limits_.y - limits_.x) * percentFull);

    // update reading to value
    reading_.updateMessage(std::to_string(currentValue_).substr(0, SLIDER_VALUE_RECTANGLE_COUNT));

    // callback function
    if (action_) {
        action_(currentValue_);
    }
    else {
        throw std::runtime_error("slider function pointer is null! for slider: " + id_);
    }
}

/*-----------------------------------------------------------------------------
------------------------------CLEANUP------------------------------------------
-----------------------------------------------------------------------------*/
void Slider::cleanup() {
    util::log(name_, "cleaning up overlay slider resources");

    if (unique_ && elementState_) {
        delete elementState_;
    }

    delete knobState_;
}
