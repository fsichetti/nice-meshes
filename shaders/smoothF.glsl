#version 460

layout(location = 1) uniform vec3 lightDir;
layout(location = 2) uniform float alpha;
layout(location = 3) uniform bool showCoordinates;
const float minLight = 0.;

layout(location = 0) in vec3 iNormal;
layout(location = 1) in vec3 iColour;

layout(location = 0) out vec4 oColour;


void main() {
    const vec3 frontColour = iColour;
    const vec3 backColour = (showCoordinates) ? frontColour : frontColour.brg;
    const float d = dot(normalize(iNormal), lightDir);
    const vec3 faceColour = (d > 0) ? frontColour : backColour;
    float light = max(abs(d), minLight);
    oColour = vec4(faceColour * light, alpha);
}