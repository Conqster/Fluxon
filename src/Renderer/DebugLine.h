#pragma once

#include "Pipelines/GraphicsPipeline.h"
#include "GPUBuffer.h"


namespace Fluxon {
	namespace Vulkan {

		class DebugLine
		{
		private:
			static constexpr int kInitialVertexLimit = 8192; //4096 lines
			static constexpr uint32_t kFramesInFlight = 2;

			GraphicsPipeline mGfxPipeline;


			struct LineFrameBuff {
				GPUBuffer gpuBuffer;
				uint32_t capacity = 0;
				uint32_t count = 0;
			};

			LineFrameBuff mLineFrame[kFramesInFlight];
			uint32_t mFrameIndex = 0;

			void CreateFrameBufferData(const VulkanContext* vk_ctx, LineFrameBuff& fb, uint32_t vertex_capacity);

		public:
			void Init(const VulkanContext* vk_ctx, VkRenderPass& global_forward_pass, VkDescriptorSetLayout& desc_set_with_cam_as_set1);
			
			void BeginFrame(uint32_t frame_idx);

			void PushLine(const GPUStructures::DebugLineVertex& v0, const GPUStructures::DebugLineVertex& v1);
			void DrawDebugLines(const VulkanContext* vk_ctx, VkCommandBuffer cmd_buff, const VkDescriptorSet* desc_set_with_cam_as_set1);

			void Cleanup(class VulkanContext* ctx);
		};

	} //namespace Vulkan
} //namespace Fluxon