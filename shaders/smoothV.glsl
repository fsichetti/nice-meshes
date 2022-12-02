#version 460

layout(location = 0) uniform mat4 mvp;

layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec3 iNormal;

layout(location = 0) out vec3 oNormal;
layout(location = 1) out vec3 oColour;

void main() {
    gl_Position = mvp * vec4(iPosition, 1.0);
    oNormal = iNormal;
}