#pragma once

#include "IRenderPass.h"
#include <vulkan/vulkan.h>

#include <vector>
#include <glm/glm.hpp>


#include "Renderer/RenderTarget.h"


namespace Fluxon {
	namespace Vulkan {
		class ForwardRenderPass : public IRenderPass
		{
		private:
			VkRenderPass mRenderPass = VK_NULL_HANDLE;

			//might chage this to vulkan texture 
			//which offer more tools like custom image trainsiton but 
			//this for testing 
			RenderTarget mColRenderTarget;
			RenderTarget mDepthRenderTarget;
			Framebuffer mFramebuffer;

		public:
			glm::vec3 mClearColour = glm::vec3(0.0f);
			void Init(class VulkanContext* vk_ctx, VkCommandPool cmd_pool, uint32 width, uint32 height);
			void Init(class VulkanContext* ctx) override;

			VkRenderPass& GetRenderPassHandle() { return mRenderPass; }

			RenderTarget& GetColourRenderTarget() { return mColRenderTarget; }

			void OnResize(VulkanContext* vk_ctx, VkCommandPool cmd_pool, uint32 width, uint32 height);

			void Begin(VulkanContext* ctx, VkCommandBuffer cmd_buff, uint32_t img_idx) override;
			void End(VkCommandBuffer cmd_buff) override;

			void Cleanup(VulkanContext* ctx) override;
		};
	} //#Vulkan
}//#Fluxon