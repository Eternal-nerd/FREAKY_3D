#pragma once

#include <string>
#include <vector>
#include <stdexcept>

#include "../types.h"
#include "../util.h"

class Text {
public:
	void init(OverlayState& state, OverlayElementState* elementState, const std::string& id, const std::string message, glm::vec2 position, glm::vec2 sizePixels);

	// returns the number of vertices (aka how much to increment pointer)
	int map(UIVertex* mapped, int overrideIndex = -1);
	int mapLines(UIVertex* mapped);

	// events
	void scale();
	void onMouseMove();
	void onMouseButton();
	void resetInteraction();
	void needsRemap();

	void cleanup();

	bool unique_ = false;

	const std::string name_ = "TEXT";

private:
	OverlayState* state_;
	OverlayElementState* elementState_;

	glm::vec2 position_ = { 0.f, 0.f };
	glm::vec2 sizePixels_ = { 0.f, 0.f };

};