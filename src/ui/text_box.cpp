#include "text_box.h"

void TextBox::init(OverlayMode mode, OverlayPositionMode positionMode, const std::string& message, glm::vec2 position, glm::vec2 boxSize, glm::vec2 fontSize, glm::vec2 extent, int texIndex) {
	util::log(name_, "initializing ui text box");

	mode_ = mode;
	positionMode_ = positionMode;
	message_ = message;
	position_ = position;
	boxSize_ = boxSize;
	fontSize_ = fontSize;
	extent_ = extent;
	texIndex_ = texIndex;
    
    generateVertices();
}

void TextBox::updateText(const std::string& message) {
	message_ = message;
    
    generateVertices();
}

void TextBox::scale(glm::vec2 extent) {
	extent_ = extent;

	generateVertices();
}

bool TextBox::compareMessage(const std::string& compareString) {
	return (compareString == message_);
}

void TextBox::generateVertices() {
    // calculate x and y offsets (position = TOP LEFT of box
	float scaledWidth = (boxSize_.x / extent_.x);
	float scaledHeight = (boxSize_.y / extent_.y);

	float scaledFontWidth = (fontSize_.x / extent_.x);
	float scaledFontHeight = (fontSize_.y / extent_.y);

	vertices_.resize(message_.length() * 4);

	// initialize vertices in vector
    float currentX = 0.f;
    float currentY = 0.f;
    float resetX = 0.f;
    switch (positionMode_) {
    case OVERLAY_POSITION_CENTERED:
        currentX = position_.x - (scaledWidth/2);
	    currentY = position_.y - (scaledHeight/2);
        resetX = position_.x -(scaledWidth/2);
        break;
    case OVERLAY_POSITION_TOP_LEFT:
        currentX = position_.x;
        currentY = position_.y;
        resetX = position_.x;
        break;
    default:
        break;
    }
	
	for (int i = 0; i < message_.length(); i++) {

		// newline char case
		if (message_[i] == '\n') {
			currentY += scaledFontHeight;
			currentX = resetX;
			continue;
		}

		// case where font letter will exceed boundary box
		if (currentX + scaledFontWidth > position_.x + (scaledWidth / 2)) {
			currentY += scaledFontHeight;
			currentX = resetX;
		}

		// TODO MAYBE add case for letters exceeding box height

		float fontX = getOffsetX(message_[i]);
		float fontY = getOffsetY(message_[i]);

		// top left
		vertices_[(i * 4) + 0].pos = { currentX, currentY };
		vertices_[(i * 4) + 0].texCoord = { fontX, fontY};
		vertices_[(i * 4) + 0].texIndex = texIndex_;

		// bottom left
		vertices_[(i * 4) + 1].pos = { currentX, currentY + scaledFontHeight };
		vertices_[(i * 4) + 1].texCoord = { fontX, fontY + FONT_OFFSET };
		vertices_[(i * 4) + 1].texIndex = texIndex_;

		// top right
		vertices_[(i * 4) + 2].pos = { currentX + scaledFontWidth, currentY };
		vertices_[(i * 4) + 2].texCoord = { fontX + FONT_OFFSET, fontY };
		vertices_[(i * 4) + 2].texIndex = texIndex_;

		// bottom right
		vertices_[(i * 4) + 3].pos = { currentX + scaledFontWidth, currentY + scaledFontHeight };
		vertices_[(i * 4) + 3].texCoord = { fontX + FONT_OFFSET, fontY + FONT_OFFSET };
		vertices_[(i * 4) + 3].texIndex = texIndex_;

		currentX += scaledFontWidth;
	}

}

int TextBox::map(UIVertex* mapped, int overrideIndex) {
	// for each vertex
	int count = 0;
	for (int i = 0; i < vertices_.size(); i++) {
		mapped->pos.x = vertices_[i].pos.x; // position x
		mapped->pos.y = vertices_[i].pos.y; // position y
		mapped->texCoord.x = vertices_[i].texCoord.x; // tex coord x
		mapped->texCoord.y = vertices_[i].texCoord.y; // tex coord y
		(overrideIndex == -1) ? mapped->texIndex = vertices_[i].texIndex : mapped->texIndex = overrideIndex; // tex index
		mapped++;
		count++;
	}
	return count;

}

int TextBox::getQuadCount() {
	return (int)message_.length();
}


float TextBox::getOffsetX(char c) {
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
	case '\'':
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
		return FONT_OFFSET*7;
	case 'I':
	case 'Y':
	case 'o':
	case '4':
	case '\\':
	case '(':
	//case '':
		return FONT_OFFSET*8;
	case 'J':
	case 'Z':
	case 'p':
	case '5':
	case '/':
	case ')':
	//case '':
		return FONT_OFFSET*9;
	case 'K':
	case 'a':
	case 'q':
	case '6':
	case '<':
	case '+':
	//case '':
		return FONT_OFFSET*10;
	case 'L':
	case 'b':
	case 'r':
	case '7':
	case '>':
	case '=':
	//case '':
		return FONT_OFFSET*11;
	case 'M':
	case 'c':
	case 's':
	case '8':
	case '[':
	case '~':
	//case '':
		return FONT_OFFSET*12;
	case 'N':
	case 'd':
	case 't':
	case '9':
	case ']':
	case '`':
	//case '':
		return FONT_OFFSET*13;
	case 'O':
	case 'e':
	case 'u':
	case '!':
	case '{':
	//case '':
	//case '':
		return FONT_OFFSET*14;
	case 'P':
	case 'f':
	case 'v':
	case '.':
	case '}':
	//case '':
	//case '':
		return FONT_OFFSET*15;
	// space
	case ' ':
		return FONT_OFFSET * 15;
	}
	// defualt is smiley?
	return FONT_OFFSET * 4;
	//return 0.f;
}

float TextBox::getOffsetY(char c) {
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
		return FONT_OFFSET*2;
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
		return FONT_OFFSET*3;
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
		return FONT_OFFSET*4;
	case '@':
	case '#':
	case '$':
	case '%':
	case '^':
	case '\'':
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
		return FONT_OFFSET*5;
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
