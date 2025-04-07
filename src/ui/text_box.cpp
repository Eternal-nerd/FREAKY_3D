#include "text_box.h"

void TextBox::init(OverlayMode mode, const std::string& message, glm::vec2 position, glm::vec2 boxSize, glm::vec2 fontSize, glm::vec2 extent, int texIndex) {
	util::log(name_, "initializing ui text box");

	mode_ = mode;
	message_ = message;
	position_ = position;
	boxSize_ = boxSize;
	fontSize_ = fontSize;

	// calculate x and y offsets (position = TOP LEFT of box
	float scaledWidth = (boxSize.x / extent.x);
	float scaledHeight = (boxSize.y / extent.y);

	float scaledFontWidth = (fontSize.x / extent.x);
	float scaledFontHeight = (fontSize.y / extent.y);

	vertices_.resize(message.length() * 4);

	// initialize vertices in vector
	float currentX = position.x - (scaledWidth/2);
	float currentY = position.y - (scaledHeight/2);
	for (int i = 0; i < message.length(); i++) {

		// case where font letter will exceed boundary box
		if (currentX + scaledFontWidth > position.x + (scaledWidth / 2)) {
			util::log("HEEEEEEEEEEEEERRRE", "ye");
		}

		// TODO MAYBE add case for letters exceeding box height

		float fontX = getOffsetX(message[i]);
		float fontY = getOffsetY(message[i]);

		// top left
		vertices_[(i * 4) + 0].pos = { currentX, currentY };
		vertices_[(i * 4) + 0].texCoord = { fontX, fontY};
		vertices_[(i * 4) + 0].texIndex = texIndex;

		// bottom left
		vertices_[(i * 4) + 1].pos = { currentX, currentY + scaledFontHeight };
		vertices_[(i * 4) + 1].texCoord = { fontX, fontY + FONT_OFFSET };
		vertices_[(i * 4) + 1].texIndex = texIndex;

		// top right
		vertices_[(i * 4) + 2].pos = { currentX + scaledFontWidth, currentY };
		vertices_[(i * 4) + 2].texCoord = { fontX + FONT_OFFSET, fontY };
		vertices_[(i * 4) + 2].texIndex = texIndex;

		// bottom right
		vertices_[(i * 4) + 3].pos = { currentX + scaledFontWidth, currentY + scaledFontHeight };
		vertices_[(i * 4) + 3].texCoord = { fontX + FONT_OFFSET, fontY + FONT_OFFSET };
		vertices_[(i * 4) + 3].texIndex = texIndex;

		currentX += scaledFontWidth;
		currentY += scaledFontHeight;

		std::cout << "letter: " << message[i] << "\n";
		
	}

	for (int i = 0; i < vertices_.size(); i++) {
		std::cout << "textbox vertex[" << i << "]: \n" <<
			"pos: [" << vertices_[i].pos.x << ", " << vertices_[i].pos.y << "]\n" <<
			"texCoord: " << vertices_[i].texCoord.x << ", " << vertices_[i].texCoord.y << "]\n" <<
			"texIndex: " << vertices_[i].texIndex << "\n";
	}
}

void TextBox::updateText(const std::string& message) {

}

int TextBox::map(UIVertex* mapped, int overrideIndex) {
	return 0;
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

void TextBox::scale(glm::vec2 extent) {

}

int TextBox::getQuadCount() {
	return message_.length();
}


float TextBox::getOffsetX(char c) {
	switch (c) {
	case 'c':
		return 0.f;

	}

	return 0.f;
}

float TextBox::getOffsetY(char c) {
	switch (c) {
	case 'c':
		return 0.f;

	}
	return 0.f;
}
