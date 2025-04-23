#include "rectangle.h"

/*-----------------------------------------------------------------------------
------------------------------INITIALIZATION-----------------------------------
-----------------------------------------------------------------------------*/
void Rectangle::init(OverlayState& state, OverlayElementState* elementState, const std::string& id, glm::vec2 position, glm::vec2 sizePixels, glm::vec4 texCoords, int texIndex) {
	util::log(name_, "initializing overlay rectangle");

	state_ = &state;
	elementState_ = elementState;

	if (elementState_ == nullptr) {
		unique_ = true;
		elementState_ = new OverlayElementState;
		elementState_->dragged = false;
		elementState_->hovered = false;
		elementState_->interaction = 0;
		elementState_->movable = true; // FIXME
		elementState_->updated = true;

	}
	else {
		unique_ = false;
	}

	id_ = id;
	position_ = position;
	sizePixels_ = sizePixels;

	// calculate x and y offsets
	float xOffset = (sizePixels_.x / state_->extent.width);
	float yOffset = (sizePixels_.y / state_->extent.height);

	// top left
	quad_.vertices[0].pos = {position_.x, position_.y};
	quad_.vertices[0].texCoord = { texCoords.x, texCoords.y };
	quad_.vertices[0].texIndex = texIndex;
	quad_.vertices[0].interaction = elementState_->interaction;

	// bottom left
	quad_.vertices[1].pos = { position_.x, position_.y + yOffset };
	quad_.vertices[1].texCoord = { texCoords.x, texCoords.y + texCoords.w };
	quad_.vertices[1].texIndex = texIndex;
	quad_.vertices[1].interaction = elementState_->interaction;

	// top right
	quad_.vertices[2].pos = { position_.x + xOffset, position_.y };
	quad_.vertices[2].texCoord = { texCoords.x + texCoords.z, texCoords.y };
	quad_.vertices[2].texIndex = texIndex;
	quad_.vertices[2].interaction = elementState_->interaction;

	// bottom right
	quad_.vertices[3].pos = { position_.x + xOffset, position_.y + yOffset };
	quad_.vertices[3].texCoord = { texCoords.x + texCoords.z, texCoords.y + texCoords.w };
	quad_.vertices[3].texIndex = texIndex;
	quad_.vertices[3].interaction = elementState_->interaction;

}

void Rectangle::replaceElementState(OverlayElementState* elementState) {
	// new ptr shouldnt be null
	assert(elementState != nullptr);

	elementState_ = elementState;
}

/*-----------------------------------------------------------------------------
------------------------------UPDATES------------------------------------------
-----------------------------------------------------------------------------*/
void Rectangle::scale() {
	// calculate x and y offsets
	float xOffset = (sizePixels_.x / state_->extent.width) * state_->scale;
	float yOffset = (sizePixels_.y / state_->extent.height) * state_->scale;

	// bottom left
	quad_.vertices[1].pos = { position_.x, position_.y + yOffset };

	// top right
	quad_.vertices[2].pos = { position_.x + xOffset, position_.y };

	// bottom right
	quad_.vertices[3].pos = { position_.x + xOffset, position_.y + yOffset };

	elementState_->updated = true;
}

// borders: leftbound, topbound, rightbound, bottombound
//bool withinBorders(glm::vec2 toCheck, glm::vec4 borders);

void Rectangle::onMouseMove() {
	// calculate boundaries
	float left = position_.x;
	float right = position_.x + ((sizePixels_.x / state_->extent.width) * state_->scale);
	float top = position_.y;
	float bottom = position_.y + ((sizePixels_.y / state_->extent.height) * state_->scale);

	bool oldHover = elementState_->hovered;

	elementState_->hovered = util::withinBorders(state_->mousePos, { left, right, top, bottom });

	// NEED TO do drag interaction here
	if (elementState_->dragged) {
		if (elementState_->movable) {
			float xDelta = state_->mousePos.x - state_->oldMousePos.x;
			float yDelta = state_->mousePos.y - state_->oldMousePos.y;
			position_.x += xDelta;
			position_.y += yDelta;
			rePosition();
		}
	}
	else {
		if (oldHover != elementState_->hovered) {
			updateInteraction();
		}
	}
}

void Rectangle::onMouseButton() {
	if (state_->mouseDown) {
		if (elementState_->hovered) {
			elementState_->dragged = true;
			updateInteraction();
		}
	}
	else {
		if (elementState_->dragged) {
			elementState_->dragged = false;
			updateInteraction();
		}
	}
}

void Rectangle::resetInteraction() {
	elementState_->hovered = false;
	elementState_->dragged = false;

	updateInteraction();

	elementState_->updated = true;
}

void Rectangle::needsRemap() {
	elementState_->updated = true;
}


/*-----------------------------------------------------------------------------
------------------------------MAPPING-TO-BUFFER--------------------------------
-----------------------------------------------------------------------------*/
int Rectangle::map(UIVertex* mapped, int overrideIndex) {
	// for each vertex
	//if (elementState_->updated) {
		for (int i = 0; i < 4; i++) {
			mapped->pos.x = quad_.vertices[i].pos.x; // position x
			mapped->pos.y = quad_.vertices[i].pos.y; // position y
			mapped->texCoord.x = quad_.vertices[i].texCoord.x; // tex coord x
			mapped->texCoord.y = quad_.vertices[i].texCoord.y; // tex coord y
			(overrideIndex == -1) ? mapped->texIndex = quad_.vertices[i].texIndex : mapped->texIndex = overrideIndex; // tex index
			mapped->interaction = quad_.vertices[i].interaction; // for checking hover
			mapped++;
		}
	//}
	//elementState_->updated = false;
	return 4;
}

/*-----------------------------------------------------------------------------
------------------------------GETTERS------------------------------------------
-----------------------------------------------------------------------------*/
glm::vec2 Rectangle::getDimensions() {
	return {(sizePixels_.x / state_->extent.width) * state_->scale, (sizePixels_.y / state_->extent.height)* state_->scale};
}

glm::vec2 Rectangle::getPosition() {
    return { position_.x, position_.y};
}

/*-----------------------------------------------------------------------------
------------------------------GETTERS------------------------------------------
-----------------------------------------------------------------------------*/
void Rectangle::setPosition(glm::vec2 position) {
	position_ = position;
	rePosition();
}

/*-----------------------------------------------------------------------------
------------------------------PRIVATE-HELPERS----------------------------------
-----------------------------------------------------------------------------*/
void Rectangle::rePosition() {
	// update top left position
	quad_.vertices[0].pos = { position_.x, position_.y };
	scale(); // updates other 3 vertices position
	elementState_->updated = true;
}

void Rectangle::updateInteraction() {
	if (elementState_->hovered) {
		elementState_->interaction = 1;
	}
	if (elementState_->dragged && elementState_->movable) {
		elementState_->interaction = 2;
	}
	if (!elementState_->hovered && !elementState_->dragged) {
		elementState_->interaction = 0;
	}

	quad_.vertices[0].interaction = elementState_->interaction;
	quad_.vertices[1].interaction = elementState_->interaction;
	quad_.vertices[2].interaction = elementState_->interaction;
	quad_.vertices[3].interaction = elementState_->interaction;
	elementState_->updated = true;
}

/*-----------------------------------------------------------------------------
------------------------------CLEANUP------------------------------------------
-----------------------------------------------------------------------------*/
void Rectangle::cleanup() {
	util::log(name_, "cleaning up Rectangle resources");

	if (unique_) {
		delete elementState_;
	}
}