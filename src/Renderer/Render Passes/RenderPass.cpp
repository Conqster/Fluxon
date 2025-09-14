#include "RenderPass.h"


#include "Renderer/VulkanUtilities.h"


#include <glm/glm.hpp>

namespace Fluxon {
	namespace Vulkan {
		
		void RenderPass::Init(class VulkanContext* vk_ctx, const RenderPassSpecification& spec)
		{
			uint32 attachment_count = spec.attachmentSpecifications.size();
			std::vector<VkAttachmentDescription> attachments(attachment_count);
			std::vector<VkAttachmentReference> col_attachment_refs;
			std::vector<VkAttachmentReference> depth_attachment_refs;

			std::vector<VkImageView> cache_render_target_imgview(attachment_count);


			mRenderTargets.resize(attachment_count);
			mClearValues.resize(attachment_count);

			VkExtent2D frame_extent = { 0, 0 };

			if (spec.unifiedRenderTargetSize.has_value())
				frame_extent = spec.unifiedRenderTargetSize.value();

			for (uint32 i  = 0; i < attachment_count; i++)
			{
				auto& curr_attachment = attachments[i];
				auto& curr_attachment_spec = spec.attachmentSpecifications[i];

				curr_attachment.format = curr_attachment_spec.format;
				curr_attachment.samples = curr_attachment_spec.samples;
				curr_attachment.loadOp = curr_attachment_spec.loadOp;
				curr_attachment.storeOp = curr_attachment_spec.storeOp;
				curr_attachment.stencilLoadOp = curr_attachment_spec.stencilLoadOp;
				curr_attachment.stencilStoreOp = curr_attachment_spec.stencilStoreOp;
				curr_attachment.initialLayout = curr_attachment_spec.initialLayout;
				curr_attachment.finalLayout = curr_attachment_spec.finalLayout;

				//attachment ref
				if (curr_attachment_spec.attachType == EAttachmentType::Colour)
				{
					VkAttachmentReference colour_attachment_ref{};
					colour_attachment_ref.attachment = i;
					colour_attachment_ref.layout = curr_attachment_spec.referenceLayout;
					col_attachment_refs.push_back(colour_attachment_ref);
				}
				else
				{
					VkAttachmentReference depth_attachment_ref{};
					depth_attachment_ref.attachment = i;
					depth_attachment_ref.layout = curr_attachment_spec.referenceLayout;
					depth_attachment_refs.push_back(depth_attachment_ref);
				}
				

				//Render target
				if (curr_attachment_spec.bHasRenderTarget)
				{
					RenderTargetSpec rt_spec{};
					rt_spec.extent = curr_attachment_spec.extent;
					rt_spec.format = curr_attachment_spec.format;
					rt_spec.arrayLayers = curr_attachment_spec.arrayLayers;
					rt_spec.targetMask = curr_attachment_spec.targetMask;
					rt_spec.explicitUsage = curr_attachment_spec.explicitUsage;
					rt_spec.imageTiling = curr_attachment_spec.imageTiling;
					rt_spec.imageAspect = curr_attachment_spec.imageAspect;
					rt_spec.bEnableSamplerAnisotropy = curr_attachment_spec.bEnableSamplerAnisotropy;
					rt_spec.samplerFilterMode = curr_attachment_spec.samplerFilterMode;
					rt_spec.samplerAddressMode = curr_attachment_spec.samplerAddressMode;
					rt_spec.samplerBorderColor = curr_attachment_spec.samplerBorderColor;

					frame_extent.width = glm::max(frame_extent.width, curr_attachment_spec.extent.width);
					frame_extent.height = glm::max(frame_extent.height, curr_attachment_spec.extent.height);

					if(RenderTarget::CreateRenderTarget(vk_ctx, vk_ctx->transientCmdPool, mRenderTargets[i], rt_spec))
						std::runtime_error("Failed to creaete render target attachment!!!!!!");

					cache_render_target_imgview[i] = mRenderTargets[i].imageView;
				}


				mClearValues[i] = curr_attachment_spec.clearValue;
			}




			VkSubpassDescription subpass{};
			subpass.pipelineBindPoint = spec.subpassPipelineBindpoint;
			subpass.colorAttachmentCount = col_attachment_refs.size();
			subpass.pColorAttachments = col_attachment_refs.data();
			subpass.pDepthStencilAttachment = depth_attachment_refs.data();


			VkSubpassDependency dependency{};
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;

			dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependency.srcAccessMask = 0;

			dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;


			//Render pass 
			VkRenderPassCreateInfo renderpass_create_info{};

			renderpass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderpass_create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
			renderpass_create_info.pAttachments = attachments.data();
			renderpass_create_info.subpassCount = 1;
			renderpass_create_info.pSubpasses = &subpass;
			renderpass_create_info.dependencyCount = 1;
			renderpass_create_info.pDependencies = &dependency;

			if (vkCreateRenderPass(vk_ctx->device, &renderpass_create_info, nullptr, &mRenderPass) != VK_SUCCESS)
				throw std::runtime_error("Failed to create render pass!!!!!!");


			if (mRenderTargets.size() > 0)
			{

				if (!Framebuffer::Create(vk_ctx, mRenderPass, mFramebuffer, frame_extent, cache_render_target_imgview))
					throw std::runtime_error("Failed to create forward pass Framebuffer");
			}

		}



		void RenderPass::OnResize(VulkanContext* vk_ctx, VkCommandPool cmd_pool, uint32 width, uint32 height)
		{
			std::vector<VkImageView> cache_render_target_imgview(mRenderTargets.size());
			for(size_t i = 0; i < mRenderTargets.size(); i++)
			{
				auto& rt = mRenderTargets[i];
				rt.OnResize(vk_ctx, cmd_pool, width, height);
				cache_render_target_imgview[i] = rt.imageView;
			}

			if(mDepthRenderTarget.has_value())
			{
				mDepthRenderTarget.value().OnResize(vk_ctx, cmd_pool, width, height);
				cache_render_target_imgview.emplace_back(mDepthRenderTarget.value().imageView);
			}

			mFramebuffer.OnResize(vk_ctx, mRenderPass, cache_render_target_imgview, width, height);
		}

		void RenderPass::Begin(/*const VulkanCommandBuffer&*/VkCommandBuffer cmd_buff)
		{
			VkExtent2D frame_extent = mFramebuffer.extent;
			VkRenderPassBeginInfo render_pass_info{};
			render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			render_pass_info.renderPass = mRenderPass;
			render_pass_info.framebuffer = mFramebuffer.bufferHandle;
			render_pass_info.renderArea.offset = { 0, 0 };
			render_pass_info.renderArea.extent = frame_extent;
			render_pass_info.clearValueCount = static_cast<uint32_t>(mClearValues.size());
			render_pass_info.pClearValues = mClearValues.data();

			vkCmdBeginRenderPass(cmd_buff/*.GetCommandHandle()*/, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);


			//might remove this as it could be static or dynmacic
			//and if dynamic it might a choose to chenge dynamiucally 
			//per obj etc
			VkViewport viewport{};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = static_cast<float>(frame_extent.width);
			viewport.height = static_cast<float>(frame_extent.height);
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			vkCmdSetViewport(cmd_buff/*.GetCommandHandle()*/, 0, 1, &viewport);

			VkRect2D scissor{};
			scissor.offset = { 0, 0 };
			scissor.extent = frame_extent;
			vkCmdSetScissor(cmd_buff/*.GetCommandHandle()*/, 0, 1, &scissor);
		}


		void RenderPass::End(/*const VulkanCommandBuffer&*/VkCommandBuffer cmd_buff)
		{
			vkCmdEndRenderPass(cmd_buff/*.GetCommandHandle()*/);
		}


		void RenderPass::Cleanup(VulkanContext* vk_ctx)
		{
			Framebuffer::DestroyFramebuffer(vk_ctx->device, mFramebuffer);

			for (auto& rt : mRenderTargets)
				RenderTarget::DestoryRenderTraget(vk_ctx->device, rt);

			if (mDepthRenderTarget.has_value())
				RenderTarget::DestoryRenderTraget(vk_ctx->device, mDepthRenderTarget.value());

			if(mRenderPass != VK_NULL_HANDLE)
				vkDestroyRenderPass(vk_ctx->device, mRenderPass, nullptr);
		}

		RenderTarget& RenderPass::GetOutputRT(uint32 idx)
		{
			FLUX_DEBUG_ASSERT(idx < mRenderTargets.size(), "Trying to retrive Rendepass output outside bounds");

			idx = glm::min(idx, static_cast<uint32>(mRenderTargets.size() - 1));

			return mRenderTargets[idx];
		}

		const std::optional<RenderTarget>& RenderPass::GetDepthOutputRT() const
		{
			return mDepthRenderTarget;
		}



		} //#Vulkan
}//#Fluxon

		Fluxon::Vulkan::RenderPassSpecification Fluxon::Vulkan::GenerateForwardPassSpecification(VulkanContext* vk_ctx, uint32 width, uint32 height)
		{
			Vulkan::SwapchainSupportDetails swap_chain_support = Utils::QuerySwapchainSupport(vk_ctx->physicalDevice, vk_ctx->surface);
			VkSurfaceFormatKHR surface_format = Vulkan::Swapchain::ChooseSwapSurfaceFormat(swap_chain_support.formats);

			RenderPassAttachmentSpecification col_attachment_spec{};
			col_attachment_spec.format = surface_format.format;

			col_attachment_spec.clearValue.color = { 0.0f, 0.0f, 0.0f, 1.0f };
			col_attachment_spec.samples = VK_SAMPLE_COUNT_1_BIT;
			col_attachment_spec.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			col_attachment_spec.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			col_attachment_spec.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			col_attachment_spec.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			col_attachment_spec.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			col_attachment_spec.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			//attachment ref attachment idx is determined on creation
			col_attachment_spec.referenceLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			//used to resolve subpass attachments
			col_attachment_spec.attachType = EAttachmentType::Colour;

			//for unification render target info i.e image for offscreen rendering
			col_attachment_spec.bHasRenderTarget = true;
			col_attachment_spec.extent = {width, height};
			col_attachment_spec.arrayLayers = 1; //if using array as depth

			col_attachment_spec.targetMask = ERenderTargetBits::Write | ERenderTargetBits::Sample | ERenderTargetBits::ImGuiView;
			col_attachment_spec.explicitUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

			col_attachment_spec.imageTiling = VK_IMAGE_TILING_OPTIMAL;
			col_attachment_spec.imageAspect = VK_IMAGE_ASPECT_COLOR_BIT;

			col_attachment_spec.bEnableSamplerAnisotropy = false;
			col_attachment_spec.samplerFilterMode = VK_FILTER_NEAREST;
			col_attachment_spec.samplerAddressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
			col_attachment_spec.samplerBorderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;


			RenderPassAttachmentSpecification depth_attachment_spec{};
			depth_attachment_spec.format = surface_format.format;

			depth_attachment_spec.format = Utils::FindDepthFormat(vk_ctx->physicalDevice);;
			depth_attachment_spec.clearValue.depthStencil = { 1.0f };
			depth_attachment_spec.samples = VK_SAMPLE_COUNT_1_BIT;
			depth_attachment_spec.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depth_attachment_spec.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			depth_attachment_spec.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			depth_attachment_spec.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depth_attachment_spec.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			depth_attachment_spec.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			//attachment ref attachment idx is determined on creation
			depth_attachment_spec.referenceLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			//used to resolve subpass attachments
			depth_attachment_spec.attachType = EAttachmentType::Depth;

			//for unification render target info i.e image for offscreen rendering
			depth_attachment_spec.bHasRenderTarget = true;
			depth_attachment_spec.extent = { width, height };
			depth_attachment_spec.arrayLayers = 1; //if using array as depth

			depth_attachment_spec.targetMask = ERenderTargetBits::Write | ERenderTargetBits::Sample | ERenderTargetBits::ImGuiView;
			depth_attachment_spec.explicitUsage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

			depth_attachment_spec.imageTiling = VK_IMAGE_TILING_OPTIMAL;
			depth_attachment_spec.imageAspect = VK_IMAGE_ASPECT_DEPTH_BIT;

			depth_attachment_spec.bEnableSamplerAnisotropy = false;
			depth_attachment_spec.samplerFilterMode = VK_FILTER_NEAREST;
			depth_attachment_spec.samplerAddressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
			depth_attachment_spec.samplerBorderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;




			RenderPassSpecification render_pass_spec{};
			render_pass_spec.attachmentSpecifications = { col_attachment_spec, depth_attachment_spec };
			render_pass_spec.unifiedRenderTargetSize = { width, height };
			render_pass_spec.bHasSubpassDescription = true;
			render_pass_spec.subpassPipelineBindpoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

			return render_pass_spec;
		}

