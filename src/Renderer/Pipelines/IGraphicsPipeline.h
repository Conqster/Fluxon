#pragma once
#include <vulkan/vulkan.h>

#include <string>
#include <vector>


namespace Fluxon {
	namespace Vulkan {

		struct IRenderPass;

		struct GraphicsPipelineSpecification
		{
			std::string vertexShaderPath;
			std::string fragmentShaderPath;

			IRenderPass* renderPass = nullptr;

			//pipelines layout requirement {descriptor set layout & push constant range} 
			std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
			std::vector<VkPushConstantRange> pushConstantRanges;


			VkCullModeFlags rasterisationCullMode = VK_CULL_MODE_BACK_BIT;
			VkFrontFace rasterisationFrontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

			bool enableDepthTest = true;
			bool enableDepthWrite = true;
			bool enableAlphaBlend = false;
			VkCompareOp depthCompareOp = VK_COMPARE_OP_LESS;
		};

		struct VulkanContext;

		class IGraphicsPipeline
		{
		public:

			virtual void Init(const VulkanContext* ctx, GraphicsPipelineSpecification& gfx_pipeline_spec) = 0;
			virtual void Bind(VkCommandBuffer cmd_buff) = 0;

			virtual VkPipelineLayout GetLayoutHandle() const = 0;
			virtual VkPipeline GetPipelineHandle() const = 0;

			virtual void BindDescriptorSets(VkCommandBuffer cmd_buff, const VkDescriptorSet* pdescriptor_sets, uint32_t first_set = 0, uint32_t set_count = 1) = 0;
			virtual void PushConstant(VkCommandBuffer cmd_buff, VkShaderStageFlags stage_flag, uint32_t offset, uint32_t size, const void* pValues) = 0;

			virtual void Cleanup(class VulkanContext* ctx) = 0;
			virtual ~IGraphicsPipeline() = default;
		};

	} //#vulkan
}//#Fluxon