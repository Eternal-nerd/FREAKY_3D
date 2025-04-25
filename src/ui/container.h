#pragma once

#include <string>
#include <vector>
#include <stdexcept>

#include "../types.h"
#include "../util.h"
#include "rectangle.h"

class Container {
public:
	void init(OverlayState& state, OverlayElementState* elementState, const std::string& id, OverlayContainerType type, glm::vec2 position, glm::vec2 sizePixels);

	// add rectangles
	void addRectangle(Rectangle rectangle);

	// remove rectangles from container
	void removeRectangles(int amount);

	glm::vec2 getPosition();

	// returns the number of vertices (aka how much to increment pointer)
	int map(UIVertex* mapped, int overrideIndex = -1);
	int mapLines(UIVertex* mapped);

	// events
	void scale();
	void onMouseMove();
	void onMouseButton();
	void resetInteraction();

	void cleanup();

	bool unique_ = false;

	std::vector<Rectangle> rectangles_ = {};

	std::string id_ = "";

	OverlayContainerType type_ = OVERLAY_CONTAINER_BOX;

	const std::string name_ = "CONTAINER";

private:
	OverlayState* state_;
	OverlayElementState* elementState_;

	glm::vec2 position_ = { 0.f, 0.f };
	glm::vec2 sizePixels_ = { 0.f, 0.f };

	std::vector<UIVertex> borderLines_ = {};

	float xOffset_ = 0.f;
	float yOffset_ = 0.f;

	glm::vec2 insertPosition_ = { 0.f, 0.f };

	void resetRectanglePositions();
	void updateInteraction();
	void rePosition();

	void createBorderLines();
};