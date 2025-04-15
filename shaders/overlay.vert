#version 450

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in int inTextureIndex;
layout(location = 3) in int inInteraction;

layout (location = 0) out vec2 outTexCoord;
layout(location = 1) flat out int outTexIndex;
layout(location = 2) flat out int outInteraction;

void main(void) {
	gl_Position = vec4(inPos, 0.0, 1.0);
	outTexCoord = inTexCoord;
    outTexIndex = inTextureIndex;
	outInteraction = inInteraction;
}