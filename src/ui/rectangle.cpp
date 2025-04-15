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
	// calculate x and y offsets
	float xOffset = (sizePixels_.x / extent_.width) * scale;
	float yOffset = (sizePixels_.y / extent_.height) * scale;

	// bottom left
	quad_.vertices[1].pos = { position_.x, position_.y + yOffset };

	// top right
	quad_.vertices[2].pos = { position_.x + xOffset, position_.y };

	// bottom right
	quad_.vertices[3].pos = { position_.x + xOffset, position_.y + yOffset };

}

/*-----------------------------------------------------------------------------
------------------------------MAPPING-TO-BUFFER--------------------------------
-----------------------------------------------------------------------------*/
int Rectangle::map(UIVertex* mapped, int overrideIndex) {
	// for each vertex
	int count = 0;
	for (int i = 0; i < 4; i++) {
		mapped->pos.x = quad_.vertices[i].pos.x; // position x
		mapped->pos.y = quad_.vertices[i].pos.y; // position y
		mapped->texCoord.x = quad_.vertices[i].texCoord.x; // tex coord x
		mapped->texCoord.y = quad_.vertices[i].texCoord.y; // tex coord y
		(overrideIndex == -1) ? mapped->texIndex = quad_.vertices[i].texIndex : mapped->texIndex = overrideIndex; // tex index
		mapped->interaction = quad_.vertices[i].interaction; // for checking hover
		mapped++;
		count++;
	}
	return count;
}