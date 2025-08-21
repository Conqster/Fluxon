#include "GraphicsPipeline.h"

#include "Core/Utilities.h"

#include "Renderer/VulkanContext.h"
#include "Renderer/Render Passes/IRenderPass.h"

namespace Fluxon {
	namespace Vulkan {

		void GraphicsPipeline::Init(const VulkanContext* ctx, GraphicsPipelineSpecification& gfx_pipeline_spec)
		{
			CreatePipelineLayout(ctx, gfx_pipeline_spec);
			CreatePipeline(ctx, gfx_pipeline_spec);
		}

		void GraphicsPipeline::Bind(VkCommandBuffer cmd_buff)
		{
			vkCmdBindPipeline(cmd_buff, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);
		}

		void GraphicsPipeline::Cleanup(VulkanContext* ctx)
		{
			vkDestroyPipeline(ctx->device, mPipeline, nullptr);
			vkDestroyPipelineLayout(ctx->device, mLayout, nullptr);
		}

		VkShaderModule GraphicsPipeline::CreateShaderModule(const VulkanContext& vk_ctx, const std::vector<char>& code)
		{
			VkShaderModuleCreateInfo shader_module_create_info{};
			shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			shader_module_create_info.codeSize = code.size();
			shader_module_create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

			VkShaderModule shader_module;
			if (vkCreateShaderModule(vk_ctx.device, &shader_module_create_info, nullptr, &shader_module) != VK_SUCCESS)
				throw std::runtime_error("Failed to create shader module!!!!!!");
			return shader_module;
		}

		void GraphicsPipeline::CreatePipelineLayout(const VulkanContext* ctx, GraphicsPipelineSpecification& gfx_pipeline_spec)
		{

			VkPipelineLayoutCreateInfo pipeline_layout_create_info{};
			pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipeline_layout_create_info.setLayoutCount = static_cast<uint32_t>(gfx_pipeline_spec.descriptorSetLayouts.size());
			pipeline_layout_create_info.pSetLayouts = gfx_pipeline_spec.descriptorSetLayouts.data(); //<--- ubo decription layout sample MVP UBO 

			pipeline_layout_create_info.pushConstantRangeCount = static_cast<uint32_t>(gfx_pipeline_spec.pushConstantRanges.size());
			pipeline_layout_create_info.pPushConstantRanges = gfx_pipeline_spec.pushConstantRanges.data();

			if (vkCreatePipelineLayout(ctx->device, &pipeline_layout_create_info, nullptr, &mLayout) != VK_SUCCESS)
				throw std::runtime_error("Failed to create pipelines layout!!!!");
		}

		void GraphicsPipeline::CreatePipeline(const VulkanContext* ctx, GraphicsPipelineSpecification& gfx_pipeline_spec)
		{
#pragma region vk_SHADER_STAGE_CREATION_INFO
			std::vector<VkPipelineShaderStageCreateInfo> shader_stages;
			//cache shader module oto keep alive for pipelines construction 
			std::vector<VkShaderModule> created_shader_modules;
			if (!gfx_pipeline_spec.vertexShaderPath.empty())
			{
				auto vert_shader_code = Fluxon::Utils::ReadFile(gfx_pipeline_spec.vertexShaderPath);
				//VkShaderModule vert_shader_module = CreateShaderModule(*ctx, vert_shader_code);
				created_shader_modules.push_back(CreateShaderModule(*ctx, vert_shader_code));

				VkPipelineShaderStageCreateInfo vert_shader_stage_info{};
				vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
				vert_shader_stage_info.module = created_shader_modules.back();
				vert_shader_stage_info.pName = "main";
				shader_stages.push_back(vert_shader_stage_info);
			}

			if (!gfx_pipeline_spec.fragmentShaderPath.empty())
			{
				auto frag_shader_code = Fluxon::Utils::ReadFile(gfx_pipeline_spec.fragmentShaderPath);
				//VkShaderModule frag_shader_module = CreateShaderModule(*ctx, frag_shader_code);
				created_shader_modules.push_back(CreateShaderModule(*ctx, frag_shader_code));

				VkPipelineShaderStageCreateInfo frag_shader_stage_info{};
				frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
				//frag_shader_stage_info.module = frag_shader_module;
				frag_shader_stage_info.module = created_shader_modules.back();
				frag_shader_stage_info.pName = "main";

				shader_stages.push_back(frag_shader_stage_info);
			}

			if (!gfx_pipeline_spec.geometryShaderPath.empty())
			{
				auto geom_shader_code = Fluxon::Utils::ReadFile(gfx_pipeline_spec.geometryShaderPath);
				created_shader_modules.push_back(CreateShaderModule(*ctx, geom_shader_code));

				VkPipelineShaderStageCreateInfo geom_shader_stage_info{};
				geom_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				geom_shader_stage_info.pName = "main";
				geom_shader_stage_info.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
				geom_shader_stage_info.module = created_shader_modules.back();

				shader_stages.push_back(geom_shader_stage_info);
			}

#pragma endregion //#vk_SHADER_STAGE_CREATION_INFO

			//VERTEX INPUT
#pragma region vk_VERTEX_INPUT
			VkPipelineVertexInputStateCreateInfo vertex_input_create_info{};
			vertex_input_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			//auto binding_description = GPUStructures::Vertex::GetBindingDescription();
			//auto attribute_description = GPUStructures::Vertex::GetAttributeDescriptions();
			//vertex_input_create_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_description.size());
			//vertex_input_create_info.pVertexBindingDescriptions = &binding_description;
			//vertex_input_create_info.pVertexAttributeDescriptions = attribute_description.data();
			vertex_input_create_info.vertexBindingDescriptionCount = 1;
			vertex_input_create_info.pVertexBindingDescriptions = &gfx_pipeline_spec.vertexBindingDescription;
			vertex_input_create_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(gfx_pipeline_spec.vertexAttribDesc.size());
			vertex_input_create_info.pVertexAttributeDescriptions = gfx_pipeline_spec.vertexAttribDesc.data();
#pragma endregion //#vk_VERTEX_INPUT

			//INPUT ASSRMBLY
#pragma region vk_INPUT_ASSEMBLY
			VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info{};
			input_assembly_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			input_assembly_create_info.topology = gfx_pipeline_spec.vertexInputPrimitiveTopology;
			input_assembly_create_info.primitiveRestartEnable = VK_FALSE;
#pragma endregion //#vk_INPUT_ASSEMBLY


			//RASTERISER
#pragma region vk_RASTERISER
		/// <summary>
		/// Rasteriser takes the geometry shaped by the vertices from verytex shader 
		/// into fragment to be coloured (by fragment shader) 
		/// performs (depth testing, face culling and the scissor test)
		/// </summary>
			VkPipelineRasterizationStateCreateInfo rasteriser_create_info{};
			rasteriser_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasteriser_create_info.depthClampEnable = static_cast<VkBool32>(gfx_pipeline_spec.bEnableDepthClamp);
			rasteriser_create_info.rasterizerDiscardEnable = VK_FALSE;
			rasteriser_create_info.polygonMode = gfx_pipeline_spec.rasterisationPolygonMode;
			rasteriser_create_info.lineWidth = 1.0f;
			rasteriser_create_info.cullMode = gfx_pipeline_spec.rasterisationCullMode;
			rasteriser_create_info.frontFace = gfx_pipeline_spec.rasterisationFrontFace;
			rasteriser_create_info.depthBiasEnable = static_cast<VkBool32>(gfx_pipeline_spec.bDepthBiasEnable);
			rasteriser_create_info.depthBiasConstantFactor = gfx_pipeline_spec.depthBiasConstantFactor;
			rasteriser_create_info.depthBiasClamp = gfx_pipeline_spec.depthBiasClamp;
			rasteriser_create_info.depthBiasSlopeFactor = gfx_pipeline_spec.depthBiasSlopeFactor;
#pragma endregion //#vk_RASTERISER

			//MULTISAMPLING
#pragma region vk_MULTISAMPLING
			VkPipelineMultisampleStateCreateInfo multisampling_create_info{};
			multisampling_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisampling_create_info.sampleShadingEnable = VK_FALSE;
			multisampling_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
			multisampling_create_info.minSampleShading = 1.0f;
			multisampling_create_info.pSampleMask = nullptr;
			multisampling_create_info.alphaToCoverageEnable = VK_FALSE;
			multisampling_create_info.alphaToOneEnable = VK_FALSE;
#pragma endregion //#vk_MULTISAMPLING

			//DEPTH & STENCIL TESTING
#pragma region vk_DEPTH_STENCIL_STATE
			VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info{};
			depth_stencil_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			depth_stencil_state_create_info.depthTestEnable = static_cast<VkBool32>(gfx_pipeline_spec.bEnableDepthTest);
			depth_stencil_state_create_info.depthWriteEnable = static_cast<VkBool32>(gfx_pipeline_spec.bEnableDepthWrite);
			depth_stencil_state_create_info.depthCompareOp = gfx_pipeline_spec.depthCompareOp;
			depth_stencil_state_create_info.depthBoundsTestEnable = VK_FALSE;
			depth_stencil_state_create_info.minDepthBounds = 0.0f; // Optional
			depth_stencil_state_create_info.maxDepthBounds = 1.0f; // Optional
			depth_stencil_state_create_info.stencilTestEnable = VK_FALSE;
			depth_stencil_state_create_info.front = {}; // Optional
			depth_stencil_state_create_info.back = {}; // Optional
#pragma endregion //#vk_DEPTH_STENCIL_STATE


			//COLOUR BLENDING
#pragma region vk_COLOUR_BLEND_ATTACHMENT
			VkPipelineColorBlendAttachmentState colour_blend_attachement{};
			if (gfx_pipeline_spec.bHasColourAttachment)
			{
				colour_blend_attachement.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
					VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

				colour_blend_attachement.blendEnable = static_cast<VkBool32>(gfx_pipeline_spec.bEnableAlphaBlend);
				if (!gfx_pipeline_spec.bEnableAlphaBlend)
				{
					colour_blend_attachement.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
					colour_blend_attachement.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
					colour_blend_attachement.colorBlendOp = VK_BLEND_OP_ADD; // Optional
					colour_blend_attachement.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
					colour_blend_attachement.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
					colour_blend_attachement.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
				}
				else
				{
					colour_blend_attachement.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
					colour_blend_attachement.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
					colour_blend_attachement.colorBlendOp = VK_BLEND_OP_ADD;
					colour_blend_attachement.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
					//colour_blend_attachement.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
					colour_blend_attachement.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
					colour_blend_attachement.alphaBlendOp = VK_BLEND_OP_ADD;
				}
			}

			VkPipelineColorBlendStateCreateInfo colour_blending_create_info{};
			colour_blending_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			colour_blending_create_info.logicOpEnable = VK_FALSE;// static_cast<VkBool32>(gfx_pipeline_spec.bEnableAlphaBlend);
			if (!gfx_pipeline_spec.bEnableAlphaBlend)
				colour_blending_create_info.logicOp = VK_LOGIC_OP_COPY; //optional since its not enabled
			else
				colour_blending_create_info.logicOp = VK_LOGIC_OP_COPY;
			if (gfx_pipeline_spec.bHasColourAttachment)
			{
				colour_blending_create_info.attachmentCount = 1;
				colour_blending_create_info.pAttachments = &colour_blend_attachement;
			}
			else
			{
				colour_blending_create_info.attachmentCount = 0;
				colour_blending_create_info.pAttachments = nullptr;
			}
			colour_blending_create_info.blendConstants[0] = 0.0f; //optional
			colour_blending_create_info.blendConstants[1] = 0.0f; //optional
			colour_blending_create_info.blendConstants[2] = 0.0f; //optional
			colour_blending_create_info.blendConstants[3] = 0.0f; //optional
#pragma endregion //#vk_COLOUR_BLEND_ATTACHMENT

			//DYNAMIC STATE
#pragma region vk_VIEWPORT_SCISSOR
			
			VkPipelineDynamicStateCreateInfo dynamic_state_create_info{}; //<--- dynamic state
			std::vector<VkDynamicState> dynamic_states = {
				VK_DYNAMIC_STATE_VIEWPORT,
				VK_DYNAMIC_STATE_SCISSOR,
			};

			if (gfx_pipeline_spec.bDepthBiasEnable)
				dynamic_states.push_back(VK_DYNAMIC_STATE_DEPTH_BIAS);
			
			dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			dynamic_state_create_info.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
			if (gfx_pipeline_spec.bDynamicViewport)
				dynamic_state_create_info.pDynamicStates = dynamic_states.data();
			else
				dynamic_state_create_info.pDynamicStates = nullptr;

			VkPipelineViewportStateCreateInfo viewport_state_create_info{};

			viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewport_state_create_info.viewportCount = 1;
			viewport_state_create_info.pViewports = &gfx_pipeline_spec.viewportDetails;
			viewport_state_create_info.scissorCount = 1;
			viewport_state_create_info.pScissors = &gfx_pipeline_spec.scissorDetails;
#pragma endregion //#vk_VIEWPORT_SCISSOR


			if (!gfx_pipeline_spec.bComputePipeline)
			{

				VkGraphicsPipelineCreateInfo pipeline_create_info{};
				pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
				pipeline_create_info.stageCount = static_cast<uint32_t>(shader_stages.size());
				pipeline_create_info.pStages = shader_stages.data();

				pipeline_create_info.pVertexInputState = &vertex_input_create_info;
				pipeline_create_info.pInputAssemblyState = &input_assembly_create_info;
				pipeline_create_info.pRasterizationState = &rasteriser_create_info;
				pipeline_create_info.pMultisampleState = &multisampling_create_info;
				//pipeline_create_info.pDepthStencilState = nullptr;
				pipeline_create_info.pColorBlendState = &colour_blending_create_info;
				pipeline_create_info.pDepthStencilState = &depth_stencil_state_create_info;

				if (gfx_pipeline_spec.bDynamicViewport)
					pipeline_create_info.pDynamicState = &dynamic_state_create_info;
				pipeline_create_info.pViewportState = &viewport_state_create_info;

				pipeline_create_info.layout = mLayout;
				pipeline_create_info.renderPass = gfx_pipeline_spec.renderPass;
				pipeline_create_info.subpass = 0;

				pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
				pipeline_create_info.basePipelineIndex = -1;

				if (vkCreateGraphicsPipelines(ctx->device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &mPipeline) != VK_SUCCESS)
					throw std::runtime_error("Failed to Create Graphics Pipeline!!!!!");

				//delete all cached shader modules 
				for (auto& shader_module : created_shader_modules)
					vkDestroyShaderModule(ctx->device, shader_module, nullptr);
			}
			else
			{
				auto comp_shader_code = Fluxon::Utils::ReadFile(gfx_pipeline_spec.computeShaderPath);
				VkShaderModule compute_shader_modules = CreateShaderModule(*ctx, comp_shader_code);

				VkPipelineShaderStageCreateInfo comp_shader_stage_info{};
				comp_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				comp_shader_stage_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
				comp_shader_stage_info.module = compute_shader_modules;
				comp_shader_stage_info.pName = "main";

				VkComputePipelineCreateInfo pipelineInfo{};
				pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
				pipelineInfo.layout = mLayout;
				pipelineInfo.stage = comp_shader_stage_info;

				if (vkCreateComputePipelines(ctx->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mPipeline) != VK_SUCCESS) {
					throw std::runtime_error("failed to create compute pipeline!");
				}

				vkDestroyShaderModule(ctx->device, compute_shader_modules, nullptr);
			}
		}
	} //#Vulkan
}//#Fluxon

