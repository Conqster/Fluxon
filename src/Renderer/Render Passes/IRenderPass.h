#pragma once

#include "Renderer/VulkanContext.h"



namespace Fluxon {
	namespace Vulkan {



		/// <summary>
		/// Render Pass Defines what and where to render to 
		/// image/framebuffer attachements, formats, layout transitions 
		/// </summary>
		class IRenderPass
		{
		public:
			virtual void Init(class VulkanContext* ctx) = 0;
			virtual void Begin(VulkanContext* ctx, VkCommandBuffer cmd_buff, uint32_t img_idx) = 0;
			virtual VkRenderPass& GetRenderPassHandle() = 0;
			virtual void End(VkCommandBuffer cmd_buff) = 0;
			virtual void Cleanup(class VulkanContext* ctx) = 0;
			virtual ~IRenderPass() = default;
		};
	} //#Vulkan
}//#Fluxon
