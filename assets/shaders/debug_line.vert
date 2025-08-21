#version 450

//vertex input is assumed to be in world pos
//so no transformation matrix is expected as an input
layout(location = 0) in vec3 pos;
layout(location = 1) in vec4 col;

layout(location = 0) out vec4 vFragColour;

//ned to project to world space 
layout(set = 1, binding = 0) uniform Camera
{
	mat4 view; 
	mat4 proj;
	mat4 lightView;
	mat4 lightProj;
	vec3 pos;
}uCamera;


void main() 
{
    gl_Position =  uCamera.proj * uCamera.view * vec4(pos, 1.0f);
    vFragColour = col;
}