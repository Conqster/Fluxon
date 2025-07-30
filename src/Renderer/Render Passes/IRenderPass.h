#pragma once

#include "Renderer/VulkanContext.h"


////Or this could be use for Grpahics Pipeline as welll
//enum ERenderPass
//{
//	RenderPass_0,						//A constant to define the starting pass
//	RenderPass_Static = RenderPass_0,	// environments and level geomtery 
//	RenderPas_Actor,					// objects and things that can move
//	RenderPass_Sky,						//the backgroung 'behind' eveything
//	RenderPass_NotRendered,				// objects that dont render but exist
//
//	RenderPass_Last,					// not used - a counter for for loop 
//};



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
