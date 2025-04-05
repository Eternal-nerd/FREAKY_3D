#include "element.h"

void Element::init(OverlayMode mode, glm::vec2 position, glm::vec2 size, glm::vec2 extent, glm::vec4 textureCoord, int texIndex) {
	util::log(name_, "initializing ui element");

	mode_ = mode;
	position_ = position;
	size_ = size;

	// calculate x and y offsets
	float xOffset = (size.x / extent.x) / 2;
	float yOffset = (size.y / extent.y) / 2;

	// top left
	UIVertex v1;
	v1.pos = { position.x - xOffset, position.y - yOffset };
	v1.texCoord = { textureCoord.x, textureCoord.y};
	v1.texIndex = texIndex;
	vertices_.push_back(v1);

	// bottom left
	UIVertex v2;
	v2.pos = { position.x - xOffset, position.y + yOffset };
	v2.texCoord = { textureCoord.x, textureCoord.y + textureCoord.w };
	v2.texIndex = texIndex;
	vertices_.push_back(v2);

	// top right
	UIVertex v3;
	v3.pos = { position.x + xOffset, position.y - yOffset };
	v3.texCoord = { textureCoord.x + textureCoord.z, textureCoord.y };
	v3.texIndex = texIndex;
	vertices_.push_back(v3);

	// bottom right
	UIVertex v4;
	v4.pos = { position.x + xOffset, position.y + yOffset };
	v4.texCoord = { textureCoord.x + textureCoord.z, textureCoord.y + textureCoord.w };
	v4.texIndex = texIndex;
	vertices_.push_back(v4);

}


int Element::map(UIVertex* mapped, int overrideIndex) {
	// for each vertex
	int count = 0;
	for (int i = 0; i < vertices_.size(); i++) {
		mapped->pos.x = vertices_[i].pos.x; // position x
		mapped->pos.y = vertices_[i].pos.y; // position y
		mapped->texCoord.x = vertices_[i].texCoord.x; // tex coord x
		mapped->texCoord.y = vertices_[i].texCoord.y; // tex coord y
		(overrideIndex==-1) ? mapped->texIndex = vertices_[i].texIndex : mapped->texIndex = overrideIndex; // tex index
		mapped++;
		count++;
	}
	return count;
}

void Element::scale(glm::vec2 extent) {
	if (vertices_.size() != 4) {
		throw std::runtime_error("unable to scale ui element (not 4 vertices)");
	}

	// calculate x and y offsets
	float xOffset = (size_.x / extent.x) / 2;
	float yOffset = (size_.y / extent.y) / 2;
	vertices_[0].pos.x = position_.x - xOffset;
	vertices_[0].pos.y = position_.y - yOffset;

	vertices_[1].pos.x = position_.x - xOffset;
	vertices_[1].pos.y = position_.y + yOffset;

	vertices_[2].pos.x = position_.x + xOffset;
	vertices_[2].pos.y = position_.y - yOffset;

	vertices_[3].pos.x = position_.x + xOffset;
	vertices_[3].pos.y = position_.y + yOffset;
}


