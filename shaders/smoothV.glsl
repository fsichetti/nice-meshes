#version 460

layout(location = 0) uniform mat4 mvp;
layout(location = 3) uniform bool showCoordinates;

layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec3 iNormal;
layout(location = 2) in vec2 iTex;

layout(location = 0) out vec3 oNormal;
layout(location = 1) out vec3 oColour;

void main() {
    gl_Position = mvp * vec4(iPosition, 1.);
    oNormal = iNormal;
    if (showCoordinates)
        oColour =
            vec3(1.) * (1-iTex.s) * (1-iTex.t) +
            vec3(1., 0., 0.) * iTex.s * (1-iTex.t) +
            vec3(0., 1., 0.) * iTex.s * iTex.t +
            vec3(0., 0., 1.) * (1-iTex.s) * iTex.t
        ;
    else oColour = vec3(1., .8, .8);
}