#version 450 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 nor;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec2 col;

layout(location = 0) out vec3 vFragColour;
layout(location = 1) out vec2 vFragTexCoord;

out VS_OUT
{
	layout(location = 2) vec3 fragPos;
	layout(location = 3) vec3 viewPos;
	layout(location = 4) vec3 normal;
	layout(location = 5) vec4 fragPosLightSpace;
}vs_out;

layout(set = 1, binding = 0) uniform Camera
{
	mat4 view; 
	mat4 proj;
	mat4 lightView;
	mat4 lightProj;
	vec3 pos;
}uCamera;

layout(push_constant) uniform Model{
	layout(offset = 0) mat4 transform;
	vec3 colour;
};


void main()
{
	vec4 vert_world_pos = transform * vec4(pos, 1.0f);
	
	vs_out.fragPos = vert_world_pos.xyz;
	vs_out.normal = mat3(transform) * normalize(nor);
	//vs_out.viewPos = transpose(uCamera.view)[3].xyz;
	vs_out.viewPos = uCamera.pos;

	vs_out.fragPosLightSpace = uCamera.lightProj * uCamera.lightView * transform * vec4(pos, 1.0f);
	
	vFragTexCoord = uv;
	vFragColour = colour;
	gl_Position = uCamera.proj * uCamera.view * vert_world_pos;
	
}

