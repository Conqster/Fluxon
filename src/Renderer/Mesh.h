#pragma once


#include "GPUBuffer.h"
#include "GPUStrutures.h"

#include <vector>

namespace Fluxon {
	namespace Vulkan {


		class Mesh
		{
		private:
			GPUBuffer mVertexBuffer;
			GPUBuffer mIndexBuffer;

			uint32_t mIndicesCount = 0;
		public:
			Mesh() {};
			Mesh(const VulkanContext* vk_ctx, VkCommandPool cmd_pool, std::vector<GPUStructures::Vertex>& vertices, std::vector<uint16_t>& indices) {
				Generate(vk_ctx, cmd_pool, vertices, indices);
			}
			~Mesh() {};

			void Generate(const VulkanContext* vk_ctx, VkCommandPool cmd_pool, std::vector<GPUStructures::Vertex> vertices, std::vector<uint16_t> indices);
			////TODO : Quick hack 
			//void Generate(const Vulkan2::VulkanContext* vk_render_ctx, VkCommandPool cmd_pool, std::vector<GPUStructures::Vertex> vertices, std::vector<uint16_t> indices) {
			//	Device temp_sys_device;
			//	temp_sys_device.instance = vk_render_ctx->instance;
			//	temp_sys_device.physicalDevice = vk_render_ctx->physicalDevice;
			//	temp_sys_device.device = vk_render_ctx->device;
			//	temp_sys_device.surface = vk_render_ctx->surface;
			//	temp_sys_device.graphicsQueue = vk_render_ctx->graphicsQueue;
			//	temp_sys_device.presentQueue = vk_render_ctx->presentQueue;
			//	temp_sys_device.queueFamilyIndices = vk_render_ctx->queueFamilyIndices;
			//	temp_sys_device.debugMessager = vk_render_ctx->debugMessager;

			//	CreateVertexBuffer(&temp_sys_device, cmd_pool, vertices);
			//	CreateIndexBuffer(&temp_sys_device, cmd_pool, indices);
			//}
			void Cleanup(VkDevice device);

			void Bind(VkCommandBuffer cmd_buffer);
			void Draw(VkCommandBuffer cmd_buffer);

			GPUBuffer const GetVertexBuffer() const { return mVertexBuffer; }
			GPUBuffer const GetIndexBuffer() const { return mIndexBuffer; }

		private:
			void CreateVertexBuffer(const VulkanContext* vk_ctx, VkCommandPool cmd_pool, std::vector<GPUStructures::Vertex>& vertices);
			void CreateIndexBuffer(const VulkanContext* vk_ctx, VkCommandPool cmd_pool, std::vector<uint16_t>& indices);
		};

	} //#Vulkan
}//# Fluxon