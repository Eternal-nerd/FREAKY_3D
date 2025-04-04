#pragma once

#include <string>
#include <vector>
#include <stdexcept>

#include "../types.h"
#include "../util.h"


class TextBox {
public:
	// 
	void init(const std::string& message, glm::vec2 position, glm::vec2 boxSize, float fontSize, glm::vec2 extent, int texIndex);

private:
	std::vector<UIVertex> vertices_ = {};
};