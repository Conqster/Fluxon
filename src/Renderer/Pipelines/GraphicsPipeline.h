#pragma once


#include "IGraphicsPipeline.h"

namespace Fluxon {
	namespace Vulkan {

		class GraphicsPipeline : public IGraphicsPipeline
		{
		private:
			VkPipelineLayout mLayout = VK_NULL_HANDLE;
			VkPipeline mPipeline = VK_NULL_HANDLE;

		public:
			GraphicsPipeline() {}

			void Init(const VulkanContext* ctx, GraphicsPipelineSpecification& gfx_pipeline_spec) override;
			void Bind(VkCommandBuffer cmd_buff) override;

			VkPipelineLayout GetLayoutHandle() const override { return mLayout; };
			VkPipeline GetPipelineHandle() const override { return mPipeline; };

			void BindDescriptorSets(VkCommandBuffer cmd_buff, const VkDescriptorSet* pdescriptor_sets, uint32_t first_set = 0, uint32_t set_count = 1) override {
				vkCmdBindDescriptorSets(cmd_buff, VK_PIPELINE_BIND_POINT_GRAPHICS, mLayout, first_set, set_count, pdescriptor_sets, 0, nullptr);
			}
			void PushConstant(VkCommandBuffer cmd_buff, VkShaderStageFlags stage_flag, uint32_t offset, uint32_t size, const void* pValues) override {
				vkCmdPushConstants(cmd_buff, mLayout, stage_flag, offset, size, pValues);
			}

			void Cleanup(class VulkanContext* ctx) override;

		private:
			VkShaderModule CreateShaderModule(const VulkanContext& vk_ctx, const std::vector<char>& code);

			void CreatePipelineLayout(const VulkanContext* ctx, GraphicsPipelineSpecification& gfx_pipeline_spec);
			void CreatePipeline(const VulkanContext* ctx, GraphicsPipelineSpecification& gfx_pipeline_spec);
		};

	} //#Vulkan
} //#Fluxon