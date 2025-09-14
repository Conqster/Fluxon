#include "ForwardRenderPass.h"

#include "Renderer/VulkanContext.h"
#include "Renderer/VulkanUtilities.h"
#include "Core/Utilities.h"


namespace Fluxon {
	namespace Vulkan {


		void ForwardRenderPass::Init(VulkanContext* ctx)
		{
			FLUX_DEBUG_LOG_WARNING("Forward render pass does not support rendering to present swap chain; moved to render target for viewport display");


			Vulkan::SwapchainSupportDetails swap_chain_support = Vulkan::Utils::QuerySwapchainSupport(ctx->physicalDevice, ctx->surface);
			VkSurfaceFormatKHR surface_format = Vulkan::Swapchain::ChooseSwapSurfaceFormat(swap_chain_support.formats);

			VkAttachmentDescription colour_attachment{};
			colour_attachment.format = surface_format.format;
			colour_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
			colour_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			colour_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			colour_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colour_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			colour_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			colour_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			//Depth attachment
			VkAttachmentDescription depth_attachment{};
			depth_attachment.format = Vulkan::Utils::FindDepthFormat(ctx->physicalDevice);
			depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
			depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

			depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;



			//SUBPASSESS & ATTACHMENT REFERENCES
			VkAttachmentReference colour_attachment_ref{};
			colour_attachment_ref.attachment = 0; //Our array consists of a single VkAttachmentDescription,->colour_attachment
			colour_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			//depth
			VkAttachmentReference depth_attachment_ref{};
			depth_attachment_ref.attachment = 1;
			depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


			VkSubpassDescription subpass{};
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpass.colorAttachmentCount = 1;
			subpass.pColorAttachments = &colour_attachment_ref;
			subpass.pDepthStencilAttachment = &depth_attachment_ref;


			VkSubpassDependency dependency{};
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;

			dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependency.srcAccessMask = 0;

			dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			//The following other types of attachments can be referenced by a subpass :
			//pInputAttachments: Attachments that are read from a shader
			//pResolveAttachments : Attachments used for multisampling color attachments
			//pDepthStencilAttachment : Attachment for depthand stencil data
			//pPreserveAttachments : Attachments that are not used by this subpass, but for which the data must be preserved

			std::array<VkAttachmentDescription, 2> attachments = { colour_attachment, depth_attachment };
			//Render pass 
			VkRenderPassCreateInfo renderpass_create_info{};
			//renderpass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			//renderpass_create_info.attachmentCount = 1;
			////renderpass_create_info.pAttachments = &colour_attachment;
			//renderpass_create_info.pAttachments = attachments.data();
			//renderpass_create_info.subpassCount = 1;
			//renderpass_create_info.pSubpasses = &subpass;
			//renderpass_create_info.dependencyCount = 1;
			//renderpass_create_info.pDependencies = &dependency;

			renderpass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderpass_create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
			renderpass_create_info.pAttachments = attachments.data();
			renderpass_create_info.subpassCount = 1;
			renderpass_create_info.pSubpasses = &subpass;
			renderpass_create_info.dependencyCount = 1;
			renderpass_create_info.pDependencies = &dependency;

			if (vkCreateRenderPass(ctx->device, &renderpass_create_info, nullptr, &mRenderPass) != VK_SUCCESS)
				throw std::runtime_error("Failed to create render pass!!!!!!");
		}



		void ForwardRenderPass::Init(VulkanContext* vk_ctx, VkCommandPool cmd_pool, uint32 width, uint32 height)
		{


			//later change 
			Vulkan::SwapchainSupportDetails swap_chain_support = Vulkan::Utils::QuerySwapchainSupport(vk_ctx->physicalDevice, vk_ctx->surface);
			VkSurfaceFormatKHR surface_format = Vulkan::Swapchain::ChooseSwapSurfaceFormat(swap_chain_support.formats);
			VkFormat colour_format = surface_format.format;


			colour_format = VK_FORMAT_B8G8R8A8_SRGB;

			VkAttachmentDescription colour_attachment{};
			colour_attachment.format = colour_format;
			colour_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
			colour_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			colour_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			colour_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colour_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			colour_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			colour_attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;/*VK_IMAGE_LAYOUT_PRESENT_SRC_KHR*/;

			VkFormat depth_format;
			depth_format = Utils::FindDepthFormat(vk_ctx->physicalDevice);

			//Depth attachment
			VkAttachmentDescription depth_attachment{};
			depth_attachment.format = depth_format;
			depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
			depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

			depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			depth_attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;// VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;



			//SUBPASSESS & ATTACHMENT REFERENCES
			VkAttachmentReference colour_attachment_ref{};
			colour_attachment_ref.attachment = 0; //Our array consists of a single VkAttachmentDescription,->colour_attachment
			colour_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			//depth
			VkAttachmentReference depth_attachment_ref{};
			depth_attachment_ref.attachment = 1;
			depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


			VkSubpassDescription subpass{};
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpass.colorAttachmentCount = 1;
			subpass.pColorAttachments = &colour_attachment_ref;
			subpass.pDepthStencilAttachment = &depth_attachment_ref;


			VkSubpassDependency dependency{};
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;

			dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependency.srcAccessMask = 0;

			dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;


			std::array<VkAttachmentDescription, 2> attachments = { colour_attachment, depth_attachment };
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



			RenderTargetSpec rt_spec{};
			rt_spec.extent = {width, height};
			rt_spec.format = colour_format;
			rt_spec.targetMask = ERenderTargetBits::Write | ERenderTargetBits::ImGuiView;// | ERenderTargetBits::Sample; <-- engine is not sampling it but imgui is for now 
			rt_spec.explicitUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			rt_spec.imageAspect = VK_IMAGE_ASPECT_COLOR_BIT;
			rt_spec.bEnableSamplerAnisotropy = true;

			bool rt_success = RenderTarget::CreateRenderTarget(vk_ctx, cmd_pool, mColRenderTarget, rt_spec);
			
			rt_spec.format = depth_format;
			rt_spec.explicitUsage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			rt_spec.imageAspect = VK_IMAGE_ASPECT_DEPTH_BIT;
			rt_success |= RenderTarget::CreateRenderTarget(vk_ctx, cmd_pool, mDepthRenderTarget, rt_spec);


			if(!rt_success)
				throw std::runtime_error("Failed to creaete forward pass render target attachments !!!!!!");



			if (!Framebuffer::Create(vk_ctx, mRenderPass, mFramebuffer, {width, height}, { mColRenderTarget.imageView, mDepthRenderTarget.imageView}))
				throw std::runtime_error("Failed to create forward pass Framebuffer");

		}

		void ForwardRenderPass::OnResize(VulkanContext* vk_ctx, VkCommandPool cmd_pool, uint32 width, uint32 height)
		{
			mColRenderTarget.OnResize(vk_ctx, cmd_pool, width, height);
			mDepthRenderTarget.OnResize(vk_ctx, cmd_pool, width, height);

			mFramebuffer.OnResize(vk_ctx, mRenderPass, {mColRenderTarget.imageView, mDepthRenderTarget.imageView}, width, height);
		}

		void ForwardRenderPass::Begin(VulkanContext* ctx, VkCommandBuffer cmd_buff, uint32_t img_idx)
		{

			std::array<VkClearValue, 2> clear_values{};
			clear_values[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
			clear_values[1].depthStencil = { 1.0f, 0 };

			//VkExtent2D frame_extent = ctx->swapchain->GetExtent();
			VkExtent2D frame_extent = mFramebuffer.extent;
			VkRenderPassBeginInfo render_pass_info{};
			render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			render_pass_info.renderPass = mRenderPass;
			//render_pass_info.framebuffer = mSwapchainFramebuffers[image_idx];
			//render_pass_info.framebuffer = ctx->swapchain->GetImages()[img_idx].framebuffer;
			render_pass_info.framebuffer = mFramebuffer.bufferHandle;
			render_pass_info.renderArea.offset = { 0, 0 };
			render_pass_info.renderArea.extent = frame_extent;
			render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
			render_pass_info.pClearValues = clear_values.data();

			vkCmdBeginRenderPass(cmd_buff, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

			VkViewport viewport{};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = static_cast<float>(frame_extent.width);
			viewport.height = static_cast<float>(frame_extent.height);
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			vkCmdSetViewport(cmd_buff, 0, 1, &viewport);

			VkRect2D scissor{};
			scissor.offset = { 0, 0 };
			scissor.extent = frame_extent;
			vkCmdSetScissor(cmd_buff, 0, 1, &scissor);
		}

		void ForwardRenderPass::End(VkCommandBuffer cmd_buff)
		{
			vkCmdEndRenderPass(cmd_buff);
		}

		void ForwardRenderPass::Cleanup(VulkanContext* ctx)
		{
			vkDestroyRenderPass(ctx->device, mRenderPass, nullptr);
		}


	}//#Vulkan
}//#Fluxon