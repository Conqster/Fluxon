#pragma once
#include "Renderer/PBRMaterial.h"
#include "Renderer/VulkanContext.h"

namespace Fluxon {

	struct MaterialDescriptor
	{
		enum class ETextureMap : uint32_t
		{
			TMap_Albedo = 0,
			TMap_Normal,
			TMap_aoRM,

			TMap_Count
		};

		int32_t id = -1;
		//in order of albedo, normal, aoRM
		//std::array<const char*, static_cast<size_t>(ETextureMap::TMap_Count)> pbrPaths;
		//std::array<char[128], static_cast<size_t>(ETextureMap::TMap_Count)> pbrPaths;
		std::array<std::string, static_cast<size_t>(ETextureMap::TMap_Count)> pbrPaths;
		VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
		bool anisotropicSampling = true;
	};

	struct MaterialResource
	{
		MaterialDescriptor materialDesc;
		Vulkan::PBRMaterial gpuMaterial;

		static MaterialResource CreatePBRMaterialResource(Vulkan::VulkanContext* vk_ctx, VkCommandPool cmd_pool, MaterialResource& mat_reso) {

			mat_reso.gpuMaterial = Vulkan::PBRMaterial::CreatePBRMaterialMaps(vk_ctx, cmd_pool, mat_reso.materialDesc.pbrPaths, mat_reso.materialDesc.format, mat_reso.materialDesc.anisotropicSampling);

			return mat_reso;
		}


		//static MaterialResource CreatePBRMaterialResource(Vulkan::VulkanContext* vk_ctx, VkCommandPool cmd_pool, std::array<const char*, Vulkan::SHADER_PBR_MATERIAL_MAX_MAPS> file_paths, VkFormat tex_image_format = VK_FORMAT_R8G8B8A8_SRGB, bool enable_anisotropu_sampling = true) {
		static MaterialResource CreatePBRMaterialResource(Vulkan::VulkanContext* vk_ctx, VkCommandPool cmd_pool, std::array<std::string, Vulkan::SHADER_PBR_MATERIAL_MAX_MAPS> file_paths, VkFormat tex_image_format = VK_FORMAT_R8G8B8A8_SRGB, bool enable_anisotropu_sampling = true) {
			
			MaterialResource mat_reso;

			mat_reso.materialDesc.pbrPaths = file_paths;
			mat_reso.materialDesc.anisotropicSampling = enable_anisotropu_sampling;
			mat_reso.materialDesc.format = tex_image_format;

			mat_reso.gpuMaterial = Vulkan::PBRMaterial::CreatePBRMaterialMaps(vk_ctx, cmd_pool, mat_reso.materialDesc.pbrPaths, mat_reso.materialDesc.format, mat_reso.materialDesc.anisotropicSampling);

			return mat_reso;
		}
	};

} //namespace Fluxon