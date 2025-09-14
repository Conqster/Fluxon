#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <functional>


#include "Core/Core.h"

namespace Fluxon {
	namespace Vulkan {
		struct VulkanContext;
	}
}

namespace Fluxon {

	namespace Vulkan {

		class VulkanCommandBuffer// : NonCopyable
		{
		public: 
			const VkCommandBuffer GetCommandHandle() const { return mCommandBuffer; }

			void Init(VkDevice device, VkCommandPool command_pool);

			void BeginRecord(VkCommandBufferUsageFlags usage_flag = 0);
			void EndRecord();

			void Reset();

			void SubmitRecording(VkQueue queue, VkFence fence,
				const std::vector<VkSemaphore>& wait_semaphores,
				const std::vector<VkPipelineStageFlags>& wait_destination_stages,
				const std::vector<VkSemaphore>& signal_semaphores);


			void Destroy(VkDevice device, VkCommandPool cmd_pool);

			void AddTask(std::function<void(VulkanCommandBuffer*)>& task);

		private:
			VkCommandBuffer mCommandBuffer;
			std::vector<std::function<void(VulkanCommandBuffer*)>> mTasks;
		};

	} //namespace Vulkan

} //namesapce Fluxon 