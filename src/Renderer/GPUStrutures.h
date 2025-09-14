#pragma once

#include <glm/glm.hpp>
#include <array>

#include <vulkan/vulkan.h>

#include <vector>

#include "Core/Colours.h"

namespace Fluxon {
	namespace GPUStructures {
		
		constexpr size_t VERTEX_ATTRIB_COUNT = 4;
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

			static std::array<VkVertexInputAttributeDescription, VERTEX_ATTRIB_COUNT> GetAttributeDescriptions() {
				std::array<VkVertexInputAttributeDescription, VERTEX_ATTRIB_COUNT> attribute_descriptions{};

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

			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptionsAsVector() {
				std::vector<VkVertexInputAttributeDescription> attribute_descriptions(VERTEX_ATTRIB_COUNT);

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


		constexpr size_t PARTICLE_ATTRIB_COUNT = 2;
		struct alignas(16) Particle
		{
			glm::vec3 position = glm::vec3(0.0f);
			float lifeTime = 1.0f;
			glm::vec3 velocity = glm::vec3(0.0f);
			float age = 0.0f;
			glm::vec3 acceleration;
			float mass = 1.0f;
			glm::vec4 colour = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);


			static VkVertexInputBindingDescription GetBindingDescription() {
				VkVertexInputBindingDescription binding_description{};

				binding_description.binding = 0;
				binding_description.stride = sizeof(Particle);
				binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
				return binding_description;
			}

			static std::array<VkVertexInputAttributeDescription, PARTICLE_ATTRIB_COUNT> GetAttributeDescriptions() {
				std::array<VkVertexInputAttributeDescription, PARTICLE_ATTRIB_COUNT> attribute_descriptions{};

				attribute_descriptions[0].binding = 0;
				attribute_descriptions[0].location = 0;
				attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
				attribute_descriptions[0].offset = offsetof(Particle, position);

				attribute_descriptions[1].binding = 0;
				attribute_descriptions[1].location = 1;
				attribute_descriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
				attribute_descriptions[1].offset = offsetof(Particle, colour);

				return attribute_descriptions;
			}

			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptionsAsVector() {
				std::vector<VkVertexInputAttributeDescription> attribute_descriptions(PARTICLE_ATTRIB_COUNT);

				attribute_descriptions[0].binding = 0;
				attribute_descriptions[0].location = 0;
				attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
				attribute_descriptions[0].offset = offsetof(Particle, position);

				attribute_descriptions[1].binding = 0;
				attribute_descriptions[1].location = 1;
				attribute_descriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
				attribute_descriptions[1].offset = offsetof(Particle, colour);

				return attribute_descriptions;
			}
		};


		constexpr size_t LINE_VERTEX_ATTRIB_COUNT = 2;
		struct alignas(16) DebugLineVertex
		{
			glm::vec3 position = glm::vec4(0.0f);
			uint32_t colour = Colour32(1.0f, 0.0f, 1.0f, 1.0f);

			static VkVertexInputBindingDescription GetBindingDescription() {
				VkVertexInputBindingDescription binding_description{};

				binding_description.binding = 0;
				binding_description.stride = sizeof(DebugLineVertex);
				binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
				return binding_description;
			}

			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptionsAsVector() {
				std::vector<VkVertexInputAttributeDescription> attribute_descriptions(LINE_VERTEX_ATTRIB_COUNT);

				attribute_descriptions[0].binding = 0;
				attribute_descriptions[0].location = 0;
				attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
				attribute_descriptions[0].offset = offsetof(DebugLineVertex, position);

				attribute_descriptions[1].binding = 0;
				attribute_descriptions[1].location = 1;
				attribute_descriptions[1].format = VK_FORMAT_R8G8B8A8_UNORM;
				attribute_descriptions[1].offset = offsetof(DebugLineVertex, colour);

				return attribute_descriptions;
			}
		};
		//changing from just frame UBO to EmitterUBO 
		struct alignas(16) EmitterUBO
		{
			glm::mat4 worldTransform = glm::mat4(1.0f);
			//glm::vec3 emitLocation = glm::vec3(0.0f);
			glm::vec3 boundsMin = glm::vec3(-2.5f, -1.0f, -2.5f);
			float deltaTime = 1.0f;
			glm::vec3 boundsMax = glm::vec3(2.5f, 5.0f, 2.5f);
			float boundsCollisionImpluse = 1.45997f;

			glm::vec3 particleStartColour = glm::vec3(0.0f, 0.0f,1.0f);
			int spawnCount = 0;
			glm::vec3 particleEndColour = glm::vec3(1.0f, 0.0f, 0.0f);
			float particleLifeTime = 1.0f;

			float startSpeed = 5.0f;
			float particleMass = 1.0f;
			float _pad0;
			float _pad1;
		};


		struct alignas(16) MVP_UniformBufferObject {
			glm::mat4 view;
			glm::mat4 projection;
			glm::mat4 lightView;
			glm::mat4 lightProj;
			glm::vec3 position;
			int _padding;
		};

		struct alignas(16) CameraUBO {
			glm::mat4 view;
			glm::mat4 projection;
		};

		struct alignas(16) VoxelParameterUBO {
			//size of one voxel in world units
			float voxelSize = 0.1f;
			//number of Z slices(layer) in 2D texture array
			int sliceCount = 128;
			float minZ = -6.4f;

			int _padding0 = 0;
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

		struct SkyboxProperties {
			float skyboxInfluencity = 0.2f;
			float objectReflectivity = 0.2f; //<-- hack remove later
			int enable = 0;
			int _padding = 0;
		};




	} //# GPUStructures
}//#Fluxon