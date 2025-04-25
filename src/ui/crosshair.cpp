#include "crosshair.h"

/*-----------------------------------------------------------------------------
------------------------------INITIALIZATION-----------------------------------
-----------------------------------------------------------------------------*/
void Crosshair::init(OverlayState& state, glm::vec2 sizePixels, int texIndex) {
    util::log(name_, "initializing overlay crosshair");

    state_ = &state;
    sizePixels_ = sizePixels;
    texIndex_ = texIndex;

    vertices_.resize(4);
    initVertices();
}

/*-----------------------------------------------------------------------------
------------------------------MAPPING-TO-BUFFER--------------------------------
-----------------------------------------------------------------------------*/
int Crosshair::mapLines(UIVertex* mapped) {
    for (int i = 0; i < 4; i++) {
        mapped->pos = vertices_[i].pos; // position 
        mapped->texCoord = {}; // tex coord x
        mapped->texIndex = texIndex_; // texture index
        mapped->interaction = 0; // interaction used in shader
        mapped++;
    }

    // 4 total points to make crosshair
    return 4;
}

/*-----------------------------------------------------------------------------
--------------------------------UPDATES----------------------------------------
-----------------------------------------------------------------------------*/
void Crosshair::scale() {
    initVertices();
}

/*-----------------------------------------------------------------------------
-----------------------------HELPER--------------------------------------------
-----------------------------------------------------------------------------*/
void Crosshair::initVertices() {
    // calculate width and height from pixels
    float xOffset = (sizePixels_.x / state_->extent.width);
    float yOffset = (sizePixels_.y / state_->extent.height);

    // top crosshair vertex
    vertices_[0].pos = { 0.f, -yOffset };
    vertices_[0].texCoord = {}; // ????
    vertices_[0].texIndex = texIndex_;
    vertices_[0].interaction = 0;

    // bottom crosshair vertex
    vertices_[1].pos = { 0.f, yOffset };
    vertices_[1].texCoord = {}; // ????
    vertices_[1].texIndex = texIndex_;
    vertices_[1].interaction = 0;

    // left crosshair vertex
    vertices_[2].pos = { -xOffset, 0.f };
    vertices_[2].texCoord = {}; // ????
    vertices_[2].texIndex = texIndex_;
    vertices_[2].interaction = 0;

    // right crosshair vertex
    vertices_[3].pos = { xOffset, 0.f };
    vertices_[3].texCoord = {}; // ????
    vertices_[3].texIndex = texIndex_;
    vertices_[3].interaction = 0;
}