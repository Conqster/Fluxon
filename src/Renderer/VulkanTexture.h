#pragma once

#include "Core/Core.h"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>
#include "Core/Enums.h"

#include "VulkanUtilities.h"

//forward declartion
namespace Fluxon {
	namespace Vulkan {
		struct VulkanContext;
	}
}



namespace Fluxon {


	namespace Vulkan {

		struct TextureSpecification
		{
			u32 width = 0;
			u32 height = 0;
			u32 depth = 1;
			VkFormat format = VK_FORMAT_D32_SFLOAT;

			TextureUsageFlags usage = ETextureUsageBits::DepthAttachment | ETextureUsageBits::Sampled;
			ETextureType type = ETextureType::Texture2D;


			VkMemoryPropertyFlags imageMemProperty = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

			std::optional<VkImageLayout> imageLayout = {};
			std::optional<Utils::LayoutTransitionInfo> explicitImageTransitionInfo = {};
			VkImageTiling imageTiling = VK_IMAGE_TILING_OPTIMAL;
			VkImageAspectFlagBits imageAspect = VK_IMAGE_ASPECT_DEPTH_BIT;


			

			bool bEnableSamplerAnisotropy = false;
			VkFilter samplerFilterMode = VK_FILTER_NEAREST;
			VkSamplerAddressMode samplerAddressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
			VkBorderColor samplerBorderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		};


		class VulkanTexture
		{
		public:
			void Create(Fluxon::Vulkan::VulkanContext* vk_ctx, VkCommandPool cmd_pool, TextureSpecification spec);
			void Destory(VkDevice device) {
				vkDestroySampler(device, mSampler, nullptr);
				vkDestroyImageView(device, mImageView, nullptr);
				vkDestroyImage(device, mImage, nullptr);
				vkFreeMemory(device, mMemory, nullptr);
			}

			//Getters 
			const VkSampler& GetSampler() const { return mSampler; }
			const VkImageView& GetImageView() const { return mImageView; }
			const VkImageLayout& GetImageLayout() const { return mImageLayout; }
		private:
			u32 mWidth						= 0;
			u32 mHeight						= 0;
			u32 mDepth						= 0;

			VkImage mImage					= VK_NULL_HANDLE;
			VkDeviceMemory mMemory			= VK_NULL_HANDLE;
			VkImageView mImageView			= VK_NULL_HANDLE;
			VkSampler mSampler				= VK_NULL_HANDLE;
			VkImageLayout mImageLayout		= {};	//<--- LATER STORE A SLIM ENUM as u16 as with mFormat
													// but the other thing to keep in mind is that, image layout is only backend
													// as its will not necessary exposed to engine user like Format 

			ETextureType mTextureType		= ETextureType::Texture2D;
			TextureUsageFlags mUsageFlags	= ETextureUsageBits::DepthAttachment | ETextureUsageBits::Sampled;

			VkFormat mFormat				= VK_FORMAT_R16_SFLOAT;
			VkDescriptorSet mImguiDescSet	= VK_NULL_HANDLE;

			VkImageLayout GetOptimalImageLayout(TextureUsageFlags usage_flag);
		};
	} //Vulkan
} //namespace Fluxon