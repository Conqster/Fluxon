#version 450 

layout(location = 0) in vec3 pos;

layout(set = 0, binding = 0) uniform LightSpace
{
	mat4 matrix; 
}uLightSpace;



layout(push_constant) uniform Model{
	layout(offset = 0) mat4 transform;
}uModel;


void main()
{
	gl_Position = uLightSpace.matrix * uModel.transform * vec4(pos, 1.0f);
}
