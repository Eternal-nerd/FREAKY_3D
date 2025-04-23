#include "container.h"

/*-----------------------------------------------------------------------------
------------------------------INITIALIZATION-----------------------------------
-----------------------------------------------------------------------------*/
void Container::init(const std::string& id, OverlayMode mode, OverlayContainerType type, glm::vec2 position, glm::vec2 sizePixels, VkExtent2D extent) {
	id_ = id;
	mode_ = mode;
	type_ = type;
	position_ = position;
	sizePixels_ = sizePixels;
	extent_ = extent;

	// calculate boundaries
	rightBoundary_ = position_.x + (sizePixels_.x / extent_.width);
	bottomBoundary_ = position_.y + (sizePixels_.y / extent_.height);

	insertPosition_ = position_;

	// immovable by default
	movable_ = false;
}

void Container::addRectangle(Rectangle rectangle) {
	// either place the rectangle above or below previous ones
	// for box mode: 
	glm::vec2 dimensions = rectangle.getDimensions();
	switch (type_) {
	case OVERLAY_CONTAINER_BOX:
		// check if rectangle would exceed right boundary, if it does, move to next row
		if (insertPosition_.x + dimensions.x > rightBoundary_) {
			insertPosition_.x = position_.x;
			insertPosition_.y += dimensions.y;   // THIS will be a problem if adding different height rectangles
		}
		rectangle.setPosition(insertPosition_);
		insertPosition_.x += dimensions.x;
		break;

	case OVERLAY_CONTAINER_ROW:
		rectangle.setPosition(insertPosition_);
		insertPosition_.x += dimensions.x;
		break;

	case OVERLAY_CONTAINER_COLUMN:
		rectangle.setPosition(insertPosition_);
		insertPosition_.y += dimensions.y;
		break;
	}

	// only should be movable if container is
	rectangle.setMovable(movable_);

	rectangles_.push_back(rectangle);
}

void Container::resetRectanglePositions() {
	// reset insert position pointer
	insertPosition_ = position_;

	for (Rectangle& r : rectangles_) {
		glm::vec2 dimensions = r.getDimensions();
		switch (type_) {
		case OVERLAY_CONTAINER_BOX:
			// check if rectangle would exceed right boundary, if it does, move to next row
			if (insertPosition_.x + dimensions.x > rightBoundary_) {
				insertPosition_.x = position_.x;
				insertPosition_.y += dimensions.y;   // THIS will be a problem if adding different height rectangles
			}
			r.setPosition(insertPosition_);
			insertPosition_.x += dimensions.x;
			break;

		case OVERLAY_CONTAINER_ROW:
			r.setPosition(insertPosition_);
			insertPosition_.x += dimensions.x;
			break;

		case OVERLAY_CONTAINER_COLUMN:
			r.setPosition(insertPosition_);
			insertPosition_.y += dimensions.y;
			break;
		}
	}
}

/*-----------------------------------------------------------------------------
------------------------------MAPPING-TO-BUFFER--------------------------------
-----------------------------------------------------------------------------*/
int Container::map(UIVertex* mapped, int overrideIndex) {
	int retVal = 0;
	for (Rectangle& r : rectangles_) {
		r.map(mapped, overrideIndex);
		mapped += 4;
		retVal += 4;
	}
	return retVal;
}


/*-----------------------------------------------------------------------------
------------------------------UPDATES------------------------------------------
-----------------------------------------------------------------------------*/
// modify attributes
void Container::scale(float scaleFactor) {
	scale_ = scaleFactor;

	// scale boundaries
	rightBoundary_ = position_.x + ((sizePixels_.x / extent_.width) * scale_);
	bottomBoundary_ = position_.y + ((sizePixels_.y / extent_.height) * scale_);

	resetRectanglePositions();
}

void Container::setMovable(bool state) {
	movable_ = state;
	for (Rectangle& r : rectangles_) {
		r.setMovable(state);
	}
}

// events
void Container::onResize(VkExtent2D extent) {
	extent_ = extent;

	for (Rectangle& r : rectangles_) {
		r.onResize(extent);
	}

	scale(scale_);

}

void Container::onMouseMove(glm::vec2 mousePos) {
	for (Rectangle& r : rectangles_) {
		r.onMouseMove(mousePos);
	}
}

void Container::onMouseButton(bool down) {
	for (Rectangle& r : rectangles_) {
		r.onMouseButton(down);
	}
}

void Container::resetInteraction() {
	for (Rectangle& r : rectangles_) {
		r.resetInteraction();
	}
}

void Container::needsRemap() {
	for (Rectangle& r : rectangles_) {
		r.needsRemap();
	}
}


/*-----------------------------------------------------------------------------
------------------------------GETTERS------------------------------------------
-----------------------------------------------------------------------------*/
glm::vec2 Container::getDimensions() {
	return {};
}

glm::vec2 Container::getPosition() {
	return {};
}