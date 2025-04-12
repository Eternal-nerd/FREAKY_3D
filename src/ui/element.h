#pragma once

#include <string>
#include <vector>
#include <stdexcept>

#include "../types.h"
#include "../util.h"

/*
class to contain details about one individual ui element to be rendered by the overlay
*/
class Element {
public:
	// position = {x=xPos, y=yPos}
	// size = { widthPixels, heightPixels, screenWidth, screenHeight }
	// textureCoord = {x=xPos, y=yPos, z=xOffset, w=yOffset}
	void init(const std::string& id, OverlayMode mode, UIQuad quad, glm::vec2 extent);

	void checkHover(float xPos, float yPos);

	void resetInteraction();

	// returns amount to iter pointer
	int map(UIVertex* mapped, int overrideIndex = -1);

	void scale(glm::vec2 extent);

	bool hovered_ = false;

	std::string id_ = "";

	OverlayMode mode_ = OVERLAY_DEFAULT;

	const std::string name_ = "ELEMENT";

private:
	UIQuad quad_;

	glm::vec2 extent_ = { 0.f, 0.f };

	std::vector<UIVertex> vertices_ = {};

};