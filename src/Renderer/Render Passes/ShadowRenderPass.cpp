#include "ShadowRenderPass.h"

#include "Renderer/VulkanUtilities.h"

#include "Renderer/Resources.h"

namespace Fluxon {
	namespace Vulkan {
		bool ShadowRenderPass::CreateGfxPipeline(VulkanContext* vk_ctx)
		{
			GraphicsPipelineSpecification gfx_specification{};
			gfx_specification.vertexShaderPath = "assets/shaders/spirvs/shadowDepth.vert.spv";
			gfx_specification.fragmentShaderPath;// = "assets/shaders/spirvs/shadowDepth.frag.spv";
			gfx_specification.renderPass = mRenderPass;

			//add required descriptor set layout & push constant 
			gfx_specification.descriptorSetLayouts = {mLightMatrixDescSetSpec.layoutHandle};

			VkPushConstantRange push_constant_range{};
			push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			push_constant_range.offset = 0; //<- i am presumiing that this is used if there is more than one push constant
			push_constant_range.size = sizeof(GPUStructures::Model);
			gfx_specification.pushConstantRanges = { push_constant_range };

			gfx_specification.rasterisationCullMode = VK_CULL_MODE_FRONT_BIT;
			gfx_specification.rasterisationFrontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

			gfx_specification.bHasColourAttachment = false;
			gfx_specification.bEnableAlphaBlend = false;

			gfx_specification.bEnableDepthTest = true;
			gfx_specification.bEnableDepthWrite = true;
			gfx_specification.bEnableDepthClamp = false;

			gfx_specification.depthCompareOp =  VK_COMPARE_OP_LESS;

			gfx_specification.bDepthBiasEnable = false;
			gfx_specification.depthBiasConstantFactor = 1.75f;	 // Optional
			gfx_specification.depthBiasClamp = 0.0f;			// Optional
			gfx_specification.depthBiasSlopeFactor = 1.25f;	   // Optional

			gfx_specification.bDynamicViewport = false;
			gfx_specification.viewportDetails.x = 0.0f;
			gfx_specification.viewportDetails.y = 0.0f;
			gfx_specification.viewportDetails.width = static_cast<float>(mExtent.width);
			gfx_specification.viewportDetails.height = static_cast<float>(mExtent.height);
			gfx_specification.viewportDetails.minDepth = 0.0f;
			gfx_specification.viewportDetails.maxDepth = 1.0f;
			gfx_specification.scissorDetails.offset = { 0, 0 };
			gfx_specification.scissorDetails.extent = mExtent;

			mGfxPipeline.Init(vk_ctx, gfx_specification);

			return true;
		}
		bool ShadowRenderPass::CreateLightDescriptorSetLayoutAndUBO(VulkanContext* vk_ctx)
		{
			//maybe later have a uniform buffer spec info that includes everything required for thes buffer & desc set
			//set 
			//binding
			//stage flag 
			mLightMatrixUBOSpec.type = GlobalUboType::Unknown; //<-- leave as unknown only used in config frame data
														  //resource not necessary here
			mLightMatrixUBOSpec.set = 0;
			mLightMatrixUBOSpec.binding = 0;
			mLightMatrixUBOSpec.requiredSize = sizeof(GPUStructures::MVP_UniformBufferObject);

			mLightMatrixDescSetSpec.set = mLightMatrixUBOSpec.set;
			VkDescriptorSetLayoutBinding layout_binding{};
			layout_binding.binding = mLightMatrixUBOSpec.binding;
			layout_binding.descriptorCount = 1;
			layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			layout_binding.pImmutableSamplers = nullptr;

			VkDescriptorSetLayoutCreateInfo layout_create_info{};
			layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layout_create_info.bindingCount = 1;
			layout_create_info.pBindings = &layout_binding;

			if (vkCreateDescriptorSetLayout(vk_ctx->device, &layout_create_info, nullptr, &mLightMatrixDescSetSpec.layoutHandle) != VK_SUCCESS)
				throw std::runtime_error("Failed to create descriptor set layout!!!!!!!!");


			//create uniform buffer 
			GPUBuffer::CreateBuffer(vk_ctx, static_cast<VkDeviceSize>(mLightMatrixUBOSpec.requiredSize), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				mLightMatrixUBO);

			GPUBuffer::MapBufferMemory(vk_ctx->device, mLightMatrixUBO);


			//descriptor pool
			VkDescriptorPoolSize ubo_pool_size{};
			ubo_pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			ubo_pool_size.descriptorCount = 1;
			VkDescriptorPoolCreateInfo pool_create_info{};
			pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			pool_create_info.poolSizeCount = 1;
			pool_create_info.pPoolSizes = &ubo_pool_size;
			pool_create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
			pool_create_info.maxSets = 1;

			if (vkCreateDescriptorPool(vk_ctx->device, &pool_create_info, nullptr, &mDescriptorPool) != VK_SUCCESS)
				throw std::runtime_error("Failed to Create Descriptor Pool!!!!!!!");


			VkDescriptorSetAllocateInfo alloc_info{};
			alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			alloc_info.descriptorPool = mDescriptorPool;
			alloc_info.descriptorSetCount = 1;
			alloc_info.pSetLayouts = &mLightMatrixDescSetSpec.layoutHandle;

			if (vkAllocateDescriptorSets(vk_ctx->device, &alloc_info, &mDescriptorSet) != VK_SUCCESS)
				throw std::runtime_error("Failed to allocate descriptor sets !!!!!!!");



			VkWriteDescriptorSet descriptor_writes;
			descriptor_writes.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptor_writes.pNext = nullptr;
			descriptor_writes.dstSet = mDescriptorSet;
			descriptor_writes.dstBinding = mLightMatrixUBOSpec.binding;
			descriptor_writes.dstArrayElement = 0;
			descriptor_writes.descriptorCount = 1;
			descriptor_writes.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			VkDescriptorBufferInfo desc_buff_info{};
			desc_buff_info.buffer = mLightMatrixUBO.buffer;
			desc_buff_info.range = mLightMatrixUBO.size;
			descriptor_writes.pBufferInfo = &desc_buff_info;


			vkUpdateDescriptorSets(vk_ctx->device, 1, &descriptor_writes, 0, nullptr);


			return true;
		}



		void ShadowRenderPass::Init(VulkanContext* vk_ctx, VkCommandPool cmd_pool)
		{
			VkFormat depth_format;
			//depth_format = Vulkan::Utils::FindSupportedFormat(ctx->physicalDevice,
			//	{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, //<<-- CANDIDATES FORMAT FOR DEPTH TESTING
			//	VK_IMAGE_TILING_OPTIMAL,
			//	VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

			
			depth_format = Utils::FindDepthFormat(vk_ctx->physicalDevice);
			///depth_format = VK_FORMAT_D16_UNORM;
			//Does not have colour attachment 
			VkAttachmentDescription depth_attachment{};
			depth_attachment.format = depth_format;
			depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
			depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

			depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			depth_attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			VkAttachmentReference depth_ref{};
			depth_ref.attachment = 0;
			depth_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			VkSubpassDescription subpass{};
			subpass.colorAttachmentCount = 0;
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpass.pDepthStencilAttachment = &depth_ref;

			// Use subpass dependencies for layout transitions
			std::array<VkSubpassDependency, 2> dependencies;

			dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
			dependencies[0].dstSubpass = 0;
			dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			dependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			dependencies[1].srcSubpass = 0;
			dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
			dependencies[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			dependencies[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			std::array<VkAttachmentDescription, 1> attachments = { depth_attachment };

			VkRenderPassCreateInfo renderpass_create_info{};
			renderpass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderpass_create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
			renderpass_create_info.pAttachments = attachments.data();
			renderpass_create_info.subpassCount = 1;
			renderpass_create_info.pSubpasses = &subpass;
			renderpass_create_info.dependencyCount = static_cast<uint32_t>(dependencies.size());
			renderpass_create_info.pDependencies = dependencies.data();


			if (vkCreateRenderPass(vk_ctx->device, &renderpass_create_info, nullptr, &mRenderPass) != VK_SUCCESS)
				throw std::runtime_error("Failed to create render pass!!!!!!");



			RenderTargetSpec rt_spec{};
			rt_spec.extent = mExtent;
			rt_spec.samplerBorderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
			if(!RenderTarget::CreateRenderTarget(vk_ctx, cmd_pool, mDepthRenderTarget, rt_spec))
				throw std::runtime_error("Failed to creaete Shadow pass render target attachment !!!!!!");


			//if (!mFramebuffer.Create(vk_ctx, mRenderPass, { mRenderTargetAttachment }))
			if (!Framebuffer::Create(vk_ctx, mRenderPass, mDepthFramebuffer, mExtent, {mDepthRenderTarget.imageView}))
			//if (!mFramebuffer.Create(vk_ctx, mRenderPass, mExtent, mRenderTargetAttachment.imageView))
				throw std::runtime_error("Failed to create Shadow pass Framebuffer");

			if(!CreateLightDescriptorSetLayoutAndUBO(vk_ctx))
				throw std::runtime_error("Failed to create Descriptor set and UBO!!!!!");

			if (!CreateGfxPipeline(vk_ctx))
				throw std::runtime_error("Failed to create GFX pipeline!!!!!");


		}
		void ShadowRenderPass::Begin(VulkanContext* ctx, VkCommandBuffer cmd_buff, uint32_t img_idx)
		{
			//maybe like a shadowConfig / shadowData
			//containing 
			//width 
			//height

			//image has alreaDy been transition on render target creation
			//VkImageMemoryBarrier barrier{};
			//barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			//barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			//barrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
			//barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			//barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			//barrier.image = mDepthRenderTarget.image;
			//barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			//barrier.subresourceRange.baseMipLevel = 0;
			//barrier.subresourceRange.levelCount = 1;
			//barrier.subresourceRange.baseArrayLayer = 0;
			//barrier.subresourceRange.layerCount = 1;
			//barrier.srcAccessMask = 0;
			//barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			//vkCmdPipelineBarrier(cmd_buff,
			//	VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
			//	0, 0, nullptr,
			//	0, nullptr,
			//	1, &barrier);

			//clear depth
			VkClearValue clear_depth_value;
			clear_depth_value.depthStencil = {1.0f, 0};

			VkRenderPassBeginInfo render_pass_info{};
			render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			render_pass_info.renderPass = mRenderPass;
			render_pass_info.pNext = nullptr;
			render_pass_info.framebuffer = mDepthFramebuffer.bufferHandle;
			render_pass_info.renderArea.offset = { 0, 0 };
			render_pass_info.renderArea.extent = mExtent;
			render_pass_info.clearValueCount = 1;
			render_pass_info.pClearValues = &clear_depth_value;

			vkCmdBeginRenderPass(cmd_buff, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

			////later support static viewport when all vaildations are fixed
			VkViewport viewport{};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = static_cast<float>(mExtent.width);
			viewport.height = static_cast<float>(mExtent.height);
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			vkCmdSetViewport(cmd_buff, 0, 1, &viewport);

			VkRect2D scissor{};
			scissor.offset = { 0, 0 };
			scissor.extent = mExtent;
			vkCmdSetScissor(cmd_buff, 0, 1, &scissor);

			vkCmdSetDepthBias(
				cmd_buff,
				depthBiasConstantFactor,
				depthBiasClamp,
				depthBiasSlopeFactor);

			//bind gfxx pipeline 
			mGfxPipeline.Bind(cmd_buff);

			//Bind descriptor sets Light matrix 
			mGfxPipeline.BindDescriptorSets(cmd_buff, &mDescriptorSet);
		}

		void ShadowRenderPass::End(VkCommandBuffer cmd_buff)
		{
			vkCmdEndRenderPass(cmd_buff);
		}

		void ShadowRenderPass::Cleanup(VulkanContext* ctx)
		{
			vkDestroyRenderPass(ctx->device, mRenderPass, nullptr);


			mGfxPipeline.Cleanup(ctx);

			GPUBuffer::DestoryBuffer(ctx->device, mLightMatrixUBO);
			RenderTarget::DestoryRenderTraget(ctx->device, mDepthRenderTarget);

			Framebuffer::DestroyFramebuffer(ctx->device, mDepthFramebuffer);

			//DescriptorSetSpecification
			vkDestroyDescriptorSetLayout(ctx->device, mLightMatrixDescSetSpec.layoutHandle, nullptr);
			
			vkDestroyDescriptorPool(ctx->device, mDescriptorPool, nullptr);
		}








} //namespace Vulkan 

} //namespace Fluxon



