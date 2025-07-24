#version 450 

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 nor;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec2 col;

layout(location = 0) out vec3 vTexCoords;

layout(set = 1, binding = 0) uniform Camera
{
	mat4 view; 
	mat4 proj;
	vec3 position;
};



void main()
{
	vTexCoords = pos.xyz;
	mat4 sky_view = mat4(mat3(view));//remove position column 3
	gl_Position = (proj * sky_view * vec4(pos, 1.0f)).xyww;
}
