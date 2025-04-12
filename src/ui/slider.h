#pragma once

#include <string>

#include "../types.h"
#include "../util.h"
#include "element.h"

class Slider {
public:
	void init(const std::string& id, OverlayMode mode, UIQuad knobQuad, UIQuad barQuad);

	std::string id_ = "";

	OverlayMode mode_ = OVERLAY_DEFAULT;

	const std::string name_ = "SLIDER";

private:



};

