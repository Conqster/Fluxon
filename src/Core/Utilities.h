#pragma once

#include <vector>
#include <string>
#include <fstream>

#include "Renderer/GPUStrutures.h"
#include <glm/gtx/quaternion.hpp>


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


		static inline bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translate, glm::quat& quat, glm::vec3& scale)
		{
			glm::mat4 local_matrix = transform;
			translate = glm::vec3(local_matrix[3]);

			if (std::abs(local_matrix[3][3] - 1.0f) > 1e-6f)
				return false;

			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					if (std::isnan(local_matrix[i][j]) || std::isinf(local_matrix[i][j]))
						return false;

			//Remove translation for scale & rot extraction 
			local_matrix[3] = glm::vec4(0.0f, 0.0f, 0.0f, local_matrix[3].w);

			//Extract scale
			glm::vec3 column0(local_matrix[0]);
			glm::vec3 column1(local_matrix[1]);
			glm::vec3 column2(local_matrix[2]);

			scale.x = glm::length(column0);
			scale.y = glm::length(column1);
			scale.z = glm::length(column2);

			if (glm::determinant(local_matrix) < 0)
				scale.x = -scale.x;

			//Remove scale from matrix to isolate rotation

			if (scale.x != 0.0f)
				column0 /= scale.x;
			if (scale.y != 0.0f)
				column1 /= scale.y;
			if (scale.z != 0.0f)
				column2 /= scale.z;

			glm::mat3 rotation_matrix(column0, column1, column2);

			quat = glm::quat_cast(rotation_matrix);

			return true;
		}



		static inline bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translate, glm::vec3& euler_degree, glm::vec3& scale)
		{
			glm::quat rotation_quat;

			if (!DecomposeTransform(transform, translate, rotation_quat, scale))
				return false;

			glm::vec3 euler_radians = glm::eulerAngles(rotation_quat);
			euler_degree = glm::degrees(euler_radians);

			return true;
		}


		/// <summary>
		/// 
		/// // Use case
		/// glm::vec3 position = glm::vec3(0.0f);
		/// glm::vec3 euler_degrees = glm::vec(30.0f, 20.f, 30.0f);
		/// glm::vec3 scale = glm::vec3(1.0f);
		/// 
		/// //Convert to quaternion
		/// glmLLquat rotation = glm::quat(glm::radian(euler_degrees));
		/// 
		/// glm::mat4 transform = ComposeTranform(posiition, euler_degrees, scale);
		/// 
		/// </summary>
		/// <param name="position"></param>
		/// <param name="quat"></param>
		/// <param name="scale"></param>
		/// <returns></returns>
		static inline glm::mat4 ComposeTransform(const glm::vec3& position, const glm::quat quat, const glm::vec3& scale)
		{
			glm::mat4 translation_matrix	= glm::translate(glm::mat4(1.0f), position); 
			//glm::mat4 rotation_matrix = glm::toMat4(quat);
			glm::mat4 rotation_matrix		= glm::mat4_cast(quat);
			glm::mat4 scale_matrix			= glm::scale(glm::mat4(1.0f), scale);

			return translation_matrix * rotation_matrix * scale_matrix;
			//return  rotation_matrix * scale_matrix * translation_matrix;
		}



		static std::array<glm::vec3, 24> GenerateAABBEdges(const glm::vec3& min, const glm::vec3& max) {

			 const std::array<glm::vec3, 8> corners = {
				glm::vec3(min.x, min.y, min.z),
				{max.x, min.y, min.z},
				{max.x, max.y, min.z},
				{min.x, max.y, min.z},
				{min.x, min.y, max.z},
				{max.x, min.y, max.z},
				{max.x, max.y, max.z},
				{min.x, max.y, max.z}
			};

			const std::array<int, 24> edgeIndices = {
				0, 1, 1, 2, 2, 3, 3, 0,
				4, 5, 5, 6, 6, 7, 7, 4,
				0, 4, 1, 5, 2, 6, 3, 7
			};

			std::array<glm::vec3, 24> edges;
			for (int i = 0; i < 24; ++i)
				edges[i] = corners[edgeIndices[i]];

			return edges;
		}

		static std::array<glm::vec3, 24> GenerateAABBEdges(const glm::mat4& world_transform, const glm::vec3& min, const glm::vec3& max) {

			const std::array<glm::vec3, 8> corners = {
			   glm::vec3(min.x, min.y, min.z),
			   {max.x, min.y, min.z},
			   {max.x, max.y, min.z},
			   {min.x, max.y, min.z},
			   {min.x, min.y, max.z},
			   {max.x, min.y, max.z},
			   {max.x, max.y, max.z},
			   {min.x, max.y, max.z}
			};

			std::array<glm::vec3, 8> transformed_corners;
			for (size_t i = 0; i < 8; ++i)
				transformed_corners[i] = glm::vec3(world_transform * glm::vec4(corners[i], 1.0f));

			const std::array<int, 24> edgeIndices = {
				0, 1, 1, 2, 2, 3, 3, 0,
				4, 5, 5, 6, 6, 7, 7, 4,
				0, 4, 1, 5, 2, 6, 3, 7
			};

			std::array<glm::vec3, 24> edges;
			for (int i = 0; i < 24; ++i)
				edges[i] = transformed_corners[edgeIndices[i]];

			return edges;
		}
	} //#Utils
}//#Fluxon