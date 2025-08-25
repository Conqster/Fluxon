#include "RenderTarget.h"

#include "Resources.h"
#include "VulkanUtilities.h"

namespace Fluxon {
	namespace Vulkan {


		bool RenderTarget::CreateRenderTarget(Fluxon::Vulkan::VulkanContext* vk_ctx, VkCommandPool cmd_pool, RenderTarget& render_target, RenderTargetSpec spec)
		{
			render_target.extent = spec.extent;
			render_target.format = spec.format;
			render_target.targetMask = spec.targetMask;
			render_target.arrayLayers = spec.arrayLayers;

			VkImageUsageFlags usage = 0;
			if (spec.targetMask.Contains(ERenderTargetBits::Write))
				usage |= Utils::IsDepthFormat(spec.format) ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			if (spec.targetMask.Contains(ERenderTargetBits::Sample))
			{
				usage |= VK_IMAGE_USAGE_SAMPLED_BIT;

				//texture sampler for sampler
				// 
				render_target.sampler = Resources::CreateTextureSampler(vk_ctx, spec.bEnableSamplerAnisotropy, spec.samplerFilterMode, spec.samplerAddressMode, spec.samplerBorderColor);
			}

			//EXPLICIT USAGE
			usage |= spec.explicitUsage;

			
			if (!spec.targetMask.Contains(ERenderTargetBits::Array2D))
			{
				Resources::CreateImage2D(vk_ctx, render_target.extent.width, render_target.extent.height, 
					render_target.format, spec.imageTiling, usage,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, render_target.image, render_target.memory);

				render_target.imageView = Resources::CreateImageView2D(vk_ctx->device, render_target.image, render_target.format, spec.imageAspect);
			}
			else
			{
				Resources::CreateImage2DArray(vk_ctx, render_target.extent.width, render_target.extent.height,
					render_target.arrayLayers, render_target.format, spec.imageTiling, usage,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, render_target.image, render_target.memory);

				render_target.imageView = Resources::CreateImageView2DArray(vk_ctx->device, render_target.image, 
					render_target.format, spec.imageAspect, render_target.arrayLayers);
			}


			Utils::LayoutTransitionInfo transition_info{}; 
			transition_info.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			transition_info.newLayout = Utils::IsDepthFormat(spec.format) ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			transition_info.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			//VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT for depth testing before draw
			//VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT for colour drawing in frag shading bit
			transition_info.dstStageMask = Utils::IsDepthFormat(spec.format) ? VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT : VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			transition_info.srcAccess = 0;
			transition_info.dstAccess = Utils::IsDepthFormat(spec.format) ?
				VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT :
				VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			transition_info.subRangeLayerCount = render_target.arrayLayers;
			
			Utils::TransitionImageLayout(vk_ctx, cmd_pool, render_target.image, render_target.format, transition_info);

			if (spec.targetMask.Contains(ERenderTargetBits::ImGuiView))
				render_target.imguiDescSet = ImGui_ImplVulkan_AddTexture(render_target.sampler, render_target.imageView,
										VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

			return true;
		}

		bool RenderTarget::MakeRenderTarget(Fluxon::Vulkan::VulkanContext* vk_ctx, RenderTarget& rt, RenderTargetFlag target_mask)
		{

			//if required a sampler or imgui debug view  both requires a sampler
			bool needs_sampler = (target_mask.Contains(ERenderTargetBits::Sample) || target_mask.Contains(ERenderTargetBits::ImGuiView));
			//bool needs_sampler = (target_mask.Contains(ERenderTargetBits::Sample | ERenderTargetBits::ImGuiView));

			bool sampler_missing = (rt.targetMask.Contains(ERenderTargetBits::Sample) || rt.sampler == VK_NULL_HANDLE);
			if (needs_sampler && sampler_missing)
			{
				//for render target (rt) having flag RTM::Sample but rt.sampler == VK_NULL_HANDLE <<-- mush be a bug in the system/creation
				// 
				//use default values for now
				rt.sampler = Resources::CreateTextureSampler(vk_ctx, true);
				rt.targetMask |= ERenderTargetBits::Sample;
			}
				
			if (target_mask.Contains(ERenderTargetBits::ImGuiView) &&
				rt.targetMask.Contains(ERenderTargetBits::ImGuiView) || rt.imguiDescSet == VK_NULL_HANDLE)
			{
				rt.imguiDescSet = ImGui_ImplVulkan_AddTexture(rt.sampler, rt.imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
				rt.targetMask |= ERenderTargetBits::ImGuiView;
			}

			return true;
		}

		void RenderTarget::DestoryRenderTraget(VkDevice device, RenderTarget& rt)
		{
			if(rt.sampler)
				vkDestroySampler(device, rt.sampler, nullptr);
			if(rt.imageView)
				vkDestroyImageView(device, rt.imageView, nullptr);
			if(rt.image)
				vkDestroyImage(device, rt.image, nullptr);
			if(rt.memory)
				vkFreeMemory(device, rt.memory, nullptr);
		}






		bool Framebuffer::Create(Fluxon::Vulkan::VulkanContext* ctx, VkRenderPass render_pass, Framebuffer& fbo, 
			VkExtent2D _extext, const std::vector<VkImageView>& view_attachments, uint32_t layers)
		{
			fbo.extent = _extext;

			VkFramebufferCreateInfo framebuffer_create_info{};
			framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebuffer_create_info.renderPass = render_pass;
			framebuffer_create_info.attachmentCount = static_cast<uint32_t>(view_attachments.size());
			framebuffer_create_info.pAttachments = view_attachments.data();
			framebuffer_create_info.width = fbo.extent.width;
			framebuffer_create_info.height = fbo.extent.height;
			framebuffer_create_info.layers = layers;

			if (vkCreateFramebuffer(ctx->device, &framebuffer_create_info, nullptr, &fbo.bufferHandle) != VK_SUCCESS)
				throw std::runtime_error("Failed to create framebuffer!!!!!!");

			return true;
		}

		bool Framebuffer::CreateDummy(Fluxon::Vulkan::VulkanContext* ctx, VkRenderPass render_pass, VkExtent2D _extext, uint32_t layers)
		{
			extent = _extext;

			VkFramebufferCreateInfo framebuffer_create_info{};
			framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebuffer_create_info.renderPass = render_pass;
			framebuffer_create_info.attachmentCount = 0;
			framebuffer_create_info.pAttachments = nullptr;
			framebuffer_create_info.width = extent.width;
			framebuffer_create_info.height = extent.height;
			framebuffer_create_info.layers = layers;

			if (vkCreateFramebuffer(ctx->device, &framebuffer_create_info, nullptr, &bufferHandle) != VK_SUCCESS)
				throw std::runtime_error("Failed to create framebuffer!!!!!!");

			return true;
		}

	}//namespace Vulkan
} //namespace Fluxon