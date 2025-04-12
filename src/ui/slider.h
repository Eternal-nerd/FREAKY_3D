#pragma once

#include <string>

#include "../types.h"
#include "../util.h"

class Slider {
public:
	void init(const std::string& id, OverlayMode mode, glm::vec2 position, glm::vec2 size, glm::vec2 extent, int knobTexIndex, int barTexIndex);

	const std::string name_ = "SLIDER";

private:


};

