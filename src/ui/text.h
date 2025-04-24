#pragma once

#include <string>
#include <vector>
#include <stdexcept>

#include "../types.h"
#include "../util.h"
#include "container.h"
#include "rectangle.h"

class Text {
public:
	void init(OverlayState& state, OverlayElementState* elementState, const std::string& id, const std::string message, glm::vec2 position, glm::vec2 sizePixels, glm::vec2 fontSize, int texIndex);

	void updateMessage(const std::string& message);
	std::string getMessage();

	glm::vec2 getPosition();

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

	std::string id_ = "";

	std::string message_ = "";

	const std::string name_ = "TEXT";

private:
	OverlayState* state_;
	OverlayElementState* elementState_;

	glm::vec2 position_ = { 0.f, 0.f };
	glm::vec2 sizePixels_ = { 0.f, 0.f };
	glm::vec2 fontSize_ = { 0.f, 0.f };
	int texIndex_ = -1;

	Container container_;

	void populateContainer();

	// specific font texture
	float getOffsetX(char c);
	float getOffsetY(char c);
};