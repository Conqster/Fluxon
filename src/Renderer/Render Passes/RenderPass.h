#pragma once

#include <vulkan/vulkan.h>

#include <vector>


#include "Renderer/RenderTarget.h"


#include <optional>


namespace Fluxon {
	namespace Vulkan {


		enum class EAttachmentType : uint8
		{
			Colour,
			Depth
		};

		struct RenderPassAttachmentSpecification
		{
			//attachement description
			VkFormat format						= VK_FORMAT_B8G8R8A8_SRGB;
			VkClearValue clearValue				= { 0.0f, 0.0f, 0.0f, 1.0f }; //<--- default clear value as colour 
			VkSampleCountFlagBits samples		= VK_SAMPLE_COUNT_1_BIT;
			VkAttachmentLoadOp loadOp			= VK_ATTACHMENT_LOAD_OP_CLEAR;
			VkAttachmentStoreOp storeOp			= VK_ATTACHMENT_STORE_OP_STORE;
			VkAttachmentLoadOp stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			VkAttachmentStoreOp stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
			VkImageLayout initialLayout			= VK_IMAGE_LAYOUT_UNDEFINED;
			VkImageLayout finalLayout			= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			//attachment ref attachment idx is determined on creation
			VkImageLayout referenceLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			//used to resolve subpass attachments
			EAttachmentType attachType = EAttachmentType::Colour;

			//for unification render target info i.e image for offscreen rendering
			bool bHasRenderTarget = true;
			VkExtent2D extent{};
			uint32_t arrayLayers = 1; //if using array as depth

			RenderTargetFlag targetMask = ERenderTargetBits::Write | ERenderTargetBits::Sample;
			VkImageUsageFlags explicitUsage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

			VkImageTiling imageTiling = VK_IMAGE_TILING_OPTIMAL;
			VkImageAspectFlagBits imageAspect = VK_IMAGE_ASPECT_DEPTH_BIT;

			bool bEnableSamplerAnisotropy = false;
			VkFilter samplerFilterMode = VK_FILTER_NEAREST;
			VkSamplerAddressMode samplerAddressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
			VkBorderColor samplerBorderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		};

		struct SubpassSpecification
		{
			VkPipelineBindPoint subpassPipelineBindpoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			std::vector<uint32> colourAttachmentIndices;
			std::optional<uint32> depthAttachmentIdx;
		};

		struct RenderPassSpecification
		{
			std::vector<RenderPassAttachmentSpecification> attachmentSpecifications;

			//unified Render Target size, when set is used during setup to make sure 
			//attachments have uniform extext
			std::optional<VkExtent2D> unifiedRenderTargetSize = std::nullopt;


			//Only supports a single subpass & Dependency at the moment
			bool bHasSubpassDescription = true;
			VkPipelineBindPoint subpassPipelineBindpoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		};


		class RenderPass
		{
		public:
			RenderPass() = default;
			void Init(class VulkanContext* ctx, const RenderPassSpecification& spec);

			const VkRenderPass& GetRenderPassHandle() const { return mRenderPass; }


			void OnResize(VulkanContext* vk_ctx, VkCommandPool cmd_pool, uint32 width, uint32 height);

			void Begin(/*const class VulkanCommandBuffer&*/VkCommandBuffer cmd_buff);
			void End(/*const VulkanCommandBuffer&*/VkCommandBuffer cmd_buff);

			void Cleanup(VulkanContext* ctx);


			//need to fix this avoid external editting
			//const RenderTarget& GetOutputRT(u32 idx) const;
			RenderTarget& GetOutputRT(uint32 idx);
			const std::optional<RenderTarget>& GetDepthOutputRT() const;


		private:
			std::vector<VkClearValue> mClearValues;
			VkRenderPass mRenderPass = VK_NULL_HANDLE;


			std::vector<RenderTarget> mRenderTargets;
			Framebuffer mFramebuffer;

			std::optional<RenderTarget> mDepthRenderTarget = std::nullopt;
		};





		RenderPassSpecification GenerateForwardPassSpecification(class VulkanContext* ctx, uint32 width, uint32 height);
	} //#Vulkan
}//#Fluxon