#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec4 colour;

layout(location = 0) out vec3 vFragColour;


//ned to project to world space 
layout(set = 1, binding = 0) uniform Camera
{
	mat4 view; 
	mat4 proj;
	mat4 lightView;
	mat4 lightProj;
	vec3 pos;
}uCamera;



layout(push_constant) uniform Emitter{
	layout(offset = 0) mat4 transform;
	vec3 colour;
}uEmitter;




void main() {

    gl_PointSize = 10.0f;
    gl_Position =  uCamera.proj * uCamera.view * uEmitter.transform * vec4(pos, 1.0f);
    //gl_Position = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    vFragColour = abs(colour.rgb);
}