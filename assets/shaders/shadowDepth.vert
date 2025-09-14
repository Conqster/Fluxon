#version 450 

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 nor;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec2 col;

layout(set = 0, binding = 0) uniform LightSpace
{
	mat4 view; 
	mat4 proj;
	vec3 pos;
}uLightSpace;


layout(set = 0, binding = 1) uniform Move
{
	vec4 position;
};



layout(push_constant) uniform Model{
	layout(offset = 0) mat4 transform;
}uModel;


out gl_PerVertex
{
	vec4 gl_Position;
};


void main()
{
	//gl_Position = uLightSpace.matrix * uModel.transform * vec4(pos, 1.0f);
	gl_Position = (uLightSpace.proj * uLightSpace.view *  uModel.transform * (vec4(pos, 1.0f) + position));
	//gl_Position = vec4(0.0f, 0.0f, 0.5f, 1.0f);
}
