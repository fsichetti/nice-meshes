#version 460
layout(location = 0) out vec4 fColour;

layout(location = 2) uniform vec4 baseColour;

void main() {
    fColour = baseColour;
}