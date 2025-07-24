#include "GPUBuffer.h"

#include <iostream>
#include "VulkanUtilities.h"
#include "VulkanContext.h"


namespace Fluxon {
	namespace Vulkan {

		void CreateBuffer(const VulkanContext* vk_ctx, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, GPUBuffer& gpu_buffer)
		{
			if (gpu_buffer.size == 0)
				gpu_buffer.size = size;

			VkBufferCreateInfo buffer_info{};
			buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			buffer_info.size = size;
			buffer_info.usage = usage;
			buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			std::cout << "\n\n\033[1;32m ======================BUFFER CREATION==========================\033[0m\n";
			std::cout << "\trequested size: " << size << "\n";
			std::cout << "\tusage: " << Utils::VK_BufferUsageFlagString(static_cast<VkBufferUsageFlagBits>(usage)) << "\n";
			std::cout << "\tproperties: " << Utils::VK_MemoryPropertyFlagString(static_cast<VkMemoryPropertyFlagBits>(properties)) << "\n";

			if (vkCreateBuffer(vk_ctx->device, &buffer_info, nullptr, &gpu_buffer.buffer) != VK_SUCCESS)
				throw std::runtime_error("Failed to create vertex buffer!!!!!!");


			VkMemoryRequirements mem_requirements;
			vkGetBufferMemoryRequirements(vk_ctx->device, gpu_buffer.buffer, &mem_requirements);

			std::cout << "\t\033[1;33mMemory Requirement for buffer\033[0m\n";
			std::cout << "\t\t Size: " << mem_requirements.size << "\n";
			std::cout << "\t\t Alignment: " << mem_requirements.alignment << "\n";
			std::cout << "\t\t memory type bits: " << mem_requirements.memoryTypeBits << "\n";

			VkMemoryAllocateInfo alloc_info{};
			alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			alloc_info.allocationSize = mem_requirements.size;
			alloc_info.memoryTypeIndex = Utils::FindMemoryType(vk_ctx->physicalDevice, mem_requirements.memoryTypeBits, properties);

			if (vkAllocateMemory(vk_ctx->device, &alloc_info, nullptr, &gpu_buffer.memory) != VK_SUCCESS)
				throw std::runtime_error("Failed to allocate vertex buffer memeory!!!!!!");

			vkBindBufferMemory(vk_ctx->device, gpu_buffer.buffer, gpu_buffer.memory, 0);
		}

		bool MapBufferMemory(VkDevice device, GPUBuffer& gpu_buffer)
		{
			if (gpu_buffer.size == 0 || gpu_buffer.buffer == VK_NULL_HANDLE || gpu_buffer.memory == VK_NULL_HANDLE)
				throw std::runtime_error("[GPU BUFFER]{MAPMEMEMORY}: buffer might be empty or was not allocated!!!!");

			vkMapMemory(device, gpu_buffer.memory, 0, gpu_buffer.size, 0, &gpu_buffer.mapped);

			return true;
		}

		bool MapGPUBufferMemory(VkDevice device, const GPUBuffer& gpu_buffer, void** ppdata_map)
		{
			if (gpu_buffer.size == 0 || gpu_buffer.buffer == VK_NULL_HANDLE || gpu_buffer.memory == VK_NULL_HANDLE)
				throw std::runtime_error("[GPU BUFFER]{MAPMEMEMORY}: buffer might be empty or was not allocated!!!!");

			vkMapMemory(device, gpu_buffer.memory, 0, gpu_buffer.size, 0, ppdata_map);

			return true;
		}

		void CopyToMappedMemory(GPUBuffer& gpu_buffer, const void* pdata, uint32_t size)
		{
			if (gpu_buffer.mapped == nullptr)
				throw std::runtime_error("[GPU BUFFER]{MAPPED MEMEORY}: GPU buffer is not mapped, trying to write into unmapped memory!!!!");

			memcpy(gpu_buffer.mapped, pdata, size);
		}

		void FillBufferWithData(VkDevice device, const GPUBuffer& gpu_buffer, void* pdata)
		{
			if (gpu_buffer.size == 0 || gpu_buffer.buffer == VK_NULL_HANDLE || gpu_buffer.memory == VK_NULL_HANDLE)
				throw std::runtime_error("[GPU BUFFER]{MAPMEMEMORY}: buffer might be empty or was not allocated!!!!");

			void* map_data = nullptr;
			vkMapMemory(device, gpu_buffer.memory, 0, gpu_buffer.size, 0, &map_data);
			memcpy(map_data, pdata, static_cast<size_t>(gpu_buffer.size));
			vkUnmapMemory(device, gpu_buffer.memory);
		}

		void DestoryBuffer(VkDevice device, const GPUBuffer& gpu_buffer)
		{
			vkDestroyBuffer(device, gpu_buffer.buffer, nullptr);
			vkFreeMemory(device, gpu_buffer.memory, nullptr);
		}

	} //#vulkan
} //#Fluxon