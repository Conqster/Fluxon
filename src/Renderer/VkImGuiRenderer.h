#pragma once

#include "Core/Core.h"

#include <vulkan/vulkan.h>

#include "RenderTarget.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_vulkan.h>

struct GLFWwindow;
namespace Fluxon {
	class Application;
	namespace Vulkan {
		struct VulkanContext;
	}
}
namespace Fluxon {



	class VkImGuiRenderer
	{
	public: 
		VkImGuiRenderer() {}
		VkImGuiRenderer(uint32 width, uint32 height) {}
		~VkImGuiRenderer(){}



		void Init(const Vulkan::VulkanContext* vk_ctx, GLFWwindow* glfwWindowHandle);


		void OnWindowResize(const Vulkan::VulkanContext* vk_ctx, uint32 width, uint32 height);

		void BeginNewFrame();
		void RenderFrame(const Vulkan::VulkanContext* vk_ctx, VkCommandBuffer cmd_buff, uint32 swap_chain_img_idx);

		//another reason to make this main/engine render pass
		VkRenderPass& GetRenderPassHandle() { return mRenderPass; }
	
		//
		void Cleanup(const Vulkan::VulkanContext* vk_ctx);
		void ShutDown();

	private:
		friend Application;

		uint32 mWidth;
		uint32 mHeight;

		ImGui_ImplVulkanH_Window mWindowData;

		VkDescriptorPool mDescriptorPool;
		VkRenderPass mRenderPass;

		std::vector<Vulkan::Framebuffer> mFramebuffers;

		void VulkanWindowData(const Vulkan::VulkanContext* vk_ctx, ImGui_ImplVulkanH_Window* win, uint32 width, uint32 height, bool enable_clear);


		bool CreateRenderPass(const Vulkan::VulkanContext* vk_ctx, std::optional<VkFormat> col_format = std::nullopt, bool clear_on_load = true);

		void ReceateWindowBackBuffer(const Vulkan::VulkanContext* vk_ctx, Vulkan::Swapchain& swap_chain);
		bool RecreateFramebuffer(const Vulkan::VulkanContext* vk_ctx, uint32 width, uint32 height, Vulkan::Swapchain& swap_chain, std::vector<Vulkan::Framebuffer>& out_framebuffers);
	};

} //Fluxon