#version 450

layout(location = 0) in vec3 vFragColour;

layout(location = 0) out vec4 outColour;

void main() {

    vec2 coord = gl_PointCoord - vec2(0.5f);
    outColour = vec4(vFragColour, 0.5f - length(coord));
    //outColour = vec4(vFragColour, 0.5f);
}