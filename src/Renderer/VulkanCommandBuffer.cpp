#include "VulkanCommandBuffer.h"

#include "Core/Core.h"

#include "GraphicsAssertLog.h"

namespace Fluxon {

	namespace Vulkan {


		void VulkanCommandBuffer::Init(VkDevice device, VkCommandPool command_pool)
		{
			//buffer create info
			VkCommandBufferAllocateInfo allocation_info{};
			allocation_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocation_info.commandPool = command_pool;
			allocation_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocation_info.commandBufferCount = 1; //<-- one bufferr per pool per frame data

			FLUX_VK_ASSERT_SUCCESS(vkAllocateCommandBuffers(device, &allocation_info, &mCommandBuffer) != VK_SUCCESS, "Failed to allocate command buffers!!!!!!");
		}

		void VulkanCommandBuffer::BeginRecord(VkCommandBufferUsageFlags usage_flag)
		{
			VkCommandBufferBeginInfo begin_info{};
			begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			begin_info.flags = usage_flag;
			begin_info.pInheritanceInfo = nullptr;

			FLUX_VK_ASSERT_SUCCESS(vkBeginCommandBuffer(mCommandBuffer, &begin_info) != VK_SUCCESS, "Failed to begin recording command buffer!!!!!");

			for (auto& task : mTasks)
				task(this);

			mTasks.clear();
		}

		void VulkanCommandBuffer::EndRecord()
		{
			FLUX_VK_ASSERT_SUCCESS(vkEndCommandBuffer(mCommandBuffer) != VK_SUCCESS, "Failed to record compute command buffer!!!!");
		}



		void VulkanCommandBuffer::Reset()
		{
			vkResetCommandBuffer(mCommandBuffer, 0);
		}

		void VulkanCommandBuffer::SubmitRecording(VkQueue queue, VkFence fence, const std::vector<VkSemaphore>& wait_semaphores, 
			const std::vector<VkPipelineStageFlags>& wait_destination_stages, const std::vector<VkSemaphore>& signal_semaphores)
		{
			VkSubmitInfo submit_info{};

			submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submit_info.waitSemaphoreCount = static_cast<uint32>(wait_semaphores.size());
			submit_info.pWaitSemaphores = wait_semaphores.data();
			submit_info.pWaitDstStageMask = wait_destination_stages.data();
			submit_info.commandBufferCount = 1;
			submit_info.pCommandBuffers = &mCommandBuffer;
			submit_info.signalSemaphoreCount = static_cast<uint32>(signal_semaphores.size());
			submit_info.pSignalSemaphores = signal_semaphores.data();

			FLUX_VK_ASSERT_SUCCESS(vkQueueSubmit(queue, 1, &submit_info, fence), "Failed to submit command buffer!!!!");
		}

		void VulkanCommandBuffer::Destroy(VkDevice device, VkCommandPool cmd_pool)
		{
			if(mCommandBuffer != VK_NULL_HANDLE)
				vkFreeCommandBuffers(device, cmd_pool, 1, &mCommandBuffer);
		}

		void VulkanCommandBuffer::AddTask(std::function<void(VulkanCommandBuffer*)>& task)
		{
			mTasks.emplace_back(task);
		}

	} //namespace Vulkan

} //namesapce Fluxon 