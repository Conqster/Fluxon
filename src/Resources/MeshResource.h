#pragma once
#include "Renderer/Mesh.h"

#include <string>
#include "Core/Utilities.h"

#include "Core/Enums.h"

namespace Fluxon {




	struct MeshDescriptor
	{
		int32_t id = -1;
		EMeshType type = EMeshType::Static;
		EPrimitiveMeshType primitiveType = EPrimitiveMeshType::None;

		std::string modelPath;
	};

	struct MeshResource
	{
		MeshDescriptor descriptor = {};
		Vulkan::Mesh gpuMesh = {};


		static MeshResource& CreateNewCube(const Vulkan::VulkanContext* vk_ctx, VkCommandPool cmd_pool, MeshResource& mesh_reso);
		static MeshResource& CreateNewSphere(const Vulkan::VulkanContext* vk_ctx, VkCommandPool cmd_pool, MeshResource& mesh_reso);
		static MeshResource& CreateNewPlane(const Vulkan::VulkanContext* vk_ctx, VkCommandPool cmd_pool, MeshResource& mesh_reso);
		static MeshResource& CreateNewModel(const Vulkan::VulkanContext* vk_ctx, VkCommandPool cmd_pool, MeshResource& mesh_reso, const std::string& file_path);

		static void DestroyMeshResource(VkDevice device, MeshResource& mesh_resource);
	};


} //namespace Fluxon