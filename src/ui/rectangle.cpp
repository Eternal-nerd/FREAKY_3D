#include "rectangle.h"

/*-----------------------------------------------------------------------------
------------------------------INITIALIZATION-----------------------------------
-----------------------------------------------------------------------------*/
void Rectangle::init(const std::string& id, OverlayMode mode, glm::vec2 position, glm::vec2 sizePixels, glm::vec4 texCoords, int texIndex, VkExtent2D extent) {
	util::log(name_, "initializing overlay rectangle");

	id_ = id;
	mode_ = mode;
	position_ = position;
	sizePixels_ = sizePixels;
	extent_ = extent;

	hovered_ = false;
	dragged_ = false;
	interaction_ = 0;
	updated_ = true;
	movable_ = false;

	// calculate x and y offsets
	float xOffset = (sizePixels_.x / extent_.width);
	float yOffset = (sizePixels_.y / extent_.height);

	// top left
	quad_.vertices[0].pos = {position_.x, position_.y};
	quad_.vertices[0].texCoord = { texCoords.x, texCoords.y };
	quad_.vertices[0].texIndex = texIndex;
	quad_.vertices[0].interaction = interaction_;

	// bottom left
	quad_.vertices[1].pos = { position_.x, position_.y + yOffset };
	quad_.vertices[1].texCoord = { texCoords.x, texCoords.y + texCoords.w };
	quad_.vertices[1].texIndex = texIndex;
	quad_.vertices[1].interaction = interaction_;

	// top right
	quad_.vertices[2].pos = { position_.x + xOffset, position_.y };
	quad_.vertices[2].texCoord = { texCoords.x + texCoords.z, texCoords.y };
	quad_.vertices[2].texIndex = texIndex;
	quad_.vertices[2].interaction = interaction_;

	// bottom right
	quad_.vertices[3].pos = { position_.x + xOffset, position_.y + yOffset };
	quad_.vertices[3].texCoord = { texCoords.x + texCoords.z, texCoords.y + texCoords.w };
	quad_.vertices[3].texIndex = texIndex;
	quad_.vertices[3].interaction = interaction_;

}

/*-----------------------------------------------------------------------------
------------------------------UPDATES------------------------------------------
-----------------------------------------------------------------------------*/
void Rectangle::rescale(float scale) {
	scale_ = scale;

	// calculate x and y offsets
	float xOffset = (sizePixels_.x / extent_.width) * scale_;
	float yOffset = (sizePixels_.y / extent_.height) * scale_;

	// bottom left
	quad_.vertices[1].pos = { position_.x, position_.y + yOffset };

	// top right
	quad_.vertices[2].pos = { position_.x + xOffset, position_.y };

	// bottom right
	quad_.vertices[3].pos = { position_.x + xOffset, position_.y + yOffset };

	updated_ = true;
}

void Rectangle::rePosition() {
	// update top left position
	quad_.vertices[0].pos = { position_.x, position_.y };
	rescale(scale_); // updates other 3 vertices position
	updated_ = true;
}

void Rectangle::updateInteraction() {
	if (hovered_) {
		interaction_ = 1;
	}
	if (dragged_ && movable_) {
		interaction_ = 2;
	}
	if (!hovered_ && !dragged_) {
		interaction_ = 0;
	}

	quad_.vertices[0].interaction = interaction_;
	quad_.vertices[1].interaction = interaction_;
	quad_.vertices[2].interaction = interaction_;
	quad_.vertices[3].interaction = interaction_;
	updated_ = true;
}

void Rectangle::setMovable(bool state) {
	movable_ = state;
}

void Rectangle::onResize(VkExtent2D extent) {
	extent_ = extent;
	rescale(scale_);
	updated_ = true;
}

void Rectangle::onMouseMove(glm::vec2 mousePos) {
	// calculate boundaries
	float leftBound = position_.x;
	float rightBound = position_.x + ((sizePixels_.x / extent_.width) * scale_);
	float topBound = position_.y;
	float bottomBound = position_.y + ((sizePixels_.y / extent_.height) * scale_);;

	bool oldHover = hovered_;

	if (mousePos.x < rightBound && mousePos.x > leftBound && mousePos.y > topBound && mousePos.y < bottomBound) {
		hovered_ = true;
	}
	else {
		hovered_ = false;
	}

	// NEED TO do drag interaction here
	if (dragged_) {
		if (movable_) {
			float xDelta = mousePos.x - mousePos_.x;
			float yDelta = mousePos.y - mousePos_.y;
			position_.x += xDelta;
			position_.y += yDelta;
			rePosition();
		}
	}
	else {
		if (oldHover != hovered_) {
			updateInteraction();
		}
	}

	mousePos_ = mousePos;
}

void Rectangle::resetInteraction() {
	hovered_ = false;
	dragged_ = false;

	updateInteraction();

	updated_ = true;
}

void Rectangle::onMouseButton(bool down) {
	if (down) {
		if (hovered_) {
			dragged_ = true;
			updateInteraction();
		}
	}
	else {
		if (dragged_) {
			dragged_ = false;
			updateInteraction();
		}
	}

}

void Rectangle::needsUpdate() {
	updated_ = true;
}


/*-----------------------------------------------------------------------------
------------------------------MAPPING-TO-BUFFER--------------------------------
-----------------------------------------------------------------------------*/
int Rectangle::map(UIVertex* mapped, int overrideIndex) {
	// for each vertex
	if (updated_) {
		for (int i = 0; i < 4; i++) {
			mapped->pos.x = quad_.vertices[i].pos.x; // position x
			mapped->pos.y = quad_.vertices[i].pos.y; // position y
			mapped->texCoord.x = quad_.vertices[i].texCoord.x; // tex coord x
			mapped->texCoord.y = quad_.vertices[i].texCoord.y; // tex coord y
			(overrideIndex == -1) ? mapped->texIndex = quad_.vertices[i].texIndex : mapped->texIndex = overrideIndex; // tex index
			mapped->interaction = quad_.vertices[i].interaction; // for checking hover
			mapped++;
		}
	}
	updated_ = false;
	return 4;
}

/*-----------------------------------------------------------------------------
------------------------------GETTERS------------------------------------------
-----------------------------------------------------------------------------*/
float Rectangle::getWidth() {
	return (sizePixels_.x / extent_.width) * scale_;
}

float Rectangle::getHeight() {
	return (sizePixels_.y / extent_.height) * scale_;
}

float Rectangle::getPositionX() {
    return position_.x;
}

float Rectangle::getPositionY() {
    return position_.y;
}
