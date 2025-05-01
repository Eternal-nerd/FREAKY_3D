#include "text.h"

/*-----------------------------------------------------------------------------
------------------------------INITIALIZATION-----------------------------------
-----------------------------------------------------------------------------*/
void Text::init(OverlayState& state, OverlayElementState* elementState, const std::string& id, const std::string message, glm::vec2 position, glm::vec2 sizePixels, glm::vec2 fontSize, int texIndex) {
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

	}
	else {
		unique_ = false;
	}

	id_ = id;
	message_ = message;
	position_ = position;
	sizePixels_ = sizePixels;
	fontSize_ = fontSize;
	texIndex_ = texIndex;

	// create a container for the letters (rectangles)
	container_.init(*state_, elementState_, "text-container", OVERLAY_CONTAINER_BOX, position_, sizePixels_);

	// initialize container with letter rectangles for each letter in "message"
	populateContainer();
	populateCoords();
}

void Text::populateContainer() {
	// for each letter in message:
	for (int i = 0; i < message_.length(); i++) {
		Rectangle r;
		r.init(
			*state_, elementState_, // shared states
			"", // id of rectangle dont matter
			{}, // position is managed by container, doesnt matter
			fontSize_, // make the rectangle as big as fontsize
			// texture coords
			{}, // texture cords
			texIndex_
		);
		container_.addRectangle(r);
	}
}

// sets texture coords for each rectangle
void Text::populateCoords() {
	// for each letter in message:
	for (int i = 0; i < message_.length(); i++) {
		container_.rectangles_[i].setTextureCoords({ getOffsetX(message_[i]), getOffsetY(message_[i]), FONT_OFFSET, FONT_OFFSET });
	}
}

/*-----------------------------------------------------------------------------
------------------------------GET-SET-MESSAGE----------------------------------
-----------------------------------------------------------------------------*/
void Text::updateMessage(const std::string& message) {
	if (message != message_) {
		if (message_.length() < message.length()) {
			// need to add rectangles
			int dif = (int)message.length() - (int)message_.length();
			for (int i = 0; i < dif; i++) {
				Rectangle r;
				r.init(
					*state_, elementState_, // shared states
					"", // id of rectangle dont matter
					{}, // position is managed by container, doesnt matter
					fontSize_, // make the rectangle as big as fontsize
					{}, // texture cords
					texIndex_
				);
				container_.addRectangle(r);
			}
		}
		if (message_.length() > message.length()) {
			// need to remove rectangles
			int dif = (int)message_.length() - (int)message.length();
			container_.removeRectangles(dif);
		}
		message_ = message;
		populateCoords();
		state_->updatedTri = true;
	}
}

glm::vec2 Text::getPosition() {
	return position_;
}

void Text::setPosition(glm::vec2 position) {
	position_ = position;
}

void Text::setBorder(bool state) {
	container_.border_ = state;
}

OverlayMode Text::getMode() {
    return elementState_->mode;
}

void Text::setMode(OverlayMode mode) {
    elementState_->mode = mode;
}

/*-----------------------------------------------------------------------------
------------------------------MAPPING-TO-BUFFER--------------------------------
-----------------------------------------------------------------------------*/
int Text::map(UIVertex* mapped, int overrideIndex) {
	return container_.map(mapped, overrideIndex);
}

int Text::mapLines(UIVertex* mapped) {
	return container_.mapLines(mapped);
}

/*-----------------------------------------------------------------------------
------------------------------UPDATES------------------------------------------
-----------------------------------------------------------------------------*/
// events
void Text::scale() {
	container_.scale();
}

void Text::onMouseMove() {
	container_.onMouseMove();
	position_ = container_.getPosition();
}

void Text::onMouseButton() {
	container_.onMouseButton();
}

void Text::resetInteraction() {
	container_.resetInteraction();
}

/*-----------------------------------------------------------------------------
------------------------------HELPER-------------------------------------------
-----------------------------------------------------------------------------*/
void Text::setElementStateUpdate(bool state) {
	container_.setElementStateUpdate(state);
}

/*-----------------------------------------------------------------------------
------------------------------CLEANUP------------------------------------------
-----------------------------------------------------------------------------*/
void Text::cleanup() {
	util::log(name_, "cleaning up Text resources");

	if (unique_) {
		delete elementState_;
	}
}

/*-----------------------------------------------------------------------------
------------------------------TEXTURE-INDEXING---------------------------------
-----------------------------------------------------------------------------*/
float Text::getOffsetX(char c) {
	switch (c) {
	case 'A':
	case 'Q':
	case 'g':
	case 'w':
	case ',':
	case '@':
		//case '':
		return 0.f;
	case 'B':
	case 'R':
	case 'h':
	case 'x':
	case ';':
	case '#':
		//case '':
		return FONT_OFFSET * 1;
	case 'C':
	case 'S':
	case 'i':
	case 'y':
	case ':':
	case '$':
		//case '':
		return FONT_OFFSET * 2;
	case 'D':
	case 'T':
	case 'j':
	case 'z':
	case '\"':
	case '%':
		//case '':
		return FONT_OFFSET * 3;
	case 'E':
	case 'U':
	case 'k':
	case '0':
	case '?':
	case '^':
		//case '':
		return FONT_OFFSET * 4;
	case 'F':
	case 'V':
	case 'l':
	case '1':
	case '_':
		//case '':
		//case '':
		return FONT_OFFSET * 5;
	case 'G':
	case 'W':
	case 'm':
	case '2':
	case '-':
	case '&':
		//case '':
		return FONT_OFFSET * 6;
	case 'H':
	case 'X':
	case 'n':
	case '3':
	case '|':
	case '*':
		//case '':
		return FONT_OFFSET * 7;
	case 'I':
	case 'Y':
	case 'o':
	case '4':
	case '\\':
	case '(':
		//case '':
		return FONT_OFFSET * 8;
	case 'J':
	case 'Z':
	case 'p':
	case '5':
	case '/':
	case ')':
		//case '':
		return FONT_OFFSET * 9;
	case 'K':
	case 'a':
	case 'q':
	case '6':
	case '<':
	case '+':
		//case '':
		return FONT_OFFSET * 10;
	case 'L':
	case 'b':
	case 'r':
	case '7':
	case '>':
	case '=':
		//case '':
		return FONT_OFFSET * 11;
	case 'M':
	case 'c':
	case 's':
	case '8':
	case '[':
	case '~':
		//case '':
		return FONT_OFFSET * 12;
	case 'N':
	case 'd':
	case 't':
	case '9':
	case ']':
	case '`':
		//case '':
		return FONT_OFFSET * 13;
	case 'O':
	case 'e':
	case 'u':
	case '!':
	case '{':
		//case '':
		//case '':
		return FONT_OFFSET * 14;
	case 'P':
	case 'f':
	case 'v':
	case '.':
	case '}':
		//case '':
		//case '':
		return FONT_OFFSET * 15;
		// space
	case ' ':
		return FONT_OFFSET * 15;
	}
	// defualt is smiley?
	return FONT_OFFSET * 4;
	//return 0.f;
}

float Text::getOffsetY(char c) {
	switch (c) {
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
		return 0.f;
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
		return FONT_OFFSET * 1;
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
		return FONT_OFFSET * 2;
	case 'w':
	case 'x':
	case 'y':
	case 'z':
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '!':
	case '.':
		return FONT_OFFSET * 3;
	case ',':
	case ';':
	case ':':
	case '"':
	case '?':
	case '_':
	case '-':
	case '|':
	case '\\':
	case '/':
	case '<':
	case '>':
	case '[':
	case ']':
	case '{':
	case '}':
		return FONT_OFFSET * 4;
	case '@':
	case '#':
	case '$':
	case '%':
	case '^':
		//case '':
	case '&':
	case '*':
	case '(':
	case ')':
	case '+':
	case '=':
	case '~':
	case '`':
		//case '':
		//case '':
		return FONT_OFFSET * 5;
		/*case '':
		case '':
		case '':
		case '':
		case '':
		case '':
		case '':
		case '':
		case '':
		case '':
		case '':
		case '':
		case '':
		case '':
		case '':
		case '':
			return FONT_OFFSET*6;*/
			// space
	case ' ':
		return FONT_OFFSET * 15;
	}
	// defualt is smiley?
	return FONT_OFFSET * 6;
	//return 0.f;
}
