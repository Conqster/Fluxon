#pragma once
#include <vulkan/vulkan.h>

#include <vector>

#include <imgui/imgui.h>
#include <imgui_impl_vulkan.h>

#include "Core/Core.h"

namespace Fluxon {
	namespace Vulkan {

		struct VulkanContext;

		namespace Resources {

			struct ImageSampler
			{
				VkImage image = VK_NULL_HANDLE;
				VkDeviceMemory memory = VK_NULL_HANDLE;
				VkImageView imageView = VK_NULL_HANDLE;
				VkSampler sampler = VK_NULL_HANDLE;
			};



			//struct SamplerCubeImageFacePaths
			//{
			//	const char* left;
			//	const char* right;
			//	const char* left;
			//	const char* left;
			//};

			void DestoryTextureImageSampler(VkDevice device, ImageSampler& img_sampler);

			VkImageView CreateImageView(VkDevice device, VkImage image, VkImageViewType view_type, VkFormat format, VkImageAspectFlags aspect_flags, uint32_t layer_count = 1);



			void CreateImage(const VulkanContext* vk_ctx, uint32_t width, uint32_t height, VkImageType type, uint32_t array_layer, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
				VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& image_memory, VkImageCreateFlags create_info_flags = 0);

			//out image layout for utility 
			void CreateImage2DFromFile(const VulkanContext* vk_ctx, VkCommandPool cmd_pool, const char* file_path, VkFormat format, VkImage& out_image, 
				VkDeviceMemory& out_image_memory, uint32& out_width, uint32& out_height, VkImageLayout& out_image_layout, bool enable_anisotropy = true);

			void CreateImageCubeFromFile(const VulkanContext* vk_ctx, VkCommandPool cmd_pool, std::vector<const char*> file_paths, VkFormat format, VkImage& out_image, 
				VkDeviceMemory& out_image_memory, uint32& out_width, uint32& out_height, VkImageLayout& out_image_layout, bool enable_anisotropy = true);


			void CopyBufferToImage(const VulkanContext* vk_ctx, VkCommandPool cmd_pool, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
			void CopyBufferToMultiImages(const VulkanContext* vk_ctx, VkCommandPool cmd_pool, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height,
				VkDeviceSize img_stride, uint32_t img_count);

			void CopyBuffer(const VulkanContext* vk_ctx, VkCommandPool cmd_pool, VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);

			/// <summary>
			/// 
			/// </summary>
			/// <param name="vk_ctx"></param>
			/// <param name="enable_anisotropy"></param>
			/// <param name="filter_mode"> default to VK_FILTER_LINEAR and value applys to both mag and min filter </param>
			/// <param name="address_mode"> default to VK_SAMPLER_ADDRESS_MODE_REPEAT and value all axes UVW</param>
			/// <returns></returns>
			VkSampler CreateTextureSampler(const VulkanContext* vk_ctx, bool enable_anisotropy, VkFilter filter_mode = VK_FILTER_LINEAR, VkSamplerAddressMode address_mode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
				VkBorderColor border_colour = VK_BORDER_COLOR_INT_OPAQUE_BLACK, bool enable_compare_op = false, VkCompareOp compare_op = VK_COMPARE_OP_ALWAYS);

			void CreateImageSamplerTexture(const VulkanContext* vk_ctx, VkCommandPool cmd_pool, const char* file_path, VkFormat format, ImageSampler& out_image_sampler, bool enable_anisotropy = true);
			void CreateImageSamplerTexture(const VulkanContext* vk_ctx, VkCommandPool cmd_pool, const char* file_path, VkFormat format, ImageSampler& out_image_sampler, VkImageLayout& out_image_layout, bool enable_anisotropy = true);
			void CreateImageSamplerTextureCube(const VulkanContext* vk_ctx, VkCommandPool cmd_pool, std::vector<const char*> file_paths, VkFormat format, ImageSampler& image_sampler, bool enable_anisotropy);

			//Utils 
			static VkImageView CreateImageView2D(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspect_flags) {
				return CreateImageView(device, image, VK_IMAGE_VIEW_TYPE_2D, format, aspect_flags);
			}

			static VkImageView CreateImageView2DArray(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspect_flags, uint32_t layer_count) {
				return CreateImageView(device, image, VK_IMAGE_VIEW_TYPE_2D_ARRAY, format, aspect_flags, layer_count);
			}

			static VkImageView CreateImageViewCube(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspect_flags) {
				return CreateImageView(device, image, VK_IMAGE_VIEW_TYPE_CUBE, format, aspect_flags, 6);
			}

			static void CreateImage2D(const VulkanContext* vk_ctx, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
				VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& image_memory) {

				CreateImage(vk_ctx, width, height, VK_IMAGE_TYPE_2D, 1, format, tiling, usage, properties, image, image_memory, 0);
			}

			static void CreateImage2DArray(const VulkanContext* vk_ctx, uint32_t width, uint32_t height, uint32_t array_layer, VkFormat format,
				VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& image_memory) {

				CreateImage(vk_ctx, width, height, VK_IMAGE_TYPE_2D, array_layer, format, tiling, usage, properties, image, image_memory, 0);
			}

			//TO-DO(Conqster): Might need to fizx CreateImage2D_4_CUBE only have 6 layer = faces 
			static void CreateImage2D_4_CUBE(const VulkanContext* vk_ctx, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
				VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& image_memory) {

				CreateImage(vk_ctx, width, height, VK_IMAGE_TYPE_2D, 6, format, tiling, usage, properties, image, image_memory, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT);
			}





			struct ImGui_Texture_Test
			{
				VkDescriptorSet desc;         // Descriptor set: this is what you'll pass to Image()
				int             width = 900;
				int             height = 900;
				int             channels;


				ImageSampler imageSampler;

				ImGui_Texture_Test() { memset(this, 0, sizeof(*this)); }

				static void CreateTexture(const VulkanContext* vk_ctx, VkCommandPool cmd_pool, const char* file_path, VkFormat format, ImGui_Texture_Test& texture, bool enable_anisotropy) {

					CreateImageSamplerTexture(vk_ctx, cmd_pool, file_path, format, texture.imageSampler, enable_anisotropy);

					texture.desc = ImGui_ImplVulkan_AddTexture(texture.imageSampler.sampler, texture.imageSampler.imageView,
						VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

					texture.width = 300;
					texture.height = 300;
				}
				static void Destroy(VkDevice device, ImGui_Texture_Test& texture){
					DestoryTextureImageSampler(device, texture.imageSampler);
					ImGui_ImplVulkan_RemoveTexture(texture.desc);
				}
			};


		} //# Resources
	} //#Vulkan
}//#Fluxon