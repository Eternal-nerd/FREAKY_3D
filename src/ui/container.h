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


	std::string id_ = "";


	OverlayMode mode_ = OVERLAY_DEFAULT;
	OverlayContainerType type_ = OVERLAY_CONTAINER_BOX;

	const std::string name_ = "CONTAINER";

private:


};