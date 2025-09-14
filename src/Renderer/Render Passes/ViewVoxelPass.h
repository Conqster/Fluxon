#pragma once

#include "IRenderPass.h"
#include <vulkan/vulkan.h>
#include "Renderer/Pipelines/GraphicsPipeline.h"

#include "Renderer/RenderTarget.h"

#include "Renderer/GPUBuffer.h"

#include "Renderer/FrameResources.h"

#include "Core/Camera.h"

#include "Renderer/VulkanTexture.h"


#include "Renderer/VulkanDescriptorSet.h"


namespace Fluxon {
	class Camera;
}
namespace Fluxon {
	namespace Vulkan {


		/// <summary>
		/// Maybe Collisions Voxel Pass
		/// </summary>
		class SceneVoxelPass : public IRenderPass
		{
		private:
			VkRenderPass mRenderPass;

			GraphicsPipeline mGfxPipeline;

			RenderTarget mRenderTarget;
			Framebuffer mFramebuffer;

			VkExtent2D mVoxelImageExtent{128, 128};
			uint32_t mVoxelImageDepth = 128;

			//std::array<GPUBuffer, kMaxFramesInFlight> mCameraBufferdata;
			//std::array<GPUBuffer, kMaxFramesInFlight> mVoxelParamBufferdata;

			//std::array<VulkanTexture, kMaxFramesInFlight> mVoxelOccupancyTex;

			DynamicCamera mCamera = DynamicCamera(ECameraType::Orthographic, 0.1f, 50.0f, 6.4f);


			//VkDescriptorPool mDescriptorPool = VK_NULL_HANDLE;
			//VkDescriptorSetLayout mDescriptorSetLayout = VK_NULL_HANDLE;
			//std::array<VkDescriptorSet, kMaxFramesInFlight> mDescriptorSets;

			VulkanDescriptorSet mDescriptorSet;

			uint32_t mFrameIndex = 0;

		public:
			void Init(class VulkanContext* vk_ctx, VkCommandPool cmd_pool, 
				const Camera* scene_cam = nullptr, const GPUStructures::VoxelParameterUBO& voxel_data = {});
			void Init(class VulkanContext* vk_ctx) override {
				printf("{void Init(class VulkanContext* vk_ctx)} not supported by ShadowRenderPass Init \n use {void Init(class VulkanContext* vk_ctx, VkCommandPool cmd_pool)}\n");
			};

			VkRenderPass& GetRenderPassHandle() { return mRenderPass; }
			RenderTarget& GetDepthRenderTarget() { return mRenderTarget; }

			void Begin(VulkanContext* ctx, VkCommandBuffer cmd_buff, uint32_t img_idx);
			void End(VkCommandBuffer cmd_buff);

			void UpdateCameraData(const Camera* p_scene_cam = nullptr, float lf = -6.4f, float rt = 6.4f, 
				float bm = -6.4f, float tp = 6.4f, float nearz = -6.4f, float farz = 6.4f);
			void UpdateVoxelParamData(const GPUStructures::VoxelParameterUBO& data);
			void PushModelTransformConstant(VkCommandBuffer cmd_buff, const GPUStructures::Model* p_value);

			void Cleanup(class VulkanContext* ctx);

		private:
			void CreateGfxPipeline(const VulkanContext* vk_ctx);
			//bool CreateDescriptorSetLayouts(const VulkanContext* vk_ctx);

			//bool CreateGPUbuffers(const VulkanContext* vk_ctx);
		};


	} //namespace Vulkan
} // namespace fluxon




//
//struct DescriptorSpecification
//{
//	std::string name;
//	uint32_t set = 0;
//	uint32_t binding = 0;
//	VkDescriptorType type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//	VkShaderStageFlags stageFlag = VK_SHADER_STAGE_VERTEX_BIT;
//};
//
////layout
//struct DescriptorSet
//{
//	std::vector<VkDescriptorSetLayoutBinding> mBindings;
//
//	void PushBinding(const DescriptorSpecification& desc_spec)
//	{
//		mBindings.push_back({});
//		auto& binding = mBindings.back();
//
//		binding.binding = desc_spec.binding;
//		binding.descriptorCount = 1;
//		binding.descriptorType = desc_spec.type;
//		binding.pImmutableSamplers = nullptr;
//		binding.stageFlags = desc_spec.stageFlag;
//	}
//};
//
//
//enum EDescriptorType : uint32_t
//{
//	EDT_Uniformbuffer,
//	EDT_SampledImage,
//	EDT_Storagebuffer,
//
//	Count
//};
//
//class DescriptorPool
//{
//public: 
//
//	VkDescriptorPool mVkDescPoolHandle;
//
//	std::vector<VkDescriptorSetLayout> mSetLayout;
//
//	std::vector<DescriptorSet> mSets;
//
//	std::vector<VkDescriptorSet> mVkSetHandle;
//
//
//	/// mDescTypesCount trackes the number of 
//	/// descriptor type to create pool to accommdate them
//	/// idx 0: VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
//	/// idx 1: VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE
//	/// idx 2: VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
//	std::array<uint32_t, 3> mDescTypesCount{ 0,0,0 };
//
//
//	void PushDescriptor(const DescriptorSpecification& desc_spec)
//	{
//		uint32_t idx = 0;
//		if (desc_spec.type != VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
//			idx = (desc_spec.type == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE) ? 1 : 2;
//
//		if (desc_spec.set <= mSets.size())
//			mSets.resize(desc_spec.set + 1);
//
//		mSets[desc_spec.set].PushBinding(desc_spec);
//
//		mDescTypesCount[idx]++;
//	}
//
//	void BakeDescriptorSets(const Fluxon::Vulkan::VulkanContext* vk_ctx)
//	{
//		for (size_t i = 0; i < mSets.size(); ++i)
//		{
//			auto& set = mSets[i];
//
//			VkDescriptorSetLayoutCreateInfo layoutInfo{};
//			layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
//			layoutInfo.bindingCount = static_cast<uint32_t>(set.mBindings.size());
//			layoutInfo.pBindings = set.mBindings.data();
//
//
//			if (vkCreateDescriptorSetLayout(vk_ctx->device, &layoutInfo, nullptr, &mSetLayout[i]) != VK_SUCCESS) {
//				throw std::runtime_error("failed to create descriptor set layout!");
//			}
//		}
//	}
//
//	void BakeDescriptorPool(VkDevice device, uint32_t max_set, uint16_t allo_desc_count = 1, VkDescriptorSet* pvk_desc_set = nullptr)
//	{
//		std::array<VkDescriptorPoolSize, 3> pool_sizes{ };
//
//		for (size_t i = 0; i < mDescTypesCount.size(); ++i)
//		{
//			VkDescriptorType type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//			if (i != 0)
//				type = (type == 1) ? VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
//			pool_sizes[i].type = type;
//			pool_sizes[i].descriptorCount = mDescTypesCount[i];
//		}
//
//		VkDescriptorPoolCreateInfo pool_create_info{};
//		pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
//		pool_create_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
//		pool_create_info.pPoolSizes = pool_sizes.data();
//		pool_create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
//		pool_create_info.maxSets = max_set;
//
//		if (vkCreateDescriptorPool(device, &pool_create_info, nullptr, &mVkDescPoolHandle) != VK_SUCCESS)
//			throw std::runtime_error("Failed to Create Descriptor Pool!!!!!!!");
//	
//	
//		const auto& p_set = (pvk_desc_set != nullptr) ? pvk_desc_set : mVkSetHandle.data();
//		AllocateDescriptorSet(device, mSetLayout, p_set, allo_desc_count);
//	}
//
//
//	void AllocateDescriptorSet(VkDevice device, const std::vector<VkDescriptorSetLayout>& layouts, 
//		VkDescriptorSet* p_sets, uint32_t allo_desc_count = 1)
//	{
//		VkDescriptorSetAllocateInfo alloc_info{};
//		alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//		alloc_info.descriptorPool = mVkDescPoolHandle;
//		alloc_info.descriptorSetCount = allo_desc_count;
//		alloc_info.pSetLayouts = layouts.data();
//
//		if (vkAllocateDescriptorSets(device, &alloc_info, p_sets) != VK_SUCCESS)
//			throw std::runtime_error("Failed to allocate descriptor sets !!!!!!!");
//
//	}
//};