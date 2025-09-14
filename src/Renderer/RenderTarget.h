#pragma once

#include <vulkan/vulkan.h>
#include "VulkanContext.h"

#include "Core/Enums.h"


namespace Fluxon {
	
	namespace Vulkan {


		struct RenderTargetSpec
		{
			VkExtent2D extent{};
			uint32_t arrayLayers					= 1; //if using array as depth
			VkFormat format							= VK_FORMAT_D32_SFLOAT;

			RenderTargetFlag targetMask			= ERenderTargetBits::Write | ERenderTargetBits::Sample;
			VkImageUsageFlags explicitUsage			= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

			VkImageTiling imageTiling				= VK_IMAGE_TILING_OPTIMAL;
			VkImageAspectFlagBits imageAspect		= VK_IMAGE_ASPECT_DEPTH_BIT;

			bool bEnableSamplerAnisotropy			= false;
			VkFilter samplerFilterMode				= VK_FILTER_NEAREST;
			VkSamplerAddressMode samplerAddressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
			VkBorderColor samplerBorderColor		= VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		};


		struct RenderTarget
		{
			VkImage image				= VK_NULL_HANDLE;
			VkDeviceMemory memory		= VK_NULL_HANDLE;
			VkImageView imageView		= VK_NULL_HANDLE;
			VkSampler sampler			= VK_NULL_HANDLE;

			VkFormat format				= VK_FORMAT_D32_SFLOAT;
			uint32_t arrayLayers					= 1; //if using array as depth
			VkExtent2D extent{};

			VkImageUsageFlags explicitUsage = {};
			VkImageAspectFlagBits imageAspect = {};

			RenderTargetFlag targetMask = ERenderTargetBits::Write | ERenderTargetBits::Sample;


			VkDescriptorSet imguiDescSet = VK_NULL_HANDLE;


			static bool CreateRenderTarget(Fluxon::Vulkan::VulkanContext* vk_ctx, VkCommandPool cmd_pool, RenderTarget& render_target, RenderTargetSpec specification);

			bool OnResize(VulkanContext* vk_ctx, VkCommandPool cmd_pool, uint32 width, uint32 height);

			
			static bool MakeRenderTarget(Fluxon::Vulkan::VulkanContext* vk_ctx, RenderTarget& rt, RenderTargetFlag target_mask);
			
			static void DestoryRenderTraget(VkDevice device, RenderTarget& rt);
		};



		//template<size_t AttachmentCount>
		struct Framebuffer
		{
			//std::array<RenderTarget, AttachmentCount> renderTargets;
			VkFramebuffer bufferHandle;
			VkExtent2D extent{};

			static bool Create(const Fluxon::Vulkan::VulkanContext* ctx, VkRenderPass render_pass, Framebuffer& fbo, 
				VkExtent2D _extext, const std::vector<VkImageView>& view_attachments, uint32_t layers = 1);

			bool CreateDummy(Fluxon::Vulkan::VulkanContext* ctx, VkRenderPass render_pass,
										VkExtent2D _extext, uint32_t layers = 1);


			bool OnResize(VulkanContext* vk_ctx, VkRenderPass render_pass, const std::vector<VkImageView>& view_attachments,
				uint32 width, uint32 height, uint32 layer = 1);

			static void DestroyFramebuffer(VkDevice device, Framebuffer& fb) {
				
				if(fb.bufferHandle != VK_NULL_HANDLE)
					vkDestroyFramebuffer(device, fb.bufferHandle, nullptr);
			}
		};

	} //namespace Vulkan

} //namespace Fluxon