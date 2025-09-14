#include "VkImGuiRenderer.h"


#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_vulkan.h>


#include "VulkanContext.h"
#include "Resources.h"


#include "VulkanUtilities.h"

namespace Fluxon {
	
	void CheckVulkanResult(VkResult err)
	{
		if (err == 0)return;
		//std::cerr << "\033[1;33m [ImGui][Vulkan]\033[0m Error: " << err << std::endl;
		fprintf(stderr, "[Vulkan] Error: VkResult = %d\n", err);
		if (err < 0)
			abort();
	}
	
	void VkImGuiRenderer::Init(const Vulkan::VulkanContext* vk_ctx, GLFWwindow* glfwWindowHandle)
	{
		ImGui_ImplGlfw_InitForVulkan(glfwWindowHandle, true);

		ImGui_ImplVulkanH_Window* wd = &mWindowData;

		Platform::Window platform_win = *vk_ctx->pMainPlatformWindow;
		VulkanWindowData(vk_ctx, wd, platform_win.width, platform_win.height, true);

		ImGui_ImplVulkan_InitInfo imgui_init_info{};
		imgui_init_info.Instance = vk_ctx->instance;
		imgui_init_info.PhysicalDevice = vk_ctx->physicalDevice;
		imgui_init_info.Device = vk_ctx->device;
		imgui_init_info.DescriptorPool = mDescriptorPool;
		imgui_init_info.RenderPass =mRenderPass;



		Fluxon::Vulkan::QueueFamilyIndices queue_families = Fluxon::Vulkan::Utils::FindDeviceQueueFamilies(*vk_ctx);
		FLUX_DEBUG_ASSERT(queue_families.graphicsAndComputeFamily.has_value(), "Faild to find graphics queue fgamily for ImGui!!!!!!");
		imgui_init_info.QueueFamily = queue_families.graphicsAndComputeFamily.value();


		Fluxon::Vulkan::SwapchainSupportDetails swap_chain_support = Fluxon::Vulkan::Utils::QuerySwapchainSupport(vk_ctx->physicalDevice, vk_ctx->surface);
		// +1 
		//because the min means we might sometimes have to wait on the driver to complete
		//internal operations before we can acquire another imgae to render
		imgui_init_info.MinImageCount = swap_chain_support.capabilities.minImageCount + 1;

		imgui_init_info.Queue = vk_ctx->graphicsQueue;
		imgui_init_info.ImageCount = vk_ctx->swapchain.get()->GetImageCount();
		imgui_init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		imgui_init_info.Subpass = VK_PIPELINE_BIND_POINT_GRAPHICS;
		imgui_init_info.CheckVkResultFn = CheckVulkanResult;



		bool success = ImGui_ImplVulkan_Init(&imgui_init_info);
	}

	void VkImGuiRenderer::VulkanWindowData(const Vulkan::VulkanContext* vk_ctx, ImGui_ImplVulkanH_Window* win, uint32 width, uint32 height, bool enable_clear)
	{


		{
			VkDescriptorPoolSize poolSizes[] = {
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
				{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000} };
			VkDescriptorPoolCreateInfo poolInfo = {};
			poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
			poolInfo.maxSets = 1000 * IM_ARRAYSIZE(poolSizes);
			poolInfo.poolSizeCount = (uint32_t)IM_ARRAYSIZE(poolSizes);
			poolInfo.pPoolSizes = poolSizes;
			FLUX_VK_ASSERT_SUCCESS(vkCreateDescriptorPool(vk_ctx->device, &poolInfo, nullptr, &mDescriptorPool), "Failed vk imgui renderer description pool");
		}

		win->Surface = vk_ctx->surface;
		win->ClearEnable = enable_clear;

		auto swap_chain = vk_ctx->swapchain.get();
		win->Swapchain = swap_chain->GetHandle();
		win->Width = width;
		win->Height = height;

		win->ImageCount = static_cast<uint32>(swap_chain->GetImages().size());

		Vulkan::SwapchainSupportDetails swap_chain_support = Vulkan::Utils::QuerySwapchainSupport(vk_ctx->physicalDevice, vk_ctx->surface);
		VkSurfaceFormatKHR surface_format = Vulkan::Swapchain::ChooseSwapSurfaceFormat(swap_chain_support.formats);
		VkFormat col_format = surface_format.format;


		//VkFormat col_format = swap_chain->GetFormat();
		CreateRenderPass(vk_ctx, col_format, true);

		win->RenderPass = mRenderPass;

		//win->Frames = (ImGui_ImplVulkanH_Frame*) IM_ALLOC(sizeof(ImGui_ImplVulkanH_Frame) * win->ImageCount);

		win->Frames.resize(win->ImageCount);
		// Create The Image Views
		ReceateWindowBackBuffer(vk_ctx, *swap_chain);


		RecreateFramebuffer(vk_ctx, width, height, *swap_chain, mFramebuffers);

	}






	bool VkImGuiRenderer::CreateRenderPass(const Vulkan::VulkanContext* vk_ctx, std::optional<VkFormat> col_format, bool clear_on_load)
	{
		if (!col_format.has_value())
		{
			FLUX_DEBUG_LOG_WARNING("No format provided to create attachments sfor trender target!!!!!!");
			return false;
		}

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 0;
		subpass.pColorAttachments = nullptr;
		subpass.pDepthStencilAttachment = nullptr;


		//if both col & depth then 0 1 resp 
		//but if only one or the other then attach with be 0 for use
		std::vector<VkAttachmentDescription> attachments;

		if(col_format.has_value())
		{
			VkAttachmentDescription colour_attachment{};
			colour_attachment.format = col_format.value();
			colour_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
			colour_attachment.loadOp = (clear_on_load) ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
			colour_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			colour_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colour_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			colour_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			colour_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			
			attachments.push_back(colour_attachment);

			VkAttachmentReference colour_attachment_ref{};
			colour_attachment_ref.attachment = attachments.size() - 1;
			colour_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			//update subpass 
			subpass.colorAttachmentCount = 1;
			subpass.pColorAttachments = &colour_attachment_ref;
		}



		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;

		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = 0;

		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		//Render pass 
		VkRenderPassCreateInfo renderpass_create_info{};

		renderpass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderpass_create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderpass_create_info.pAttachments = attachments.data();
		renderpass_create_info.subpassCount = 1;
		renderpass_create_info.pSubpasses = &subpass;
		renderpass_create_info.dependencyCount = 1;
		renderpass_create_info.pDependencies = &dependency;


		FLUX_VK_ASSERT_SUCCESS(vkCreateRenderPass(vk_ctx->device, &renderpass_create_info, nullptr, &mRenderPass), "Failed to create render pass!!!!!!");


		return true;
	}

	void VkImGuiRenderer::ReceateWindowBackBuffer(const Vulkan::VulkanContext* vk_ctx, Vulkan::Swapchain& swap_chain)
	{
		auto* win = &mWindowData;
		
		for (uint32_t i = 0; i < win->ImageCount; i++)
		{
			auto scBuffer = swap_chain.GetImages()[0];
			win->Frames[i].Backbuffer = scBuffer.image;
			win->Frames[i].BackbufferView = scBuffer.imageView;
		}
		
	}

	bool VkImGuiRenderer::RecreateFramebuffer(const Vulkan::VulkanContext* vk_ctx, uint32 width, uint32 height, Vulkan::Swapchain& swap_chain, std::vector<Vulkan::Framebuffer>& out_framebuffers)
	{
		auto* win = &mWindowData;

		if (win == nullptr)
			return false;


		//might have gpu dangling address

		out_framebuffers.clear();
		auto& swapchain_images = swap_chain.GetImages();
		out_framebuffers.resize(swapchain_images.size());

		VkImageView image_view;

		for (size_t i = 0; i < swapchain_images.size(); i++)
		{
			image_view = swapchain_images[i].imageView;
			if (!Vulkan::Framebuffer::Create(vk_ctx, mRenderPass, out_framebuffers[i], { width, height }, { image_view }, 1))
			{
				FLUX_DEBUG_LOG_WARNING("Failed to Create Framebuffer");
				//destroy any created fbos 
				for (auto& fbo : out_framebuffers)
					Vulkan::Framebuffer::DestroyFramebuffer(vk_ctx->device, fbo);
				return false;
			}
			win->Frames[i].Framebuffer = out_framebuffers[i].bufferHandle;
		}

		return true;
	}

	void VkImGuiRenderer::OnWindowResize(const Vulkan::VulkanContext* vk_ctx, uint32 width, uint32 height)
	{
		auto* win = &mWindowData;

		//In the hope that the swap chain has been reconstrutedd 
		auto  swap_chain = vk_ctx->swapchain.get();

		win->Width = width;
		win->Height = height;
		win->Swapchain = swap_chain->GetHandle();

		ReceateWindowBackBuffer(vk_ctx, *swap_chain);


		std::vector<Vulkan::Framebuffer> new_framebuffers;
		if (RecreateFramebuffer(vk_ctx, width, height, *swap_chain, new_framebuffers))
		{
			for (auto& fbo : mFramebuffers)
				Vulkan::Framebuffer::DestroyFramebuffer(vk_ctx->device, fbo);

			mFramebuffers = new_framebuffers;
		}
		else
			FLUX_DEBUG_LOG_INFO("On window Resize failed, framebuffers resize for VkImGuiRenderer");




		//there is no need to recreate render pass as its not dependent 
		//on the view size, nor image & image view its self nor framebuffer 
		//only re creaete render pass if 
		// attachment format changed i.e (image/image view format) 
		// attachement target op (load op, store op, clear op, stencil op) 
		// image layout 
		// essentail rendering pass behaviour 
		//
		//CreateRenderPass(vk_ctx, swap_chain->GetFormat(), true);
	}

	void VkImGuiRenderer::BeginNewFrame()
	{
		// Start the Dear ImGui frame
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void VkImGuiRenderer::RenderFrame(const Vulkan::VulkanContext* vk_ctx, VkCommandBuffer cmd_buff, uint32 swap_chain_img_idx)
	{
		ImGui::Render();

		VkClearValue clear_values{};
		clear_values.color = { 0.0f, 0.0f, 0.0f, 1.0f };

		VkExtent2D swapchain_extent = vk_ctx->swapchain->GetExtent();
		VkRenderPassBeginInfo render_pass_info{};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		render_pass_info.renderPass = mRenderPass;
		//render_pass_info.framebuffer = mSwapchainFramebuffers[image_idx];
		render_pass_info.framebuffer = mFramebuffers[swap_chain_img_idx].bufferHandle;
		render_pass_info.renderArea.offset = { 0, 0 };
		render_pass_info.renderArea.extent = swapchain_extent;
		render_pass_info.clearValueCount = 1;
		render_pass_info.pClearValues = &clear_values;

		vkCmdBeginRenderPass(cmd_buff, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapchain_extent.width);
		viewport.height = static_cast<float>(swapchain_extent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(cmd_buff, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = swapchain_extent;
		vkCmdSetScissor(cmd_buff, 0, 1, &scissor);


		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd_buff);

		vkCmdEndRenderPass(cmd_buff);

		//ImGui_ImplVulkanH_Window* wd = &g_WindowData;
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}



	void VkImGuiRenderer::Cleanup(const Vulkan::VulkanContext* vk_ctx)
	{
		//Allow ImGui to manage the Descriptor pool as other imgui 
		// resource would be using the pool
		//if(mDescriptorPool != VK_NULL_HANDLE)
		//	vkDestroyDescriptorPool(vk_ctx->device, mDescriptorPool, nullptr);


		for(auto& fbo : mFramebuffers)
			Vulkan::Framebuffer::DestroyFramebuffer(vk_ctx->device, fbo);


		if(mRenderPass != VK_NULL_HANDLE)
			vkDestroyRenderPass(vk_ctx->device, mRenderPass, nullptr);


	}


} //namespace Fluxon

