#version 460

layout(location = 1) uniform vec3 lightDir;
layout(location = 2) uniform float alpha;
const float minLight = 0.;
const vec3 frontColour = vec3(0.8, 0.8, 1.0);
const vec3 backColour = vec3(1.0, 0.8, 0.8);

layout(location = 0) in vec3 iNormal;

layout(location = 0) out vec4 oColour;


void main() {
    const float d = dot(normalize(iNormal), lightDir);
    const vec3 faceColour = d > 0 ? frontColour : backColour;
    float light = max(abs(d), minLight);
    oColour = vec4(faceColour * light, alpha);
}