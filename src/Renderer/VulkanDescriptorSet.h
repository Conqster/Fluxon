#pragma once



#include <unordered_map>
#include <string>


#include <vulkan/vulkan.h>

#include "GPUBuffer.h"

#include "Renderer/Resources.h"

#include "Core/Enums.h"

#include <unordered_set>

#include "VulkanTexture.h"

namespace Fluxon {
	namespace Vulkan {

		//a layout could be shared / mix and match 
		//with multi descriptor set
		//does not care what set it for specific shader/pipeline
		//struct VulkanDescriptorLayout
		//{
		//	std::string name;
		//	VkDescriptorSetLayout layout = VK_NULL_HANDLE;
		//};


				//struct DataInfo
		//{
		//	EDescriptorType type = EDescriptorType::UniformBuffer;
		//	union {
		//		VkDescriptorBufferInfo bufferInfo;
		//		VkDescriptorImageInfo imageInfo;
		//	};
		//};



		enum EDescriptorType : uint8
		{
			UniformBuffer,
			CombinedImageSampler, //combined image sampler is used when image is combined with sampler
			ImageStorage, 

			ExternalImageSampler
		};


		struct GPUImageSampler
		{
			VkImage image = VK_NULL_HANDLE;
			VkImageLayout layout ;
			VkImageView imageView = VK_NULL_HANDLE;
			VkSampler sampler = VK_NULL_HANDLE;
			VkDeviceMemory memory = VK_NULL_HANDLE;


			void Destory(const VkDevice& device)
			{
				if(sampler != VK_NULL_HANDLE)
					vkDestroySampler(device, sampler, nullptr);
				if(imageView != VK_NULL_HANDLE)
					vkDestroyImageView(device, imageView, nullptr);
				if(image != VK_NULL_HANDLE)
					vkDestroyImage(device, image, nullptr);
				if (memory != VK_NULL_HANDLE)
					vkFreeMemory(device, memory, nullptr);
			}
		};



		//later lookup could be shared to get only set without binding
		//make binding = -1 etc 
		struct DescriptorLookupValue
		{
			uint32 set;
			uint32 binding;
		};





		struct DescriptorBindingInfo
		{
			std::string name;
			VkDescriptorType type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

			//DataInfo data;
			EDescriptorType descType = EDescriptorType::UniformBuffer;
			
			TextureSpecification tex_spec = {};
			GPUImageSampler externalImageSampler{}; //<-- for image sampler managed externally
			uint32 bufferSize = 0;

			uint32 binding = 0;
			VkShaderStageFlags stageFlag = VK_SHADER_STAGE_VERTEX_BIT;
		};


		struct DescriptorLayoutInfo
		{
			std::string name;
			//std::vector<std::string> requiredLayouts;
			//std::vector<uint32> requiredDescriptorDataByIdx;
			std::vector<DescriptorBindingInfo> requiredBindings;
		};

		enum class EDescriptorBuffering : uint32
		{
			Single = 1, 
			Double = 2, 
			//Triple = 3
		};

		struct DescriptorSetInfo
		{
			std::string name;
			uint32 set = 0;
			std::vector<std::string> requiredLayouts;

			EDescriptorBuffering buffering = EDescriptorBuffering::Double;

			//creation 
			VkDescriptorSet handle = VK_NULL_HANDLE;
		};



		struct DescriptorSpecification
		{
			//std::vector<DescriptorBindingInfo> descriptors;
			std::vector<DescriptorLayoutInfo> layoutInfos;
			std::vector<DescriptorSetInfo> setsInfos;

			bool bTightFitDescriporPool = true;
			bool bHasDescriptorPool = true;
			VkDescriptorPool* pDescriptorPool = nullptr;
			uint32 frameInFlight = 1;
		};



		//////////////
		// Storage data
		//////////////
		//a descriptor set data is fixed
		struct DescriptorSetData
		{
			std::string debugName;
			uint32 set = 0;
			VkDescriptorSet handle;

			uint32 layoutIdx = UINT32_MAX;

			EDescriptorBuffering buffering = EDescriptorBuffering::Double;

			struct Binding
			{
				std::string debugName;
				uint32 index = UINT32_MAX;
				EDescriptorType type = EDescriptorType::UniformBuffer;
				ShaderStageFlag shaderStageFlag = EShaderStageBit::Vertex;

				std::vector<GPUBuffer> uniformBuffers;
				GPUImageSampler imageSampler;
			};

			std::vector<Binding> bindings;


			void Destory(const VulkanContext* vk_ctx)
			{
				for (auto& bind : bindings)
				{
					if(bind.type == EDescriptorType::UniformBuffer)
						for (auto& buff : bind.uniformBuffers)
							GPUBuffer::DestoryBuffer(vk_ctx->device, buff);

					if(bind.type == EDescriptorType::ImageStorage ||
						bind.type == EDescriptorType::CombinedImageSampler)
						bind.imageSampler.Destory(vk_ctx->device);
				}
			}
		};





		//for caching for now 
		struct DescriptorLayoutData
		{
			std::string name;
			VkDescriptorSetLayout handle;
			//later mabe instead of SetData holding the binding layout holds the bindings 
			//std::vector<uint32> requiredDescriptorDataByIdx;
			uint32 binCacheOffset = UINT32_MAX;
			uint32 binCacheStride = UINT32_MAX;
		};


		struct PendingWrite
		{
			//VkDescriptorSet setHandle = VK_NULL_HANDLE;
			//handle cant work because after all pushes 
			//the handle would be updated which break vk pointer handle 
			uint32 setIdx = UINT32_MAX;
			EDescriptorType descriptorType = EDescriptorType::UniformBuffer;
			int32 binding = -1;
			VkDescriptorImageInfo imageInfo{};
			VkDescriptorBufferInfo bufferInfo{};
		};





		//class is not tread safe
		//VulkanDescriptorSetManag3efr
		class VulkanDescriptorSet
		{
		public:
			VulkanDescriptorSet() {};
			VulkanDescriptorSet(VulkanContext* vk_ctx, const DescriptorSpecification& desc_set_spec) {
				Create(vk_ctx, desc_set_spec);
			}


			void Create(VulkanContext* vk_ctx, const DescriptorSpecification& desc_set_spec);



			inline DescriptorSetData* GetDescriptorSetData(const std::string& name) {
				auto& it = mDescriptorSetIdxMap.find(name);

				if (it != mDescriptorSetIdxMap.end())
				{
					FLUX_DEBUG_ASSERT_CRITICAL(it->second < (uint32)mDescriptorSetData.size(), "retrived descriptor set idx out of bounds");
					return &mDescriptorSetData[it->second];
				}

				FLUX_DEBUG_ASSERT(false, "Descriptor set ", name, " does not exist");
				return nullptr;
			}


			const VkDescriptorSet GetHandle(const std::string& name) const { 

				//auto desc_set = GetDescriptorSetData(name);
				//if (desc_set != nullptr)
				//	return desc_set->handle;

				//return GetDescriptorSetData(name).handle;

				auto& it = mDescriptorSetIdxMap.find(name);

				if (it != mDescriptorSetIdxMap.end())
				{
					FLUX_DEBUG_ASSERT_CRITICAL(it->second < (uint32)mDescriptorSetData.size(), "retrived descriptor set idx out of bounds");
					return mDescriptorSetData[it->second].handle;
				}

				FLUX_DEBUG_ASSERT(false, "Descriptor set ", name, " does not exist");

				return VK_NULL_HANDLE;
			}

			VkDescriptorSet& GetHandleByIdx(uint32 idx) { return mDescriptorSetData[idx].handle; }


			VkDescriptorSetLayout GetSetLayoutHandle(const std::string& name) {
				//auto& it = mLayoutsIdxMap.find(name);

				//if (it != mLayoutsIdxMap.end())
				//{
				//	FLUX_DEBUG_ASSERT_CRITICAL(it->second < mLayouts.size(), "layout map idx is out of bounds layout data");
				//	return mLayouts[it->second].handle;
				//}



				auto& it = mDescriptorSetIdxMap.find(name);
				if (it != mDescriptorSetIdxMap.end())
				{
					FLUX_DEBUG_ASSERT_CRITICAL(it->second < (uint32)mDescriptorSetData.size(), "retrived descriptor set idx out of bounds");
					
					uint32 layout_idx = mDescriptorSetData[it->second].layoutIdx;

					FLUX_DEBUG_ASSERT_CRITICAL(layout_idx != UINT32_MAX && 
						layout_idx < (uint32)mLayouts.size(), "invalid set layout or out of bounds");

					return mLayouts[layout_idx].handle;
				}


				FLUX_DEBUG_ASSERT(false, "set layout ", name, " does not exist");
				return VK_NULL_HANDLE;
			}


			VkDescriptorSetLayout GetLayoutHandle(const std::string& name) {
				auto& it = mLayoutsIdxMap.find(name);

				if (it != mLayoutsIdxMap.end())
				{
					FLUX_DEBUG_ASSERT_CRITICAL(it->second < mLayouts.size(), "layout map idx is out of bounds layout data");
					return mLayouts[it->second].handle;
				}
				FLUX_DEBUG_ASSERT(false, "set layout ", name, " does not exist");
				return VK_NULL_HANDLE;
			}


			void UpdateUBOBinding(const std::string& name, const void* data, uint32 frame_idx = 0);
			
	
			//later support the ability to push new layout / set 
			//genrate a large pool, then push new datas(ubo, ssbo, texture) would be easy

			//if set does not exist a new set would be 
			void PushUBOBinding(const std::string& name, const std::string& set_name, uint32 binding, 
				uint32 buffer_size, const ShaderStageFlag& shaderStageFlag = EShaderStageBit::Vertex);
			void PushStorageTextureBinding(const std::string& name, const std::string& set_name, uint32 binding, 
				const TextureSpecification& tex_spec, const ShaderStageFlag& shaderStageFlag = EShaderStageBit::Vertex);

			void PushTextureBinding(const std::string& name, const std::string& set_name, uint32 binding,
				const Resources::ImageSampler& image_sampler, const VkImageLayout& image_layout = VK_IMAGE_LAYOUT_UNDEFINED, 
				const ShaderStageFlag& shaderStageFlag = EShaderStageBit::Vertex, const EDescriptorType& descriptor_type = EDescriptorType::CombinedImageSampler);


			void SetDescriptorSetBuffering(const std::string& name, const EDescriptorBuffering& buffering) {

				DescriptorSetData* desc_set = GetDescriptorSetData(name);
				if (desc_set != nullptr)
					desc_set->buffering = buffering;
				else
					FLUX_DEBUG_LOG_WARNING("Descriptor Set ", name, " does not exist");
			}

			void PushDescriptorSet(const std::string& name, uint32 gpu_set, const EDescriptorBuffering& buffering) {

				if (auto& it = mDescriptorSetIdxMap.find(name); it != mDescriptorSetIdxMap.end())
				{
					auto& v = mDescriptorSetData[it->second];
					
					FLUX_DEBUG_LOG_WARNING("set already exist ", name, " as: ",
						"\n\t set[", v.set, "],\n\t layout idx: ", v.layoutIdx,
						",\n\t buffering: ", static_cast<uint32>(v.buffering), ",\n\t binding count: ", static_cast<uint32>(v.bindings.size()),
						".\n return");
					return;
				}


				mDescriptorSetIdxMap[name] = mDescriptorSetData.size();
				DescriptorSetData set_data{};
				set_data.debugName = name;
				set_data.set = gpu_set;
				set_data.buffering = buffering;
				mDescriptorSetData.emplace_back(set_data);
			}
			//void PushImageSampler(const std::string& name, const std::string& set_name, uint32 binding, uint32 buffer_size);
			void BakePendingWrites();


			void Destory();

		private: 

			static constexpr uint32 kMaxDescriptorInfo = 20;
			VulkanContext* pGraphicsContext = nullptr;
			EDescriptorBuffering mFrameBuffering = EDescriptorBuffering::Double;

			void GenerateDescriptorSetLayout(const VulkanContext* vk_ctx, const DescriptorSpecification& desc_set_spec);
			void GenerateTightFitDescriptorPool(const VulkanContext* vk_ctx, const DescriptorSpecification& desc_set_spec);
			void GenerateLargeDescriptorPool(const VulkanContext* vk_ctx);
			void GenerateSets(const VulkanContext* vk_ctx, const DescriptorSpecification& desc_set_spec);
			

			void WriteGeneretedDescriptorSets(const VulkanContext* vk_ctx, std::vector<DescriptorSetData>& update_descriptor_set);

			DescriptorSetData* FindBindingSet(const std::string& name, const std::string& set_name, uint32 binding);

			//this checks the desctiptot set bindinf for overlaping 
			//
			DescriptorSetErrorFlag ResolveDescriptorSet(const DescriptorSetData& descriptor_set);


			//this is to aid that the lookup set idx match the data 
			void ResolveDescriptorSetOrder();

			//descriptor set a stored contigous in a vector array 
			//mDescriptorSetIdxMap maps name to teh idx in its std::vector
			//if using multi frames in flight 
			//mDescriptorSetIdxMap returns the first frame flight for data
			// to get next flight would be + curr frame idx to get teh VKDEscriptor 
			// mvp 0 | mvp 1 | shadow map 0 | shadow map 1
			//over 
			// mvp 0 | shadow map 0 | mvp 1| shadow map 1 <=== has this required to number of availale sets 
			std::vector<DescriptorSetData> mDescriptorSetData;
			std::unordered_map<std::string, uint32> mDescriptorSetIdxMap;
			std::unordered_map<std::string, DescriptorLookupValue> mBindingLookup;

			std::unordered_set<uint32> mDirtyDescriptorSetIdx;
			//std::vector<DescriptorSetInfo> mDescriptorSet;
			
			//std::unordered_map<std::string, u32> mUniformBuffersIdxMap;
			//u32 mUniformBufferCount = 0;
			//std::vector<GPUBuffer> mUniformBuffers;

			//std::unordered_map<std::string, VkDescriptorSetLayout> mLayouts;
			//std::unordered_map<std::string, DescriptorLayoutData> mLayoutsData;

			std::vector<DescriptorLayoutData> mLayouts;
			std::unordered_map<std::string, uint32> mLayoutsIdxMap;

			//std::vector<DescriptorInfo> mCacheDescriptorData;


			VkDescriptorPool mDescriptorPool = VK_NULL_HANDLE;


			DescriptorSetData::Binding CreateUBODescriptorBinding(const VulkanContext* vk_ctx, const DescriptorSetInfo& set_info, 
				const DescriptorBindingInfo& binding_info, const EDescriptorBuffering& buffering);

			std::vector<DescriptorBindingInfo> mOnloadBinningBindings;

			void CreateSetLayout();
		};
	}//namespace Vulkan
} //namespace Fluxon
