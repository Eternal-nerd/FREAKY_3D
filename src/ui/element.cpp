#include "element.h"

void Element::init(const std::vector<UIVertex>& vertices, float scale) {
	util::log(name_, "initializing ui element");
	
	if (vertices.size() < 1) {
		throw std::runtime_error("not enough vertices to initialize ui element");
	}

	vertices_ = vertices;
	texIndex_ = vertices[0].texIndex;
	scale_ = scale;


}

void Element::map(UIVertex* mapped, int overrideIndex) {
	// for each vertex
	for (int i = 0; i < vertices_.size(); i++) {
		mapped->pos.x = vertices_[i].pos.x; // position x
		mapped->pos.y = vertices_[i].pos.y; // position y
		mapped->texCoord.x = vertices_[i].texCoord.x; // tex coord x
		mapped->texCoord.y = vertices_[i].texCoord.y; // tex coord y
		(overrideIndex==-1) ? mapped->texIndex = vertices_[i].texIndex : mapped->texIndex = overrideIndex; // tex index
		mapped++;
	}

}

void Element::updateTextureIndex(int index) {
	texIndex_ = index;
	// TODO
	for (auto v : vertices_) {
		v.texIndex = index;
	}

}

void Element::scale(float factor) {
	scale_ = factor;
}


