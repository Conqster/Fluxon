#include "ComputePass.h"
#include "Renderer/GPUStrutures.h"


#include <random>

#include <glm/gtc/matrix_transform.hpp>

namespace Fluxon {
	namespace Vulkan {

		void ComputePass::Init(class VulkanContext* vk_ctx, VkCommandPool cmd_pool, VkRenderPass& forward_render_pass, VkDescriptorSetLayout& camera_desc_set_layout)
		{

			VkDeviceSize buffer_size = static_cast<VkDeviceSize>(sizeof(GPUStructures::EmitterUBO));
			VkBufferUsageFlags usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
			VkMemoryPropertyFlags prop = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT; //<--- to be able to write per frame
			for (auto& buffer : mShaderFrameTimeBuffer)
			{
				GPUBuffer::CreateBuffer(vk_ctx, buffer_size, usage, prop, buffer);
				GPUBuffer::MapBufferMemory(vk_ctx->device, buffer);
			}


			std::default_random_engine rnd_engine((unsigned)time(nullptr));
			std::uniform_real_distribution<float> rnd_dist(0.0f, 1.0f);
			float width = static_cast<float>(vk_ctx->pMainPlatformWindow->width);
			float height = static_cast<float>(vk_ctx->pMainPlatformWindow->height);
			//initial particles 
			mParticles.resize(PARTICLE_COUNT);
			for (auto& particle : mParticles)
			{
				float r = 0.25f * std::sqrt(rnd_dist(rnd_engine));
				float theta = rnd_dist(rnd_engine) * 2 * 3.14159265358979323846;
				float x = r * cos(theta) * height / width;
				float y = r * sin(theta);
				float z = (rnd_dist(rnd_engine) * 2.0f - 1.0f);// *5;
				particle.position = glm::vec3(x, y, z);// *50.0f;
				particle.velocity = glm::normalize(glm::vec3(x, y, z)); //* 0.00025f;
			}
			
			mEmitter.worldTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-12.5f, 0.0f, 0.0f));

			GPUBuffer staging_buffer;
			buffer_size = static_cast<VkDeviceSize>(sizeof(GPUStructures::Particle) * PARTICLE_COUNT);
			GPUBuffer::CreateBuffer(vk_ctx, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
				VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer);

			GPUBuffer::FillBufferWithData(vk_ctx->device, staging_buffer, mParticles.data());


			usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			prop = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			for (auto& storage_buffer : mShaderStorageBuffer)
			{
				GPUBuffer::CreateBuffer(vk_ctx, buffer_size, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, storage_buffer);
				GPUBuffer::TransferGPUBufferData(vk_ctx, cmd_pool, staging_buffer, storage_buffer);
			}
			GPUBuffer::DestoryBuffer(vk_ctx->device, staging_buffer);


#pragma region Atomic Spawn Counter GPU Buffer 

			buffer_size = static_cast<VkDeviceSize>(sizeof(int));
			usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
			prop = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT; //<--- to be able to write per frame
			for (auto& buffer : mShaderEmitterSpawnCounterSSBO)
			{
				GPUBuffer::CreateBuffer(vk_ctx, buffer_size, usage, prop, buffer);
				GPUBuffer::MapBufferMemory(vk_ctx->device, buffer);
			}
#pragma endregion

			CreateDescriptorSetLayouts(vk_ctx);
			CreatePipeline(vk_ctx, forward_render_pass, camera_desc_set_layout);
		}
		void ComputePass::PreBeginFrameSync(uint32_t curr_frame, double delta_time)
		{
			mEmitter.deltaTime = delta_time;

			GPUBuffer::CopyToMappedMemory(mShaderFrameTimeBuffer[curr_frame], &mEmitter, sizeof(mEmitter));


			//int* counter_ptr1 = static_cast<int*>(mShaderEmitterSpawnCounterSSBO[curr_frame].mapped);
			//int emitted = *counter_ptr1;

			////compute carry over
			//int carry_over_spawn = mEmitter.spawnCount - emitted;
			//printf("last frame emitted, %d\n", emitted);
			//printf("last frame Carry over spawn, %d\n", carry_over_spawn);

			////Reset atomic counter 
			//FLUX_ASSERT_GPUBUFFER_MAPPED(mShaderEmitterSpawnCounterSSBO[curr_frame]);
			//int* counter_ptr = static_cast<int*>(mShaderEmitterSpawnCounterSSBO[curr_frame].mapped);
			//counter_ptr[0] = 0;

		}
		void ComputePass::HackRenderComputeParticle(VulkanContext* vk_ctx, VkCommandBuffer cmd_buff, const VkDescriptorSet* pcam_desc_set)
		{

			//vkCmdBindPipeline(cmd_buff, VK_PIPELINE_BIND_POINT_GRAPHICS, mComputeGfxPipeline.GetPipelineHandle());
			mComputeGfxPipeline.Bind(cmd_buff);

			mComputeGfxPipeline.BindDescriptorSets(cmd_buff, pcam_desc_set, 1, 1);
			mComputeGfxPipeline.PushConstant(cmd_buff, VK_SHADER_STAGE_VERTEX_BIT, 0, (uint32_t)sizeof(GPUStructures::Model), &mEmitter);

			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(cmd_buff, 0, 1, &mShaderStorageBuffer[vk_ctx->frameIndex].buffer, offsets);

			vkCmdDraw(cmd_buff, PARTICLE_COUNT, 1, 0, 0);
		}
		bool ComputePass::CreateDescriptorSetLayouts(const VulkanContext* vk_ctx)
		{
			std::array<VkDescriptorSetLayoutBinding, 4> layout_bindings{};
			layout_bindings[0].binding = 0;
			layout_bindings[0].descriptorCount = 1;
			layout_bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			layout_bindings[0].pImmutableSamplers = nullptr;
			layout_bindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

			layout_bindings[1].binding = 1;
			layout_bindings[1].descriptorCount = 1;
			layout_bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			layout_bindings[1].pImmutableSamplers = nullptr;
			layout_bindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

			layout_bindings[2].binding = 2;
			layout_bindings[2].descriptorCount = 1;
			layout_bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			layout_bindings[2].pImmutableSamplers = nullptr;
			layout_bindings[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;


#pragma region Atomic Spawn Counter Set 
			layout_bindings[3].binding = 3;
			layout_bindings[3].descriptorCount = 1;
			layout_bindings[3].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			layout_bindings[3].pImmutableSamplers = nullptr;
			layout_bindings[3].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
#pragma endregion


			VkDescriptorSetLayoutCreateInfo layoutInfo{};
			layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layoutInfo.bindingCount = static_cast<uint32_t>(layout_bindings.size());
			layoutInfo.pBindings = layout_bindings.data();

			mComputeDescSetSpec.set = 0;
			if (vkCreateDescriptorSetLayout(vk_ctx->device, &layoutInfo, nullptr, &mComputeDescSetSpec.layoutHandle) != VK_SUCCESS) {
				throw std::runtime_error("failed to create compute descriptor set layout!");
			}



			//descriptor pool
			VkDescriptorPoolSize frametime_ubo_pool_size{};
			frametime_ubo_pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			frametime_ubo_pool_size.descriptorCount = 1 * static_cast<uint32_t>(kMaxFramesInFlight);

			VkDescriptorPoolSize particle_ssbo_pool_size{};
			particle_ssbo_pool_size.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			//2 * one SSBO for In Particle and second for Out Particle 
			particle_ssbo_pool_size.descriptorCount = 2 * static_cast<uint32_t>(kMaxFramesInFlight); 

#pragma region Atomic Spawn Counter Set 
			VkDescriptorPoolSize atomic_spawn_ssbo_pool_size{};
			atomic_spawn_ssbo_pool_size.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			//2 * one SSBO for In Particle and second for Out Particle 
			atomic_spawn_ssbo_pool_size.descriptorCount = 1 * static_cast<uint32_t>(kMaxFramesInFlight);
#pragma endregion

			std::array<VkDescriptorPoolSize, 3> pool_sizes = { frametime_ubo_pool_size, particle_ssbo_pool_size, atomic_spawn_ssbo_pool_size };
			VkDescriptorPoolCreateInfo pool_create_info{};
			pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			pool_create_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
			pool_create_info.pPoolSizes = pool_sizes.data();
			pool_create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
			pool_create_info.maxSets = static_cast<uint32_t>(kMaxFramesInFlight);

			if (vkCreateDescriptorPool(vk_ctx->device, &pool_create_info, nullptr, &mDescriptorPool) != VK_SUCCESS)
				throw std::runtime_error("Failed to Create Descriptor Pool!!!!!!!");

			std::vector<VkDescriptorSetLayout> layouts(kMaxFramesInFlight, mComputeDescSetSpec.layoutHandle);
			VkDescriptorSetAllocateInfo alloc_info{};
			alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			alloc_info.descriptorPool = mDescriptorPool;
			alloc_info.descriptorSetCount = static_cast<uint32_t>(kMaxFramesInFlight);
			alloc_info.pSetLayouts = layouts.data();

			if (vkAllocateDescriptorSets(vk_ctx->device, &alloc_info, mDescriptorSets.data()) != VK_SUCCESS)
				throw std::runtime_error("Failed to allocate descriptor sets !!!!!!!");



			for (size_t i = 0; i < kMaxFramesInFlight; i++) 
			{


				VkDescriptorBufferInfo uniformBufferInfo{};
				uniformBufferInfo.buffer = mShaderFrameTimeBuffer[i].buffer;
				uniformBufferInfo.offset = 0;
				uniformBufferInfo.range = mShaderFrameTimeBuffer[i].size;

				std::array<VkWriteDescriptorSet, 4> descriptorWrites{};
				descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[0].dstSet = mDescriptorSets[i];
				descriptorWrites[0].dstBinding = 0;
				descriptorWrites[0].dstArrayElement = 0;
				descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorWrites[0].descriptorCount = 1;
				descriptorWrites[0].pBufferInfo = &uniformBufferInfo;

				VkDescriptorBufferInfo storageBufferInfoLastFrame{};
				storageBufferInfoLastFrame.buffer = mShaderStorageBuffer[(i - 1) % kMaxFramesInFlight].buffer;
				storageBufferInfoLastFrame.offset = 0;
				storageBufferInfoLastFrame.range = mShaderStorageBuffer[(i - 1) % kMaxFramesInFlight].size;// *PARTICLE_COUNT; <<-- size/range already computed

				descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[1].dstSet = mDescriptorSets[i];
				descriptorWrites[1].dstBinding = 1;
				descriptorWrites[1].dstArrayElement = 0;
				descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				descriptorWrites[1].descriptorCount = 1;
				descriptorWrites[1].pBufferInfo = &storageBufferInfoLastFrame;

				VkDescriptorBufferInfo storageBufferInfoCurrentFrame{};
				storageBufferInfoCurrentFrame.buffer = mShaderStorageBuffer[i].buffer;
				storageBufferInfoCurrentFrame.offset = 0;
				storageBufferInfoCurrentFrame.range = mShaderStorageBuffer[i].size;// *PARTICLE_COUNT; <<-- size/range already computed

				descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[2].dstSet = mDescriptorSets[i];
				descriptorWrites[2].dstBinding = 2;
				descriptorWrites[2].dstArrayElement = 0;
				descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				descriptorWrites[2].descriptorCount = 1;
				descriptorWrites[2].pBufferInfo = &storageBufferInfoCurrentFrame;

				VkDescriptorBufferInfo atmoic_spawn_counter_SSBO_info{};
				atmoic_spawn_counter_SSBO_info.buffer = mShaderEmitterSpawnCounterSSBO[i].buffer;
				atmoic_spawn_counter_SSBO_info.offset = 0;
				atmoic_spawn_counter_SSBO_info.range = mShaderEmitterSpawnCounterSSBO[i].size;// *PARTICLE_COUNT; <<-- size/range already computed

				descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[3].dstSet = mDescriptorSets[i];
				descriptorWrites[3].dstBinding = 3;
				descriptorWrites[3].dstArrayElement = 0;
				descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				descriptorWrites[3].descriptorCount = 1;
				descriptorWrites[3].pBufferInfo = &atmoic_spawn_counter_SSBO_info;

				vkUpdateDescriptorSets(vk_ctx->device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
			}


			return true;
		}
		

		bool ComputePass::CreatePipeline(const VulkanContext* vk_ctx, VkRenderPass& forward_render_pass, VkDescriptorSetLayout& camera_desc_set_layout)
		{
			GraphicsPipelineSpecification compute_spec{};
			compute_spec.bComputePipeline = true;
			compute_spec.computeShaderPath = "assets/shaders/spirvs/particle.comp.spv";
			//compute_spec.renderPass = mRenderPass; //<-- compute pipeline doesnt need renderPass handle

			//add required descriptor set layout & push constant 
			compute_spec.descriptorSetLayouts = { mComputeDescSetSpec.layoutHandle };
			mComputePipeline.Init(vk_ctx, compute_spec);

			GraphicsPipelineSpecification comp_gfx_spec{};

			comp_gfx_spec.vertexShaderPath = "assets/shaders/spirvs/particle.vert.spv";
			comp_gfx_spec.fragmentShaderPath = "assets/shaders/spirvs/particle.frag.spv";
			comp_gfx_spec.renderPass = forward_render_pass;

			comp_gfx_spec.bEnableAlphaBlend = true;
			//			description_set_spec.set = set;

			VkPushConstantRange push_constant_range{};
			push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			push_constant_range.offset = 0; //<- i am presumiing that this is used if there is more than one push constant
			push_constant_range.size = sizeof(GPUStructures::Model);

			comp_gfx_spec.pushConstantRanges = { push_constant_range };

			//VkDescriptorSetLayout dummy = Utils::CreateDummyDescriptorSetLayout(vk_ctx);
			VkDescriptorSetLayout dummy = vk_ctx->dummyDescSetLayout;

			comp_gfx_spec.descriptorSetLayouts = { dummy, camera_desc_set_layout };
			
			//comp_gfx_spec.rasterisationPolygonMode = VK_POLYGON_MODE_POINT;

			comp_gfx_spec.vertexBindingDescription = GPUStructures::Particle::GetBindingDescription();
			comp_gfx_spec.vertexAttribDesc = GPUStructures::Particle::GetAttributeDescriptionsAsVector();

			comp_gfx_spec.vertexInputPrimitiveTopology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;

			mComputeGfxPipeline.Init(vk_ctx, comp_gfx_spec);
			return true;
		}


		void ComputePass::Begin(VulkanContext* ctx, VkCommandBuffer compute_cmd_buff, uint32_t img_idx)
		{
			//bind gfxx pipeline 
			//mGfxPipeline.Bind(cmd_buff);
			vkCmdBindPipeline(compute_cmd_buff, VK_PIPELINE_BIND_POINT_COMPUTE, mComputePipeline.GetPipelineHandle());

			//Bind descriptor sets 
			mComputePipeline.BindComputeDescriptorSets(compute_cmd_buff, &mDescriptorSets[ctx->frameIndex]);
			
			
			//int TOTAL_WIDTH = ceil(sqrt(PARTICLE_COUNT));
			//int TOTAL_HEIGHT = ceil(PARTICLE_COUNT / TOTAL_WIDTH);

			//int workgroups_x = (TOTAL_WIDTH + 128 - 1) / 128;
			//int workgroups_y = (TOTAL_HEIGHT + 8 - 1) / 8;

			vkCmdDispatch(compute_cmd_buff, PARTICLE_COUNT / 256, 1, 1);
			//vkCmdDispatch(compute_cmd_buff, workgroups_x, workgroups_y, 1);
	
			//get emitted count from atomic counter 
			FLUX_ASSERT_GPUBUFFER_MAPPED(mShaderEmitterSpawnCounterSSBO[ctx->frameIndex]);
			int* counter_ptr = static_cast<int*>(mShaderEmitterSpawnCounterSSBO[ctx->frameIndex].mapped);
			int emitted = *counter_ptr;

			//compute carry over
			int carry_over_spawn = mEmitter.spawnCount - emitted;
			//printf("emitted, %d\n", emitted);
			//printf("Carry over spawn, %d\n", carry_over_spawn);
			//mEmitter.spawnCount = glm::max(0, carry_over_spawn);


			//Reset atomic counter 
			//FLUX_ASSERT_GPUBUFFER_MAPPED(mShaderEmitterSpawnCounterSSBO[curr_frame]);
			//int* counter_ptr = static_cast<int*>(mShaderEmitterSpawnCounterSSBO[curr_frame].mapped);
			counter_ptr[0] = 0;
		}


		void ComputePass::Cleanup(class VulkanContext* ctx)
		{

			mComputePipeline.Cleanup(ctx);
			mComputeGfxPipeline.Cleanup(ctx);


			for(auto& buffer : mShaderFrameTimeBuffer)
				GPUBuffer::DestoryBuffer(ctx->device, buffer);

			for (auto& buffer : mShaderStorageBuffer)
				GPUBuffer::DestoryBuffer(ctx->device, buffer);

			for(auto& buffer : mShaderEmitterSpawnCounterSSBO)
				GPUBuffer::DestoryBuffer(ctx->device, buffer);


			//DescriptorSetSpecification
			vkDestroyDescriptorSetLayout(ctx->device, mComputeDescSetSpec.layoutHandle, nullptr);

			vkDestroyDescriptorPool(ctx->device, mDescriptorPool, nullptr);
		}



	} //#Vulkan
}//#Fluxon
