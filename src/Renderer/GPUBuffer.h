#pragma once
#include <vulkan/vulkan.h>


namespace Fluxon {
	namespace Vulkan {
		struct VulkanContext;
	}
}


namespace Fluxon {
	namespace Vulkan {

		struct GPUBuffer
		{
			VkBuffer buffer = VK_NULL_HANDLE;
			VkDeviceMemory memory = VK_NULL_HANDLE;

			//util
			VkDeviceSize size = 0;
			void* mapped = nullptr;
		};

		void CreateBuffer(const VulkanContext* vk_ctx, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, GPUBuffer& gpu_buffer);
		bool MapBufferMemory(VkDevice device, GPUBuffer& gpu_buffer);
		bool MapGPUBufferMemory(VkDevice device, const GPUBuffer& gpu_buffer, void** ppdata_map);

		void CopyToMappedMemory(GPUBuffer& gpu_buffer, const void* pdata, uint32_t size);
		void FillBufferWithData(VkDevice device, const GPUBuffer& gpu_buffer, void* pdata);

		void DestoryBuffer(VkDevice device, const GPUBuffer& gpu_buffer);



	} //#vulkan
}//#Fluxon