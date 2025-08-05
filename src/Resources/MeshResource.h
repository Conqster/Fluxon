#pragma once
#include "Renderer/Mesh.h"

#include <string>
#include "Core/Utilities.h"

namespace Fluxon {

	enum class EMeshType : uint32_t
	{
		EMT_Static
	};

	enum class EPrimitiveMeshType : uint32_t
	{
		EPMT_None,
		EPMT_ModelMesh,			//Loaded from external file
		EPMT_Cube,
		EPMT_Sphere,
		EPMT_Plane
	};


	struct MeshDescriptor
	{
		int32_t id = -1;
		EMeshType type = EMeshType::EMT_Static;
		EPrimitiveMeshType primitiveType = EPrimitiveMeshType::EPMT_None;

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
	};


} //namespace Fluxon