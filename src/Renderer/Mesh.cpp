#include "Mesh.h"
#include "Resources.h"

#include "VulkanContext.h"

namespace Fluxon {
	namespace Vulkan {

		void Mesh::Generate(const VulkanContext* vk_ctx, VkCommandPool cmd_pool, std::vector<GPUStructures::Vertex> vertices, std::vector<uint16_t> indices)
		{
			if (vertices.empty() || indices.empty())
			{
				printf("Cant Genereate Mesh either, vertices or indice is empty!!!!\n");
				return;
			}
			CreateVertexBuffer(vk_ctx, cmd_pool, vertices);
			CreateIndexBuffer(vk_ctx, cmd_pool, indices);
		}
		void Mesh::Cleanup(VkDevice device)
		{
			DestoryBuffer(device, mVertexBuffer);
			DestoryBuffer(device, mIndexBuffer);
		}
		void Mesh::Bind(VkCommandBuffer cmd_buffer)
		{
			//VkBuffer vertex_buffers[] = { mVertexBuffer.buffer };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(cmd_buffer, 0, 1, &mVertexBuffer.buffer, offsets);
			//VkBuffer vertex_buffers[] = { mVertexBuffer, mIndexBuffer };
			//VkBuffer index_buffers[] = { mIndexBuffer.buffer };
			//VkDeviceSize index_offsets[] = { 0 }; //later size_of vertex size
			vkCmdBindIndexBuffer(cmd_buffer, mIndexBuffer.buffer, 0, VK_INDEX_TYPE_UINT16);
		}
		void Mesh::Draw(VkCommandBuffer cmd_buffer)
		{
			Bind(cmd_buffer);
			vkCmdDrawIndexed(cmd_buffer, mIndicesCount, 1, 0, 0, 0);
		}
		void Mesh::CreateVertexBuffer(const VulkanContext* vk_ctx, VkCommandPool cmd_pool, std::vector<GPUStructures::Vertex>& vertices)
		{
			VkDeviceSize buffer_size = sizeof(vertices[0]) * vertices.size();

			GPUBuffer staging_buffer;

			CreateBuffer(vk_ctx, buffer_size,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				staging_buffer);

			FillBufferWithData(vk_ctx->device, staging_buffer, vertices.data());

			CreateBuffer(vk_ctx, buffer_size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				//VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				mVertexBuffer);



			Resources::CopyBuffer(vk_ctx, cmd_pool, staging_buffer.buffer, mVertexBuffer.buffer, buffer_size);

			DestoryBuffer(vk_ctx->device, staging_buffer);
		}
		void Mesh::CreateIndexBuffer(const VulkanContext* vk_ctx, VkCommandPool cmd_pool, std::vector<uint16_t>& indices)
		{
			VkDeviceSize buffer_size = sizeof(indices[0]) * indices.size();
			mIndicesCount = indices.size();

			GPUBuffer staging_buffer;

			CreateBuffer(vk_ctx, buffer_size,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				staging_buffer);

			FillBufferWithData(vk_ctx->device, staging_buffer, indices.data());

			CreateBuffer(vk_ctx, buffer_size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
				//VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				mIndexBuffer);

			Resources::CopyBuffer(vk_ctx, cmd_pool, staging_buffer.buffer, mIndexBuffer.buffer, buffer_size);

			DestoryBuffer(vk_ctx->device, staging_buffer);
		}
	} //#vulkan
}//# Fluxon
