#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

#include "../types.h"
#include "../util.h"

class TextBox {
public:
	// position is top left, size and font size are in pixels.
	void init(OverlayMode mode, OverlayPositionMode positionMode, const std::string& message, glm::vec2 position, glm::vec2 boxSize, glm::vec2 fontSize, glm::vec2 extent, int texIndex);

	void updateText(const std::string& message);
	// returns true if passed message == this object's message
	bool compareMessage(const std::string& compareString);

	int map(UIVertex* mapped, int overrideIndex = -1);

	void scale(glm::vec2 extent);

	int getQuadCount();

	OverlayMode mode_ = OVERLAY_DEFAULT;

	const std::string name_ = "TEXT BOX";

private:
	OverlayPositionMode positionMode_ = OVERLAY_POSITION_CENTERED;

	std::string message_ = "";

	int texIndex_ = 0;

	glm::vec2 position_ = { 0.f, 0.f };
	glm::vec2 boxSize_ = { 0.f, 0.f };
	glm::vec2 fontSize_ = { 0.f, 0.f };

	glm::vec2 extent_ = { 0.f, 0.f };

	std::vector<UIVertex> vertices_ = {};

	void generateVertices();

	float getOffsetX(char c);
	float getOffsetY(char c);
};