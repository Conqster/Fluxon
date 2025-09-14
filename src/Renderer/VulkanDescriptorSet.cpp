#include "VulkanDescriptorSet.h"

#include "VulkanContext.h"

#include <array>

#include "Core/Enums.h"

#include "VulkanTexture.h"


namespace Fluxon {
	namespace Vulkan {
		




		static std::unordered_map<EDescriptorSetErrorBit, std::string> DescriptorSetErrorBitsNames =
		{
				{None, "None"},
				{InvalidBinding, "InvalidBinding"},
				{BindingOverlap, "BindingOverlap"},
				{NoData, "NoData"},
				{TypeDoesNotMatchData, "TypeDoesNotMatchData"},
				{TypeDoesNotExist , "TypeDoesNotExist"},
		};
		
		static std::string DescriptorSetErrorFlagToString(DescriptorSetErrorFlag flag)
		{

			std::string temp = "|";
			for (const auto& [bit, name] : DescriptorSetErrorBitsNames)
			{
				if(flag.Contains(bit))
					temp += name + "|";
			}

			return temp;
		}

		
		void VulkanDescriptorSet::Create(VulkanContext* vk_ctx, const DescriptorSpecification& desc_set_spec)
		{
			mOnloadBinningBindings.clear();
			pGraphicsContext = vk_ctx;
			//
			GenerateDescriptorSetLayout(vk_ctx, desc_set_spec);

			//create based on required and available descriptors
			//if(desc_set_spec.bGeneratedTightFitPool)
			if (desc_set_spec.bHasDescriptorPool)
			{
				if (desc_set_spec.bTightFitDescriporPool)
					GenerateTightFitDescriptorPool(vk_ctx, desc_set_spec);
				else
					GenerateLargeDescriptorPool(vk_ctx);
			}
			else
			{
				FLUX_DEBUG_ASSERT_CRITICAL(desc_set_spec.pDescriptorPool != nullptr, "Create Descriptor Set is not enabled and descriptor pool pointer to use is null!!!", 
					"\n fallback large descriptor set and bHasDescriptorPool would be flagged.");

				GenerateLargeDescriptorPool(vk_ctx);
				//desc_set_spec.bHasDescriptorPool = true;
			}

			GenerateSets(vk_ctx, desc_set_spec);
			WriteGeneretedDescriptorSets(vk_ctx, mDescriptorSetData);

			mOnloadBinningBindings.clear();
			ResolveDescriptorSetOrder();
		}

		void VulkanDescriptorSet::UpdateUBOBinding(const std::string& name, const void* data, uint32 frame_idx)
		{

			//auto it = mBindingLookup.find(name);
			//if (it == mBindingLookup.end())
			//{
			//	FLUX_DEBUG_LOG("couldnt find ubo ", name, "");
			//	return;
			//}

			//const auto& look_up = it->second;
			//auto& desc_set = mDescriptorSetData[look_up.set + frame_idx];
			//auto& binding = desc_set.bindings[look_up.binding];


			//FLUX_DEBUG_ASSERT(binding.type == EDescriptorType::UniformBuffer,"couldnt find ubo ", name, "");
			//auto& buff = binding.uniformBuffer;
			//FLUX_DEBUG_ASSERT_CRITICAL(buff.IsMapped(), "buffer memory is not mapped");
			//memcpy(buff.mapped, data, buff.size);




			auto it = mBindingLookup.find(name);
			if (it == mBindingLookup.end())
			{
				FLUX_DEBUG_LOG("couldnt find ubo ", name, "");
				return;
			}

			const auto& look_up = it->second;
			auto& desc_set = mDescriptorSetData[look_up.set];
			auto& binding = desc_set.bindings[look_up.binding];


			FLUX_DEBUG_ASSERT(binding.type == EDescriptorType::UniformBuffer, "couldnt find ubo ", name, "");

			uint32 idx = frame_idx % static_cast<uint32>(desc_set.buffering);

			auto& buff = binding.uniformBuffers[idx];
			FLUX_DEBUG_ASSERT_CRITICAL(buff.IsMapped(), "buffer memory is not mapped");
			memcpy(buff.mapped, data, buff.size);






			////get the right frame set 
			// auto& desc_set = mDescriptorSetData[frame_idx];
			//
			//GPUBuffer* target_buff = nullptr;
			//for (auto& bind : desc_set.bindings)
			//{
			//	if (bind.debugName == name)
			//		target_buff = &bind.uniformBuffer;
			//}

			//
			//FLUX_DEBUG_ASSERT(target_buff != nullptr,"couldnt find ubo ", name, "");
			//auto& buff = *target_buff;
			//FLUX_DEBUG_ASSERT_CRITICAL(buff.IsMapped(), "buffer memory is not mapped");

			////auto* dst = reinterpret_cast<T*>(mapped);
			////dst[0] = data;
			////buff.mapped = data;
			//memcpy(buff.mapped, data, buff.size);



			//auto& it = mUniformBuffersIdxMap.find(name);

			//if (it == mUniformBuffersIdxMap.end())
			//{
			//	FLUX_DEBUG_LOG("couldnt find ubo ", name, "");
			//	return;
			//}

			//u32 idx = it->second + (mUniformBufferCount * frame_idx);
			//auto& buff = mUniformBuffers[idx];
			//FLUX_DEBUG_ASSERT_CRITICAL(buff.IsMapped(), "buffer memory is not mapped");

			////auto* dst = reinterpret_cast<T*>(mapped);
			////dst[0] = data;
			////buff.mapped = data;
			//memcpy(buff.mapped, data, buff.size);
		}

		void VulkanDescriptorSet::PushUBOBinding(const std::string& name, const std::string& set_name, uint32 binding, 
			uint32 buffer_size, const ShaderStageFlag& shaderStageFlag)
		{
			if (pGraphicsContext == nullptr)
			{
				FLUX_DEBUG_LOG_STATUS("pGraphics is null!!!");
				return;
			}


			auto desc_set = FindBindingSet(name, set_name, binding);

			if (desc_set == nullptr)
			{
				FLUX_DEBUG_LOG_WARNING("Invalid Descriptor Set null");
				return;
			}

			uint32 frame_in_flight = static_cast<uint32>(desc_set->buffering);
			DescriptorSetData::Binding temp_binding{};

			temp_binding.debugName = name;
			temp_binding.index = binding;
			temp_binding.type = EDescriptorType::UniformBuffer;
			temp_binding.shaderStageFlag = shaderStageFlag;

			temp_binding.uniformBuffers.resize(frame_in_flight);
			for (uint32 i = 0; i < frame_in_flight; i++)
			{

				GPUBuffer::CreateBuffer(pGraphicsContext,
					buffer_size,
					VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					temp_binding.uniformBuffers[i]);

				GPUBuffer::MapBufferMemory(pGraphicsContext->device, temp_binding.uniformBuffers[i]);

			}

			
			desc_set->bindings.emplace_back(temp_binding);
			//
			//auto& desc_set = mDescriptorSetData[desc_set_idx];
			//desc_set.bindings.emplace_back(temp_binding);
		}

		void VulkanDescriptorSet::PushStorageTextureBinding(const std::string& name, const std::string& set_name, uint32 binding, 
			const TextureSpecification& tex_spec, const ShaderStageFlag& shaderStageFlag)
		{
			if (pGraphicsContext == nullptr)
			{
				FLUX_DEBUG_LOG_STATUS("pGraphics is null!!!");
				return;
			}


			auto desc_set = FindBindingSet(name, set_name, binding);

			if (desc_set == nullptr)
			{
				FLUX_DEBUG_LOG_WARNING("Invalid Descriptor Set null");
				return;
			}

			DescriptorSetData::Binding temp_binding{};

			temp_binding.debugName = name;
			temp_binding.index = binding;
			temp_binding.type = EDescriptorType::ImageStorage;
			temp_binding.shaderStageFlag = shaderStageFlag;

			VulkanTexture texture{};
			texture.Create(pGraphicsContext, pGraphicsContext->transientCmdPool, tex_spec);

			//move data 
			auto& image_sampler = temp_binding.imageSampler;
			image_sampler.image = std::move(texture.mImage);
			image_sampler.imageView = std::move(texture.mImageView);
			image_sampler.layout = std::move(texture.mImageLayout);
			image_sampler.sampler = std::move(texture.mSampler);
			image_sampler.memory = std::move(texture.mMemory);


			desc_set->bindings.emplace_back(std::move(temp_binding));
		}

		void VulkanDescriptorSet::PushTextureBinding(const std::string& name, const std::string& set_name, uint32 binding, 
			const Resources::ImageSampler& image_sampler, const VkImageLayout& image_layout, 
			const ShaderStageFlag& shaderStageFlag, const EDescriptorType& descriptor_type)
		{
			if (pGraphicsContext == nullptr)
			{
				FLUX_DEBUG_LOG_STATUS("pGraphics is null!!!");
				return;
			}


			auto desc_set = FindBindingSet(name, set_name, binding);

			if (desc_set == nullptr)
			{
				FLUX_DEBUG_LOG_WARNING("Invalid Descriptor Set null");
				return;
			}

			DescriptorSetData::Binding temp_binding{};

			temp_binding.debugName = name;
			temp_binding.index = binding;
			temp_binding.type = descriptor_type;
			temp_binding.shaderStageFlag = shaderStageFlag;

			//move data 
			auto& image_sampler_ref = temp_binding.imageSampler;
			image_sampler_ref.image = image_sampler.image;
			image_sampler_ref.imageView = image_sampler.imageView;
			image_sampler_ref.layout = image_layout;
			image_sampler_ref.sampler = image_sampler.sampler;


			desc_set->bindings.emplace_back(std::move(temp_binding));
		}

		void VulkanDescriptorSet::BakePendingWrites()
		{

#pragma region Old

			//if (pGraphicsContext == nullptr)
			//{
			//	FLUX_DEBUG_LOG_STATUS("pGraphics is null!!!");
			//	return;
			//}
			////before we might first need to resolve 
			////sets as layout would be miss matching 


			//uint32 frame_in_flights = static_cast<uint32>(mFrameBuffering);
			////actuall mark set on push
			//// 
			//// 
			//// 
			//// Resolve dirty sets
			////
			//for (const auto& idx : mDirtyDescriptorSetIdx)
			//{

			//	//gather vulkan handles to update & recreate set 
			//	//std::vector<VkDescriptorSet> set_frame_in_flight(frame_in_flights);

			//	FLUX_DEBUG_ASSERT_CRITICAL(idx < mDescriptorSetData.size(), "curr set idx out of mDescriptorSetData bounds");
			//	auto& set = mDescriptorSetData[idx];

			//	//set_frame_in_flight[0] = &set.handle;

			//	//get cache layout 
			//	uint32 layout_idx;
			//	if (set.layoutIdx != UINT32_MAX)
			//		layout_idx = set.layoutIdx;
			//	else
			//	{
			//		layout_idx = mLayouts.size();
			//		mLayouts.push_back({});
			//	}

			//	std::vector<VkDescriptorSetLayoutBinding> vk_set_bindings;
			//	vk_set_bindings.reserve(set.bindings.size());

			//	for (const auto& binding : set.bindings)
			//	{
			//		vk_set_bindings.emplace_back();
			//		auto& curr_binding = vk_set_bindings.back();

			//		curr_binding.binding = binding.index;
			//		curr_binding.descriptorCount = 1;
			//		curr_binding.pImmutableSamplers = nullptr;

			//		switch (binding.type)
			//		{
			//		case EDescriptorType::UniformBuffer:
			//			curr_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			//			break;

			//		case EDescriptorType::ImageSampler:
			//			curr_binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			//			break;

			//		default:

			//			FLUX_DEBUG_LOG_ERROR("Binding type is not supported");
			//			break;
			//		}

			//		switch (binding.shaderStageFlag)
			//		{
			//		case EShaderStageBit::Vertex:
			//			curr_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			//			break;

			//		case EShaderStageBit::Fragment:
			//			curr_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			//			break;

			//		default:

			//			FLUX_DEBUG_LOG_ERROR("Binding shader stage is not supported");
			//			break;
			//		}


			//		//inject pending writes 
			//		//later update to support ImageSampler descriptor typr
			//		PendingWrite pending_write{};
			//		//pending_write.setHandle = desc_set.handle;
			//		pending_write.setIdx = idx;

			//		pending_write.descriptorType = EDescriptorType::UniformBuffer;
			//		pending_write.binding = static_cast<int32>(binding.index);
			//		pending_write.bufferInfo = {
			//			binding.uniformBuffer.buffer,
			//			0,
			//			binding.uniformBuffer.size
			//		};

			//		mPendingWrites.emplace_back(pending_write);
			//		
			//	}

			//	VkDescriptorSetLayoutCreateInfo layout_create_info{};
			//	layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			//	layout_create_info.bindingCount = static_cast<uint32>(vk_set_bindings.size());
			//	layout_create_info.pBindings = vk_set_bindings.data();


			//	//move to a function creation
			//	std::string layout_name = set.debugName + "_layout";
			//	mLayoutsIdxMap[layout_name] = layout_idx;

			//	FLUX_DEBUG_ASSERT_CRITICAL(layout_idx < mLayouts.size(), "layout idx is out of bounds");
			//	DescriptorLayoutData& desc_layout = mLayouts[layout_idx];
			//	desc_layout.name = layout_name;
			//	//Create new layoutr 
			//	FLUX_VK_ASSERT_SUCCESS(vkCreateDescriptorSetLayout(pGraphicsContext->device, &layout_create_info, nullptr, &desc_layout.handle), "Failed to create descriptor set layout!!!!!!!!");

			//	
			//	VkDescriptorSetAllocateInfo alloc_info{};
			//	alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			//	alloc_info.descriptorPool = mDescriptorPool;
			//	alloc_info.descriptorSetCount = 1/*frame_in_flights*/; //<-- since we are now creating one at a time
			//	alloc_info.pSetLayouts = &desc_layout.handle;

			//	FLUX_VK_ASSERT_SUCCESS(vkAllocateDescriptorSets(pGraphicsContext->device, &alloc_info, &set.handle), "Failed to allocate descriptor sets !!!!!!!");

			//	//update all other ref of this set in flight
			//	for (uint32 i = 1; i < frame_in_flights; i++)
			//	{
			//		FLUX_DEBUG_ASSERT_CRITICAL((idx + i) < mDescriptorSetData.size(), "curr set idx out of mDescriptorSetData bounds");
			//		auto& set = mDescriptorSetData[idx + i];

			//		FLUX_VK_ASSERT_SUCCESS(vkAllocateDescriptorSets(pGraphicsContext->device, &alloc_info, &set.handle), "Failed to allocate descriptor sets !!!!!!!");
			//		//set_frame_in_flight.emplace_back(&set);
			//		set.layoutIdx = layout_idx;


			//		for (const auto& binding : set.bindings)
			//		{

			//			//inject pending writes 
			//			PendingWrite pending_write{};
			//			//pending_write.setHandle = desc_set.handle;
			//			pending_write.setIdx = idx + i;

			//			pending_write.descriptorType = EDescriptorType::UniformBuffer;
			//			pending_write.binding = static_cast<int32>(binding.index);
			//			pending_write.bufferInfo = {
			//				binding.uniformBuffer.buffer,
			//				0,
			//				binding.uniformBuffer.size
			//			};

			//			mPendingWrites.emplace_back(pending_write);

			//		}
			//	}



			//	//create new set
			//	//if (vkAllocateDescriptorSets(pGraphicsContext->device, &alloc_info, set_frame_in_flight.data()) != VK_SUCCESS)
			//	//	throw std::runtime_error("Failed to allocate descriptor sets !!!!!!!");
			//}



			//mDirtyDescriptorSetIdx.clear();

			//std::vector<VkWriteDescriptorSet> desc_writes{};
			//desc_writes.reserve(mPendingWrites.size());

			//for (auto& pending_write : mPendingWrites)
			//{
			//	//if (pending_write.setHandle == VK_NULL_HANDLE || pending_write.binding == -1)
			//	//	continue;

			//	if (pending_write.setIdx == UINT32_MAX || pending_write.binding == -1)
			//	{
			//		FLUX_DEBUG_LOG_WARNING("Invalid pending write, ignored!!!");
			//		continue;
			//	}


			//	VkWriteDescriptorSet desc_write{};
			//	desc_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			//	//desc_write.dstSet = pending_write.setHandle;
			//	desc_write.dstSet = mDescriptorSetData[pending_write.setIdx].handle;
			//	desc_write.dstBinding = static_cast<uint32>(pending_write.binding);
			//	desc_write.dstArrayElement = 0;
			//	desc_write.descriptorCount = 1;

			//	if (pending_write.descriptorType == EDescriptorType::UniformBuffer)
			//	{
			//		desc_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			//		desc_write.pBufferInfo = &pending_write.bufferInfo;
			//	}
			//	else if (pending_write.descriptorType == EDescriptorType::ImageSampler)
			//	{
			//		desc_write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			//		desc_write.pImageInfo = &pending_write.imageInfo;
			//	}
			//	else
			//		FLUX_DEBUG_LOG_WARNING("pending_write.descriptorType not supported yet!!!!!");

			//	desc_writes.emplace_back(desc_write);
			//}


			//FLUX_DEBUG_ASSERT_CRITICAL(desc_writes.size() == mPendingWrites.size(), "writing does not match pending", 
			//	"\n\t Writing count: ", desc_writes.size(), 
			//	"\n\t Pending count: ", mPendingWrites.size(), "\n");
			//vkUpdateDescriptorSets(pGraphicsContext->device, static_cast<uint32>(desc_writes.size()), desc_writes.data(), 0, nullptr);

			//mPendingWrites.clear();
#pragma endregion

			if (pGraphicsContext == nullptr)
			{
				FLUX_DEBUG_LOG_STATUS("pGraphics is null!!!");
				return;
			}
			//before we might first need to resolve 

			for (const auto& idx : mDirtyDescriptorSetIdx)
			{

				//gather vulkan handles to update & recreate set 
				//std::vector<VkDescriptorSet> set_frame_in_flight(frame_in_flights);

				FLUX_DEBUG_ASSERT_CRITICAL(idx < mDescriptorSetData.size(), "curr set idx out of mDescriptorSetData bounds");
				auto& set = mDescriptorSetData[idx];

				//set_frame_in_flight[0] = &set.handle;

				//get cache layout 
				uint32 layout_idx;
				if (set.layoutIdx != UINT32_MAX)
					layout_idx = set.layoutIdx;
				else
				{
					layout_idx = mLayouts.size();
					mLayouts.push_back({});
				}

				std::vector<VkDescriptorSetLayoutBinding> vk_set_bindings;
				vk_set_bindings.reserve(set.bindings.size());

				for (const auto& binding : set.bindings)
				{
					vk_set_bindings.emplace_back();
					auto& curr_binding = vk_set_bindings.back();

					curr_binding.binding = binding.index;
					curr_binding.descriptorCount = 1;
					curr_binding.pImmutableSamplers = nullptr;

					switch (binding.type)
					{
					case EDescriptorType::UniformBuffer:
						curr_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
						break;

					case EDescriptorType::CombinedImageSampler:
						curr_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
						break;


					case EDescriptorType::ImageStorage:
						curr_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
						break;


					case EDescriptorType::ExternalImageSampler:
						curr_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
						break;

					default:

						FLUX_DEBUG_ASSERT_CRITICAL((false),"Binding type is not supported");
						break;
					}


					if (binding.shaderStageFlag.Contains(EShaderStageBit::Vertex))
						curr_binding.stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
					
					if (binding.shaderStageFlag.Contains(EShaderStageBit::Fragment))
						curr_binding.stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
					
					if (binding.shaderStageFlag.Contains(EShaderStageBit::Compute))
						curr_binding.stageFlags |= VK_SHADER_STAGE_COMPUTE_BIT;
				}

				VkDescriptorSetLayoutCreateInfo layout_create_info{};
				layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				layout_create_info.bindingCount = static_cast<uint32>(vk_set_bindings.size());
				layout_create_info.pBindings = vk_set_bindings.data();


				//move to a function creation
				std::string layout_name = set.debugName +"_layout";
				set.layoutIdx = layout_idx;
				mLayoutsIdxMap[layout_name] = layout_idx;

				FLUX_DEBUG_ASSERT_CRITICAL(layout_idx < mLayouts.size(), "layout idx is out of bounds");
				DescriptorLayoutData& desc_layout = mLayouts[layout_idx];
				desc_layout.name = layout_name;
				//Create new layoutr 
				FLUX_VK_ASSERT_SUCCESS(vkCreateDescriptorSetLayout(pGraphicsContext->device, &layout_create_info, nullptr, &desc_layout.handle), "Failed to create descriptor set layout!!!!!!!!");


				VkDescriptorSetAllocateInfo alloc_info{};
				alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
				alloc_info.descriptorPool = mDescriptorPool;
				alloc_info.descriptorSetCount = 1/*frame_in_flights*/; //<-- since we are now creating one at a time
				alloc_info.pSetLayouts = &desc_layout.handle;

				FLUX_VK_ASSERT_SUCCESS(vkAllocateDescriptorSets(pGraphicsContext->device, &alloc_info, &set.handle), "Failed to allocate descriptor sets !!!!!!!");

			}

			std::vector<VkWriteDescriptorSet> desc_writes{};

			std::vector<VkDescriptorBufferInfo> cache_desc_buff_info;
			cache_desc_buff_info.reserve(kMaxDescriptorInfo);
			std::vector<VkDescriptorImageInfo> cache_desc_img_info;
			cache_desc_img_info.reserve(kMaxDescriptorInfo);
			for (const auto& set_idx : mDirtyDescriptorSetIdx)
			{
				auto& set = mDescriptorSetData[set_idx];
				uint32 frame_in_flights = static_cast<uint32>(set.buffering);
				for (auto& binding : set.bindings)
				{
					if (binding.type == EDescriptorType::UniformBuffer)
					{
						for(uint32 i = 0; i < frame_in_flights; i++)
						{
							desc_writes.emplace_back();
							auto& curr_desc_write = desc_writes.back();

							curr_desc_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
							curr_desc_write.dstSet = set.handle;
							curr_desc_write.dstBinding = binding.index;
							curr_desc_write.dstArrayElement = 0;
							curr_desc_write.descriptorCount = 1;

							curr_desc_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

							cache_desc_buff_info.emplace_back();
							auto& curr_buff_info = cache_desc_buff_info.back();

							curr_buff_info.buffer = binding.uniformBuffers[i].buffer;
							curr_buff_info.offset = 0;
							curr_buff_info.range = binding.uniformBuffers[i].size;
							curr_desc_write.pBufferInfo = &cache_desc_buff_info.back();
						}
					}
					else if (binding.type == EDescriptorType::CombinedImageSampler ||
						binding.type == EDescriptorType::ImageStorage ||
						binding.type == EDescriptorType::ExternalImageSampler)
					{
						desc_writes.emplace_back();
						auto& curr_desc_write = desc_writes.back();

						curr_desc_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
						curr_desc_write.dstSet = set.handle;
						curr_desc_write.dstBinding = binding.index;
						curr_desc_write.dstArrayElement = 0;
						curr_desc_write.descriptorCount = 1;

						if(binding.type == EDescriptorType::CombinedImageSampler)
							curr_desc_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
						else if(binding.type == EDescriptorType::ImageStorage)
							curr_desc_write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
						else if(binding.type == EDescriptorType::ExternalImageSampler)
							curr_desc_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
						else
							FLUX_DEBUG_ASSERT_CRITICAL((false), "Binding type is not supported");


						cache_desc_img_info.emplace_back();
						auto& curr_img_info = cache_desc_img_info.back();

						curr_img_info.imageLayout = binding.imageSampler.layout;
						curr_img_info.imageView = binding.imageSampler.imageView;
						curr_img_info.sampler = binding.imageSampler.sampler;

						curr_desc_write.pImageInfo = &curr_img_info;
					}
					else
						FLUX_DEBUG_LOG_WARNING("pending_write.descriptorType not supported yet!!!!!");
				}
			}

			vkUpdateDescriptorSets(pGraphicsContext->device, static_cast<uint32>(desc_writes.size()), desc_writes.data(), 0, nullptr);

			mDirtyDescriptorSetIdx.clear();

			ResolveDescriptorSetOrder();
		}

		void VulkanDescriptorSet::Destory()
		{
			if (pGraphicsContext == nullptr)
				return;


			for (auto& desc_set_data : mDescriptorSetData)
				desc_set_data.Destory(pGraphicsContext);

			for (auto& layout : mLayouts)
			{
				if (layout.handle != VK_NULL_HANDLE)
					vkDestroyDescriptorSetLayout(pGraphicsContext->device, layout.handle, nullptr);
			}


			if (mDescriptorPool != VK_NULL_HANDLE)
				vkDestroyDescriptorPool(pGraphicsContext->device, mDescriptorPool, nullptr);
		}


		void VulkanDescriptorSet::GenerateDescriptorSetLayout(const VulkanContext* vk_ctx, const DescriptorSpecification& desc_set_spec)
		{
			for (const auto& layout_info : desc_set_spec.layoutInfos)
			{
				std::vector<VkDescriptorSetLayoutBinding> vk_bindings;

				//bin bindings for later use 
				uint32 bin_cache_offset = static_cast<uint32>(mOnloadBinningBindings.size());
				mOnloadBinningBindings.insert(mOnloadBinningBindings.end(), layout_info.requiredBindings.begin(), layout_info.requiredBindings.end());
				uint32 bin_cache_stride = static_cast<uint32>(mOnloadBinningBindings.size()) - bin_cache_offset;

				for (const auto& binding : layout_info.requiredBindings)
				{
					VkDescriptorSetLayoutBinding vk_binding{};
					vk_binding.binding = binding.binding;
					vk_binding.descriptorCount = 1;
					vk_binding.descriptorType = binding.type;
					vk_binding.stageFlags = binding.stageFlag;
					vk_binding.pImmutableSamplers = nullptr;

					vk_bindings.push_back(vk_binding);
				}

				VkDescriptorSetLayoutCreateInfo layout_create_info{};
				layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				layout_create_info.bindingCount = static_cast<uint32>(vk_bindings.size());
				layout_create_info.pBindings = vk_bindings.data();

				VkDescriptorSetLayout layout_handle;
				FLUX_VK_ASSERT_SUCCESS(vkCreateDescriptorSetLayout(vk_ctx->device, &layout_create_info, nullptr, &layout_handle), "Failed to create descriptor set layout!!!!!!!!");
				mLayoutsIdxMap[layout_info.name] = mLayouts.size();
				mLayouts.emplace_back();
				auto& layout_data = mLayouts.back();
				layout_data.handle = layout_handle;
				layout_data.name = layout_info.name;
				layout_data.binCacheOffset = bin_cache_offset;
				layout_data.binCacheStride = bin_cache_stride;
			}
			//for (const auto& set_layout : desc_set_spec.layoutInfos)
			//{
			//	std::vector<VkDescriptorSetLayoutBinding> bindings;
			//	bindings.reserve(set_layout.requiredDescriptorDataByIdx.size());
			//	//get matching layout
			//	for (const auto& desc_data : set_layout.requiredDescriptorDataByIdx)
			//	{
			//		//find pair 
			//		FLUX_DEBUG_ASSERT_CRITICAL(desc_data < desc_set_spec.descriptors.size(), "layout bindings out of bounds");
			//		bindings.emplace_back();
			//		auto& curr_binding = bindings.back();

			//		auto& curr_info = desc_set_spec.descriptors[desc_data];
			//		curr_binding.binding = curr_info.binding;
			//		curr_binding.descriptorCount = 1;
			//		curr_binding.descriptorType = curr_info.type;
			//		curr_binding.stageFlags = curr_info.stageFlag;
			//		curr_binding.pImmutableSamplers = nullptr;
			//	}

			//	VkDescriptorSetLayoutCreateInfo layout_create_info{};
			//	layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			//	layout_create_info.bindingCount = static_cast<uint32_t>(bindings.size());
			//	layout_create_info.pBindings = bindings.data();


			//	//move to a function creation
			//	mLayoutsIdxMap[set_layout.name] = mLayouts.size();
			//	mLayouts.push_back({});
			//	DescriptorLayoutData& desc_layout = mLayouts.back();
			//	desc_layout.name = set_layout.name;
			//	desc_layout.requiredDescriptorDataByIdx = set_layout.requiredDescriptorDataByIdx;
			//	//Create new layoutr 
			//	//VkDescriptorSetLayout& desc_layout = mLayouts[set_layout.name];
			//	if (vkCreateDescriptorSetLayout(vk_ctx->device, &layout_create_info, nullptr, &desc_layout.handle) != VK_SUCCESS)
			//		throw std::runtime_error("Failed to create descriptor set layout!!!!!!!!");
			//	//mLayouts["set_layout.name"] = desc_layout;

			//	//mLayoutsData[set_layout.name] = {
			//	//	set_layout.name, 
			//	//	desc_layout,
			//	//	set_layout.requiredDescriptorDataByIdx
			//	//};
			//}
		}

		void VulkanDescriptorSet::GenerateTightFitDescriptorPool(const VulkanContext* vk_ctx, const DescriptorSpecification& desc_set_spec)
		{
			std::vector<VkDescriptorPoolSize> pool_sizes;
			uint32 frame_in_flight = desc_set_spec.frameInFlight;

			for (const auto& layout_info : desc_set_spec.layoutInfos)
			{
				////get matching layout
				//for (const auto& layout : set_layout.requiredDescriptorDataByIdx)
				//{
				//	auto& curr_info = desc_set_spec.descriptors[layout];
				//	pool_sizes.push_back({ curr_info.type, 1 * frame_in_flight});
				//}

				//get matching layout
				for (const auto& binding : layout_info.requiredBindings)
				{
					pool_sizes.push_back({ binding.type, 1 * frame_in_flight });
				}
			}

			VkDescriptorPoolCreateInfo pool_create_info{};
			pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			pool_create_info.poolSizeCount = static_cast<uint32>(pool_sizes.size());
			pool_create_info.pPoolSizes = pool_sizes.data();
			pool_create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
			pool_create_info.maxSets = frame_in_flight;

			FLUX_VK_ASSERT_SUCCESS(vkCreateDescriptorPool(vk_ctx->device, &pool_create_info, nullptr, &mDescriptorPool), "Failed to Create Descriptor Pool!!!!!!!");
		}

		void VulkanDescriptorSet::GenerateLargeDescriptorPool(const VulkanContext* vk_ctx)
		{
			
			std::array<VkDescriptorPoolSize, 11> pool_sizes{ {
				{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
				{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
				{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
				{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
				{VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
				{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
				{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
				{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
				{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
				{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
				{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}
			} };

			VkDescriptorPoolCreateInfo poolInfo = {};
			poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
			poolInfo.maxSets = 1000 * pool_sizes.size();
			poolInfo.poolSizeCount = static_cast<uint32>(pool_sizes.size());
			poolInfo.pPoolSizes = pool_sizes.data();
			FLUX_VK_ASSERT_SUCCESS(vkCreateDescriptorPool(vk_ctx->device, &poolInfo, nullptr, &mDescriptorPool), "Failed to Create Descriptor Pool!!!!!!!");
			
		}
		





		DescriptorSetData::Binding VulkanDescriptorSet::CreateUBODescriptorBinding(const VulkanContext* vk_ctx, const DescriptorSetInfo& set_info, 
			const DescriptorBindingInfo& binding_info, const EDescriptorBuffering& buffering)
		{
			DescriptorSetData::Binding temp_binding{};

			temp_binding.debugName = binding_info.name;
			temp_binding.index = binding_info.binding;
			temp_binding.type = binding_info.descType;

			uint32 frames_in_flight = static_cast<uint32>(buffering);
			if (temp_binding.type == EDescriptorType::UniformBuffer)
			{
				for(uint32 i = 0; i < frames_in_flight; i++)
				{
					temp_binding.uniformBuffers.emplace_back();
					auto& ubo = temp_binding.uniformBuffers.back();
					GPUBuffer::CreateBuffer(vk_ctx,
						binding_info.bufferSize,
						VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
						VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						ubo);

					GPUBuffer::MapBufferMemory(vk_ctx->device, ubo);
				}
			}
			else
			{
				//need to get a specification 
				//to create/generate image details for set
			}



			std::string key = set_info.name + "::" + binding_info.name;
			mBindingLookup[key] = { set_info.set, binding_info.binding };

			return temp_binding;
		}

		void VulkanDescriptorSet::GenerateSets(const VulkanContext* vk_ctx, const DescriptorSpecification& desc_set_spec)
		{
			//////////////////////////////////////////////////
			// Generate Set from Redquired layout
			//////////////////////////////////////////////////

			for (auto& curr_set : desc_set_spec.setsInfos)
			{
				std::vector<VkDescriptorSetLayout> layouts;
				std::vector<DescriptorSetData::Binding> layout_bindings;

				uint32 set_layout_idx = UINT32_MAX;

				for (auto& req_layout : curr_set.requiredLayouts)
				{
					//find layout 
					//auto& it = mLayoutsData.find(req_layout);
					auto& it = mLayoutsIdxMap.find(req_layout);

					//if (it != mLayoutsData.end())
					if (it != mLayoutsIdxMap.end())
					{
						//auto& layout_data = it->second;
						FLUX_DEBUG_ASSERT_CRITICAL(it->second < mLayouts.size(), "layout map idx is out of bounds layout data");
						set_layout_idx = it->second;
						auto& layout_data = mLayouts[it->second];
						std::vector<VkDescriptorSetLayout> layout(desc_set_spec.frameInFlight, layout_data.handle);
						layouts.insert(layouts.end(), layout.begin(), layout.end());


						FLUX_DEBUG_ASSERT_CRITICAL(layout_data.binCacheOffset < mOnloadBinningBindings.size(), "bin offset is out of mOnloadBinningBindings bounds");
						FLUX_DEBUG_ASSERT_CRITICAL(layout_data.binCacheOffset + layout_data.binCacheStride <= mOnloadBinningBindings.size(), "target max data is out of mOnloadBinningBindings bounds");
						for (uint32 i = layout_data.binCacheOffset; i < (layout_data.binCacheOffset + layout_data.binCacheStride); i++)
						{
							auto& binding = mOnloadBinningBindings[i];

							if (binding.descType == EDescriptorType::UniformBuffer)
								layout_bindings.emplace_back(CreateUBODescriptorBinding(vk_ctx, curr_set, binding, curr_set.buffering));
							else if (binding.descType == EDescriptorType::CombinedImageSampler ||
								binding.descType == EDescriptorType::ImageStorage)
							{
								DescriptorSetData::Binding temp_binding{};

								temp_binding.debugName = binding.name;
								temp_binding.index = binding.binding;
								temp_binding.type = binding.descType;

								VulkanTexture texture{};
								texture.Create(pGraphicsContext, pGraphicsContext->transientCmdPool, binding.tex_spec);

								//move data 
								auto& image_sampler = temp_binding.imageSampler;
								image_sampler.image = std::move(texture.mImage);
								image_sampler.imageView = std::move(texture.mImageView);
								image_sampler.layout = std::move(texture.mImageLayout);
								image_sampler.sampler = std::move(texture.mSampler);
								image_sampler.memory = std::move(texture.mMemory);


								layout_bindings.emplace_back(std::move(temp_binding));
							}
							else if (binding.descType == EDescriptorType::ExternalImageSampler)
							{
								DescriptorSetData::Binding temp_binding{};

								temp_binding.debugName = binding.name;
								temp_binding.index = binding.binding;
								temp_binding.type = binding.descType;

			
								//not move data but copy ref and lifetime is not managed
								auto& image_sampler = temp_binding.imageSampler;
								image_sampler.image = binding.externalImageSampler.image;
								image_sampler.imageView = binding.externalImageSampler.imageView;
								image_sampler.layout = binding.externalImageSampler.layout;
								image_sampler.sampler = binding.externalImageSampler.sampler;

								layout_bindings.emplace_back(std::move(temp_binding));
							}
						}

						//for (auto& binding_idx : layout_data.requiredDescriptorDataByIdx)
						//{
						//	auto& layout_bind = desc_set_spec.descriptors[binding_idx];
						//	if(layout_bind.descType == EDescriptorType::UniformBuffer)
						//		layout_bindings.emplace_back(CreateUBODescriptorBinding(vk_ctx, curr_set, layout_bind, curr_set.buffering));
						//	else if (layout_bind.descType == EDescriptorType::CombinedImageSampler ||
						//		layout_bind.descType == EDescriptorType::ImageStorage)
						//	{
						//		DescriptorSetData::Binding temp_binding{};

						//		temp_binding.debugName = layout_bind.name;
						//		temp_binding.index = layout_bind.binding;
						//		temp_binding.type = layout_bind.descType;

						//		VulkanTexture texture{};
						//		texture.Create(pGraphicsContext, pGraphicsContext->transientCmdPool, layout_bind.tex_spec);

						//		//move data 
						//		auto& image_sampler = temp_binding.imageSampler;
						//		image_sampler.image = std::move(texture.mImage);
						//		image_sampler.imageView = std::move(texture.mImageView);
						//		image_sampler.layout = std::move(texture.mImageLayout);
						//		image_sampler.sampler = std::move(texture.mSampler);


						//		layout_bindings.emplace_back(std::move(temp_binding));
						//	}

						//}

					}
					else
						FLUX_DEBUG_ASSERT_CRITICAL(false, "Failed to find layout by name: ", req_layout);
				}

				VkDescriptorSetAllocateInfo alloc_info{};
				alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
				alloc_info.descriptorPool = mDescriptorPool;
				alloc_info.descriptorSetCount = /*desc_set_spec.frameInFlight*/1;
				alloc_info.pSetLayouts = layouts.data();

				//create new set
				//std::vector<VkDescriptorSet> temp_sets(desc_set_spec.frameInFlight, VK_NULL_HANDLE);
				std::vector<VkDescriptorSet> temp_sets(/*desc_set_spec.frameInFlight*/1, VK_NULL_HANDLE);
				if (vkAllocateDescriptorSets(vk_ctx->device, &alloc_info, temp_sets.data()) != VK_SUCCESS)
					throw std::runtime_error("Failed to allocate descriptor sets !!!!!!!");

				//descriptor set a stored contigous in a vector array 
				//mDescriptorSetIdxMap maps name to teh idx in its std::vector
				//if using multi frames in flight 
				//mDescriptorSetIdxMap returns the first frame flight for data
				// to get next flight would be + curr frame idx to get teh VKDEscriptor 
				// mvp 0 | mvp 1 | shadow map 0 | shadow map 1
				mDescriptorSetIdxMap[curr_set.name] = mDescriptorSetData.size();
				//mDescriptorSet.insert(mDescriptorSet.end(), temp_sets.begin(), temp_sets.end());
				for (auto& set : temp_sets)
				{
					//mDescriptorSet.emplace_back();
					//auto& desc_set_data0 = mDescriptorSet.back();

					//desc_set_data0.name = curr_set.name;
					//desc_set_data0.handle = set;
					//desc_set_data0.requiredLayouts = curr_set.requiredLayouts;


					//new hack 
					mDescriptorSetData.emplace_back();
					auto& desc_set_data = mDescriptorSetData.back();

					desc_set_data.debugName = curr_set.name;
					desc_set_data.set = curr_set.set;
					desc_set_data.handle = set;
					desc_set_data.bindings = layout_bindings;
					desc_set_data.layoutIdx = set_layout_idx;
					//desc_set_data.requiredLayouts = curr_set.requiredLayouts;



					auto& result = ResolveDescriptorSet(desc_set_data);
					if (result != EDescriptorSetErrorBit::None)
						FLUX_DEBUG_LOG_WARNING(DescriptorSetErrorFlagToString(result));
				}
			}
		}




		void VulkanDescriptorSet::WriteGeneretedDescriptorSets(const VulkanContext* vk_ctx, std::vector<DescriptorSetData>& update_descriptor_set)
		{

			//this makes sense as we a looping throught available set 
			//not specifiation 
			for (auto& curr_set : update_descriptor_set)
			{
				std::vector<VkWriteDescriptorSet> desc_writes{};

				//cache might nor be required 
				std::vector<VkDescriptorBufferInfo> cache_desc_buff_info;
				cache_desc_buff_info.reserve(kMaxDescriptorInfo);
				std::vector<VkDescriptorImageInfo> cache_desc_img_info;
				cache_desc_img_info.reserve(kMaxDescriptorInfo);


				for (const auto& binding : curr_set.bindings)
				{
					if (binding.type == EDescriptorType::UniformBuffer)
					{
						for (auto& buffer : binding.uniformBuffers)
						{
							desc_writes.emplace_back();
							auto& curr_desc_write = desc_writes.back();

							curr_desc_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
							curr_desc_write.dstSet = curr_set.handle;
							curr_desc_write.dstBinding = binding.index;
							curr_desc_write.dstArrayElement = 0;
							curr_desc_write.descriptorCount = 1;

							curr_desc_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

							cache_desc_buff_info.emplace_back();
							auto& curr_buff_info = cache_desc_buff_info.back();

							curr_buff_info.buffer = buffer.buffer;
							curr_buff_info.offset = 0;
							curr_buff_info.range = buffer.size;
							curr_desc_write.pBufferInfo = &cache_desc_buff_info.back();
						}
					}
					else if (binding.type == EDescriptorType::CombinedImageSampler ||
						binding.type == EDescriptorType::ImageStorage ||
						binding.type == EDescriptorType::ExternalImageSampler)
					{
						desc_writes.emplace_back();
						auto& curr_desc_write = desc_writes.back();

						curr_desc_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
						curr_desc_write.dstSet = curr_set.handle;
						curr_desc_write.dstBinding = binding.index;
						curr_desc_write.dstArrayElement = 0;
						curr_desc_write.descriptorCount = 1;

						if (binding.type == EDescriptorType::CombinedImageSampler || binding.type == EDescriptorType::ExternalImageSampler)
							curr_desc_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
						else if (binding.type == EDescriptorType::ImageStorage)
							curr_desc_write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
						else
							FLUX_DEBUG_LOG_WARNING("pending_write.descriptorType not supported yet!!!!!");


						cache_desc_img_info.emplace_back();
						auto& curr_img_info = cache_desc_img_info.back();

						curr_img_info.imageLayout = binding.imageSampler.layout;
						curr_img_info.imageView = binding.imageSampler.imageView;
						curr_img_info.sampler = binding.imageSampler.sampler;

						curr_desc_write.pImageInfo = &curr_img_info;
					}
					else
						FLUX_DEBUG_LOG_WARNING("pending_write.descriptorType not supported yet!!!!!");
				}

				vkUpdateDescriptorSets(vk_ctx->device, static_cast<uint32>(desc_writes.size()), desc_writes.data(), 0, nullptr);
			}

		}

		DescriptorSetData* VulkanDescriptorSet::FindBindingSet(const std::string& name, const std::string& set_name, uint32 binding)
		{
			//check if binding already exist 
			if (auto& it = mBindingLookup.find(name); it != mBindingLookup.end())
			{
				auto& v = it->second;
				FLUX_DEBUG_LOG_WARNING("binding key already exist ", name, "for: ",
					"\n\t set[", v.set, "].\n\t with binding: ", name, " -- binding[", v.binding, "].",
					"\n return");
				return nullptr;
			}
			else if (auto& it = mBindingLookup.find(set_name + "::" + name); it != mBindingLookup.end())
			{
				auto& v = it->second;
				FLUX_DEBUG_LOG_WARNING("binding key set already exist as ", set_name + "::" + name, " for: "
					"\n\t set: ", set_name, " -- set[", v.set, "].\n\t with binding: ", name, " -- binding[", v.binding, "].",
					"\n returning");
				return nullptr;
			}

			
			uint32 desc_set_idx = UINT32_MAX;

			if (auto& it = mDescriptorSetIdxMap.find(set_name); it != mDescriptorSetIdxMap.end())
			{
				desc_set_idx = it->second;
				//mark as dirty 
				mDirtyDescriptorSetIdx.insert(desc_set_idx);
			}
			else
			{
				desc_set_idx = mDescriptorSetData.size();
				mDescriptorSetData.emplace_back();
				//desc_set = {};

				//update map
				mDescriptorSetIdxMap[set_name] = desc_set_idx;
				//mark as dirty 
				mDirtyDescriptorSetIdx.insert(desc_set_idx);
			}


			FLUX_DEBUG_ASSERT_CRITICAL(desc_set_idx != UINT32_MAX, "Error Finding descriptor set");
			FLUX_DEBUG_ASSERT_CRITICAL(desc_set_idx < mDescriptorSetData.size(), "out of descriptor set bounds");

			std::string key = name;
			mBindingLookup[key] = { desc_set_idx, binding };

			return &mDescriptorSetData[desc_set_idx];
		}


		DescriptorSetErrorFlag VulkanDescriptorSet::ResolveDescriptorSet(const DescriptorSetData& descriptor_set)
		{
			DescriptorSetErrorFlag result = EDescriptorSetErrorBit::None;

			std::unordered_set<uint32> exist_bindings;

			for (const auto& binding : descriptor_set.bindings)
			{

				if (binding.index == UINT32_MAX)
					result |= EDescriptorSetErrorBit::InvalidBinding;

				bool has_buffer = binding.uniformBuffers.size() > 0;// && binding.uniformBuffers[0] != VK_NULL_HANDLE;
				bool has_sampler = binding.imageSampler.sampler != VK_NULL_HANDLE;

				if(!has_buffer && !has_sampler)
					result |= EDescriptorSetErrorBit::NoData;

				switch (binding.type)
				{
				case EDescriptorType::UniformBuffer:
					if (!has_buffer)
						result |= EDescriptorSetErrorBit::TypeDoesNotMatchData;
					break;

				case EDescriptorType::CombinedImageSampler:
					if (!has_sampler)
						result |= EDescriptorSetErrorBit::TypeDoesNotMatchData;
					break;

				default:
					result |= EDescriptorSetErrorBit::TypeDoesNotExist;
					break;
				}

				if(!exist_bindings.insert(binding.index).second)
					result |= EDescriptorSetErrorBit::BindingOverlap;
			}

			return result;
		}

		void VulkanDescriptorSet::ResolveDescriptorSetOrder()
		{
			//might have to fill some gaps if a set does not exist
			//like cam set[1], skybox[3]
			//set 0 and 2 is missing and the sd::vector serve as a array continer 
			//but lookup uses {set, binding} for look up not the actual location in array 
			//so dummy data needs to be inserted in 0 and 2 to balance look up 

			//a quick sort by size
			std::sort(mDescriptorSetData.begin(), mDescriptorSetData.end(), [](const DescriptorSetData& a,
				const DescriptorSetData& b)
				{
					return a.set < b.set;
				});


			for (const auto& descriptor_set : mDescriptorSetData)
			{
				mDescriptorSetIdxMap[descriptor_set.debugName] = descriptor_set.set;
			}
		}




#pragma region OLD WRITE DESCRIPTOR SET
		//void VulkanDescriptorSet::WriteDescriptorSets(const VulkanContext* vk_ctx, const DescriptorSpecification& desc_set_spec)
		//{
		//	std::string name;
		//	uint32 set = 0;
		//	std::vector<std::string> requiredLayouts;

		//	mDescriptorSetData.clear();


		//	//for (auto& curr_set : mDescriptorSetData)
		//	for (auto& curr_set : mDescriptorSet)
		//	{
		//		std::vector<VkWriteDescriptorSet> desc_writes{};

		//		std::vector<VkDescriptorBufferInfo> cache_desc_buff_info;
		//		cache_desc_buff_info.reserve(kMaxDescriptorInfo);
		//		std::vector<VkDescriptorImageInfo> cache_desc_img_info;
		//		cache_desc_img_info.reserve(kMaxDescriptorInfo);

		//		mDescriptorSetData.emplace_back();
		//		auto& runtime_set = mDescriptorSetData.back();
		//		runtime_set.debugName = curr_set.name;
		//		runtime_set.handle = curr_set.handle;

		//		for (auto& req_layout : curr_set.requiredLayouts)
		//		{
		//			//find layout 
		//			auto& it = mLayoutsData.find(req_layout);

		//			if (it != mLayoutsData.end())
		//			{
		//				//found 
		//				//retrive datas
		//				DescriptorLayoutData& desc_data = it->second;
		//				//desc_data.s

		//				for (auto& descriptor_idx : desc_data.requiredDescriptorDataByIdx)
		//				{

		//					auto& descriptor_data = desc_set_spec.descriptors[descriptor_idx];

		//					desc_writes.emplace_back();
		//					auto& curr_desc_write = desc_writes.back();

		//					curr_desc_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		//					curr_desc_write.dstSet = curr_set.handle;
		//					curr_desc_write.dstBinding = descriptor_data.binding;
		//					curr_desc_write.dstArrayElement = 0;
		//					curr_desc_write.descriptorType = descriptor_data.type;
		//					curr_desc_write.descriptorCount = 1;


		//					std::string key = curr_set.name + "::" + descriptor_data.name;
		//					mBindingLookup[key] = { curr_set.set, descriptor_data.binding };

		//					runtime_set.bindings.emplace_back();
		//					auto& bind = runtime_set.bindings.back();
		//					bind.debugName = descriptor_data.name;
		//					bind.index = descriptor_data.binding;

		//					if (descriptor_data.descType == EDescriptorType::UniformBuffer)
		//					{
		//						////check if exist 
		//						//auto& it = mUniformBuffersIdxMap.find(descriptor_data.name);
		//						//if(it == mUniformBuffersIdxMap.end())
		//						//	mUniformBuffersIdxMap[descriptor_data.name] = track_ubo_idx_count;

		//						//track_ubo_idx_count++;

		//						//mUniformBuffers.emplace_back();
		//						//auto& curr_ubo = mUniformBuffers.back();


		//						//mUniformBufferCount = glm::max(mUniformBufferCount, track_ubo_idx_count);



		//						//GPUBuffer::CreateBuffer(vk_ctx,
		//						//	descriptor_data.data.bufferInfo.range,
		//						//	VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		//						//	VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		//						//	curr_ubo);

		//						//GPUBuffer::MapBufferMemory(vk_ctx->device, curr_ubo);

		//						////curr_set.bindings.emplace_back();
		//						//runtime_set.bindings.emplace_back();
		//						////auto& bind = curr_set.bindings.back();
		//						//auto& bind = runtime_set.bindings.back();
		//						//bind.dataInfo.bufferInfo.buffer = curr_ubo.buffer;
		//						//bind.dataInfo.bufferInfo.offset = 0;
		//						//bind.dataInfo.bufferInfo.range = curr_ubo.size;

		//						//curr_desc_write.pBufferInfo = &bind.dataInfo.bufferInfo;

		//						curr_desc_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		//						auto& curr_ubo = bind.uniformBuffer;

		//						GPUBuffer::CreateBuffer(vk_ctx,
		//							descriptor_data.bufferSize,
		//							VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		//							VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		//							curr_ubo);

		//						GPUBuffer::MapBufferMemory(vk_ctx->device, curr_ubo);


		//						cache_desc_buff_info.emplace_back();
		//						auto& curr_buff_info = cache_desc_buff_info.back();
		//						//auto& curr_buff_info = cache_desc_buff_info[track_ubo_idx_count];
		//						//track_ubo_idx_count++;

		//						curr_buff_info.buffer = curr_ubo.buffer;
		//						curr_buff_info.offset = 0;
		//						curr_buff_info.range = curr_ubo.size;
		//						//curr_desc_write.pBufferInfo = &bind.dataInfo.bufferInfo;
		//						curr_desc_write.pBufferInfo = &cache_desc_buff_info.back();

		//					}
		//					else
		//					{
		//						curr_desc_write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		//						auto& curr_img = bind.imageSampler;

		//						//need to get a specification 
		//						//to create/generate image details for set

		//						cache_desc_img_info.emplace_back();
		//						auto curr_img_info = cache_desc_img_info.back();

		//						curr_img_info.imageLayout = bind.imageSampler.layout;
		//						curr_img_info.imageView = bind.imageSampler.imageView;
		//						curr_img_info.sampler = bind.imageSampler.sampler;

		//						curr_desc_write.pImageInfo = &curr_img_info;
		//					}


		//				}

		//			}
		//		}

		//		vkUpdateDescriptorSets(vk_ctx->device, static_cast<uint32>(desc_writes.size()), desc_writes.data(), 0, nullptr);
		//	}
		//}
#pragma endregion




	}//namespace Vulkan
}//namespace Fluxon


