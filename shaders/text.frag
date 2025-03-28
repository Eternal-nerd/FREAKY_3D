#version 450

#extension GL_EXT_nonuniform_qualifier : require

layout(binding = 0) uniform sampler2D texSamplers[];

layout(location = 0) in vec2 inTexCoord;
layout(location = 1) flat in int inTexIndex;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(texSamplers[nonuniformEXT(inTexIndex)], inTexCoord);
    if (outColor.w < 0.8) {
        discard;
    }
}