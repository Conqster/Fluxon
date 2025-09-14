#pragma once

#include "GPUBuffer.h"
#include "Resources.h"
#include "GPUStrutures.h"

#include "VulkanUtilities.h"

#include "VulkanCommandBuffer.h"

namespace Fluxon {
	namespace Vulkan {

#define IMGUI_IN_USE 1

		struct DescriptorSetSpecification
		{
			uint32_t set = 0;
			VkDescriptorSetLayout layoutHandle = VK_NULL_HANDLE;
		};

		struct UniformBufferInfo
		{
			VkBuffer buffer = VK_NULL_HANDLE;
			uint64_t range = 0; //<-- is also know to be the size of buffer

			VkDescriptorBufferInfo info = {};
		};


		//hack for now 
		enum class GlobalUboType : uint8_t {
			Camera_UBO,
			Skybox_UBO,
			DirectionalLight_UBO,
			Unknown
		};

		//later rather than having an enum use buffer index
		//where 0 ... a specific buffer type could register to 
		//to have it more dynamic, than having to hardcode the global uniform buffer frame resources
		struct UniformBufferSpecification // Or UniformBufferBulePrint //Or UniformBufferSchematics //Or UniformBufferRequirement
		{
			GlobalUboType type = GlobalUboType::Unknown;
			uint32_t set = 0;
			uint32_t binding = 0;
			uint32_t requiredSize = 0;
		};


		struct ImageSamplerInfo
		{
			VkSampler sampler = VK_NULL_HANDLE;
			VkImageView imageView = VK_NULL_HANDLE;

			VkDescriptorImageInfo info = {};
		};


		struct DescriptorBinding
		{
			enum class DescritorType : uint8_t
			{
				UniformBuffer,
				ImageSampler,
				Unknown
			}type = DescritorType::Unknown;

			uint32_t set = 0;
			uint32_t binding = 0;
			union
			{
				UniformBufferInfo ubo;
				ImageSamplerInfo image;
			};

			DescriptorBinding() { std::memset(this, 0, sizeof(*this)); }

			//Utilities
			static DescriptorBinding MakeUniformBuffer(uint32_t set, uint32_t binding, UniformBufferInfo& ubo) {

				ubo.info.buffer = ubo.buffer;
				ubo.info.range = ubo.range;

				DescriptorBinding desc_binding{};
				desc_binding.type = DescritorType::UniformBuffer;
				desc_binding.set = set;
				desc_binding.binding = binding;
				desc_binding.ubo = ubo;
				return desc_binding;
			}

			//static DescriptorBinding MakeUniformBuffer(uint32_t binding, VkBuffer buffer, VkDeviceSize range) {
			//	return MakeUniformBuffer(binding, { buffer, range });
			//}

			static DescriptorBinding MakeUniformBuffer(uint32_t set, uint32_t binding, GPUBuffer gpu_buffer) {

				UniformBufferInfo uniform_buffer{};
				uniform_buffer.buffer = gpu_buffer.buffer;
				uniform_buffer.range = gpu_buffer.size;

				uniform_buffer.info.buffer = gpu_buffer.buffer;
				uniform_buffer.info.range = gpu_buffer.size;

				DescriptorBinding desc_binding{};
				desc_binding.type = DescritorType::UniformBuffer;
				desc_binding.set = set;
				desc_binding.binding = binding;
				desc_binding.ubo = uniform_buffer;
				return desc_binding;
			}

			static DescriptorBinding MakeImageSampler(uint32_t set, uint32_t binding, ImageSamplerInfo& image_sampler) {

				image_sampler.info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				image_sampler.info.imageView = image_sampler.imageView;
				image_sampler.info.sampler = image_sampler.sampler;

				DescriptorBinding desc_binding{};
				desc_binding.type = DescritorType::ImageSampler;
				desc_binding.set = set;
				desc_binding.binding = binding;
				desc_binding.image = image_sampler;
				return desc_binding;
			}

			//static DescriptorBinding MakeImageSampler(uint32_t binding, VkImageView img_view, VkSampler sampler) {
			//	return  MakeImageSampler(binding, ImageSamplerInfo({ sampler, img_view, {} }));
			//}

			static DescriptorBinding MakeImageSampler(uint32_t set, uint32_t binding, Resources::ImageSampler img_sampler) {

				ImageSamplerInfo image_sampler{};
				image_sampler.imageView = img_sampler.imageView;
				image_sampler.sampler = img_sampler.sampler;

				image_sampler.info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				image_sampler.info.imageView = image_sampler.imageView;
				image_sampler.info.sampler = image_sampler.sampler;

				DescriptorBinding desc_binding{};
				desc_binding.type = DescritorType::ImageSampler;
				desc_binding.set = set;
				desc_binding.binding = binding;
				desc_binding.image = image_sampler;
				return desc_binding;
			}
		};

		struct FrameResources
		{
			//TODO: might have one pool that all the bufer can use
			VkCommandPool commandPool									= VK_NULL_HANDLE;
			//VkCommandBuffer commandBuffer								= VK_NULL_HANDLE;
			VulkanCommandBuffer gfxCmdBuffer;

			//sync objects (used per-frame)
			VkSemaphore imageAvailable									= VK_NULL_HANDLE;
			//render finsinhed semaphore needs to be image/swap chain image based count 
			//because nots not based on the frame count/flight count its based  on
			//is the graphic queue finshed rendering/writing/using the spaw chain image then 
			//if so it can now be used, not flight based 
			//Each swapchain image needs its own render-finshed semaphore 
			//This is because the semaphore is tied to the usage of a specific swapcchain image, 
			// not to the frame-in-flight index. A semaphore used in vkQueuePresentKHR
			// may still be in use by the presentation engine even after the GPU work is Reusing 
			// it too early (e.g, based on frame count) can cause validation errors.
			// By indexing semaphores by the acquired image index, we ensure that a semaphore
			// is only reused after the presentation engine has finshed with that image.
			//
			VkFence inFlight											= VK_NULL_HANDLE;

			//compute 
			//VkCommandBuffer computeCommandBuffer						= VK_NULL_HANDLE;
			 VulkanCommandBuffer computeCmdBuffer;
			//compute sync object
			VkSemaphore computeCompleteSemaphore						= VK_NULL_HANDLE;
			VkFence computeInFlight										= VK_NULL_HANDLE;


		};



		inline void CreateFrameObjects(const VulkanContext* vk_ctx, std::vector<FrameResources>& frame_data_objs) {

			if (frame_data_objs.empty())
				throw std::runtime_error("Failed to create frame object, std::vector data collection empty!!!");

			//pool create info
			QueueFamilyIndices queue_family_indices = Utils::FindDeviceQueueFamilies(*vk_ctx);
			VkCommandPoolCreateInfo command_pool_create_info{};
			command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			command_pool_create_info.queueFamilyIndex = queue_family_indices.graphicsAndComputeFamily.value();

			//for (auto& [command_pool, command_buff, image_avail, in_flight_fence, descriptor_pool, descriptor_set, pending_desc_bind, uniformbuffers, uniformbuffers1, uniformbuffers2] : frame_data_objs)
			for (auto& frame_reso : frame_data_objs)
			{
				VkCommandPool& command_pool = frame_reso.commandPool;
				//VkCommandBuffer& command_buff = frame_reso.commandBuffer;
				//VkCommandBuffer& compute_cmd_buff = frame_reso.computeCommandBuffer;

				VkFence& in_flight_fence = frame_reso.inFlight;
				VkSemaphore& image_avail = frame_reso.imageAvailable;

				VkFence& compute_in_flight_fence = frame_reso.computeInFlight;
				VkSemaphore& compute_complete_semaphore = frame_reso.computeCompleteSemaphore;

				if (vkCreateCommandPool(vk_ctx->device, &command_pool_create_info, nullptr, &command_pool) != VK_SUCCESS)
					throw std::runtime_error("Failed to create command pools!!!!");

				//buffer create info
				//VkCommandBufferAllocateInfo allocation_info{};
				//allocation_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				//allocation_info.commandPool = command_pool;
				//allocation_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				//allocation_info.commandBufferCount = 1; //<-- one bufferr per pool per frame data


				////create buffer 
				//if (vkAllocateCommandBuffers(vk_ctx->device, &allocation_info, &command_buff) != VK_SUCCESS)
				//	throw std::runtime_error("Failed to allocate command buffers!!!!!!");

				//if (vkAllocateCommandBuffers(vk_ctx->device, &allocation_info, &compute_cmd_buff) != VK_SUCCESS)
				//	throw std::runtime_error("Failed to allocate command buffers!!!!!!");

				frame_reso.gfxCmdBuffer.Init(vk_ctx->device, command_pool);
				frame_reso.computeCmdBuffer.Init(vk_ctx->device, command_pool);


				VkSemaphoreCreateInfo semaphore_create_info{};
				semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

				VkFenceCreateInfo fence_create_info{};
				fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
				fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

				if (vkCreateSemaphore(vk_ctx->device, &semaphore_create_info, nullptr, &image_avail) != VK_SUCCESS ||
					vkCreateFence(vk_ctx->device, &fence_create_info, nullptr, &in_flight_fence) != VK_SUCCESS)
					throw std::runtime_error("Failed to create semaphore!!!!!!");

				if (vkCreateSemaphore(vk_ctx->device, &semaphore_create_info, nullptr, &compute_complete_semaphore) != VK_SUCCESS ||
					vkCreateFence(vk_ctx->device, &fence_create_info, nullptr, &compute_in_flight_fence) != VK_SUCCESS)
					throw std::runtime_error("Failed to create semaphore!!!!!!");

				//create descriptor pool 
				//CreateDescriptorPool(device->device, descriptor_pool);

				//CreateDescriptorSets(descriptor_set, device->device, descriptor_pool)
			}
		}


		inline void DestroyFrameObjects(const VulkanContext* vk_ctx, std::vector<FrameResources>& frame_data_objs)
		{
			//std::vector<VkCommandBuffer> cmd_buffers;
			//cmd_buffers.reserve(2 * frame_data_objs.size());


			//for (auto& [command_pool, command_buff, image_avail, in_flight_fence, descriptor_pool, descriptor_set, pending_desc_bind, uniform_buffer, uniform_buffer1, uniform_buffer2] : frame_data_objs)
			for (auto& frame_reso : frame_data_objs)
			{
				VkFence& in_flight_fence = frame_reso.inFlight;
				VkSemaphore& image_avail = frame_reso.imageAvailable;

				VkFence& compute_in_flight_fence = frame_reso.computeInFlight;
				VkSemaphore& compute_complete_semaphore = frame_reso.computeCompleteSemaphore;

				VkCommandPool& command_pool = frame_reso.commandPool;



				auto& device = vk_ctx->device;
				vkDestroySemaphore(device, image_avail, nullptr);
				vkDestroyFence(device, in_flight_fence, nullptr);

				vkDestroyFence(device, compute_in_flight_fence, nullptr);
				vkDestroySemaphore(device, compute_complete_semaphore, nullptr);

				frame_reso.gfxCmdBuffer.Destroy(vk_ctx->device, command_pool);
				frame_reso.computeCmdBuffer.Destroy(vk_ctx->device, command_pool);

				//group free
				//cmd_buffers.emplace_back(cmd_buffers);

				vkDestroyCommandPool(device, command_pool, nullptr);
			}
		}





	} //#Vulkan
}//#Fluxon