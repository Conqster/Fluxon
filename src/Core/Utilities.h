#pragma once

#include <vector>
#include <string>
#include <fstream>

#include "Renderer/GPUStrutures.h"


namespace Fluxon {

	namespace Utils {

		static std::vector<char> ReadFile(const std::string& file_name) {
			std::ifstream file(file_name, std::ios::ate | std::ios::binary);

			if (!file.is_open())
				throw std::runtime_error("Failed to open file!!!!!");

			//ate -> read from the back 
			//use read position to deteremine the sie of file 
			size_t file_size = static_cast<size_t>(file.tellg());
			std::vector<char> buffer(file_size);

			//now seek back to beginning of file to read bytes at once
			file.seekg(0);
			file.read(buffer.data(), file_size);

			file.close();
			return buffer;
		}


		static std::string VK_BufferUsageFlagString(VkBufferUsageFlagBits usage) {

			switch (usage)
			{
			case VK_BUFFER_USAGE_TRANSFER_SRC_BIT:
				return "VK_BUFFER_USAGE_TRANSFER_SRC_BIT";
				break;
			case VK_BUFFER_USAGE_TRANSFER_DST_BIT:
				return "VK_BUFFER_USAGE_TRANSFER_DST_BIT";
			case VK_BUFFER_USAGE_VERTEX_BUFFER_BIT:
				return "VK_BUFFER_USAGE_VERTEX_BUFFER_BIT";
			case VK_BUFFER_USAGE_INDEX_BUFFER_BIT:
				return "VK_BUFFER_USAGE_INDEX_BUFFER_BIT";
			}

			return "\033[1;31mUnknown Usage!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\033[0m";
		}


		static std::string VK_MemoryPropertyFlagString(VkMemoryPropertyFlagBits property) {

			switch (property)
			{
			case VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT:
				return "VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT ";
				break;
			case VK_MEMORY_PROPERTY_HOST_COHERENT_BIT:
				return "VK_MEMORY_PROPERTY_HOST_COHERENT_BIT";
			case VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT:
				return "VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT";
			}

			return "\033[1;31mUnknown property!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\033[0m";
		}

		static constexpr double PI = 3.141592653589793238462643;

		static void GetUnitQuadVertices(std::vector<GPUStructures::Vertex>& out_vertices, std::vector<uint16_t>& out_indices) {

			const std::vector<GPUStructures::Vertex> vertices = {
				{{-0.5f, -0.5f, 0.0f},	{0.0f, 0.0f, 1.0f},	{1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
				{{0.5f, -0.5f, 0.0f},	{0.0f, 0.0f, 1.0f},	{0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} },
				{{0.5f, 0.5f, 0.0f},	{0.0f, 0.0f, 1.0f},	{0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
				{{-0.5f, 0.5f, 0.0f},	{0.0f, 0.0f, 1.0f},	{1.0f, 1.0f}, {0.0f, 0.0f, 1.0f} }
			};

			const std::vector<uint16_t> indices = {
				0, 1, 2,
				2, 3, 0
			};

			out_vertices = vertices;
			out_indices = indices;
		}


		static void GetUnitCubeVertices(std::vector<GPUStructures::Vertex>& out_vertices, std::vector<uint16_t>& out_indices) {


			const std::vector<GPUStructures::Vertex> vertices = {

				//front face (+z)
				{ { -0.5, -0.5, 0.5},	{0.0, 0.0, 1.0},	{1.0f, 0.0f}, {1.0, 0.0, 0.0} },
				{ {  0.5, -0.5, 0.5},	{0.0, 0.0, 1.0},	{0.0f, 0.0f}, {0.0, 1.0, 0.0} },
				{ {  0.5,  0.5, 0.5 },	{0.0, 0.0, 1.0},	{0.0f, 1.0f}, {1.0, 1.0, 0.0} },
				{ { -0.5,  0.5, 0.5},	{0.0, 0.0, 1.0},	{1.0f, 1.0f}, {0.0, 0.0, 1.0} },

				// Back face (-z) 									 
			{ {-0.5, -0.5, -0.5},	{0.0, 0.0, 1.0},	{1.0f, 0.0f}, {1.0, 0.0, 0.0} },
			{ { 0.5 ,-0.5 ,-0.5},	{0.0, 0.0, 1.0},	{0.0f, 0.0f}, {0.0, 0.0, 1.0}  },
			{ { 0.5 , 0.5, -0.5},	{0.0, 0.0, 1.0},	{0.0f, 1.0f}, {1.0, 0.0, 1.0} },
			{ {-0.5,  0.5 ,-0.5},	{0.0, 0.0, 1.0},	{1.0f, 1.0f}, {1.0, 1.0, 1.0}  },

			// Right face (+x) 										 
			{ {0.5, -0.5, -0.5},	{1.0, 0.0, 0.0},	{1.0f, 0.0f}, {1.0, 0.0, 0.0} },
			{ { 0.5,  0.5, -0.5},	{1.0, 0.0, 0.0},	{0.0f, 0.0f}, {0.0, 1.0, 0.0}  },
			{ { 0.5,  0.5,  0.5	},	{1.0, 0.0, 0.0},	{0.0f, 1.0f}, {1.0, 1.0, 0.0} },
			{ { 0.5, -0.5,  0.5},	{1.0, 0.0, 0.0},	{1.0f, 1.0f}, {0.0, 0.0, 1.0}  },

			//Left face  (-x) 										 
			{ {-0.5, -0.5, -0.5},	{-1.0, 0.0, 0.0},	{1.0f, 0.0f}, {1.0, 0.0, 0.0} },
			{ {-0.5,  0.5 ,-0.5},	{-1.0, 0.0, 0.0},	{0.0f, 0.0f}, {0.0, 0.0, 1.0}  },
			{ {-0.5,  0.5 , 0.5 },	{-1.0, 0.0, 0.0},	{0.0f, 1.0f}, {1.0, 0.0, 1.0} },
			{ {-0.5, -0.5,  0.5},	{-1.0, 0.0, 0.0},	{1.0f, 1.0f}, {1.0, 1.0, 1.0}  },

			//Top face (+y) 										 
			{{-0.5, 0.5, -0.5},		{0.0, 1.0, 0.0},	{1.0f, 0.0f}, {1.0, 0.0, 0.0} },
			{{ 0.5, 0.5, -0.5},		{0.0, 1.0, 0.0},	{0.0f, 0.0f}, {0.0, 1.0, 0.0}  },
			{{ 0.5, 0.5,  0.5},		{0.0, 1.0, 0.0},	{0.0f, 1.0f}, {1.0, 1.0, 0.0} },
			{{-0.5, 0.5,  0.5},		{0.0, 1.0, 0.0},	{1.0f, 1.0f}, {0.0, 0.0, 1.0}  },

			//Bottom face (-y) 										 
			{ {-0.5, -0.5, -0.5},	{0.0, -1.0, 0.0},	{1.0f, 0.0f}, {1.0, 0.0, 0.0} },
			  {{0.5, -0.5, -0.5},	{0.0, -1.0, 0.0},	{0.0f, 0.0f}, {0.0, 0.0, 1.0}  },
			  {{0.5, -0.5,  0.5},	{0.0, -1.0, 0.0},	{0.0f, 1.0f}, {1.0, 0.0, 1.0} },
			{{-0.5, -0.5,  0.5},	{0.0, -1.0, 0.0},	{1.0f, 1.0f}, {1.0, 1.0, 1.0}  }
			};

			const std::vector<uint16_t> indices = {
				//Front face
				0, 1, 2,
				0, 2, 3,

				//Back face
				4, 6, 5,
				4, 7, 6,

				//Right face
				8, 9, 10,
				8, 10, 11,

				//Left face
				12, 14, 13,
				12, 15, 14,

				//Top face
				16, 18, 17,
				16, 19, 18,

				//Bottom face
				20, 21, 22,
				20, 22, 23
			};

			out_vertices = vertices;
			out_indices = indices;
		}

		static void GenerateSphereVertices(std::vector<GPUStructures::Vertex>& out_vertices, std::vector<uint16_t>& out_indices,
			unsigned int sector_count = 36, unsigned int span_count = 18) {

			//vextex position
			float x, y, z, w;
			//vectex texture coord
			float u, v;

			float sector_angle;
			float span_angle;

			//TO-DO: for now use float, double will be too redundant to use 
			float sector_step = (float)(2 * PI / sector_count);  // 0 - 360(2pi)/count =>  angle btw each steps
			float span_step = (float)(PI / span_count);		  // 0 - 180(pi)/count => angle btw each step

			//float radius = 1.0f;
			float radius = 0.5f;

			//compute & store vertices
			for (unsigned int i = 0; i <= span_count; i++)
			{
				// 180 degree(pi) to 0 degree //0 degree to 180 degree(pi)
				span_angle = (float)PI - i * span_step;

				//parametric equation for sphere
				// x = center_x + r * sin(theta) * cos(phi)    
				// y = center_y + r * sin(thata) * sin(phi)
				// z = center_z + r * cos(theta)
				// where 
				//		theta = span_angle range 0 to pi(180 degrees)
				//		phi = sector_angle range 0 to 2pi(360 degrees)
				//RETERIVED: https://en.m.wikipedia.org/wiki/Sphere
				//			In their case z is up axis


				w = radius * glm::sin(span_angle);
				y = radius * glm::cos(span_angle);

				//add (sector_count + 1) vertices per stack
				//first and last vertices have same position, but different tex coords 
				for (unsigned int j = 0; j <= sector_count; ++j)
				{
					//start from 0 >> current step * step angle >> 360 
					sector_angle = j * sector_step;

					//vectex position (x, y, z)
					x = w * glm::cos(sector_angle);			//r * cos(u) * cos(v)
					z = w * glm::sin(sector_angle);			//r * cos(u) * sin(v)

					//vertex texture coord range between [0, 1]
					u = (float)j / sector_count;
					v = (float)i / span_count;

					GPUStructures::Vertex vertex
					{
						{x,y, z},    //layout 0 => pos
						{x, y, z},				  //layout 1 => nor
						{u, v},					   //layout 2 => uv
						{1.0f, 0.0f, 1.0f},	//layout 3 => col  //magenta for debugging
						//{0.0f, 0.0f, 0.0f},       //layout 4 => tan
						//{0.0f, 0.0f, 0.0f},       //layout 5 => bi tan
					};
					out_vertices.push_back(vertex);
				}
			}


			//compute & store indices
			unsigned int k1, k2;
			for (unsigned int i = 0; i < span_count; ++i)
			{
				k1 = i * (sector_count + 1);		//beginning of current stack
				k2 = k1 + sector_count + 1;			//beginning of next stack

				for (unsigned int j = 0; j < sector_count; ++j, ++k1, ++k2)
				{
					//2 triangles per sector excluding first and last stacks
					//k1 => k2 => k1+1
					if (i != 0)
					{
						out_indices.push_back(k1);
						out_indices.push_back(k2);
						out_indices.push_back(k1 + 1);
					}

					//k1+1 => k2 => k2+ 1
					if (i != (span_count - 1))
					{
						out_indices.push_back(k1 + 1);
						out_indices.push_back(k2);
						out_indices.push_back(k2 + 1);
					}

				}
			}
		}


		static inline void DecomposeTransform(const glm::mat4& transform, glm::vec3& translate, glm::vec3& euler, glm::vec3& scale)
		{
			glm::mat4 Mt = transform;
			translate = glm::vec3(Mt[3]);

			if (std::abs(Mt[3][3] - 1.0f) > 1e-6f)
				return;

			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					if (std::isnan(Mt[i][j]))
						return;

			//Remove translation for scale & rot extraction 
			Mt[3] = glm::vec4(0.0f, 0.0f, 0.0f, Mt[3].w);

			scale.x = glm::length(glm::vec3(Mt[0][0], Mt[1][0], Mt[2][0]));
			scale.y = glm::length(glm::vec3(Mt[0][1], Mt[1][1], Mt[2][1]));
			scale.z = glm::length(glm::vec3(Mt[0][2], Mt[1][2], Mt[2][2]));

			if (glm::determinant(Mt) < 0)
				scale.x = -scale.x;

			glm::mat3 Mr;
			Mr[0] = glm::vec3(Mt[0][0], Mt[1][0], Mt[2][0]) / scale.x;
			Mr[1] = glm::vec3(Mt[0][1], Mt[1][1], Mt[2][1]) / scale.y;
			Mr[2] = glm::vec3(Mt[0][2], Mt[1][2], Mt[2][2]) / scale.z;

			euler.x = atan2(Mr[2][1], Mr[2][2]);
			euler.y = atan2(-Mr[2][0], glm::sqrt(Mr[2][1] * Mr[2][1] + Mr[2][2] * Mr[2][2]));
			euler.z = atan2(Mr[1][0], Mr[0][0]);

			euler = glm::degrees(euler);
		}

	} //#Utils
}//#Fluxon