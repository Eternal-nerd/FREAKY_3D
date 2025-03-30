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
	void init(const std::vector<UIVertex>& vertices, float scale);

	void map(UIVertex* mapped, int overrideIndex=-1);

	void updateTextureIndex(int index);

	void scale(float factor);

	const std::string name_ = "ELEMENT";

private:
	int vertexCount_ = 0;
	std::vector<UIVertex> vertices_ = {};
	
	int texIndex_ = 0;
	float scale_ = 0.f;

};