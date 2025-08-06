#include "ShadowRenderPass.h"

#include "Renderer/VulkanUtilities.h"

#include "Renderer/Resources.h"

namespace Fluxon {
	namespace Vulkan {
		bool ShadowRenderPass::CreateGfxPipeline(VulkanContext* vk_ctx)
		{
			GraphicsPipelineSpecification gfx_specification{};
			gfx_specification.vertexShaderPath = "";
			gfx_specification.fragmentShaderPath = "";
			gfx_specification.renderPass = this;

			//add required descriptor set layout & push constant 
			gfx_specification.descriptorSetLayouts = {};
			gfx_specification.pushConstantRanges = {};

			gfx_specification.rasterisationCullMode = VK_CULL_MODE_FRONT_BIT;
			gfx_specification.rasterisationFrontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

			gfx_specification.enableDepthTest = true;
			gfx_specification.enableDepthWrite = true;
			gfx_specification.enableAlphaBlend = false;
			gfx_specification.depthCompareOp = VK_COMPARE_OP_LESS;

			mGfxPipeline.Init(vk_ctx, gfx_specification);

			return true;
		}
		void ShadowRenderPass::Init(VulkanContext* vk_ctx)
		{
			VkFormat depth_format;
			//depth_format = Vulkan::Utils::FindSupportedFormat(ctx->physicalDevice,
			//	{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, //<<-- CANDIDATES FORMAT FOR DEPTH TESTING
			//	VK_IMAGE_TILING_OPTIMAL,
			//	VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
			
			depth_format = Utils::FindDepthFormat(vk_ctx->physicalDevice);
			//Does not have colour attachment 
			VkAttachmentDescription depth_attachment{};
			depth_attachment.format = depth_format;
			depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
			depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

			depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;
			//depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
			//depth_attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			VkAttachmentReference depth_ref{};
			depth_ref.attachment = 0;
			depth_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			VkSubpassDescription subpass{};
			subpass.colorAttachmentCount = 0;
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpass.pDepthStencilAttachment = &depth_ref;

			VkRenderPassCreateInfo renderpass_create_info{};
			renderpass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderpass_create_info.attachmentCount = 1;
			renderpass_create_info.pAttachments = &depth_attachment;
			renderpass_create_info.subpassCount = 1;
			renderpass_create_info.pSubpasses = &subpass;


			if (vkCreateRenderPass(vk_ctx->device, &renderpass_create_info, nullptr, &mRenderPass) != VK_SUCCESS)
				throw std::runtime_error("Failed to create render pass!!!!!!");


			if(!mRenderTargetAttachment.Create(vk_ctx, mRenderPass, mExtent))
				throw std::runtime_error("Failed to creaete Shadow pass render target attachment !!!!!!");


			if (!mFramebuffer.Create(vk_ctx, mRenderPass, { mRenderTargetAttachment }))
				throw std::runtime_error("Failed to create Shadow pass Framebuffer");

			if (!CreateGfxPipeline(vk_ctx))
				throw std::runtime_error("Failed to create GFX pipeline!!!!!");


		}
		void ShadowRenderPass::Begin(VulkanContext* ctx, VkCommandBuffer cmd_buff, uint32_t img_idx)
		{
			//maybe like a shadowConfig / shadowData
			//containing 
			//width 
			//height

			//clear depth
			VkClearValue clear_depth_value{};
			clear_depth_value.depthStencil = { 1.0f, 0 };

			VkRenderPassBeginInfo render_pass_info{};
			render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			render_pass_info.renderPass = mRenderPass;
			//render_pass_info.framebuffer = mSwapchainFramebuffers[image_idx];
			render_pass_info.framebuffer = mFramebuffer.bufferHandle;
			render_pass_info.renderArea.offset = { 0, 0 };
			render_pass_info.renderArea.extent = mExtent;
			render_pass_info.clearValueCount = 0;
			render_pass_info.pClearValues = &clear_depth_value;

			vkCmdBeginRenderPass(cmd_buff, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);


		}
























		bool ShadowRenderPass::RenderTarget::Create(Fluxon::Vulkan::VulkanContext* vk_ctx, VkRenderPass render_pass, VkExtent2D _extent)
		{
			extent = _extent;
			//in namespace Resource (Resource.cpp)
			//void CreateImage(const VulkanContext* vk_ctx, 
			// uint32_t width, uint32_t height, 
			// VkImageType type, 
			// uint32_t array_layer, 
			// VkFormat format, 
			// VkImageTiling tiling, VkImageUsageFlags usage,
			// VkMemoryPropertyFlags properties, VkImage& image, 
			// VkDeviceMemory& image_memory, 
			// VkImageCreateFlags create_info_flags = 0);

			//static void CreateImage2D(const VulkanContext * vk_ctx, 
			// uint32_t width, uint32_t height, 
			// VkFormat format, 
			// VkImageTiling tiling, VkImageUsageFlags usage,
			// VkMemoryPropertyFlags properties, VkImage & image, 
			// VkDeviceMemory & image_memory) {

			VkImageUsageFlags usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			Resources::CreateImage2D(vk_ctx, extent.width, extent.height, format,
				VK_IMAGE_TILING_OPTIMAL, usage,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, memory);



			//in namespace Resource (Resource.cpp)
			//VkImageView CreateImageView(VkDevice device, 
			// VkImage image, VkImageViewType view_type, 
			// VkFormat format, VkImageAspectFlags aspect_flags, 
			// uint32_t face_layer_count = 1);

			//static VkImageView CreateImageView2D(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspect_flags) {
				//return CreateImageView(device, image, VK_IMAGE_VIEW_TYPE_2D, format, aspect_flags);

			imageView = Resources::CreateImageView2D(vk_ctx->device, image, format, VK_IMAGE_ASPECT_DEPTH_BIT);

			//texture sampler for sampler
			bool enable_anisotropy = false;
			VkFilter filter_mode = VK_FILTER_LINEAR;
			VkSamplerAddressMode address_mode = VK_SAMPLER_ADDRESS_MODE_REPEAT;

			//max anisotropy is determined by support 
			VkSamplerCreateInfo sampler_create_info{};
			sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			sampler_create_info.magFilter = filter_mode;//VK_FILTER_LINEAR;
			sampler_create_info.minFilter = filter_mode;// VK_FILTER_LINEAR;
			sampler_create_info.addressModeU = address_mode; //VK_SAMPLER_ADDRESS_MODE_REPEAT;
			sampler_create_info.addressModeV = address_mode; //VK_SAMPLER_ADDRESS_MODE_REPEAT;
			sampler_create_info.addressModeW = address_mode; //VK_SAMPLER_ADDRESS_MODE_REPEAT;

			//could have this cache during load time 
			//to use for data recration when need without the need 
			//for not always having to get physical device multiple time during load 
			VkPhysicalDeviceProperties retrived_physical_device_prop{};
			vkGetPhysicalDeviceProperties(vk_ctx->physicalDevice, &retrived_physical_device_prop);

			sampler_create_info.anisotropyEnable = static_cast<VkBool32>(enable_anisotropy);
			sampler_create_info.maxAnisotropy = (enable_anisotropy) ? retrived_physical_device_prop.limits.maxSamplerAnisotropy : 1.0f;
			sampler_create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
			sampler_create_info.unnormalizedCoordinates = VK_FALSE;
			sampler_create_info.compareEnable = VK_FALSE;
			sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS;

			sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			sampler_create_info.mipLodBias = 0.0f;
			sampler_create_info.minLod = 0.0f;
			sampler_create_info.maxLod = 0.0f;


			if (vkCreateSampler(vk_ctx->device, &sampler_create_info, nullptr, &sampler) != VK_SUCCESS)
				throw std::runtime_error("Failed to Create Sampler!!!!!!!");

			return true;
		}


		bool ShadowRenderPass::Framebuffer::Create(Fluxon::Vulkan::VulkanContext* ctx, VkRenderPass render_pass, std::vector<RenderTarget> render_targets)
		{
			if (render_targets.empty())
				return false;

			//hack for now to test
			extent = render_targets[0].extent;

			imageViewAttachments.reserve(render_targets.size());

			//retrives & cache image view attachements
			for (auto& render_target : render_targets)
				imageViewAttachments.push_back(render_target.imageView);

			VkFramebufferCreateInfo framebuffer_create_info{};
			framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebuffer_create_info.renderPass = render_pass;
			framebuffer_create_info.attachmentCount = static_cast<uint32_t>(imageViewAttachments.size());
			framebuffer_create_info.pAttachments = imageViewAttachments.data();
			framebuffer_create_info.width = extent.width;
			framebuffer_create_info.height = extent.height;
			framebuffer_create_info.layers = 1;

			if (vkCreateFramebuffer(ctx->device, &framebuffer_create_info, nullptr, &bufferHandle) != VK_SUCCESS)
				throw std::runtime_error("Failed to create framebuffer!!!!!!");
		}
} //namespace Vulkan 

} //namespace Fluxon



