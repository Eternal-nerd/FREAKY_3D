#pragma once

#include <string>
#include <vector>
#include <stdexcept>

#include "../types.h"
#include "../util.h"
#include "base_element.h"
#include "rectangle.h"
#include "text.h"

class Slider : public BaseElement {
public:
	void init(OverlayState& state, OverlayElementState* elementState, const std::string& id, const std::string label, glm::vec2 limits, float initValue, glm::vec2 position, glm::vec2 sizePixels, int fontIndex, int backgroundIndex, int knobIndex, int barIndex);

	// returns the number of vertices (aka how much to increment pointer)
	int map(UIVertex* mapped, int overrideIndex = -1);
	// int mapLines(UIVertex* mapped);

	void setAction(void (*func)(float));

	float getValue();

	// events
	void scale();
	void onMouseMove();
	void onMouseButton();
	void resetInteraction();

	// override cleanup function to cleanup knob state variable
	void cleanup();

	std::string id_ = "";

	const std::string name_ = "SLIDER";

private:
	void (*action_)(float) = nullptr;

	glm::vec2 limits_ = {0.f, 0.f};
	float currentValue_ = 0.f;

	Text label_;
	Text reading_;
	Rectangle background_;
	Rectangle bar_;
	Rectangle knob_;
	OverlayElementState* knobState_ = nullptr;

	int fontTexIndex_ = -1;
	int backgroundTexIndex_ = -1;
	int knobTexIndex_ = -1;
	int barTexIndex_ = -1;

	// helpers
	void orientElements();
	void knobMove();
};