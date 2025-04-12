#include "element.h"

void Element::init(const std::string& id, OverlayMode mode, UIQuad quad, glm::vec2 extent) {
	util::log(name_, "initializing ui element");

	id_ = id;
	mode_ = mode;
	quad_ = quad;
	extent_ = extent;

	hovered_ = false;
	int interaction = 0;

	// calculate x and y offsets
	float xOffset = (quad_.sizePixels.x / extent.x) / 2;
	float yOffset = (quad_.sizePixels.y / extent.y) / 2;

	// top left
	UIVertex v1;
	v1.pos = { quad_.position.x - xOffset, quad_.position.y - yOffset };
	v1.texCoord = { quad_.textureCoord.x, quad_.textureCoord.y};
	v1.texIndex = quad_.texIndex;
	v1.interaction = interaction;
	vertices_.push_back(v1);

	// bottom left
	UIVertex v2;
	v2.pos = { quad_.position.x - xOffset, quad_.position.y + yOffset };
	v2.texCoord = { quad_.textureCoord.x, quad_.textureCoord.y + quad_.textureCoord.w };
	v2.texIndex = quad_.texIndex;
	v2.interaction = interaction;
	vertices_.push_back(v2);

	// top right
	UIVertex v3;
	v3.pos = { quad_.position.x + xOffset, quad_.position.y - yOffset };
	v3.texCoord = { quad_.textureCoord.x + quad_.textureCoord.z, quad_.textureCoord.y };
	v3.texIndex = quad_.texIndex;
	v3.interaction = interaction;
	vertices_.push_back(v3);

	// bottom right
	UIVertex v4;
	v4.pos = { quad_.position.x + xOffset, quad_.position.y + yOffset };
	v4.texCoord = { quad_.textureCoord.x + quad_.textureCoord.z, quad_.textureCoord.y + quad_.textureCoord.w };
	v4.texIndex = quad_.texIndex;
	v4.interaction = interaction;
	vertices_.push_back(v4);
}

void Element::checkHover(float xPos, float yPos) {
	hovered_ = quad_.isPointWithin(xPos, yPos, extent_);

	// maybe not good way to do it
	if (vertices_.size() != 4) {
		throw std::runtime_error("unable to highlight ui element (not 4 vertices)");
	}

	int interaction = (hovered_) ? 1 : 0;

	vertices_[0].interaction = interaction;
	vertices_[1].interaction = interaction;
	vertices_[2].interaction = interaction;
	vertices_[3].interaction = interaction;
}

void Element::resetInteraction() {
	// maybe not good way to do it
	if (vertices_.size() != 4) {
		throw std::runtime_error("unable to highlight ui element (not 4 vertices)");
	}
	vertices_[0].interaction = 0;
	vertices_[1].interaction = 0;
	vertices_[2].interaction = 0;
	vertices_[3].interaction = 0;
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
		mapped->interaction = vertices_[i].interaction; // for checking hover
		mapped++;
		count++;
	}
	return count;
}

void Element::scale(glm::vec2 extent) {
	if (vertices_.size() != 4) {
		throw std::runtime_error("unable to scale ui element (not 4 vertices)");
	}

	extent_ = extent;

	// calculate x and y offsets
	float xOffset = (quad_.sizePixels.x / extent.x) / 2;
	float yOffset = (quad_.sizePixels.y / extent.y) / 2;
	vertices_[0].pos.x = quad_.position.x - xOffset;
	vertices_[0].pos.y = quad_.position.y - yOffset;

	vertices_[1].pos.x = quad_.position.x - xOffset;
	vertices_[1].pos.y = quad_.position.y + yOffset;

	vertices_[2].pos.x = quad_.position.x + xOffset;
	vertices_[2].pos.y = quad_.position.y - yOffset;

	vertices_[3].pos.x = quad_.position.x + xOffset;
	vertices_[3].pos.y = quad_.position.y + yOffset;
}