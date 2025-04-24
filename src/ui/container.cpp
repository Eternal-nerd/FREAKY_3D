#include "container.h"

/*-----------------------------------------------------------------------------
------------------------------INITIALIZATION-----------------------------------
-----------------------------------------------------------------------------*/
void Container::init(OverlayState& state, OverlayElementState* elementState, const std::string& id, OverlayContainerType type, glm::vec2 position, glm::vec2 sizePixels) {
	util::log(name_, "initializing overlay container");
	
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
	type_ = type;
	position_ = position;
	sizePixels_ = sizePixels;

	insertPosition_ = position_;

	// create border lines
	borderLines_.resize(8); // 8 vertices needed for 4 lines
	createBorderLines();
}

void Container::addRectangle(Rectangle rectangle) {

	// give rectangle shared state
	rectangle.replaceElementState(elementState_);

	// either place the rectangle above or below previous ones
	// for box mode: 
	glm::vec2 dimensions = rectangle.getDimensions();
	switch (type_) {
	case OVERLAY_CONTAINER_BOX:
		// check if rectangle would exceed right boundary, if it does, move to next row
		if (insertPosition_.x + dimensions.x > position_.x + xOffset_) {
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
			if (insertPosition_.x + dimensions.x > position_.x + xOffset_) {
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
		int incr = r.map(mapped, overrideIndex);
		mapped += incr;
		retVal += incr;
	}
	return retVal;
}

int Container::mapLines(UIVertex* mapped) {
	//if (elementState_->updated) {
	for (UIVertex& point : borderLines_) {
		mapped->texIndex = point.texIndex;
		mapped->pos = point.pos;
		mapped++;
	}
	//}
	//elementState_->updated = false;
	return 8;
}


/*-----------------------------------------------------------------------------
------------------------------UPDATES------------------------------------------
-----------------------------------------------------------------------------*/
// events
void Container::scale() {
	// scale rectangles
	for (Rectangle& r : rectangles_) {
		r.scale();
	}

	// scale boundaries
	xOffset_ = (sizePixels_.x / state_->extent.width) * state_->scale;
	yOffset_ = (sizePixels_.y / state_->extent.height) * state_->scale;

	resetRectanglePositions();
}

void Container::onMouseMove() {
	// check if mouse is within borders
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
	
	// tell rectangles to update
	updateInteraction();
}

void Container::onMouseButton() {
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

void Container::updateInteraction() {
	for (Rectangle& r : rectangles_) {
		r.updateInteraction();
	}
}

void Container::rePosition() {
	// border box
	createBorderLines();
	
	// rectangles
	resetRectanglePositions();
}

void Container::createBorderLines() {
	// calculate boundaries FIXME
	xOffset_ = sizePixels_.x / state_->extent.width;
	yOffset_ = sizePixels_.y / state_->extent.height;

	// top line
	borderLines_[0].texIndex = 0; // wireframe index
	borderLines_[0].pos = { position_.x, position_.y };
	borderLines_[1].texIndex = 0;
	borderLines_[1].pos = { position_.x + xOffset_, position_.y };

	// left line
	borderLines_[2].texIndex = 0;
	borderLines_[2].pos = { position_.x, position_.y };
	borderLines_[3].texIndex = 0;
	borderLines_[3].pos = { position_.x, position_.y + yOffset_ };

	// right line
	borderLines_[4].texIndex = 0;
	borderLines_[4].pos = { position_.x + xOffset_, position_.y };
	borderLines_[5].texIndex = 0;
	borderLines_[5].pos = { position_.x + xOffset_, position_.y + yOffset_ };

	// bottom line
	borderLines_[6].texIndex = 0;
	borderLines_[6].pos = { position_.x, position_.y + yOffset_ };
	borderLines_[7].texIndex = 0;
	borderLines_[7].pos = { position_.x + xOffset_, position_.y + yOffset_ };
}

/*-----------------------------------------------------------------------------
------------------------------GETTERS------------------------------------------
-----------------------------------------------------------------------------*/
/*glm::vec2 Container::getDimensions() {
	return {};
}

glm::vec2 Container::getPosition() {
	return {};
}*/


/*-----------------------------------------------------------------------------
------------------------------CLEANUP------------------------------------------
-----------------------------------------------------------------------------*/
void Container::cleanup() {
	util::log(name_, "cleaning up Rectangle resources");

	if (unique_) {
		delete elementState_;
	}
}