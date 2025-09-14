#include "GPUBuffer.h"

#include <iostream>
#include "VulkanUtilities.h"
#include "VulkanContext.h"

#include "Resources.h"


namespace Fluxon {
	namespace Vulkan {

		void GPUBuffer::CreateBuffer(const VulkanContext* vk_ctx, uint32 size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, GPUBuffer& gpu_buffer)
		{
			if (gpu_buffer.size == 0)
				gpu_buffer.size = static_cast<VkDeviceSize>(size);

			VkBufferCreateInfo buffer_info{};
			buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			buffer_info.size = static_cast<VkDeviceSize>(size);
			buffer_info.usage = usage;
			buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			FLUX_DEBUG_LOG_INFO("\n\n\033[1;32m ======================BUFFER CREATION==========================\033[0m");
			FLUX_DEBUG_LOG_INFO("\trequested size: ", size);
			FLUX_DEBUG_LOG_INFO("\tusage: ", Utils::VK_BufferUsageFlagString(static_cast<VkBufferUsageFlagBits>(usage)));
			FLUX_DEBUG_LOG_INFO("\tproperties: ", Utils::VK_MemoryPropertyFlagString(static_cast<VkMemoryPropertyFlagBits>(properties)));

			//std::cout << "\n\n\033[1;32m ======================BUFFER CREATION==========================\033[0m\n";
			//std::cout << "\trequested size: " << size << "\n";
			//std::cout << "\tusage: " << Utils::VK_BufferUsageFlagString(static_cast<VkBufferUsageFlagBits>(usage)) << "\n";
			//std::cout << "\tproperties: " << Utils::VK_MemoryPropertyFlagString(static_cast<VkMemoryPropertyFlagBits>(properties)) << "\n";

			if (vkCreateBuffer(vk_ctx->device, &buffer_info, nullptr, &gpu_buffer.buffer) != VK_SUCCESS)
				throw std::runtime_error("Failed to create vertex buffer!!!!!!");


			VkMemoryRequirements mem_requirements;
			vkGetBufferMemoryRequirements(vk_ctx->device, gpu_buffer.buffer, &mem_requirements);

			FLUX_DEBUG_LOG_INFO("\t\033[1;33mMemory Requirement for buffer\033[0m");
			FLUX_DEBUG_LOG_INFO("\t\t Size: ", mem_requirements.size);
			FLUX_DEBUG_LOG_INFO("\t\t Alignment: ", mem_requirements.alignment);
			FLUX_DEBUG_LOG_INFO("\t\t memory type bits: ", mem_requirements.memoryTypeBits);


			//std::cout << "\t\033[1;33mMemory Requirement for buffer\033[0m\n";
			//std::cout << "\t\t Size: " << mem_requirements.size << "\n";
			//std::cout << "\t\t Alignment: " << mem_requirements.alignment << "\n";
			//std::cout << "\t\t memory type bits: " << mem_requirements.memoryTypeBits << "\n";

			VkMemoryAllocateInfo alloc_info{};
			alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			alloc_info.allocationSize = mem_requirements.size;
			alloc_info.memoryTypeIndex = Utils::FindMemoryType(vk_ctx->physicalDevice, mem_requirements.memoryTypeBits, properties);

			if (vkAllocateMemory(vk_ctx->device, &alloc_info, nullptr, &gpu_buffer.memory) != VK_SUCCESS)
				throw std::runtime_error("Failed to allocate vertex buffer memeory!!!!!!");

			vkBindBufferMemory(vk_ctx->device, gpu_buffer.buffer, gpu_buffer.memory, 0);
		}

		bool GPUBuffer::MapBufferMemory(VkDevice device, GPUBuffer& gpu_buffer)
		{
			if (gpu_buffer.size == 0 || gpu_buffer.buffer == VK_NULL_HANDLE || gpu_buffer.memory == VK_NULL_HANDLE)
				throw std::runtime_error("[GPU BUFFER]{MAPMEMEMORY}: buffer might be empty or was not allocated!!!!");

			vkMapMemory(device, gpu_buffer.memory, 0, gpu_buffer.size, 0, &gpu_buffer.mapped);

			return true;
		}

		bool GPUBuffer::MapGPUBufferMemory(VkDevice device, const GPUBuffer& gpu_buffer, void** ppdata_map)
		{
			if (gpu_buffer.size == 0 || gpu_buffer.buffer == VK_NULL_HANDLE || gpu_buffer.memory == VK_NULL_HANDLE)
				throw std::runtime_error("[GPU BUFFER]{MAPMEMEMORY}: buffer might be empty or was not allocated!!!!");

			vkMapMemory(device, gpu_buffer.memory, 0, gpu_buffer.size, 0, ppdata_map);

			return true;
		}

		void GPUBuffer::CopyToMappedMemory(GPUBuffer& gpu_buffer, const void* pdata, uint32_t size)
		{
			FLUX_DEBUG_ASSERT_CRITICAL(gpu_buffer.mapped != nullptr, "[GPU BUFFER]{MAPPED MEMEORY}: GPU buffer is not mapped, trying to write into unmapped memory!!!!");
			memcpy(gpu_buffer.mapped, pdata, size);
		}

		template<typename T>
		void GPUBuffer::DirectWriteToMappedMemory(const T& data)
		{
			FLUX_DEBUG_ASSERT_CRITICAL(mapped != nullptr, "[GPU BUFFER]{MAPPED MEMEORY}: GPU buffer is not mapped, trying to write into unmapped memory!!!!");
			auto* dst = reinterpret_cast<T*>(mapped);
			dst[0] = data;
		}

		void GPUBuffer::FillBufferWithData(VkDevice device, const GPUBuffer& gpu_buffer, void* pdata)
		{
			if (gpu_buffer.size == 0 || gpu_buffer.buffer == VK_NULL_HANDLE || gpu_buffer.memory == VK_NULL_HANDLE)
				throw std::runtime_error("[GPU BUFFER]{MAPMEMEMORY}: buffer might be empty or was not allocated!!!!");

			void* map_data = nullptr;
			vkMapMemory(device, gpu_buffer.memory, 0, gpu_buffer.size, 0, &map_data);
			memcpy(map_data, pdata, static_cast<size_t>(gpu_buffer.size));
			vkUnmapMemory(device, gpu_buffer.memory);
		}

		void GPUBuffer::TransferGPUBufferData(const VulkanContext* vk_ctx, VkCommandPool cmd_pool, const GPUBuffer& src_buff, GPUBuffer& dst_buff)
		{
			FLUX_DEBUG_ASSERT_CRITICAL(src_buff.size == dst_buff.size && src_buff.size != 0, "[GPU BUFFER]{MAPMEMEMORY}: src buffer might be empty or dst size != src size!!!!");

			VkDeviceSize& buffer_size = dst_buff.size;
			Resources::CopyBuffer(vk_ctx, cmd_pool, src_buff.buffer, dst_buff.buffer, buffer_size);
		}

		void GPUBuffer::DestoryBuffer(VkDevice device, const GPUBuffer& gpu_buffer)
		{
			vkDestroyBuffer(device, gpu_buffer.buffer, nullptr);
			vkFreeMemory(device, gpu_buffer.memory, nullptr);
		}

	} //#vulkan
} //#Fluxon


//required layouts 
// model layout set 
// skybox layout set 
// material layout set 
//
// 
// ///
// 
// model layout set includes
// ubo {ubo, vertex,} 0
// smapler {combined image sampler, fragment 1
// directional light {ubo, fragment} 2
// shadow sampler {combined image sampler, fragment} 3
// 
// skybox layout set includes
// sampler cube {combined image sampler, fragement} 0 <--- this contains the images to render sky box and for its reflection
// ubo {ubo, fragment} 1 <-- this contain reflective properties used in model shader 
// 
// 
// material layout set include s
// loop all avail mat
//	mat_image 0 {combined image sampler, fragment} 0
//	mat_image i+1 {combined image sampler, fragment} i+1
// 
//	mat_image N-1 {combined image sampler, fragment} N-1
// 
// 
// 
// /////////////Sets//////////////////////////
// required sets 
// model pipeline set
// skybox pipeline set
// 
// 
// ////set incclude layouts
// model pipeline set
//	matertial layout set 0,
//	model layout set 1,
//	skybox layout set 2 <--- sampling skybox for reflections 
// 
// skybox pipelione set
//	dummy set 0
//	model layout set 1
//	skybox layout set 2
//	
//
///

//
//
//			//this is common data descriptor
//DescriptorBindingInfo light_matric_ubo_desc{};
//light_matric_ubo_desc.name = "light_matrix";
//light_matric_ubo_desc.binding = 0;
//light_matric_ubo_desc.stageFlag = VK_SHADER_STAGE_VERTEX_BIT;
//light_matric_ubo_desc.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//light_matric_ubo_desc.descType = EDescriptorType::UniformBuffer;
//light_matric_ubo_desc.bufferSize = static_cast<uint32>(sizeof(GPUStructures::MVP_UniformBufferObject));
////light_matric_ubo_desc.data.type = EDescriptorType::UniformBuffer;
////light_matric_ubo_desc.data.bufferInfo = {VK_NULL_HANDLE, 0, sizeof(GPUStructures::MVP_UniformBufferObject) };
//
////DescriptorBindingInfo move_ubo_desc{};
////move_ubo_desc.name = "move";
////move_ubo_desc.binding = 1;
////move_ubo_desc.stageFlag = VK_SHADER_STAGE_VERTEX_BIT;
////move_ubo_desc.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//////move_ubo_desc.data.type = EDescriptorType::UniformBuffer;
//////move_ubo_desc.data.bufferInfo = { VK_NULL_HANDLE, 0, sizeof(glm::mat4) };
////move_ubo_desc.descType = EDescriptorType::UniformBuffer;
////move_ubo_desc.bufferSize = static_cast<uint32>(sizeof(GPUStructures::MVP_UniformBufferObject));
//
////a layout for a set
//DescriptorLayoutInfo desc_layout_info{};
//desc_layout_info.name = "light_matrix";
//desc_layout_info.requiredDescriptorDataByIdx = { 0/*, 1 */ };   //<-- use the first one i.e light_matrix_ubo_desc
//
////DescriptorLayoutInfo desc_layout_info1{};
////desc_layout_info1.name = "move";
////desc_layout_info1.requiredDescriptorDataByIdx = { 1 };   //<-- use the first one i.e light_matrix_ubo_desc
//
//
//DescriptorSetInfo desc_set_info{};
//desc_set_info.name = "light_matrix";
//desc_set_info.set = 0;
//desc_set_info.requiredLayouts = { "light_matrix" };
//
//DescriptorSpecification desc_set_spec{};
//desc_set_spec.frameInFlight = 2;
//desc_set_spec.bTightFitDescriporPool = false;
//desc_set_spec.descriptors = { light_matric_ubo_desc/*, move_ubo_desc */ };
//desc_set_spec.layoutInfos = { desc_layout_info/*, desc_layout_info1*/ };
//desc_set_spec.setsInfos = { desc_set_info };
//
//
//
//mNewDescriptorSet.Create(vk_ctx, desc_set_spec);