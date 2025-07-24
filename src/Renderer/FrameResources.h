#pragma once

#include "GPUBuffer.h"
#include "Resources.h"
#include "GPUStrutures.h"

#include "VulkanUtilities.h"

namespace Fluxon {
	namespace Vulkan {

		const int MAX_FRAMES_IN_FLIGHT = 2;
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
			VkCommandPool commandPool;
			VkCommandBuffer commandBuffer;

			//sync objects (used per-frame)
			VkSemaphore imageAvailable;
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
			VkFence inFlight;


			//NOTE(Conqster): Its nice for each frame to have thier own resource 
			//but for expansion later its best to have a pool of resource that has semaphore&/ fences 
			//that could be acquired by frame data or render pass. i.e resources like constants (resources that does not change frequency)
			//with a fixed descriptor set & layout like (image sampler, level 2 camera that is fixed layout and global data could only change once per cpu cycle) 
			//unlike model transform the changes extermely freq as all model would have different transforms. 
			//so the constant resources count N could be (N > frame count) and managed that when a frame is acquired, a descriptor pool/&set is acquired based on availablity 
			//not tied to frame data. later might explore render pass arcitecture rather than frame pass data arctecture 
			VkDescriptorPool globalDescriptorPool;
			std::vector<VkDescriptorSet> globalDescriptorSets;


			std::vector<DescriptorBinding> pendingDescriptorBindings;
			//GPUBuffer globalUniformBuffer;
			GPUBuffer globalCameraUniformBuffer;
			GPUBuffer globalDirectionalLightUniformBuffer;
			GPUBuffer globalSkyboxUniformBuffer;


			const VkDescriptorSet& GetGlobalDescriptorSet(const DescriptorSetSpecification desc_set_spec) {
				//saftey 
				if (desc_set_spec.set >= globalDescriptorSets.size()) return VK_NULL_HANDLE;
				return globalDescriptorSets[desc_set_spec.set];
			}


			inline void AppendDescriptorBinding(const DescriptorBinding& descriptor_binding) {
				pendingDescriptorBindings.push_back(descriptor_binding);
			}

			inline void WritePendingDescriptorBinding(VkDevice device) {

				if (pendingDescriptorBindings.empty())
					return;

				std::vector<VkWriteDescriptorSet> descriptor_writes;
				descriptor_writes.reserve(pendingDescriptorBindings.size());

				for (size_t i = 0; i < pendingDescriptorBindings.size(); i++)
				{
					auto& curr_desc_bind = pendingDescriptorBindings[i];
					auto& corresponding_desc_set = globalDescriptorSets[curr_desc_bind.set];

					VkWriteDescriptorSet write{};
					if (corresponding_desc_set != VK_NULL_HANDLE)
					{
						write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
						write.dstSet = corresponding_desc_set;
						write.dstBinding = curr_desc_bind.binding;
						write.dstArrayElement = 0;
						write.descriptorCount = 1;
						if (curr_desc_bind.type == DescriptorBinding::DescritorType::UniformBuffer)
						{
							write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
							//TODO(Conqster): Need to find a work around
							//VkDescriptorBufferInfo buffer_info{ubo.buffer, 0, ubo.range};
							//descriptor_write.pBufferInfo = &buffer_info;
							write.pBufferInfo = &curr_desc_bind.ubo.info;
						}
						else if (curr_desc_bind.type == DescriptorBinding::DescritorType::ImageSampler)
						{
							write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
							//TODO(Conqster): Need to find a work around
							//auto& img_sampler = descriptor_binding.image;
							//image_info.sampler = descriptor_binding.image.sampler;
							//image_info.imageView = descriptor_binding.image.imageView;
							//image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
							//descriptor_write.pImageInfo = &image_info;
							write.pImageInfo = &curr_desc_bind.image.info;
						}
						descriptor_writes.emplace_back(write);
					}
					else
					{
						std::cout << "\033[1;33m [FRAME RESOURCES Descriptor write]\033[0m Descriptor set layout; set: " << curr_desc_bind.set << ", binding: " << curr_desc_bind.binding << " was not registered during 'Vulkan::GenerateFrameObjectDescriptorPoolSet(ctx, DescriptorSetSpecification, @frame_ref)' \n";
					}



				}

				vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptor_writes.size()), descriptor_writes.data(), 0, nullptr);

				//pendingDescriptorBindings.clear();
			}
		};


		inline void CreateAndAppendFrameResourceUBO(const VulkanContext* vk_ctx, GPUBuffer& ubo, FrameResources& frame_resource, UniformBufferSpecification ubo_spec) {

			CreateBuffer(vk_ctx, static_cast<VkDeviceSize>(ubo_spec.requiredSize), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				ubo);

			MapBufferMemory(vk_ctx->device, ubo);

			frame_resource.AppendDescriptorBinding(Vulkan::DescriptorBinding::MakeUniformBuffer(ubo_spec.set, ubo_spec.binding, ubo));
		}

		inline void CreateAndAppendFrameDatasUniformBuffers(const VulkanContext* vk_ctx, std::vector<FrameResources>& frame_datas, std::vector<UniformBufferSpecification> ubo_specs) {

			for (auto& frame : frame_datas)
			{
				for (auto& ubo_spec : ubo_specs)
				{
					//GPUBuffer& ubo = frame.globalDirectionalLightUniformBuffer;
					switch (ubo_spec.type)
					{
					case GlobalUboType::Camera_UBO:
						CreateAndAppendFrameResourceUBO(vk_ctx, frame.globalCameraUniformBuffer, frame, ubo_spec);
						break;
					case GlobalUboType::Skybox_UBO:
						CreateAndAppendFrameResourceUBO(vk_ctx, frame.globalSkyboxUniformBuffer, frame, ubo_spec);
						break;
					case GlobalUboType::DirectionalLight_UBO:
						CreateAndAppendFrameResourceUBO(vk_ctx, frame.globalDirectionalLightUniformBuffer, frame, ubo_spec);
						break;
					default:
						continue;
						//break;
					}


					//CreateBuffer(vk_ctx, static_cast<VkDeviceSize>(ubo_spec.requiredSize), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					//	VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					//	ubo);

					//MapBufferMemory(vk_ctx->device, ubo);

					//frame.AppendDescriptorBinding(Vulkan::DescriptorBinding::MakeUniformBuffer(ubo_spec.set, ubo_spec.binding, ubo));
				}

			}
		}



		inline void CreateFrameDatasUniformCameraBuffer(const VulkanContext* vk_ctx, std::vector<FrameResources>& frame_datas) {

			for (auto& frame : frame_datas)
			{
				auto& ubo = frame.globalCameraUniformBuffer;
				VkDeviceSize buffer_size = sizeof(GPUStructures::MVP_UniformBufferObject);

				CreateBuffer(vk_ctx, buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					ubo);

				//MapGPUBufferMemory(device->device, uniform_buffers[i], &uniform_buff_maps[i]);
				MapBufferMemory(vk_ctx->device, ubo);

				frame.AppendDescriptorBinding(Vulkan::DescriptorBinding::MakeUniformBuffer(0, 0, ubo));
			}
		}

		inline void CreateFrameDatasDirectionalLightUniformbuffer(const VulkanContext* vk_ctx, std::vector<FrameResources>& frame_datas) {

			for (auto& frame : frame_datas)
			{
				auto& ubo = frame.globalDirectionalLightUniformBuffer;
				VkDeviceSize buffer_size = sizeof(GPUStructures::DirectionalLight);

				CreateBuffer(vk_ctx, buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					ubo);

				//MapGPUBufferMemory(device->device, uniform_buffers[i], &uniform_buff_maps[i]);
				MapBufferMemory(vk_ctx->device, ubo);

				frame.AppendDescriptorBinding(Vulkan::DescriptorBinding::MakeUniformBuffer(0, 2, ubo));
			}
		}

		inline void CreateFrameObjects(const VulkanContext* vk_ctx, std::vector<FrameResources>& frame_data_objs) {

			if (frame_data_objs.empty())
				throw std::runtime_error("Failed to create frame object, std::vector data collection empty!!!");

			//pool create info
			QueueFamilyIndices queue_family_indices = Utils::FindDeviceQueueFamilies(*vk_ctx);
			VkCommandPoolCreateInfo command_pool_create_info{};
			command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			command_pool_create_info.queueFamilyIndex = queue_family_indices.graphicsFamily.value();

			for (auto& [command_pool, command_buff, image_avail, in_flight_fence, descriptor_pool, descriptor_set, pending_desc_bind, uniformbuffers, uniformbuffers1, uniformbuffers2] : frame_data_objs)
			{
				if (vkCreateCommandPool(vk_ctx->device, &command_pool_create_info, nullptr, &command_pool) != VK_SUCCESS)
					throw std::runtime_error("Failed to create command pools!!!!");

				//buffer create info
				VkCommandBufferAllocateInfo allocation_info{};
				allocation_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				allocation_info.commandPool = command_pool;
				allocation_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				allocation_info.commandBufferCount = 1; //<-- one bufferr per pool per frame data


				//create buffer 
				if (vkAllocateCommandBuffers(vk_ctx->device, &allocation_info, &command_buff) != VK_SUCCESS)
					throw std::runtime_error("Failed to allocate command buffers!!!!!!");


				VkSemaphoreCreateInfo semaphore_create_info{};
				semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

				VkFenceCreateInfo fence_create_info{};
				fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
				fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

				if (vkCreateSemaphore(vk_ctx->device, &semaphore_create_info, nullptr, &image_avail) != VK_SUCCESS ||
					vkCreateFence(vk_ctx->device, &fence_create_info, nullptr, &in_flight_fence) != VK_SUCCESS)
					throw std::runtime_error("Failed to create semaphore!!!!!!");

				//create descriptor pool 
				//CreateDescriptorPool(device->device, descriptor_pool);

				//CreateDescriptorSets(descriptor_set, device->device, descriptor_pool)
			}
		}

		inline void CreateDescriptorPool(VkDevice device, VkDescriptorPool& descriptor_pool)
		{
			//descriptor pool for uniform buffer
			VkDescriptorPoolSize ubo_pool_size{};
			ubo_pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
#if IMGUI_IN_USE
			ubo_pool_size.descriptorCount = 1000;
#else
			ubo_pool_size.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
#endif // IMGUI_IN_USE



			//descriptor pool for image sampler
			VkDescriptorPoolSize sampler_pool_size{};
			sampler_pool_size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
#if IMGUI_IN_USE
			sampler_pool_size.descriptorCount = 1000;
#else
			sampler_pool_size.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
#endif // IMGUI_IN_USE


			//group pool size
			std::array<VkDescriptorPoolSize, 2> grp_pool_sizes = { ubo_pool_size, sampler_pool_size };

			VkDescriptorPoolCreateInfo pool_create_info{};
			pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			pool_create_info.poolSizeCount = static_cast<uint32_t>(grp_pool_sizes.size());
			pool_create_info.pPoolSizes = grp_pool_sizes.data();

#if IMGUI_IN_USE
			//when using imgui because during shut down imgui 
			//would attempt to free descriptor set 
			pool_create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

			pool_create_info.maxSets = 1000 * grp_pool_sizes.size();
#else
			pool_create_info.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
#endif // IMGUI_IN_USE


			if (vkCreateDescriptorPool(device, &pool_create_info, nullptr, &descriptor_pool) != VK_SUCCESS)
				throw std::runtime_error("Failed to Create Descriptor Pool!!!!!!!");
		}

		inline void CreateDescriptorSet(VkDevice device, VkDescriptorPool& descriptor_pool, VkDescriptorSet& descriptor_sets, VkDescriptorSetLayout& descriptor_set_layout) {
			//std::vector<VkDescriptorSetLayout> layouts(1, descriptor_set_layout);
			VkDescriptorSetAllocateInfo alloc_info{};
			alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			alloc_info.descriptorPool = descriptor_pool;
			alloc_info.descriptorSetCount = 1;
			alloc_info.pSetLayouts = &descriptor_set_layout;

			if (vkAllocateDescriptorSets(device, &alloc_info, &descriptor_sets) != VK_SUCCESS)
				throw std::runtime_error("Failed to allocate descriptor sets !!!!!!!");
		}

		inline void GenerateFrameObjectDescriptorPoolSet(VkDevice device, std::vector<DescriptorSetSpecification> descriptor_set_layout_specs, std::vector<FrameResources>& frame_datas) {

			for (uint32_t i = 0; i < frame_datas.size(); i++)
			{
				auto& descriptor_pool = frame_datas[i].globalDescriptorPool;
				CreateDescriptorPool(device, descriptor_pool);


				//This makes this a bit dynamic
				//so the only sets are only registed if need so based on the set value 
				//
				auto& descriptor_sets = frame_datas[i].globalDescriptorSets;
				for (auto& descriptor_set_layout_spec : descriptor_set_layout_specs)
				{
					if (descriptor_set_layout_spec.set >= descriptor_sets.size())
						descriptor_sets.resize(descriptor_set_layout_spec.set + 1);
					CreateDescriptorSet(device, descriptor_pool, descriptor_sets[descriptor_set_layout_spec.set], descriptor_set_layout_spec.layoutHandle);
				}
			}

		}

		inline void DestroyFrameObjects(const VulkanContext* vk_ctx, std::vector<FrameResources> frame_data_objs)
		{

			for (auto& [command_pool, command_buff, image_avail, in_flight_fence, descriptor_pool, descriptor_set, pending_desc_bind, uniform_buffer, uniform_buffer1, uniform_buffer2] : frame_data_objs)
			{
				auto& device = vk_ctx->device;
				vkDestroySemaphore(device, image_avail, nullptr);
				vkDestroyFence(device, in_flight_fence, nullptr);

				vkDestroyCommandPool(device, command_pool, nullptr);

				//vkFreeDescriptorSets(mDevice.device, mDescriptorPool, static_cast<uint32_t>(mDescriptorSets.size()), mDescriptorSets.data());
				vkDestroyDescriptorPool(device, descriptor_pool, nullptr);

				Vulkan::DestoryBuffer(vk_ctx->device, uniform_buffer);
				Vulkan::DestoryBuffer(vk_ctx->device, uniform_buffer1);
				Vulkan::DestoryBuffer(vk_ctx->device, uniform_buffer2);
			}
		}





		inline VkWriteDescriptorSet AppendDescriptorSetWrite(VkDescriptorSet& desc_set, const DescriptorBinding& descriptor_binding) {

			VkWriteDescriptorSet descriptor_write{};
			descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptor_write.dstSet = desc_set;
			descriptor_write.dstBinding = descriptor_binding.binding;
			descriptor_write.dstArrayElement = 0;
			descriptor_write.descriptorCount = 1;
			if (descriptor_binding.type == DescriptorBinding::DescritorType::UniformBuffer)
			{
				descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				//TODO(Conqster): Need to find a work around
				//VkDescriptorBufferInfo buffer_info{ubo.buffer, 0, ubo.range};
				//descriptor_write.pBufferInfo = &buffer_info;
				descriptor_write.pBufferInfo = &descriptor_binding.ubo.info;
			}
			else if (descriptor_binding.type == DescriptorBinding::DescritorType::ImageSampler)
			{
				descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				//TODO(Conqster): Need to find a work around
				//auto& img_sampler = descriptor_binding.image;
				//image_info.sampler = descriptor_binding.image.sampler;
				//image_info.imageView = descriptor_binding.image.imageView;
				//image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				//descriptor_write.pImageInfo = &image_info;
				descriptor_write.pImageInfo = &descriptor_binding.image.info;
			}
			return descriptor_write;
		}

		inline void AppendDescriptorSetWrite(VkWriteDescriptorSet& writer, VkDescriptorImageInfo& image_info, VkDescriptorSet& desc_set, Vulkan::Resources::ImageSampler& img_sampler) {


			writer.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writer.dstSet = desc_set;
			writer.dstBinding = 1;
			writer.dstArrayElement = 0;
			writer.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			writer.descriptorCount = 1;
			writer.pImageInfo = &image_info;

		}

		//inline void UpdateFrameDataDescriptorSet(VkDevice device, const std::vector<DescriptorBinding>& descriptor_bindings, FrameData& frame_data) {

		//	VkDescriptorSet descriptor_set = frame_data.descriptorSet;
		//	std::vector<VkWriteDescriptorSet> desc_writes(descriptor_bindings.size());
		//	for (size_t i = 0; i < descriptor_bindings.size(); i++)
		//		desc_writes[i] = WriteDescriptorSet(device, descriptor_set,descriptor_bindings[i]);

		//	vkUpdateDescriptorSets(device, static_cast<uint32_t>(desc_writes.size()), desc_writes.data(), 0, nullptr);
		//}


		//inline void UpdateFrameDataDescriptorSet(VkDevice device, const std::vector<DescriptorBinding>& descriptor_bindings, FrameData* frame_data) {

		//	if (!frame_data)
		//		throw std::runtime_error("No Frames available");
		//	
		//	uint32_t frames_count = sizeof(frame_data) / sizeof(FrameData);
		//	for (uint32_t i = 0; i < frames_count; i++)
		//	{
		//		VkDescriptorSet descriptor_set = frame_data[i].descriptorSet;

		//		std::vector<VkWriteDescriptorSet> desc_writes(descriptor_bindings.size());
		//		for (size_t i = 0; i < descriptor_bindings.size(); i++)
		//			desc_writes[i] = WriteDescriptorSet(device, descriptor_set, descriptor_bindings[i]);

		//		vkUpdateDescriptorSets(device, static_cast<uint32_t>(desc_writes.size()), desc_writes.data(), 0, nullptr);
		//	}
		//}


		inline void CreatePipelineUniformBuffer(const VulkanContext* vk_ctx, std::vector<GPUBuffer>& uniform_buffers, std::vector<FrameResources>& frame_data) {


			VkDeviceSize buffer_size = sizeof(GPUStructures::MVP_UniformBufferObject);

			uniform_buffers.resize(frame_data.size());

			for (size_t i = 0; i < frame_data.size(); i++)
			{

				CreateBuffer(vk_ctx, buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					uniform_buffers[i]);

				//MapGPUBufferMemory(device->device, uniform_buffers[i], &uniform_buff_maps[i]);
				MapBufferMemory(vk_ctx->device, uniform_buffers[i]);

				frame_data[i].AppendDescriptorBinding(Vulkan::DescriptorBinding::MakeUniformBuffer(0, 0, uniform_buffers[i]));
			}
		}




		////Utility to call with Vulkancontext to deprecated Device structure
		//inline void CreateFrameObjects(const Vulkan2::VulkanContext* vk_render_ctx, std::vector<FrameData>& frame_data_objs) {

		//	Device temp_sys_device;
		//	temp_sys_device.instance = vk_render_ctx->instance;
		//	temp_sys_device.physicalDevice = vk_render_ctx->physicalDevice;
		//	temp_sys_device.device = vk_render_ctx->device;
		//	temp_sys_device.surface = vk_render_ctx->surface;
		//	temp_sys_device.graphicsQueue = vk_render_ctx->graphicsQueue;
		//	temp_sys_device.presentQueue = vk_render_ctx->presentQueue;
		//	temp_sys_device.queueFamilyIndices = vk_render_ctx->queueFamilyIndices;
		//	temp_sys_device.debugMessager = vk_render_ctx->debugMessager;

		//	CreateFrameObjects(&temp_sys_device, frame_data_objs);
		//}


		//inline void CreateFrameDatasUniformBuffer(const Vulkan2::VulkanContext* vk_render_ctx, std::vector<FrameData>& frame_datas) {
		//	Device temp_sys_device;
		//	temp_sys_device.instance = vk_render_ctx->instance;
		//	temp_sys_device.physicalDevice = vk_render_ctx->physicalDevice;
		//	temp_sys_device.device = vk_render_ctx->device;
		//	temp_sys_device.surface = vk_render_ctx->surface;
		//	temp_sys_device.graphicsQueue = vk_render_ctx->graphicsQueue;
		//	temp_sys_device.presentQueue = vk_render_ctx->presentQueue;
		//	temp_sys_device.queueFamilyIndices = vk_render_ctx->queueFamilyIndices;
		//	temp_sys_device.debugMessager = vk_render_ctx->debugMessager;

		//	CreateFrameDatasUniformBuffer(&temp_sys_device, frame_datas);
		//}



	} //#Vulkan
}//#Fluxon