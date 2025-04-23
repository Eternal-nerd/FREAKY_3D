#pragma once

#include <string>
#include <vector>
#include <stdexcept>

#include "../types.h"
#include "../util.h"
#include "rectangle.h"

class Container {
public:
	void init(const std::string& id, OverlayMode mode, OverlayContainerType type, glm::vec2 position, glm::vec2 sizePixels, VkExtent2D extent);

	// add rectangles
	void addRectangle(Rectangle rectangle);

	// returns the number of vertices (aka how much to increment pointer)
	int map(UIVertex* mapped, int overrideIndex = -1);

	// modify attributes
	void scale(float scaleFactor);
	void setMovable(bool state);

	// events
	void onResize(VkExtent2D extent);
	void onMouseMove(glm::vec2 mousePos);
	void onMouseButton(bool down);
	void resetInteraction();
	void needsRemap();

	// getters for if in container
	glm::vec2 getDimensions();

	// getters for updating config file
	glm::vec2 getPosition();

	bool hovered_ = false;
	bool dragged_ = false;
	int interaction_ = 0;

	std::string id_ = "";

	OverlayMode mode_ = OVERLAY_DEFAULT;
	OverlayContainerType type_ = OVERLAY_CONTAINER_BOX;

	const std::string name_ = "CONTAINER";

private:
	glm::vec2 position_ = { 0.f, 0.f };
	glm::vec2 sizePixels_ = { 0.f, 0.f };
	VkExtent2D extent_;

	float scale_ = 1.f;
	bool movable_ = false;

	std::vector<Rectangle> rectangles_ = {};

	float rightBoundary_ = 0.f;
	float bottomBoundary_ = 0.f;

	glm::vec2 insertPosition_ = { 0.f, 0.f };

	void resetRectanglePositions();
};