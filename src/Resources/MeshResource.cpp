#include "MeshResource.h"

#include "ModelLoader.h"

namespace Fluxon {


	MeshResource& MeshResource::CreateNewCube(const Vulkan::VulkanContext* vk_ctx, VkCommandPool cmd_pool, MeshResource& mesh_reso)
	{
		std::vector<GPUStructures::Vertex> vertices_data;
		std::vector<uint16_t> indices_data;

		//mesh_reso.descriptor.id ==== handle extrenally 
		mesh_reso.descriptor.primitiveType = EPrimitiveMeshType::Cube;

		Utils::GetUnitCubeVertices(vertices_data, indices_data);
		mesh_reso.gpuMesh.Generate(vk_ctx, cmd_pool, vertices_data, indices_data);

		return mesh_reso;
	}

	MeshResource& MeshResource::CreateNewSphere(const Vulkan::VulkanContext* vk_ctx, VkCommandPool cmd_pool, MeshResource& mesh_reso)
	{
		std::vector<GPUStructures::Vertex> vertices_data;
		std::vector<uint16_t> indices_data;

		//mesh_reso.descriptor.id ==== handle extrenally 
		mesh_reso.descriptor.primitiveType = EPrimitiveMeshType::Sphere;

		Utils::GenerateSphereVertices(vertices_data, indices_data);
		mesh_reso.gpuMesh.Generate(vk_ctx, cmd_pool, vertices_data, indices_data);

		return mesh_reso;
	}

	MeshResource& MeshResource::CreateNewPlane(const Vulkan::VulkanContext* vk_ctx, VkCommandPool cmd_pool, MeshResource& mesh_reso)
	{
		std::vector<GPUStructures::Vertex> vertices_data;
		std::vector<uint16_t> indices_data;

		//mesh_reso.descriptor.id ==== handle extrenally 
		mesh_reso.descriptor.primitiveType = EPrimitiveMeshType::Plane;

		Utils::GetUnitQuadVertices(vertices_data, indices_data);
		mesh_reso.gpuMesh.Generate(vk_ctx, cmd_pool, vertices_data, indices_data);

		return mesh_reso;
	}

	MeshResource& MeshResource::CreateNewModel(const Vulkan::VulkanContext* vk_ctx, VkCommandPool cmd_pool, MeshResource& mesh_reso, const std::string& file_path)
	{
		// TODO: insert return statement here
		mesh_reso.descriptor.primitiveType = EPrimitiveMeshType::ModelMesh;
		mesh_reso.descriptor.modelPath = file_path;
		ModelLoader quick_loader;
		auto [model_vertices, model_indices] = quick_loader.LoadAsVerticesData(file_path, true, false);
		mesh_reso.gpuMesh.Generate(vk_ctx, cmd_pool, model_vertices, model_indices);


		return mesh_reso;

	}

	void MeshResource::DestroyMeshResource(VkDevice device, MeshResource& mesh_resource)
	{
		mesh_resource.gpuMesh.Cleanup(device);
	}

} //namespace Fluxon


