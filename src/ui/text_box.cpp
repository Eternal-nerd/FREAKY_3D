#include "text_box.h"

void TextBox::init(OverlayMode mode, const std::string& message, glm::vec2 position, glm::vec2 boxSize, float fontSize, glm::vec2 extent, int texIndex) {
	util::log(name_, "initializing ui text box");

	mode_ = mode;
	message_ = message;
	position_ = position;
	boxSize_ = boxSize;
	fontSize_ = fontSize;

	// calculate x and y offsets (position = TOP LEFT of box
	float scaledWidth = (boxSize.x / extent.x);
	float scaledHeight = (boxSize.y / extent.y);

	vertices_.resize(message.length() * 4);
	util::log("vertices_.size()", std::to_string(vertices_.size()));
	vertices_[0].pos.x = 1;
	/*
	// top left
	UIVertex v1;
	v1.pos = { position.x, position.y };
	v1.texCoord = { 0, 0 };
	v1.texIndex = texIndex;
	vertices_.push_back(v1);

	// bottom left
	UIVertex v2;
	v2.pos = { position.x, position.y + yOffset };
	v2.texCoord = { 0, 1 };
	v2.texIndex = texIndex;
	vertices_.push_back(v2);

	// top right
	UIVertex v3;
	v3.pos = { position.x + xOffset, position.y };
	v3.texCoord = { 1, 0 };
	v3.texIndex = texIndex;
	vertices_.push_back(v3);

	// bottom right
	UIVertex v4;
	v4.pos = { position.x + xOffset, position.y + yOffset };
	v4.texCoord = { 1, 1 };
	v4.texIndex = texIndex;
	vertices_.push_back(v4);
	*/

}

void TextBox::updateText(const std::string& message) {

}

int TextBox::map() {
	return 0;
}

void TextBox::scale(glm::vec2 extent) {

}