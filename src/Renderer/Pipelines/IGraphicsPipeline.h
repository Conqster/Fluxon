#pragma once
#include <vulkan/vulkan.h>

#include <string>
#include <vector>

#include "Renderer/GPUStrutures.h"


namespace Fluxon {
	namespace Vulkan {

		struct IRenderPass;


		struct GraphicsPipelineSpecification
		{

			//TODO(Consqter): Later change base class to Pipeline
			// change "GraphicsPipelineSpecification" to "PipelineSpecification"
			//where Graphics & Compute Pipeline can inherit from ...
			//quick hack 
			bool bComputePipeline										= false;
			
			//leave paths empty if not available
			std::string vertexShaderPath;
			std::string fragmentShaderPath;
			std::string geometryShaderPath;

			std::string computeShaderPath;

			VkRenderPass renderPass										= VK_NULL_HANDLE;

			//pipelines layout requirement {descriptor set layout & push constant range} 
			std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
			std::vector<VkPushConstantRange> pushConstantRanges;


			/// <summary>
			/// Below are customisation for graphic pipeline 
			/// </summary>
			VkCullModeFlags rasterisationCullMode						= VK_CULL_MODE_BACK_BIT;
			VkFrontFace rasterisationFrontFace							= VK_FRONT_FACE_COUNTER_CLOCKWISE;
			VkPolygonMode rasterisationPolygonMode						= VK_POLYGON_MODE_FILL;

			//only support a single binding desc per vertex
			VkVertexInputBindingDescription vertexBindingDescription	= GPUStructures::Vertex::GetBindingDescription();
			std::vector<VkVertexInputAttributeDescription> vertexAttribDesc = GPUStructures::Vertex::GetAttributeDescriptionsAsVector();
			VkPrimitiveTopology vertexInputPrimitiveTopology			= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

			bool bHasColourAttachment									= true;
			bool bEnableAlphaBlend										= false;

			bool bEnableDepthTest										= true;
			bool bEnableDepthWrite										= true;
			//this clamp depth value within threshold [0, 1]
			bool bEnableDepthClamp										= false;
			VkCompareOp depthCompareOp									= VK_COMPARE_OP_LESS;

			bool bDepthBiasEnable										= false;
			float depthBiasConstantFactor								= 0.0f; // Optional
			float depthBiasClamp										= 0.0f; // Optional
			float depthBiasSlopeFactor									= 0.0f; // Optional

			//if dynamic viewport is false, set details for static viewport
			// if bDynamicViewport both viewport & scissor will be dynamic
			// later if needed separate viewport & scissor
			bool bDynamicViewport										= true;
			//if dynamic viewport is false, set details for static viewport
			//Only supports one viewport at the moment
			VkViewport viewportDetails;
			VkRect2D scissorDetails;
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