#include "ViewVoxelPass.h"

#include "Renderer/VulkanUtilities.h"

#include "Core/AssertLog.h"

#include "Core/Camera.h"

namespace Fluxon {
	namespace Vulkan {

		void SceneVoxelPass::Init(VulkanContext * vk_ctx, VkCommandPool cmd_pool, const Camera* scene_cam, const GPUStructures::VoxelParameterUBO& voxel_data)
		{

			//NEED DEPTH TO DEBUG VIEW
			//NEED DEPTH TO DEBUG VIEW
			//NEED DEPTH TO DEBUG VIEW
			// 
			//
			VkFormat depth_format = Utils::FindDepthFormat(vk_ctx->physicalDevice);
			///depth_format = VK_FORMAT_D16_UNORM;
			//Does not have colour attachment 
			VkAttachmentDescription depth_attachment{};
			depth_attachment.format = depth_format;
			depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
			depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

			depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			depth_attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			VkAttachmentReference depth_ref{};
			depth_ref.attachment = 0;
			depth_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			// 
			//
			//NEED DEPTH TO DEBUG VIEW
			//NEED DEPTH TO DEBUG VIEW
			//NEED DEPTH TO DEBUG VIEW
			VkSubpassDescription subpass{};
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpass.colorAttachmentCount = 0;
			subpass.pColorAttachments = nullptr;
			subpass.pDepthStencilAttachment = &depth_ref;

			VkSubpassDependency dependency{};
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;

			dependency.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dependency.srcAccessMask = 0;

			dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			dependency.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;

			std::array<VkSubpassDependency, 2> dependencies;
			//dependencies[0] = dependency;

			//dependencies[1].srcSubpass = 0;
			//dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
			//dependencies[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			//dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			//dependencies[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			//dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			//dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;


			dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
			dependencies[0].dstSubpass = 0;
			dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			dependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

			dependencies[1].srcSubpass = 0;
			dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
			dependencies[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			dependencies[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;


			VkRenderPassCreateInfo renderpass_create_info{};
			renderpass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			renderpass_create_info.attachmentCount = 1;
			renderpass_create_info.pAttachments = &depth_attachment;
			renderpass_create_info.subpassCount = 1;
			renderpass_create_info.pSubpasses = &subpass;
			renderpass_create_info.dependencyCount = dependencies.size();
			renderpass_create_info.pDependencies = dependencies.data();

			if (vkCreateRenderPass(vk_ctx->device, &renderpass_create_info, nullptr, &mRenderPass) != VK_SUCCESS)
				throw std::runtime_error("Failed to create render pass!!!!!!");


			TextureSpecification tex_spec{};
			tex_spec.width = mVoxelImageExtent.width;
			tex_spec.height = mVoxelImageExtent.height;
			tex_spec.depth = mVoxelImageDepth;
			tex_spec.usage = ETextureUsageBits::Storage;
			tex_spec.type = ETextureType::Texture2DArray;
			//tex_spec.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
			tex_spec.imageAspect = VK_IMAGE_ASPECT_COLOR_BIT;
			tex_spec.format = VK_FORMAT_R16_SFLOAT;
			Utils::LayoutTransitionInfo image_transition{};
			image_transition.newLayout = VK_IMAGE_LAYOUT_GENERAL;
			image_transition.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			image_transition.dstAccess = VK_ACCESS_SHADER_WRITE_BIT;
			image_transition.subRangeLayerCount = mVoxelImageDepth;
			tex_spec.explicitImageTransitionInfo = image_transition;

			for (auto& voxel_occpancy_tex : mVoxelOccupancyTex)
				voxel_occpancy_tex.Create(vk_ctx, cmd_pool, tex_spec);



			RenderTargetSpec rt_spec;
			rt_spec.extent = mVoxelImageExtent;
			rt_spec.arrayLayers = 1;
			rt_spec.format = depth_format;
			rt_spec.targetMask = ERenderTargetBits::Write |
				ERenderTargetBits::Sample;
			rt_spec.explicitUsage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			rt_spec.imageAspect = VK_IMAGE_ASPECT_DEPTH_BIT;

			FLUX_DEBUG_ASSERT(RenderTarget::CreateRenderTarget(vk_ctx, cmd_pool, mRenderTarget, rt_spec), "Failed to creaete Shadow pass render target attachment !!!!!!");
			//if(!RenderTarget::CreateRenderTarget(vk_ctx, cmd_pool, mRenderTarget, rt_spec))
				//throw std::runtime_error("Failed to creaete Shadow pass render target attachment !!!!!!");

			if (!Framebuffer::Create(vk_ctx, mRenderPass, mFramebuffer, mVoxelImageExtent, { mRenderTarget.imageView}))
				throw std::runtime_error("Failed to create Shadow pass Framebuffer");
			//if (!mFramebuffer.CreateDummy(vk_ctx, mRenderPass, mVoxelImageExtent, mVoxelImageDepth))

			CreateGPUbuffers(vk_ctx);

			if(!CreateDescriptorSetLayouts(vk_ctx))
				throw std::runtime_error("Failed to create Descriptor set and UBO!!!!!");


			CreateGfxPipeline(vk_ctx);


			mCamera.SetPosition(scene_cam->GetPosition());
			//mCamera.SetType(ECameraType::Orthographic);
			//mCamera.SetNearZ(0.0f);
			//mCamera.SetFarZ(5.0f);
			//mCamera.SetScale(6.4f);

			UpdateCameraData(scene_cam);
			UpdateVoxelParamData(voxel_data);
		}


		bool SceneVoxelPass::CreateDescriptorSetLayouts(const VulkanContext* vk_ctx)
		{
			std::array<VkDescriptorSetLayoutBinding, 3> layout_bindings{};
			layout_bindings[0].binding = 0;
			layout_bindings[0].descriptorCount = 1;
			layout_bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			layout_bindings[0].pImmutableSamplers = nullptr;
			layout_bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

			layout_bindings[1].binding = 1;
			layout_bindings[1].descriptorCount = 1;
			layout_bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			layout_bindings[1].pImmutableSamplers = nullptr;
			layout_bindings[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

			layout_bindings[2].binding = 2;
			layout_bindings[2].descriptorCount = 1;
			layout_bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			layout_bindings[2].pImmutableSamplers = nullptr;
			layout_bindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

			VkDescriptorSetLayoutCreateInfo layoutInfo{};
			layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layoutInfo.bindingCount = static_cast<uint32_t>(layout_bindings.size());
			layoutInfo.pBindings = layout_bindings.data();

			//mComputeDescSetSpec.set = 0;
			if (vkCreateDescriptorSetLayout(vk_ctx->device, &layoutInfo, nullptr, &mDescriptorSetLayout) != VK_SUCCESS) {
				throw std::runtime_error("failed to create compute descriptor set layout!");
			}



			//descriptor pool
			VkDescriptorPoolSize ubo_pool_size{};
			ubo_pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			ubo_pool_size.descriptorCount = 2 * static_cast<uint32_t>(kMaxFramesInFlight);

			VkDescriptorPoolSize image2darray_pool_size{};
			image2darray_pool_size.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			//2 * one SSBO for In Particle and second for Out Particle 
			image2darray_pool_size.descriptorCount = 1 * static_cast<uint32_t>(kMaxFramesInFlight);

			std::array<VkDescriptorPoolSize, 2> pool_sizes = { ubo_pool_size, image2darray_pool_size };
			VkDescriptorPoolCreateInfo pool_create_info{};
			pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			pool_create_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
			pool_create_info.pPoolSizes = pool_sizes.data();
			pool_create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
			pool_create_info.maxSets = static_cast<uint32_t>(kMaxFramesInFlight);

			if (vkCreateDescriptorPool(vk_ctx->device, &pool_create_info, nullptr, &mDescriptorPool) != VK_SUCCESS)
				throw std::runtime_error("Failed to Create Descriptor Pool!!!!!!!");

			std::vector<VkDescriptorSetLayout> layouts(kMaxFramesInFlight, mDescriptorSetLayout);
			VkDescriptorSetAllocateInfo alloc_info{};
			alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			alloc_info.descriptorPool = mDescriptorPool;
			alloc_info.descriptorSetCount = static_cast<uint32_t>(kMaxFramesInFlight);
			alloc_info.pSetLayouts = layouts.data();

			if (vkAllocateDescriptorSets(vk_ctx->device, &alloc_info, mDescriptorSets.data()) != VK_SUCCESS)
				throw std::runtime_error("Failed to allocate descriptor sets !!!!!!!");



			for (size_t i = 0; i < kMaxFramesInFlight; i++)
			{

				std::array<VkWriteDescriptorSet, 3> descriptorWrites{};

				VkDescriptorBufferInfo cam_ubo_info{};
				cam_ubo_info.buffer = mCameraBufferdata[i].buffer;
				cam_ubo_info.offset = 0;
				cam_ubo_info.range = mCameraBufferdata[i].size;

				descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[0].dstSet = mDescriptorSets[i];
				descriptorWrites[0].dstBinding = 0;
				descriptorWrites[0].dstArrayElement = 0;
				descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorWrites[0].descriptorCount = 1;
				descriptorWrites[0].pBufferInfo = &cam_ubo_info;

				VkDescriptorBufferInfo voxel_param_ubo_info{};
				voxel_param_ubo_info.buffer = mVoxelParamBufferdata[i].buffer;
				voxel_param_ubo_info.offset = 0;
				voxel_param_ubo_info.range = mVoxelParamBufferdata[i].size;

				descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[1].dstSet = mDescriptorSets[i];
				descriptorWrites[1].dstBinding = 1;
				descriptorWrites[1].dstArrayElement = 0;
				descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorWrites[1].descriptorCount = 1;
				descriptorWrites[1].pBufferInfo = &voxel_param_ubo_info;

				VkDescriptorImageInfo voxel_occupancy_tex_info{};
				voxel_occupancy_tex_info.sampler = mVoxelOccupancyTex[i].GetSampler();
				voxel_occupancy_tex_info.imageView = mVoxelOccupancyTex[i].GetImageView();
				voxel_occupancy_tex_info.imageLayout = mVoxelOccupancyTex[i].GetImageLayout();

				

				descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[2].dstSet = mDescriptorSets[i];
				descriptorWrites[2].dstBinding = 2;
				descriptorWrites[2].dstArrayElement = 0;
				descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;/// | VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
				descriptorWrites[2].descriptorCount = 1;
				descriptorWrites[2].pImageInfo = &voxel_occupancy_tex_info;

				vkUpdateDescriptorSets(vk_ctx->device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
			}


			return true;
		}

		bool SceneVoxelPass::CreateGPUbuffers(const VulkanContext* vk_ctx)
		{
			for (uint32_t i = 0; i < kMaxFramesInFlight; ++i)
			{
				GPUBuffer& camera_buff = mCameraBufferdata[i];
				//create uniform buffer 
				VkDeviceSize buffer_req_size = static_cast<VkDeviceSize>(sizeof(GPUStructures::CameraUBO));
				GPUBuffer::CreateBuffer(vk_ctx, static_cast<VkDeviceSize>(buffer_req_size), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, camera_buff);

				GPUBuffer::MapBufferMemory(vk_ctx->device, camera_buff);

				GPUBuffer& voxel_param_buff = mVoxelParamBufferdata[i];
				buffer_req_size = static_cast<VkDeviceSize>(sizeof(GPUStructures::VoxelParameterUBO));

				GPUBuffer::CreateBuffer(vk_ctx, static_cast<VkDeviceSize>(buffer_req_size), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, voxel_param_buff);

				GPUBuffer::MapBufferMemory(vk_ctx->device, voxel_param_buff);
			}
			return true;
		}


		void SceneVoxelPass::CreateGfxPipeline(const VulkanContext* vk_ctx)
		{
			GraphicsPipelineSpecification gfx_specification{};
			gfx_specification.vertexShaderPath = "assets/shaders/spirvs/model_voxel.vert.spv";
			gfx_specification.fragmentShaderPath = "assets/shaders/spirvs/model_voxel.frag.spv";
			//gfx_specification.geometryShaderPath = "assets/shaders/spirvs/model_voxel.geom.spv";
			gfx_specification.renderPass = mRenderPass;

			//add required descriptor set layout & push constant 
			gfx_specification.descriptorSetLayouts = { mDescriptorSetLayout };

			VkPushConstantRange push_constant_range{};
			push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			push_constant_range.offset = 0; //<- i am presumiing that this is used if there is more than one push constant
			push_constant_range.size = sizeof(GPUStructures::Model);
			gfx_specification.pushConstantRanges = { push_constant_range };

			gfx_specification.rasterisationCullMode = VK_CULL_MODE_BACK_BIT;// VK_CULL_MODE_FRONT_BIT;
			gfx_specification.rasterisationFrontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

			gfx_specification.bHasColourAttachment = false;
			gfx_specification.bEnableAlphaBlend = false;

			gfx_specification.bEnableDepthTest = true;
			gfx_specification.bEnableDepthWrite = true;
			gfx_specification.bEnableDepthClamp = false;

			gfx_specification.depthCompareOp = VK_COMPARE_OP_LESS;

			gfx_specification.bDepthBiasEnable = false;
			gfx_specification.depthBiasConstantFactor = 1.75f;	 // Optional
			gfx_specification.depthBiasClamp = 0.0f;			// Optional
			gfx_specification.depthBiasSlopeFactor = 1.25f;	   // Optional

			gfx_specification.bDynamicViewport = false;
			gfx_specification.viewportDetails.x = 0.0f;
			gfx_specification.viewportDetails.y = 0.0f;
			gfx_specification.viewportDetails.width = static_cast<float>(mVoxelImageExtent.width);
			gfx_specification.viewportDetails.height = static_cast<float>(mVoxelImageExtent.height);
			gfx_specification.viewportDetails.minDepth = 0.0f;
			gfx_specification.viewportDetails.maxDepth = 1.0f;
			gfx_specification.scissorDetails.offset = { 0, 0 };
			gfx_specification.scissorDetails.extent = mVoxelImageExtent;

			mGfxPipeline.Init(vk_ctx, gfx_specification);
		}

		void SceneVoxelPass::Begin(VulkanContext* ctx, VkCommandBuffer cmd_buff, uint32_t img_idx)
		{
			mFrameIndex = ctx->frameIndex % kMaxFramesInFlight;

			//clear depth
			VkClearValue clear_value{};
			clear_value.depthStencil = { 1.0f, 0 };

			VkRenderPassBeginInfo render_pass_info{};
			render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			render_pass_info.renderPass = mRenderPass;
			render_pass_info.pNext = nullptr;
			render_pass_info.framebuffer = mFramebuffer.bufferHandle;
			render_pass_info.renderArea.offset = { 0, 0 };
			render_pass_info.renderArea.extent = mVoxelImageExtent;
			render_pass_info.clearValueCount = 1;
			render_pass_info.pClearValues = &clear_value;

			vkCmdBeginRenderPass(cmd_buff, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

			////later support static viewport when all vaildations are fixed
			VkViewport viewport{};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = static_cast<float>(mVoxelImageExtent.width);
			viewport.height = static_cast<float>(mVoxelImageExtent.height);
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			vkCmdSetViewport(cmd_buff, 0, 1, &viewport);

			VkRect2D scissor{};
			scissor.offset = { 0, 0 };
			scissor.extent = mVoxelImageExtent;
			vkCmdSetScissor(cmd_buff, 0, 1, &scissor);

			//vkCmdSetDepthBias(
			//	cmd_buff,
			//	depthBiasConstantFactor,
			//	depthBiasClamp,
			//	depthBiasSlopeFactor);


			//bind gfxx pipeline 
			mGfxPipeline.Bind(cmd_buff);

			//Bind descriptor sets Light matrix 
			auto& desc_set = mDescriptorSets[mFrameIndex];
			mGfxPipeline.BindDescriptorSets(cmd_buff, &desc_set);
		}


		void SceneVoxelPass::End(VkCommandBuffer cmd_buff)
		{
			vkCmdEndRenderPass(cmd_buff);
		}

		void SceneVoxelPass::UpdateCameraData(const Camera* p_scene_cam, float lf, float rt,
			float bm, float tp, float nearz, float farz)
		{

			if (p_scene_cam != nullptr)
			{
				mCamera.SetPosition(p_scene_cam->GetPosition());
				mCamera.SetForward(p_scene_cam->GetForward());
			}


			auto& buffer = mCameraBufferdata[mFrameIndex];
			nearz = 0.1f;
			farz = 100.0f;

			//quick test 
			GPUStructures::CameraUBO data;
			auto cam = (p_scene_cam != nullptr) ? *p_scene_cam : Camera();

			cam.SetNearZ(0.0f);
			cam.SetFarZ(50.0f);
			data.view = cam.GetView();
			data.projection = cam.GetOrthoProjection((float)1920 / (float)1080);
			data.projection = glm::ortho(lf, rt, bm, tp, nearz, farz);
			data.projection[1][1] *= -1;


			data.view = mCamera.GetView();
			data.projection = mCamera.GetOrthoProjection(1.0f);


			//data.view = glm::lookAt(cam.position, cam.position + cam.forward, cam.up);
			//
			//data.projection = glm::perspective(glm::radians(p_scene_cam->mFovY), (float)1920/(float)1080, -100.4f, p_scene_cam->mFarZ);
			//data.projection[1][1] *= -1;
			//data.projection = p_scene_cam->GetOrthoProjection((float)1920/(float)1080);
			//data.view = Camera::GetView(cam);
			//data.projection = Camera::GetProjection(cam, 1920, 1080);

			GPUBuffer::CopyToMappedMemory(buffer, &data, sizeof(data));

			//buffer.DirectWriteToMappedMemory<GPUStructures::CameraUBO>(data);
		}

		void SceneVoxelPass::UpdateVoxelParamData(const GPUStructures::VoxelParameterUBO& data)
		{
			auto& buffer = mVoxelParamBufferdata[mFrameIndex];
			GPUBuffer::CopyToMappedMemory(buffer, &data, sizeof(data));

			//buffer.DirectWriteToMappedMemory<GPUStructures::VoxelParameterUBO>(data);
		}

		void SceneVoxelPass::PushModelTransformConstant(VkCommandBuffer cmd_buff, const GPUStructures::Model* p_value) 
		{
			mGfxPipeline.PushConstant(cmd_buff, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPUStructures::Model), p_value);
		}

		void SceneVoxelPass::Cleanup(VulkanContext* ctx)
		{
			vkDestroyRenderPass(ctx->device, mRenderPass, nullptr);


			mGfxPipeline.Cleanup(ctx);

			for (auto& tex : mVoxelOccupancyTex)
				tex.Destory(ctx->device);
			Framebuffer::DestroyFramebuffer(ctx->device, mFramebuffer);


			for (auto& buffer : mCameraBufferdata)
				GPUBuffer::DestoryBuffer(ctx->device, buffer);

			for (auto& buffer : mVoxelParamBufferdata)
				GPUBuffer::DestoryBuffer(ctx->device, buffer);



			//DescriptorSetSpecification
			vkDestroyDescriptorSetLayout(ctx->device, mDescriptorSetLayout, nullptr);

			vkDestroyDescriptorPool(ctx->device, mDescriptorPool, nullptr);
		}


	}// namespace Vulkan
}//namespace Fluxon