#pragma once


#include "IRenderPass.h"
#include <vulkan/vulkan.h>

#include "Renderer/Pipelines/GraphicsPipeline.h"

namespace Fluxon {
	namespace Vulkan {

		class ShadowRenderPass : public IRenderPass
		{
		private:
			VkRenderPass mRenderPass;
			VkExtent2D mExtent{};
			
			GraphicsPipeline mGfxPipeline;


			bool CreateGfxPipeline(VulkanContext* vk_ctx);
			bool CreateModelDescriptorSetLayout(VkDevice device);

			struct RenderTarget {
				//TODO(Conqster): later extract image, memory, imageView, sampler 
				//to a ImageResources want a flag to detremine if it can be sampled from
				//if it could be smapled from then the sampler would be created if not 
				// would have a vk null pointer handle
				VkImage image			= VK_NULL_HANDLE;
				VkDeviceMemory memory	= VK_NULL_HANDLE;
				VkImageView imageView	= VK_NULL_HANDLE;
				VkSampler sampler		= VK_NULL_HANDLE;

				VkFormat format			= VK_FORMAT_D32_SFLOAT;
				
				VkExtent2D extent{};
				
				bool Create(Fluxon::Vulkan::VulkanContext* ctx, VkRenderPass render_pass, VkExtent2D _extent);
			}mRenderTargetAttachment;

			struct Framebuffer {
				std::vector<VkImageView> imageViewAttachments; 
				VkFramebuffer bufferHandle;

				VkExtent2D extent{};

				bool Create(Fluxon::Vulkan::VulkanContext* ctx, VkRenderPass render_pass, std::vector<RenderTarget> render_targets);
			} mFramebuffer;

		public:
			void Init(class VulkanContext* vk_ctx) override;

			VkRenderPass& GetRenderPassHandle() { return mRenderPass; }

			void Begin(VulkanContext* ctx, VkCommandBuffer cmd_buff, uint32_t img_idx) override;
			void End(VkCommandBuffer cmd_buff) override;

			void Cleanup(VulkanContext* ctx) override;
		};
	} //namspace Vulkan
} //namespace Fluxon







struct ShadowDraw {

	struct RenderPass {
		VkRenderPass vkRenderPass;
	} mShadowRenderPass;

	struct RenderTarget {
		VkImage image = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;
		VkImageView imageView = VK_NULL_HANDLE;
	}mRenderTargetAttachment;

	struct GraphicsPipeline { // or struct DrawPipeline // or struct RenderPipeline
		VkPipelineLayout layout;
		VkPipeline pipeline;
	};
};


//struct Framebuffer {
//	std::vector<VkImageView> imageViewAttachments;
//	VkFramebuffer bufferHandle;
//	
//	VkExtent2D mExtent;
//
//	void Create(Fluxon::Vulkan::VulkanContext* ctx, VkRenderPass render_pass) {
//
//		VkFramebufferCreateInfo framebuffer_create_info{};
//		framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
//		framebuffer_create_info.renderPass = render_pass;
//		framebuffer_create_info.attachmentCount = static_cast<uint32_t>(imageViewAttachments.size());
//		framebuffer_create_info.pAttachments = imageViewAttachments.data();
//		framebuffer_create_info.width = mExtent.width;
//		framebuffer_create_info.height = mExtent.height;
//		framebuffer_create_info.layers = 1;
//
//		if (vkCreateFramebuffer(ctx->device, &framebuffer_create_info, nullptr, &bufferHandle) != VK_SUCCESS)
//			throw std::runtime_error("Failed to create framebuffer!!!!!!");
//	}
//};