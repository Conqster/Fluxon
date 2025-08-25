#pragma once

#include "IRenderPass.h"
#include <vulkan/vulkan.h>

#include <vector>
#include <glm/glm.hpp>


namespace Fluxon {
	namespace Vulkan {
		class ForwardRenderPass : public IRenderPass
		{
		private:
			VkRenderPass mRenderPass = VK_NULL_HANDLE;

		public:
			glm::vec3 mClearColour = glm::vec3(0.0f);
			void Init(class VulkanContext* ctx) override;

			VkRenderPass& GetRenderPassHandle() { return mRenderPass; }

			void Begin(VulkanContext* ctx, VkCommandBuffer cmd_buff, uint32_t img_idx) override;
			void End(VkCommandBuffer cmd_buff) override;

			void Cleanup(VulkanContext* ctx) override;
		};
	} //#Vulkan
}//#Fluxon