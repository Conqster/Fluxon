#version 450 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 nor;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec2 col;

layout(set = 0, binding = 0) uniform Camera
{
	mat4 view;
	mat4 proj;
};


layout(set = 0, binding = 1) uniform VoxelParameter
{
	//size of one voxel in world units
	float voxelSize;
	//number of Z slices(layer) in 2D texture array
	int sliceCount;
	float minZ;

	int _padding0;
};

layout(push_constant) uniform Model{
	layout(offset = 0) mat4 transform;
	vec3 colour;
};

layout(location = 0) out int vSliceIndex;

void main()
{
	vec4 world_pos = transform * vec4(pos, 1.0f);

	//compute slice index based on Z pos
	float z_offset = world_pos.z - minZ;
	int slice = int(z_offset / voxelSize);
	slice = clamp(slice, 0, sliceCount - 1);
	vSliceIndex = slice;

	//Project to orthographics XY plane
	vec4 view_pos = view * world_pos;
	vec4 clip_pos = proj * view_pos;

	gl_Position = clip_pos;
}