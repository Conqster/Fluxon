#version 450

layout(location = 0) out vec4 FragColour;

layout(location = 0) in vec3 vTexCoords;

layout(set = 2, binding = 0) uniform samplerCube uSkyboxTex;

void main()
{
	FragColour = texture(uSkyboxTex, vTexCoords);
}
