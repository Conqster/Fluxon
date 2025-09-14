#pragma once
#include <vulkan/vulkan.h>

#include "Core/AssertLog.h"


namespace Fluxon {
	namespace Vulkan {
		struct VulkanContext;
	}
}



#define FLUX_ASSERT_GPUBUFFER_MAPPED(gpu_buffer) \
	FLUX_DEBUG_ASSERT(gpu_buffer.IsMapped(), "GPU buffer not mapped to write!!!!!!")

namespace Fluxon {
	namespace Vulkan {

		struct GPUBuffer
		{
			VkBuffer buffer = VK_NULL_HANDLE;
			VkDeviceMemory memory = VK_NULL_HANDLE;

			//util
			VkDeviceSize size = 0;
			void* mapped = nullptr;

			bool IsValid() const {
				return buffer != VK_NULL_HANDLE && memory != VK_NULL_HANDLE;
			}

			bool IsMapped() const {
				return mapped != nullptr;
			}

			static void CreateBuffer(const VulkanContext* vk_ctx, uint32 size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, GPUBuffer& gpu_buffer);
			static bool MapBufferMemory(VkDevice device, GPUBuffer& gpu_buffer);
			/// <summary>
			/// Map for future write
			/// </summary>
			/// <param name="device"></param>
			/// <param name="gpu_buffer"></param>
			/// <param name="ppdata_map"></param>
			/// <returns></returns>
			static bool MapGPUBufferMemory(VkDevice device, const GPUBuffer& gpu_buffer, void** ppdata_map);
			
			static void CopyToMappedMemory(GPUBuffer& gpu_buffer, const void* pdata, uint32_t size);

			template<typename T>
			void DirectWriteToMappedMemory(const T& data);
			/// <summary>
			/// Expensive "Creates a vkMap & UnMap after write"
			/// If gpu_buffer has already been mapped, 
			/// use @static void CopyToMappedMemory(GPUBuffer& gpu_buffer, const void* pdata, uint32_t size);
			/// 
			/// </summary>
			/// <param name="device"></param>
			/// <param name="gpu_buffer"></param>
			/// <param name="pdata"></param>
			static void FillBufferWithData(VkDevice device, const GPUBuffer& gpu_buffer, void* pdata);
		
			static void TransferGPUBufferData(const VulkanContext* vk_ctx, VkCommandPool cmd_pool, const GPUBuffer& src_buff, GPUBuffer& dst_buff);

			static void DestoryBuffer(VkDevice device, const GPUBuffer& gpu_buffer);
		};


	} //#vulkan
}//#Fluxon