#pragma once

#include <string>
#include <vector>
#include <stdexcept>

#include "../types.h"
#include "../util.h"

/*
class to contain details about one individual ui element to be rendered by the overlay
*/
class Element {
public:
	void init(const std::vector<UIVertex>& vertices, glm::vec2 scale);

	void map(UIVertex* mapped, int overrideIndex=-1);

	void updateTextureIndex(int index);

	void scale(glm::vec2 factor);

	const std::string name_ = "ELEMENT";

private:
	int vertexCount_ = 0;
	std::vector<UIVertex> vertices_ = {};
	
	int texIndex_ = 0;
	glm::vec2 scale_ = { 0.f, 0.f };

	glm::vec2 position_ = {0.f, 0.f};

};