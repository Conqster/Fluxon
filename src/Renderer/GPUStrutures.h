#pragma once

#include <glm/glm.hpp>
#include <array>

#include <vulkan/vulkan.h>


namespace Fluxon {
	namespace GPUStructures {

		struct Vertex
		{
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec2 texCoord;
			glm::vec3 colour;


			static VkVertexInputBindingDescription GetBindingDescription() {
				VkVertexInputBindingDescription binding_description{};

				binding_description.binding = 0;
				binding_description.stride = sizeof(Vertex);
				binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
				return binding_description;
			}

			static std::array<VkVertexInputAttributeDescription, 4> GetAttributeDescriptions() {
				std::array<VkVertexInputAttributeDescription, 4> attribute_descriptions{};

				attribute_descriptions[0].binding = 0;
				attribute_descriptions[0].location = 0;
				attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
				attribute_descriptions[0].offset = offsetof(Vertex, position);

				attribute_descriptions[1].binding = 0;
				attribute_descriptions[1].location = 1;
				attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
				attribute_descriptions[1].offset = offsetof(Vertex, normal);

				attribute_descriptions[2].binding = 0;
				attribute_descriptions[2].location = 2;
				attribute_descriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
				attribute_descriptions[2].offset = offsetof(Vertex, texCoord);

				attribute_descriptions[3].binding = 0;
				attribute_descriptions[3].location = 3;
				attribute_descriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
				attribute_descriptions[3].offset = offsetof(Vertex, colour);
				return attribute_descriptions;
			}
		};


		struct alignas(16) MVP_UniformBufferObject {
			glm::mat4 view;
			glm::mat4 projection;
			glm::vec3 position;
			int _padding;
		};

		struct Model {
			alignas(16) glm::mat4 transform = glm::mat4(1.0f);
			alignas(4) glm::vec3 colour = glm::vec3(0.3f);
		};

		struct DirectionalLight {
			glm::vec3 direction = glm::vec3(0.0f, 0.9f, 0.3f);
			int enable = 1;
			glm::vec3 colour = glm::vec3(1.0f);
			int shinness = 64;
		};

		struct Skybox {
			float skyboxInfluencity = 0.2f;
			float objectReflectivity = 0.2f; //<-- hack remove later
			int enable = 0;
			int _padding = 0;
		};


	} //# GPUStructures
}//#Fluxon