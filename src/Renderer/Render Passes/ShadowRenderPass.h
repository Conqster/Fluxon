#pragma once


#include "IRenderPass.h"
#include <vulkan/vulkan.h>

#include "Renderer/Pipelines/GraphicsPipeline.h"

#include "Renderer/FrameResources.h"
#include "Renderer/RenderTarget.h"


#include "Renderer/VulkanDescriptorSet.h"

namespace Fluxon {
	namespace Vulkan {

		class ShadowRenderPass : public IRenderPass
		{
		private:
			VkRenderPass mRenderPass;
			VkExtent2D mExtent{ 2048, 2048 };
			//VkExtent2D mExtent{ 1024, 1024};
			//VkExtent2D mExtent{ 1920, 1080 };
			
			GraphicsPipeline mGfxPipeline;

			//UniformBufferSpecification mLightMatrixUBOSpec;
			//GPUBuffer mLightMatrixUBO;

			//DescriptorSetSpecification mLightMatrixDescSetSpec;

			///VkDescriptorPool mDescriptorPool;
			//VkDescriptorSet mDescriptorSet;

			VulkanDescriptorSet mNewDescriptorSet;
		
			//GPUStructures::MVP_UniformBufferObject mLightMatrixUBOData;
		public:
			void Init(class VulkanContext* vk_ctx, VkCommandPool cmd_pool);
			void Init(class VulkanContext* vk_ctx) override {
				printf("{void Init(class VulkanContext* vk_ctx)} not supported by ShadowRenderPass Init \n use {void Init(class VulkanContext* vk_ctx, VkCommandPool cmd_pool)}\n");
			};

			float depthBiasConstantFactor = 1.75f; // Optional
			float depthBiasClamp = 0.0f; // Optional
			float depthBiasSlopeFactor = 1.25f; // Optional
			VkRenderPass& GetRenderPassHandle() { return mRenderPass; }
			RenderTarget& GetShadowDepthRenderTarget() { return mDepthRenderTarget; }

			DescriptorBinding GetSamplerBinding(uint32_t set, uint32_t binding)
			{
				Resources::ImageSampler img_sampler{
					mDepthRenderTarget.image,
					mDepthRenderTarget.memory,
					mDepthRenderTarget.imageView, 
					mDepthRenderTarget.sampler
				};

				return DescriptorBinding::MakeImageSampler(set, binding, img_sampler);
			}

			void Begin(VulkanContext* ctx, VkCommandBuffer cmd_buff, uint32_t img_idx) override;
			void UpdateLightMatrixBuffer(const glm::mat4 view_data, const glm::mat4 proj_data, uint32 frame_idx) {
				GPUStructures::MVP_UniformBufferObject data;
				data.view = view_data;
				data.projection = proj_data;

				mNewDescriptorSet.UpdateUBOBinding("light_matrix::light_matrix", &data, frame_idx);
				//mNewDescriptorSet.SetUniformbuffer("light_matrix", &data, 1);
				//mLightMatrixUBOData.view = view_data;
				//mLightMatrixUBOData.projection = proj_data;
				//Vulkan::GPUBuffer::CopyToMappedMemory(mLightMatrixUBO, &mLightMatrixUBOData, sizeof(GPUStructures::MVP_UniformBufferObject));
			}

			void UpdateLightMoveBuffer(const glm::vec4 _move, uint32 frame_idx) {
				glm::vec4 data = _move;

				mNewDescriptorSet.UpdateUBOBinding("light_matrix::move", &data, frame_idx);
			}
			void PushModelTransformConstant(VkCommandBuffer cmd_buff, const GPUStructures::Model* p_value) {
				mGfxPipeline.PushConstant(cmd_buff, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPUStructures::Model), p_value);
			}
			void End(VkCommandBuffer cmd_buff) override;

			void Cleanup(VulkanContext* ctx) override;

		private:

			bool CreateGfxPipeline(VulkanContext* vk_ctx);
			bool CreateLightDescriptorSetLayoutAndUBO(VulkanContext* vk_ctx);



			RenderTarget mDepthRenderTarget;
			Framebuffer  mDepthFramebuffer;


		};
	} //namspace Vulkan
} //namespace Fluxon






