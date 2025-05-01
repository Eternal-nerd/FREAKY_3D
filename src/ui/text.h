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

	// update
	void updateMessage(const std::string& message);
	void setBorder(bool state);

	glm::vec2 getPosition();
	void setPosition(glm::vec2 position);

	void setElementStateUpdate(bool state);

	// returns the number of vertices (aka how much to increment pointer)
	int map(UIVertex* mapped, int overrideIndex = -1);
	int mapLines(UIVertex* mapped);

	// events
	void scale();
	void onMouseMove();
	void onMouseButton();
	void resetInteraction();

    OverlayMode getMode();
    void setMode(OverlayMode mode);

	void cleanup();

	bool unique_ = false;

	std::string id_ = "";

	const std::string name_ = "TEXT";

private:
	OverlayState* state_ = nullptr;
	OverlayElementState* elementState_ = nullptr;

	std::string message_ = "";
	glm::vec2 position_ = { 0.f, 0.f };
	glm::vec2 sizePixels_ = { 0.f, 0.f };
	glm::vec2 fontSize_ = { 0.f, 0.f };
	int texIndex_ = -1;

	Container container_;

	// inits container with blank rectangles for each letter in initial string
	void populateContainer();
	// sets texture coords for each rectangle
	void populateCoords();


	// specific font texture
	float getOffsetX(char c);
	float getOffsetY(char c);
};
