#include "DebugLine.h"

#include "VulkanUtilities.h"

#include "Core/AssertLog.h"

namespace Fluxon {
	namespace Vulkan {
		void DebugLine::CreateFrameBufferData(const VulkanContext* vk_ctx, LineFrameBuff& fb, uint32_t vertex_capacity)
		{
			if (fb.gpuBuffer.buffer != VK_NULL_HANDLE || fb.gpuBuffer.mapped != nullptr)
				GPUBuffer::DestoryBuffer(vk_ctx->device, fb.gpuBuffer);

			VkDeviceSize btye_size = sizeof(GPUStructures::DebugLineVertex) * vertex_capacity;

			GPUBuffer::CreateBuffer(vk_ctx, btye_size,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			fb.gpuBuffer);

			GPUBuffer::MapBufferMemory(vk_ctx->device, fb.gpuBuffer);

			fb.capacity = vertex_capacity;
			fb.count = 0;
		}
		void DebugLine::Init(const VulkanContext* vk_ctx, VkRenderPass& global_forward_pass, VkDescriptorSetLayout& desc_set_with_cam_as_set1)
		{
			Vulkan::GraphicsPipelineSpecification dubug_gfx_pipeline_spec{};
			dubug_gfx_pipeline_spec.vertexShaderPath = "assets/shaders/spirvs/debug_line.vert.spv";
			dubug_gfx_pipeline_spec.fragmentShaderPath = "assets/shaders/spirvs/debug_line.frag.spv";

			dubug_gfx_pipeline_spec.renderPass = global_forward_pass;
			dubug_gfx_pipeline_spec.rasterisationCullMode = VK_CULL_MODE_NONE;

			dubug_gfx_pipeline_spec.vertexInputPrimitiveTopology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;

			dubug_gfx_pipeline_spec.bEnableDepthTest = true;
			dubug_gfx_pipeline_spec.bEnableDepthWrite = false;

			dubug_gfx_pipeline_spec.vertexBindingDescription = GPUStructures::DebugLineVertex::GetBindingDescription();
			dubug_gfx_pipeline_spec.vertexAttribDesc = GPUStructures::DebugLineVertex::GetAttributeDescriptionsAsVector();

			dubug_gfx_pipeline_spec.descriptorSetLayouts = {
				//Utils::CreateDummyDescriptorSetLayout(vk_ctx),
				vk_ctx->dummyDescSetLayout,
				desc_set_with_cam_as_set1
			};

			mGfxPipeline.Init(vk_ctx, dubug_gfx_pipeline_spec);


			//create per-frame mapped buffer
			for (uint32_t i = 0; i < kFramesInFlight; ++i)
				CreateFrameBufferData(vk_ctx, mLineFrame[i], kInitialVertexLimit);
		}
		void DebugLine::BeginFrame(uint32_t frame_idx)
		{
			mFrameIndex = frame_idx % kFramesInFlight;
			mLineFrame[mFrameIndex].count = 0;
		}
		void DebugLine::PushLine(const GPUStructures::DebugLineVertex& v0, const GPUStructures::DebugLineVertex& v1)
		{
			LineFrameBuff& line_fbo = mLineFrame[mFrameIndex];
			const uint32_t needed = line_fbo.count + 2;

			FLUX_DEBUG_ASSERT(needed < line_fbo.capacity, "Debug Line surpass limit!!!!!");

			auto* dst = reinterpret_cast<GPUStructures::DebugLineVertex*>(line_fbo.gpuBuffer.mapped);
			dst[line_fbo.count + 0] = v0;
			dst[line_fbo.count + 1] = v1;
			line_fbo.count += 2;
		}
		void DebugLine::DrawDebugLines(const VulkanContext* vk_ctx, VkCommandBuffer cmd_buff, const VkDescriptorSet* desc_set_with_cam_as_set1)
		{
			LineFrameBuff& line_fbo = mLineFrame[mFrameIndex];
			if (line_fbo.count == 0)
				return;

			mGfxPipeline.Bind(cmd_buff);
			mGfxPipeline.BindDescriptorSets(cmd_buff, desc_set_with_cam_as_set1, 1, 1);

			VkBuffer vbs[] = { line_fbo.gpuBuffer.buffer };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(cmd_buff, 0, 1, vbs, offsets);
			vkCmdDraw(cmd_buff, line_fbo.count, 1, 0, 0);
		}


		void DebugLine::Cleanup(class VulkanContext* ctx)
		{
			mGfxPipeline.Cleanup(ctx);
			for(uint32_t i = 0; i < kFramesInFlight; ++i)
				if(mLineFrame[i].gpuBuffer.buffer)
					GPUBuffer::DestoryBuffer(ctx->device, mLineFrame[i].gpuBuffer);
		}
	} //namespace Vulkan
} //namespace Fl