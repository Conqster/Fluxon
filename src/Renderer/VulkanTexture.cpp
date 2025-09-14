#include "VulkanTexture.h"

#include "Renderer/Resources.h"

namespace Fluxon {
	namespace Vulkan {

		void VulkanTexture::Create(Fluxon::Vulkan::VulkanContext* vk_ctx, VkCommandPool cmd_pool, TextureSpecification spec)
		{
			mWidth = spec.width;
			mHeight = spec.height;
			mDepth = spec.depth;

			mFormat = spec.format;
	
			if (spec.imageLayout.has_value())
				mImageLayout = spec.imageLayout.value();
			else
				mImageLayout = GetOptimalImageLayout(spec.usage);


			VkImageUsageFlags usage = 0;
			if (spec.usage.Contains(ETextureUsageBits::DepthAttachment) && Utils::IsDepthFormat(spec.format))
				usage |=  VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			else if(spec.usage.Contains(ETextureUsageBits::ColourAttachment))
				usage |=  VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

			if(spec.usage.Contains(ETextureUsageBits::Storage))
				usage |= VK_IMAGE_USAGE_STORAGE_BIT;

			
			if (spec.usage.Contains(ETextureUsageBits::Sampled))
			{
				usage |= VK_IMAGE_USAGE_SAMPLED_BIT;

				//texture sampler for sampler
				// 
				mSampler = Resources::CreateTextureSampler(vk_ctx, spec.bEnableSamplerAnisotropy, spec.samplerFilterMode, spec.samplerAddressMode, spec.samplerBorderColor);
			}


			//mRenderer->CreateBuffer(image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer);

			//quick hack image from file has ben auto Transitioned on load
			//may be allow Resources::CreateImage2D etc to transition image as well 
			bool require_transition = false;


			switch (spec.type)
			{
			case ETextureType::Texture2D:

				if (spec.imageDiscPaths.empty())
				{
					Resources::CreateImage2D(vk_ctx, mWidth, mHeight,
						mFormat, spec.imageTiling, usage,
						spec.imageMemProperty, mImage, mMemory);

					require_transition = true;
				}
				else
				{
					Resources::CreateImage2DFromFile(vk_ctx, cmd_pool, spec.imageDiscPaths[0], mFormat,
						mImage, mMemory, mWidth, mHeight, mImageLayout, spec.bEnableSamplerAnisotropy);
				}

				mImageView = Resources::CreateImageView2D(vk_ctx->device, mImage, mFormat, spec.imageAspect);
				break;

			case ETextureType::Texture2DArray:
				
				if (!spec.imageDiscPaths.empty())
					FLUX_DEBUG_ASSERT_CRITICAL(false, "image from disc not supported as 2D array, try as cube.", 
						"\n default as basic image array");

				Resources::CreateImage2DArray(vk_ctx, mWidth, mHeight, mDepth,
					mFormat, spec.imageTiling, usage,
					spec.imageMemProperty, mImage, mMemory);

				require_transition = true;

				mImageView = Resources::CreateImageView2DArray(vk_ctx->device, mImage,
					mFormat, spec.imageAspect, mDepth);

				break;

			case ETextureType::TextureCube:

				if (spec.imageDiscPaths.empty())
				{
					Resources::CreateImage2D_4_CUBE(vk_ctx, mWidth, mHeight, mFormat,
						spec.imageTiling, usage, spec.imageMemProperty, mImage, mMemory);
				}
				else
				{
					Resources::CreateImageCubeFromFile(vk_ctx, cmd_pool, spec.imageDiscPaths, mFormat,
						mImage, mMemory, mWidth, mHeight, mImageLayout, spec.bEnableSamplerAnisotropy);
				}

				mImageView = Resources::CreateImageViewCube(vk_ctx->device, mImage,
					mFormat, spec.imageAspect);

				break;
			default:

				FLUX_DEBUG_ASSERT_CRITICAL(false, "Vulkan texture ETextureType::  not supported default to basic 2d texture");
				if (spec.imageDiscPaths.empty())
				{
					Resources::CreateImage2D(vk_ctx, mWidth, mHeight,
						mFormat, spec.imageTiling, usage,
						spec.imageMemProperty, mImage, mMemory);

					require_transition = true;
				}
				else
				{
					Resources::CreateImage2DFromFile(vk_ctx, cmd_pool, spec.imageDiscPaths[0], mFormat,
						mImage, mMemory, mWidth, mHeight, mImageLayout, spec.bEnableSamplerAnisotropy);
				}

				mImageView = Resources::CreateImageView2D(vk_ctx->device, mImage, mFormat, spec.imageAspect);

				break;
			}


			if(require_transition)
			{
				if (spec.explicitImageTransitionInfo.has_value())
					Utils::TransitionImageLayout(vk_ctx, cmd_pool, mImage, spec.format, spec.explicitImageTransitionInfo.value());
				else
				{
					Utils::LayoutTransitionInfo transition_info{};
					transition_info.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
					transition_info.newLayout = mImageLayout;
					transition_info.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
					//VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT for depth testing before draw
					//VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT for colour drawing in frag shading bit
					transition_info.dstStageMask = (spec.usage.Contains(ETextureUsageBits::DepthAttachment) && Utils::IsDepthFormat(mFormat)) ?
						VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT : VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
					transition_info.srcAccess = 0;
					transition_info.dstAccess = (spec.usage.Contains(ETextureUsageBits::DepthAttachment) && Utils::IsDepthFormat(mFormat)) ?
						VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT :
						VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
					transition_info.subRangeLayerCount = mDepth;

					Utils::TransitionImageLayout(vk_ctx, cmd_pool, mImage, mFormat, transition_info);
				}
			}

			if(spec.usage.Contains(ETextureUsageBits::ImguiView))
				mImguiDescSet = ImGui_ImplVulkan_AddTexture(mSampler, mImageView,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		}

		VkImageLayout VulkanTexture::GetOptimalImageLayout(TextureUsageFlags usage_flag)
		{

			switch (static_cast<ETextureUsageBits>(usage_flag.Value()))
			{
			case ETextureUsageBits::DepthAttachment:
				return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
				break;
			case ETextureUsageBits::ColourAttachment:
				return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				break;
			case ETextureUsageBits::Sampled:
				return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				break;
			case ETextureUsageBits::Storage:
				return VK_IMAGE_LAYOUT_GENERAL;
				break;
			case ETextureUsageBits::ImguiView:
				return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				break;
			default:
				throw std::runtime_error("TextureUsageFlags, format not supported for transition!!!!!");
				break;
			}
	
			throw std::runtime_error("TextureUsageFlags, format not supported for transition!!!!!");
			return VkImageLayout();
		}
	} //namespace

} //namespace Fluxon