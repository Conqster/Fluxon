#include "Swapchain.h"

#include <algorithm>
#include "VulkanUtilities.h"

#include "Resources.h"


namespace Fluxon {
	namespace Vulkan {


		VkSurfaceFormatKHR Swapchain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats) {

			for (const auto& available_format : available_formats)
				if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB &&
					available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
					//available_format.colorSpace == VK_COLOR_SPACE_HDR10_ST2084_EXT)
					return available_format;

			
			//for (const auto& available_format : available_formats)
			//		if (available_format.format == VK_FORMAT_B8G8R8A8_UNORM &&
			//			available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)

			return available_formats[0];
		}

		static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes, bool vsync) {

			if(vsync)
				return VK_PRESENT_MODE_FIFO_KHR;

			for (const auto& available_present_mode : available_present_modes)
				if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
					return available_present_mode;

			return VK_PRESENT_MODE_FIFO_KHR;
		}

		static VkExtent2D ChooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities) {
			if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)()) {
				return capabilities.currentExtent;
			}
			else {
				int width, height;
				glfwGetFramebufferSize(window, &width, &height);

				VkExtent2D actualExtent = {
					static_cast<uint32_t>(width),
					static_cast<uint32_t>(height)
				};

				actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
				actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

				return actualExtent;
			}
		}

		Swapchain::Swapchain(VulkanContext* vk_ctx, const Platform::Window& window) : mVulkanContextRef(vk_ctx)
		{
			Init(vk_ctx, window);
		}

		bool Swapchain::Init(VulkanContext* vk_ctx, const Platform::Window& window)
		{

			mVulkanContextRef = vk_ctx;
			bool success = CreateSwapchain(vk_ctx, window);

			std::vector<VkImage> swapchain_vkImage;
			success |= GetSwapchainVkImagesKHR(vk_ctx->device, UINT32_MAX, swapchain_vkImage);

			success |= CreateSwapchainImageResources(vk_ctx->device, swapchain_vkImage, mImages);

			return success;
		}


		Swapchain::~Swapchain()
		{
			//TODO (Conqster): having issue where when a swapchain is create & destroy immediately
			//Cleanup(mVulkanContextRef);
		}



		void Swapchain::Cleanup(const VulkanContext* vk_ctx)
		{
			if (!vk_ctx)
			{
				FLUX_DEBUG_LOG_WARNING("Swap chain clean not null Vulkan Context!!");
				return;
			}


			if(vk_ctx->device !=  VK_NULL_HANDLE)
			{
				//images
				for (auto& [img, img_view, render_complete] : mImages)
				{
					//vkDestroyFramebuffer(mVulkanContext->device, framebuffer, nullptr);
					vkDestroyImageView(vk_ctx->device, img_view, nullptr);

					vkDestroySemaphore(vk_ctx->device, render_complete, nullptr);
				}
				//mImages.clear();

				//swap chain vulkan handle
				vkDestroySwapchainKHR(vk_ctx->device, mSwapchain, nullptr);
			}
		}

		uint32 Swapchain::AcquireNextImageIdx(VulkanContext* vk_ctx, VkSemaphore frame_img_avail_semaphore)
		{
			VkResult result = vkAcquireNextImageKHR(
				vk_ctx->device, mSwapchain, UINT64_MAX,
				frame_img_avail_semaphore, VK_NULL_HANDLE,
				&mAcquiredImageIdx);

			CheckVkResultImageOutOfDate(result, vk_ctx, "Acquire");

			return mAcquiredImageIdx;
		}

		SwapchainImage& Swapchain::AcquireNextImage(VulkanContext* vk_ctx, VkSemaphore frame_img_avail_semaphore)
		{
			return mImages[AcquireNextImageIdx(vk_ctx, frame_img_avail_semaphore)];
		}

		void Swapchain::Present(VulkanContext* vk_ctx)
		{
			//Present
			VkPresentInfoKHR present_info{};
			present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			present_info.waitSemaphoreCount = 1;
			present_info.pWaitSemaphores = &mImages[mAcquiredImageIdx].renderComplete;//&render_complete_semaphore;
			present_info.swapchainCount = 1;
			present_info.pSwapchains = &mSwapchain;
			present_info.pImageIndices = &mAcquiredImageIdx;
			present_info.pResults = nullptr;

			VkResult result = vkQueuePresentKHR(vk_ctx->presentQueue, &present_info);

			CheckVkResultImageOutOfDate(result, vk_ctx, "Present");
			//if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || vk_ctx->pMainPlatformWindow->windowResized)
			//{
			//	std::cout << "\033[1;31m [DRAW FRAME]\033[0m: " << "Need to Resize image acquire, for Presentation" << std::endl;
			//	vk_ctx->pMainPlatformWindow->windowResized = false;
			//	OnResize(vk_ctx, cmd_pool);
			//}
			//else if (result != VK_SUCCESS)
			//	std::cout << "\033[1;31m [DRAW FRAME]\033[0m: " << "Failed to Present Image" << std::endl;
		}

		void Swapchain::OnResize(VulkanContext* vk_ctx)
		{
			int width = 0;
			int height = 0;


			glfwGetFramebufferSize(vk_ctx->pMainPlatformWindow->glfwWindowHandle, &width, &height);
			while (width == 0 || height == 0)
			{
				glfwGetFramebufferSize(vk_ctx->pMainPlatformWindow->glfwWindowHandle, &width, &height);
				glfwWaitEvents();
			}
			//if (vk_ctx->pMainPlatformWindow->IsWindowMinimised())
			//	return;


			if(mExtent.width != static_cast<uint32>(width) || mExtent.height != static_cast<uint32>(height))
			{
				//vkDeviceWaitIdle(vk_ctx->device);

				//no need to clean at this time when new swapchain as been created and old 
				//is destroied it cleans up old image & imge views 
				//Cleanup(vk_ctx);
				//TODO(Conqster): Need to fix REsizing

				mOldSwapchain = mSwapchain;
				bool success = CreateSwapchain(vk_ctx, *vk_ctx->pMainPlatformWindow);

				//no need  for heavy init 
				//Init(vk_ctx, *vk_ctx->pMainPlatformWindow);
				std::vector<VkImage> swapchain_vkImage;
				if (GetSwapchainVkImagesKHR(vk_ctx->device, static_cast<uint32>(mImages.size()), swapchain_vkImage))
					UpdateSwapchainImageResources(vk_ctx->device, swapchain_vkImage, mImages);

				//success |= CreateSwapchainImages(vk_ctx->device, static_cast<uint32>(mImages.size()));

				//when swapchin is reconstructed mExtend would have been updated
				for (auto& on_resize : mRegisteryResize)
					on_resize(mExtent.width, mExtent.height);
			}
		}

		bool Swapchain::CreateSwapchain(const VulkanContext* vk_ctx, const Platform::Window& window)
		{
			SwapchainSupportDetails swap_chain_support = Utils::QuerySwapchainSupport(vk_ctx->physicalDevice, vk_ctx->surface);

			VkSurfaceFormatKHR surface_format = ChooseSwapSurfaceFormat(swap_chain_support.formats);
			VkPresentModeKHR present_mode = ChooseSwapPresentMode(swap_chain_support.presentModes, false);
			VkExtent2D extent = ChooseSwapExtent(window.glfwWindowHandle, swap_chain_support.capabilities);

			// +1 
			//because the min means we might sometimes have to wait on the driver to complete
			//internal operations before we can acquire another imgae to render
			uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;

			//maxImageCount = 0, special case if no maximum
			if (swap_chain_support.capabilities.maxImageCount > 0 && image_count > swap_chain_support.capabilities.maxImageCount)
				image_count = swap_chain_support.capabilities.maxImageCount;


			VkSwapchainCreateInfoKHR swapchain_create_info{};
			swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			swapchain_create_info.surface = vk_ctx->surface;

			swapchain_create_info.minImageCount = image_count;
			swapchain_create_info.imageFormat = surface_format.format;
			swapchain_create_info.imageColorSpace = surface_format.colorSpace;
			swapchain_create_info.imageExtent = extent;
			swapchain_create_info.imageArrayLayers = 1;
			swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

			QueueFamilyIndices indices = Utils::FindDeviceQueueFamilies(*vk_ctx);
			uint32_t queue_family_indices[] = {
				indices.graphicsAndComputeFamily.value(),
				indices.presentFamily.value()
			};

			if (indices.graphicsAndComputeFamily != indices.presentFamily)
			{
				swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
				swapchain_create_info.queueFamilyIndexCount = 2;
				swapchain_create_info.pQueueFamilyIndices = queue_family_indices;
			}
			else
			{
				swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
				swapchain_create_info.queueFamilyIndexCount = 0;
				swapchain_create_info.pQueueFamilyIndices = nullptr;
			}


			swapchain_create_info.preTransform = swap_chain_support.capabilities.currentTransform;
			swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			swapchain_create_info.presentMode = present_mode;
			swapchain_create_info.clipped = VK_TRUE;

			swapchain_create_info.oldSwapchain = mOldSwapchain;


			bool success = vkCreateSwapchainKHR(vk_ctx->device, &swapchain_create_info, nullptr, &mSwapchain) == VK_SUCCESS;

			FLUX_DEBUG_ASSERT(success, "Failed to create swap chain!!!!");

			if(mOldSwapchain != VK_NULL_HANDLE && success)
				vkDestroySwapchainKHR(vk_ctx->device, mOldSwapchain, nullptr);

			//utilities
			mImageFormat = surface_format.format;
			mExtent = extent;

			return success;
		}





		bool Swapchain::GetSwapchainVkImagesKHR(VkDevice vk_ctx_device, uint32 requested_counts, std::vector<VkImage>& out_swapchain_vkimages)
		{
			bool success = true;


			uint32 image_count = requested_counts;
			if(requested_counts == 0 || requested_counts == UINT32_MAX) //preventing if user pass requested_counts as -1
				success = (vkGetSwapchainImagesKHR(vk_ctx_device, mSwapchain, &image_count, nullptr) == VK_SUCCESS);

			FLUX_DEBUG_ASSERT(success, " Requested count: ", requested_counts, ", if invalid & and failed get GPU supported swap chain image count.");
			if (!success)
				return false;

			out_swapchain_vkimages.resize(image_count);
			success = (vkGetSwapchainImagesKHR(vk_ctx_device, mSwapchain, &image_count, out_swapchain_vkimages.data()) == VK_SUCCESS);
			FLUX_DEBUG_ASSERT(success, "Failed to get swap chain KHR images!!!!");
			if (!success)
				return false;

			return true;
		}


		bool Swapchain::CreateSwapchainImageResources(VkDevice vk_ctx_device, std::vector<VkImage>& images, std::vector<SwapchainImage>& out_swapchain_images)
		{
			out_swapchain_images.resize(images.size());
			VkSemaphoreCreateInfo semaphore_create_info{};
			semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			for (size_t i = 0; i < out_swapchain_images.size(); i++)
			{
				SwapchainImage swapchain_image{};
				swapchain_image.image = images[i];
				swapchain_image.imageView = Resources::CreateImageView2D(vk_ctx_device, images[i], mImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
				FLUX_VK_ASSERT_SUCCESS(vkCreateSemaphore(vk_ctx_device, &semaphore_create_info, nullptr, &swapchain_image.renderComplete),
					"Failed to create semaphore!!!!!!");

				out_swapchain_images[i] = swapchain_image;
			}
			return true;
		}

		bool Swapchain::UpdateSwapchainImageResources(VkDevice vk_ctx_device, std::vector<VkImage>& images, std::vector<SwapchainImage>& out_swapchain_images)
		{
			//if a complete resize is required
			if (images.size() != out_swapchain_images.size())
			{
				//destory old image elements 
				for (auto& image : out_swapchain_images)
				{
					vkDestroyImageView(vk_ctx_device, image.imageView, nullptr);
					vkDestroySemaphore(vk_ctx_device, image.renderComplete, nullptr);
				}

				return CreateSwapchainImageResources(vk_ctx_device, images, out_swapchain_images);
			}

			for (size_t i = 0; i < out_swapchain_images.size(); i++)
			{
				//destroy old image view 
				vkDestroyImageView(vk_ctx_device, mImages[i].imageView, nullptr);

				out_swapchain_images[i].image = images[i];
				out_swapchain_images[i].imageView = Resources::CreateImageView2D(vk_ctx_device, images[i], mImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
			}

			return true;
		}

		void Swapchain::CheckVkResultImageOutOfDate(const VkResult& result, VulkanContext* vk_ctx, const char* label)
		{
			if (result == VK_ERROR_OUT_OF_DATE_KHR)
			{
				FLUX_DEBUG_LOG_INFO("\033[1;31m [DRAW FRAME]\033[0m: Need to Recreate swap @", label);
				OnResize(vk_ctx);
				return;
			}
			else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
				FLUX_DEBUG_LOG_INFO("\033[1;31m [DRAW FRAME]\033[0m: Failed to ", label, " Swap Chain Image");
		}
	} //#vulkan
}//#Fluxon