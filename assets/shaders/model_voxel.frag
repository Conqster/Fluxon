#version 450 core

layout(location = 0) flat in int fSliceIndex;

layout(set = 0, binding = 2, r16f) uniform image2DArray uVoxelOccupancyTex;
//layout(location = 0, r16f) out image2DArray uVoxelOccupancyTex;
//layout(location = 0) out image2DArray uVoxelOccupancyTex;

void main()
{
	ivec3 coord = ivec3(gl_FragCoord.xy, fSliceIndex);
	//mark/fill voxel as occupied
	imageStore(uVoxelOccupancyTex, coord, vec4(1.0f)); 
}