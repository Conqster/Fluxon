#pragma once

#include "Resources.h"


#include <array>
#include "VulkanContext.h"
#include "FrameResources.h"


namespace Fluxon {
	namespace Vulkan {


		struct DebugMaterial {
			glm::vec3 colour = glm::vec3(2.0f);
			float metallic = 0.2f; //<-- hack remove later
			float roughness = 1.0f;
			float ao = 1.0f;

			glm::vec2 _padding;
		};

#define DEBUG_MAT_HACK 0

		constexpr int SHADER_PBR_MATERIAL_MAX_MAPS = 3;
		struct PBRMaterial
		{
			Resources::ImageSampler albedo;
			Resources::ImageSampler normal;
			/// <summary>
			/// aoRM
			/// ao	-> ambient occulsion
			/// R	-> Rougness
			/// M	-> metal
			/// </summary>
			Resources::ImageSampler aoRM;


			//hack for layout 
			VkImageLayout imagelayout;

#if DEBUG_MAT_HACK
			DebugMaterial hackDebugMaterial{};
			GPUBuffer hackDebugMaterialGPUBuffer{};
#endif // DEBUG_MAT_HACK


			static void DestoryPBRMaterial(VulkanContext* vk_ctx, PBRMaterial& mat) {
				Vulkan::Resources::DestoryTextureImageSampler(vk_ctx->device, mat.albedo);
				Vulkan::Resources::DestoryTextureImageSampler(vk_ctx->device, mat.normal);
				Vulkan::Resources::DestoryTextureImageSampler(vk_ctx->device, mat.aoRM);
#if DEBUG_MAT_HACK
				Vulkan::DestoryBuffer(vk_ctx->device, mat.hackDebugMaterialGPUBuffer);
#endif // DEBUG_MAT_HACK

			}

			//static PBRMaterial CreatePBRMaterialMaps(VulkanContext* vk_ctx, VkCommandPool cmd_pool, std::array<const char*, SHADER_PBR_MATERIAL_MAX_MAPS> file_paths, VkFormat tex_image_format = VK_FORMAT_R8G8B8A8_SRGB, bool enable_anisotropu_sampling = true) {
			static PBRMaterial CreatePBRMaterialMaps(VulkanContext* vk_ctx, VkCommandPool cmd_pool, std::array<std::string, SHADER_PBR_MATERIAL_MAX_MAPS> file_paths, VkFormat tex_image_format = VK_FORMAT_R8G8B8A8_SRGB, bool enable_anisotropu_sampling = true) {
				PBRMaterial mat;
				Vulkan::Resources::CreateImageSamplerTexture(vk_ctx, cmd_pool, file_paths[0].c_str(), tex_image_format, mat.albedo, mat.imagelayout, enable_anisotropu_sampling);
				Vulkan::Resources::CreateImageSamplerTexture(vk_ctx, cmd_pool, file_paths[1].c_str(), tex_image_format, mat.normal, mat.imagelayout, enable_anisotropu_sampling);
				Vulkan::Resources::CreateImageSamplerTexture(vk_ctx, cmd_pool, file_paths[2].c_str(), tex_image_format, mat.aoRM, mat.imagelayout, enable_anisotropu_sampling);
				return mat;
			}

			/// <summary>
			/// 
			/// </summary>
			/// <param name="vk_ctx"></param>
			/// <param name="pbr_material"></param>
			/// <param name="set"></param>
			/// <param name="base_binding"> defines the starting/first binding index</param>
			/// <returns></returns>
			static DescriptorSetSpecification GenerateMaterialDecriptorSetLayoutSpecification(VulkanContext* vk_ctx, PBRMaterial& pbr_material, uint32_t set, uint32_t base_binding = 0) {

#if DEBUG_MAT_HACK
				std::array<VkDescriptorSetLayoutBinding, SHADER_PBR_MATERIAL_MAX_MAPS + 1> material_set_layout_bindings = {};
#else
				std::array<VkDescriptorSetLayoutBinding, SHADER_PBR_MATERIAL_MAX_MAPS> material_set_layout_bindings = {};
#endif // DEBUG_MAT_HACK

				for (uint32_t mat_binding = 0; mat_binding < SHADER_PBR_MATERIAL_MAX_MAPS; mat_binding++) // MAPs are assign binds in order 0, 1, 2 ..... N {N = number of materials}
				{
					auto& mat_set_layout_binding = material_set_layout_bindings[mat_binding];
					mat_set_layout_binding.binding = base_binding + mat_binding;
					mat_set_layout_binding.descriptorCount = 1;
					mat_set_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					mat_set_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
					mat_set_layout_binding.pImmutableSamplers = VK_NULL_HANDLE;
				}

#if DEBUG_MAT_HACK
				VkDeviceSize required_size = sizeof(DebugMaterial);
				CreateBuffer(vk_ctx, static_cast<VkDeviceSize>(required_size), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					pbr_material.hackDebugMaterialGPUBuffer);

				MapBufferMemory(vk_ctx->device, pbr_material.hackDebugMaterialGPUBuffer);

				auto& mat_set_layout_binding = material_set_layout_bindings[material_set_layout_bindings.size() - 1]; //<- hack to last binding 
				mat_set_layout_binding.binding = base_binding + material_set_layout_bindings.size() - 1;
				mat_set_layout_binding.descriptorCount = 1;
				mat_set_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				mat_set_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
				mat_set_layout_binding.pImmutableSamplers = VK_NULL_HANDLE;
#endif // DEBUG_MAT_HACK


				VkDescriptorSetLayoutCreateInfo layout_create_info{};
				layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				layout_create_info.flags = 0;
				layout_create_info.bindingCount = material_set_layout_bindings.size();
				layout_create_info.pBindings = material_set_layout_bindings.data();

				DescriptorSetSpecification set_specification;
				set_specification.set = set;
				if (vkCreateDescriptorSetLayout(vk_ctx->device, &layout_create_info, nullptr, &set_specification.layoutHandle) != VK_SUCCESS)
					throw std::runtime_error("Failed to create MATERIAL descriptor set layout!!!!!!!!");
				return set_specification;
			}



#if DEBUG_MAT_HACK
			static  std::array<DescriptorBinding, SHADER_PBR_MATERIAL_MAX_MAPS + 1>& GetDescriptorSetBindings(PBRMaterial& pbr_material, uint32_t set, uint32_t base_binding = 0) {

				std::array<DescriptorBinding, SHADER_PBR_MATERIAL_MAX_MAPS + 1> desc_bindings = {
					Vulkan::DescriptorBinding::MakeImageSampler(set, base_binding + 0, pbr_material.albedo),
					Vulkan::DescriptorBinding::MakeImageSampler(set, base_binding + 1, pbr_material.normal),
					Vulkan::DescriptorBinding::MakeImageSampler(set, base_binding + 2, pbr_material.aoRM),

					Vulkan::DescriptorBinding::MakeUniformBuffer(set, base_binding + 3, pbr_material.hackDebugMaterialGPUBuffer),
				};
				return desc_bindings;
			}
#else
			static  std::array<DescriptorBinding, SHADER_PBR_MATERIAL_MAX_MAPS> GetDescriptorSetBindings(PBRMaterial& pbr_material, uint32_t set, uint32_t base_binding = 0) {
				std::array<DescriptorBinding, SHADER_PBR_MATERIAL_MAX_MAPS> desc_bindings = {
					Vulkan::DescriptorBinding::MakeImageSampler(set, base_binding + 0, pbr_material.albedo),
					Vulkan::DescriptorBinding::MakeImageSampler(set, base_binding + 1, pbr_material.normal),
					Vulkan::DescriptorBinding::MakeImageSampler(set, base_binding + 2, pbr_material.aoRM),
				};
				return desc_bindings;
			}
#endif // DEBUG_MAT_HACK






		};
	} //#Vulkan
}//#Fluxon