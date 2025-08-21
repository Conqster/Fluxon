
#include "Resources.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "VulkanUtilities.h"

#include "GPUBuffer.h"
#include "VulkanContext.h"

namespace Fluxon {
	namespace Vulkan {
		namespace Resources {

			void DestoryTextureImageSampler(VkDevice device, ImageSampler& img_sampler) {
				vkDestroySampler(device, img_sampler.sampler, nullptr);
				vkDestroyImageView(device, img_sampler.imageView, nullptr);
				vkDestroyImage(device, img_sampler.image, nullptr);
				vkFreeMemory(device, img_sampler.memory, nullptr);
			}

			VkImageView CreateImageView(VkDevice device, VkImage image, VkImageViewType view_type, VkFormat format, VkImageAspectFlags aspect_flags, uint32_t layer_count) {
				VkImageViewCreateInfo img_view_create_info{};
				img_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				img_view_create_info.image = image;

				img_view_create_info.viewType = view_type;// VK_IMAGE_VIEW_TYPE_2D;
				img_view_create_info.format = format;

				img_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
				img_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
				img_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
				img_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

				img_view_create_info.subresourceRange.aspectMask = aspect_flags;
				img_view_create_info.subresourceRange.baseMipLevel = 0;
				img_view_create_info.subresourceRange.levelCount = 1;
				img_view_create_info.subresourceRange.baseArrayLayer = 0;
				img_view_create_info.subresourceRange.layerCount = layer_count;// 1;

				VkImageView temp_image_view;
				if (vkCreateImageView(device, &img_view_create_info, nullptr, &temp_image_view) != VK_SUCCESS)
					throw std::runtime_error("Failed to create image view!!");

				return temp_image_view;
			}




			void CreateImage(const VulkanContext* vk_ctx, uint32_t width, uint32_t height, VkImageType type, uint32_t array_layer, VkFormat format, VkImageTiling tiling,
				VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& image_memory, VkImageCreateFlags create_info_flags) {

				VkImageCreateInfo image_create_info{};
				image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
				image_create_info.imageType = type; // VK_IMAGE_TYPE_2D;
				image_create_info.extent.width = width;
				image_create_info.extent.height = height;
				image_create_info.extent.depth = 1;
				image_create_info.mipLevels = 1;
				image_create_info.arrayLayers = array_layer;//1;
				image_create_info.format = format;
				image_create_info.tiling = tiling;
				image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				image_create_info.usage = usage;
				image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
				image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
				image_create_info.flags = create_info_flags;

				if (vkCreateImage(vk_ctx->device, &image_create_info, nullptr, &image) != VK_SUCCESS)
					throw std::runtime_error("Failed to Create Image!!!!!!");

				VkMemoryRequirements memRequirements;
				vkGetImageMemoryRequirements(vk_ctx->device, image, &memRequirements);

				VkMemoryAllocateInfo alloc_info{};
				alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				alloc_info.allocationSize = memRequirements.size;
				alloc_info.memoryTypeIndex = Utils::FindMemoryType(vk_ctx->physicalDevice, memRequirements.memoryTypeBits, properties);

				if (vkAllocateMemory(vk_ctx->device, &alloc_info, nullptr, &image_memory) != VK_SUCCESS) {
					throw std::runtime_error("failed to allocate image memory!");
				}


				vkBindImageMemory(vk_ctx->device, image, image_memory, 0);
			}


			void CopyBufferToImage(const VulkanContext* vk_ctx, VkCommandPool cmd_pool, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
				VkCommandBuffer cmd_buf = Utils::BeginSingleTimeCommand(vk_ctx->device, cmd_pool);

				VkBufferImageCopy region{};
				region.bufferOffset = 0;
				region.bufferRowLength = 0;
				region.bufferImageHeight = 0;
				region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				region.imageSubresource.mipLevel = 0;
				region.imageSubresource.baseArrayLayer = 0;
				region.imageSubresource.layerCount = 1;

				region.imageOffset = { 0,0,0 };
				region.imageExtent = {
					width,
					height,
					1
				};

				vkCmdCopyBufferToImage(
					cmd_buf,
					buffer,
					image,
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					1,
					&region
				);



				Utils::EndSingleTimeCommand(vk_ctx, cmd_pool, cmd_buf);
			}


			void CopyBufferToMultiImages(const VulkanContext* vk_ctx, VkCommandPool cmd_pool, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height,
				VkDeviceSize img_stride, uint32_t img_count) {

				VkCommandBuffer cmd_buf = Utils::BeginSingleTimeCommand(vk_ctx->device, cmd_pool);

				std::vector<VkBufferImageCopy> regions;
				regions.resize(6);
				for (uint32_t img = 0; img < img_count; img++)
				{
					auto& region = regions[img];
					region.bufferOffset = img_stride * static_cast<VkDeviceSize>(img);
					region.bufferRowLength = 0;
					region.bufferImageHeight = 0;
					region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					region.imageSubresource.mipLevel = 0;
					region.imageSubresource.baseArrayLayer = img;
					region.imageSubresource.layerCount = 1;

					region.imageOffset = { 0,0,0 };
					region.imageExtent = {
						width,
						height,
						1
					};
				}

				vkCmdCopyBufferToImage(
					cmd_buf,
					buffer,
					image,
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					static_cast<int32_t>(regions.size()),
					regions.data()
				);

				Utils::EndSingleTimeCommand(vk_ctx, cmd_pool, cmd_buf);
			}


			void CopyBuffer(const VulkanContext* vk_ctx, VkCommandPool cmd_pool, VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size) {
				VkCommandBuffer cmd_buffer = Utils::BeginSingleTimeCommand(vk_ctx->device, cmd_pool);

				VkBufferCopy copy_region{};
				copy_region.srcOffset = 0;
				copy_region.dstOffset = 0;
				copy_region.size = size;
				vkCmdCopyBuffer(cmd_buffer, src_buffer, dst_buffer, 1, &copy_region);

				Utils::EndSingleTimeCommand(vk_ctx, cmd_pool, cmd_buffer);
			}

			VkSampler CreateTextureSampler(const VulkanContext* vk_ctx, bool enable_anisotropy, VkFilter filter_mode, VkSamplerAddressMode address_mode, VkBorderColor border_colour, 
				bool enable_compare_op, VkCompareOp compare_op) {
				//later pass in unique parameters 
				//like 
				//filters 
				//repeat mode
				//border colour etc
				//properties like 
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
				sampler_create_info.borderColor = border_colour;
				sampler_create_info.unnormalizedCoordinates = VK_FALSE;
				sampler_create_info.compareEnable = static_cast<VkBool32>(enable_compare_op);
				sampler_create_info.compareOp = compare_op;

				sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
				sampler_create_info.mipLodBias = 0.0f;
				sampler_create_info.minLod = 0.0f;
				sampler_create_info.maxLod = 0.0f;

				VkSampler temp_sampler;
				if (vkCreateSampler(vk_ctx->device, &sampler_create_info, nullptr, &temp_sampler) != VK_SUCCESS)
					throw std::runtime_error("Failed to Create Texture Sampler!!!!!!!");
				return temp_sampler;
			}

			void CreateImageSamplerTexture(const VulkanContext* vk_ctx, VkCommandPool cmd_pool, const char* file_path, VkFormat format, ImageSampler& image_sampler, bool enable_anisotropy) {
				int tex_width;
				int tex_height;
				int tex_channels;

				int required_channels = static_cast<int>(STBI_rgb_alpha);
				stbi_uc* pixels = stbi_load(file_path, &tex_width, &tex_height, &tex_channels, required_channels);
				VkDeviceSize image_size = static_cast<VkDeviceSize>(tex_width) *
					static_cast<VkDeviceSize>(tex_height) *
					static_cast<VkDeviceSize>(required_channels);

				if (!pixels)
				{
					printf("path: %s", file_path);
					throw std::runtime_error("Failed to load texture image !!!!!!");
				}

				GPUBuffer staging_buffer;

				GPUBuffer::CreateBuffer(vk_ctx, image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					staging_buffer);

				GPUBuffer::FillBufferWithData(vk_ctx->device, staging_buffer, pixels);

				stbi_image_free(pixels);

				CreateImage2D(vk_ctx, static_cast<uint32_t>(tex_width), static_cast<uint32_t>(tex_height),
					format, VK_IMAGE_TILING_OPTIMAL,
					VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
					image_sampler.image, image_sampler.memory);

				Utils::TransitionImageLayout(vk_ctx, cmd_pool,
					image_sampler.image, format,
					Utils::kImageUndefinedToDstOptimal);

				CopyBufferToImage(vk_ctx, cmd_pool, staging_buffer.buffer,
					image_sampler.image,
					static_cast<uint32_t>(tex_width),
					static_cast<uint32_t>(tex_height));

				Utils::TransitionImageLayout(vk_ctx, cmd_pool,
					image_sampler.image, format,
					Utils::kImageDstOptimalToShaderOptimalRead);

				GPUBuffer::DestoryBuffer(vk_ctx->device, staging_buffer);

				//image view for sampler
				image_sampler.imageView = CreateImageView2D(vk_ctx->device, image_sampler.image, format, VK_IMAGE_ASPECT_COLOR_BIT);

				//texture sampler for sampler
				image_sampler.sampler = CreateTextureSampler(vk_ctx, enable_anisotropy);
			}

			void CreateImageSamplerTextureCube(const VulkanContext* vk_ctx, VkCommandPool cmd_pool, std::vector<const char*> file_paths, VkFormat format, ImageSampler& image_sampler, bool enable_anisotropy)
			{
				int tex_width;
				int tex_height;
				int tex_channels;

				const int required_channels = static_cast<int>(STBI_rgb_alpha);
				//face pixels 
				//combined image pixel
				stbi_uc* first_face_pixel = stbi_load(file_paths[0], &tex_width, &tex_height, &tex_channels, required_channels);
				if (!first_face_pixel)
					throw std::runtime_error("Failed to load texture image !!!!!!");

				uint32_t per_face_size = static_cast<uint32_t>(tex_width * tex_height * required_channels);
				uint32_t offset_of_pixels = 0; // VkDeviceSize total_image_face_size = 0; {offset_of_pixels == total_image_face_size}
				std::vector<stbi_uc> pixels(per_face_size * file_paths.size());

				memcpy(pixels.data() + offset_of_pixels, first_face_pixel, per_face_size);
				stbi_image_free(first_face_pixel);
				offset_of_pixels += per_face_size;

				for (size_t i = 1; i < file_paths.size(); i++)
				{
					stbi_uc* face_pixels = stbi_load(file_paths[i], &tex_width, &tex_height, &tex_channels, required_channels);

					if (!face_pixels)
						throw std::runtime_error("Failed to load texture image !!!!!!");

					memcpy(pixels.data() + offset_of_pixels, face_pixels, per_face_size);
					stbi_image_free(face_pixels);
					offset_of_pixels += per_face_size;
				}

				GPUBuffer staging_buffer;

				//size_t offset_of_pixels = 0; // VkDeviceSize total_image_face_size = 0; {offset_of_pixels == total_image_face_size}
				GPUBuffer::CreateBuffer(vk_ctx, static_cast<VkDeviceSize>(offset_of_pixels), VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					staging_buffer);

				//fill staging buffer with pixels data
				GPUBuffer::FillBufferWithData(vk_ctx->device, staging_buffer, pixels.data());
				//stbi_image_free(pixels_data);

				CreateImage2D_4_CUBE(vk_ctx, static_cast<uint32_t>(tex_width), static_cast<uint32_t>(tex_height),
					format, VK_IMAGE_TILING_OPTIMAL,
					VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
					image_sampler.image, image_sampler.memory);

				Utils::LayoutTransitionInfo transition_info = Utils::kImageUndefinedToDstOptimal;
				transition_info.subRangeLayerCount = static_cast<uint32_t>(file_paths.size());

				Utils::TransitionImageLayout(vk_ctx, cmd_pool,
					image_sampler.image, format,
					transition_info);


				CopyBufferToMultiImages(vk_ctx, cmd_pool, staging_buffer.buffer,
					image_sampler.image, static_cast<uint32_t>(tex_width), static_cast<uint32_t>(tex_height),
					per_face_size, file_paths.size());


				transition_info = Utils::kImageDstOptimalToShaderOptimalRead;
				transition_info.subRangeLayerCount = static_cast<uint32_t>(file_paths.size());
				Utils::TransitionImageLayout(vk_ctx, cmd_pool,
					image_sampler.image, format,
					transition_info);

				GPUBuffer::DestoryBuffer(vk_ctx->device, staging_buffer);

				//create image view for sampler and sampler
				image_sampler.imageView = CreateImageViewCube(vk_ctx->device, image_sampler.image, format, VK_IMAGE_ASPECT_COLOR_BIT);
				image_sampler.sampler = CreateTextureSampler(vk_ctx, enable_anisotropy, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
			}



		} //# Resources
	} //#Vulkan
}//#Fluxon