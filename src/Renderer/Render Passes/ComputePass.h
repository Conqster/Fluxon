#pragma once
#include "Renderer/VulkanContext.h"
#include "Renderer/GPUBuffer.h"

#include "Renderer/Pipelines/GraphicsPipeline.h"

#include "Renderer/GPUStrutures.h"
#include <vector>

#include "Renderer/FrameResources.h"

namespace Fluxon {
	namespace Vulkan {



		/// <summary>
		/// Render Pass Defines what and where to render to 
		/// image/framebuffer attachements, formats, layout transitions 
		/// </summary>
	
		class ComputePass
		{
		private:

			const int PARTICLE_COUNT =  2 << 8/*13*/; //2^20 1,048,576
			//const int PARTICLE_COUNT = 131072; //2^20 1,048,576
			std::array<GPUBuffer, kMaxFramesInFlight> mShaderFrameTimeBuffer;
			std::array<GPUBuffer, kMaxFramesInFlight> mShaderStorageBuffer;

			std::array<GPUBuffer, kMaxFramesInFlight> mShaderEmitterSpawnCounterSSBO;

			GraphicsPipeline mComputePipeline;
			GraphicsPipeline mComputeGfxPipeline;

			DescriptorSetSpecification mComputeDescSetSpec;

			VkDescriptorPool mDescriptorPool;
			std::array<VkDescriptorSet, kMaxFramesInFlight> mDescriptorSets;

			std::vector<GPUStructures::Particle> mParticles;

		public:
			void Init(class VulkanContext* vk_ctx, VkCommandPool cmd_pool, VkRenderPass& forward_render_pass, VkDescriptorSetLayout& camera_desc_set_layout);
			void PreBeginFrameSync(uint32_t  curr_frame, double delta_time);
			void Begin(VulkanContext* ctx, VkCommandBuffer compute_cmd_buff, uint32_t img_idx);
			void Cleanup(class VulkanContext* ctx);
			~ComputePass() {

			}

			GPUStructures::EmitterUBO mEmitter;

			void HackRenderComputeParticle(VulkanContext* vk_ctx, VkCommandBuffer cmd_buff, const VkDescriptorSet* p_cam_desc_set);

		private:
			bool CreateDescriptorSetLayouts(const VulkanContext* vk_ctx);

			bool CreatePipeline(const VulkanContext* vk_ctx, VkRenderPass& forward_render_pass, VkDescriptorSetLayout& camera_desc_set_layout);
		};
	} //#Vulkan
}//#Fluxon
