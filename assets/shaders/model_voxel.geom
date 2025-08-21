#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

layout(location = 0) in int vSliceIndex[];
layout(location = 0) out int fSliceIndex;

const int kMaxSliceCount = 128;

void main()
{
	int slice = clamp(vSliceIndex[0], 0, kMaxSliceCount - 1);
	//tell gpu the 2d array right layer to raster in
	gl_Layer = slice;

	for(int i = 0; i < 3; ++i)
	{
		gl_Position = gl_in[i].gl_Position;
		//tell gpu the 2d array right layer to raster in
		//for the emitting vert in frag
		fSliceIndex = slice;
		EmitVertex();
	}
	EndPrimitive();
}	