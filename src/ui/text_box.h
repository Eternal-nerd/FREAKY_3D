#pragma once

#include <string>
#include <vector>
#include <stdexcept>

#include "../types.h"
#include "../util.h"


class TextBox {
public:
	// position is top left, size and font size are in pixels.
	void init(OverlayMode mode, const std::string& message, glm::vec2 position, glm::vec2 boxSize, float fontSize, glm::vec2 extent, int texIndex);

	void updateText(const std::string& message);

	int map();

	void scale(glm::vec2 extent);

	OverlayMode mode_ = OVERLAY_DEFAULT;

	const std::string name_ = "TEXT BOX";

private:
	std::string message_ = "";

	glm::vec2 position_ = { 0.f, 0.f };
	glm::vec2 boxSize_ = { 0.f, 0.f };
	float fontSize_ = 0.f;

	std::vector<UIVertex> vertices_ = {};
};