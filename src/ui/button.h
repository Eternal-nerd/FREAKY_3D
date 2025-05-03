#pragma once

#include <string>
#include <vector>
#include <stdexcept>

#include "../types.h"
#include "../util.h"
#include "base_element.h"
#include "text.h"
#include "rectangle.h"

// TODO: allow assignment of a function pointer to button that executes on click
class Button : public BaseElement {
public:
	void init(OverlayState& state, OverlayElementState* elementState, const std::string& id, const std::string label, glm::vec2 position, glm::vec2 sizePixels, glm::vec2 fontSize, int fontIndex, int backgroundIndex);

	// returns the number of vertices (aka how much to increment pointer)
	int map(UIVertex* mapped, int overrideIndex = -1);
	// int mapLines(UIVertex* mapped);

	void setAction(void (*func)());

	// events
	void scale();
	void onMouseMove();
	void onMouseButton();
	void resetInteraction();

	std::string id_ = "";

	const std::string name_ = "BUTTON";

private:
    void (*action_)() = nullptr;

	int labelLength_ = 0;
    Text label_;
	Rectangle clickable_;

	glm::vec2 sizePixels_ = { 0.f, 0.f };
	glm::vec2 fontSize_ = { 0.f, 0.f };

	int fontTexIndex_ = -1;
	int backgroundTexIndex_ = -1;

	void centerLabel();
};
