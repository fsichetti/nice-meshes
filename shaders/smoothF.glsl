#version 460

layout(location = 1) uniform vec3 lightDir;
layout(location = 2) uniform float alpha;
const float minLight = 0.2;

layout(location = 0) in vec3 iNormal;
layout(location = 1) in vec3 iColour;

layout(location = 0) out vec4 oColour;


void main() {
    float light = max(dot(iNormal, lightDir), minLight);
    oColour = vec4(iColour * light, alpha);
}