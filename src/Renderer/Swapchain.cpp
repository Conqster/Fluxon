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

			return available_formats[0];
		}

		static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes) {

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

		Swapchain::Swapchain(VulkanContext* vk_ctx, const Platform::Window& window, VkCommandPool command_pool, VkRenderPass render_pass) : mVulkanContext(vk_ctx)
		{
			CreateSwapchain(vk_ctx, window);

			std::vector<VkImage> temp_swapchain_images = CreateImages();
			std::vector<VkImageView> temp_swapchain_image_views = CreateImageViews((uint32_t)temp_swapchain_images.size(), temp_swapchain_images);

			CreateDepthResources(command_pool);

			std::vector<VkFramebuffer> temp_swapchain_framebuffers = CreateFramebuffers(temp_swapchain_image_views, render_pass);

			UnifySwapchainImageResources(temp_swapchain_images, temp_swapchain_image_views, temp_swapchain_framebuffers);

		}

		//Swapchain::Swapchain(const Vulkan2::VulkanContext* vk_render_ctx, const Platform::Window& window, VkCommandPool command_pool, VkRenderPass render_pass)
		//{

		//	mDevice = new Device();
		//	mDevice->instance = vk_render_ctx->instance;
		//	mDevice->physicalDevice = vk_render_ctx->physicalDevice;
		//	mDevice->device = vk_render_ctx->device;
		//	mDevice->surface = vk_render_ctx->surface;
		//	mDevice->graphicsQueue = vk_render_ctx->graphicsQueue;
		//	mDevice->presentQueue = vk_render_ctx->presentQueue;
		//	mDevice->queueFamilyIndices = vk_render_ctx->queueFamilyIndices;
		//	mDevice->debugMessager = vk_render_ctx->debugMessager;

		//	CreateSwapchain(mDevice, window);

		//	std::vector<VkImage> temp_swapchain_images = CreateImages();
		//	std::vector<VkImageView> temp_swapchain_image_views = CreateImageViews((uint32_t)temp_swapchain_images.size(), temp_swapchain_images);

		//	CreateDepthResources(command_pool);

		//	std::vector<VkFramebuffer> temp_swapchain_framebuffers = CreateFramebuffers(temp_swapchain_image_views, render_pass);

		//	UnifySwapchainImageResources(temp_swapchain_images, temp_swapchain_image_views, temp_swapchain_framebuffers);
		//}

		Swapchain::~Swapchain()
		{
			//TODO (Conqster): having issue where when a swapchain is create & destroy immediately
			Cleanup();
		}



		void Swapchain::Cleanup()
		{
			if (!mVulkanContext)
			{
				printf("Probably managed by unique system!!!!!! BAD BAD\n");
				return;
			}
			//depth resources
			vkDestroyImage(mVulkanContext->device, mDepth.image, nullptr);
			vkFreeMemory(mVulkanContext->device, mDepth.memory, nullptr);
			vkDestroyImageView(mVulkanContext->device, mDepth.imageView, nullptr);

			//images
			for (auto& [img, img_view, framebuffer, render_complete] : mImages)
			{
				vkDestroyFramebuffer(mVulkanContext->device, framebuffer, nullptr);
				vkDestroyImageView(mVulkanContext->device, img_view, nullptr);

				vkDestroySemaphore(mVulkanContext->device, render_complete, nullptr);
			}

			//swap chain vulkan handle
			vkDestroySwapchainKHR(mVulkanContext->device, mSwapchain, nullptr);
			mVulkanContext = nullptr;
		}

		void Swapchain::CreateSwapchain(const VulkanContext* vk_ctx, const Platform::Window& window)
		{
			SwapchainSupportDetails swap_chain_support = Utils::QuerySwapchainSupport(vk_ctx->physicalDevice, vk_ctx->surface);

			VkSurfaceFormatKHR surface_format = ChooseSwapSurfaceFormat(swap_chain_support.formats);
			VkPresentModeKHR present_mode = ChooseSwapPresentMode(swap_chain_support.presentModes);
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

			swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;


			if (vkCreateSwapchainKHR(vk_ctx->device, &swapchain_create_info, nullptr, &mSwapchain) != VK_SUCCESS)
				throw std::runtime_error("Failed to create swap chain!!!!");

			//utilities
			mImageFormat = surface_format.format;
			mExtent = extent;
		}


		std::vector<VkImage> Swapchain::CreateImages()
		{
			uint32_t image_count = 0;
			vkGetSwapchainImagesKHR(mVulkanContext->device, mSwapchain, &image_count, nullptr);
			std::vector<VkImage> temp_swapchain_images(image_count);
			vkGetSwapchainImagesKHR(mVulkanContext->device, mSwapchain, &image_count, temp_swapchain_images.data());


			return temp_swapchain_images;
		}
		std::vector<VkImageView> Swapchain::CreateImageViews(uint32_t counts, std::vector<VkImage> images)
		{
			std::vector<VkImageView> temp_swapchain_img_views(counts);

			for (size_t i = 0; i < temp_swapchain_img_views.size(); i++)
				temp_swapchain_img_views[i] = Resources::CreateImageView2D(mVulkanContext->device, images[i], mImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);

			return temp_swapchain_img_views;
		}

		//utility
		static VkFormat  FindDepthFormat(const VkPhysicalDevice& physical_device) {
			return Utils::FindSupportedFormat(
				physical_device,
				{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, //<<-- CANDIDATES FORMAT FOR DEPTH TESTING
				VK_IMAGE_TILING_OPTIMAL,
				VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
			);
		}

		void Swapchain::CreateDepthResources(VkCommandPool command_pool)
		{
			VkFormat depth_format = FindDepthFormat(mVulkanContext->physicalDevice);


			/////////////////////////////////
			// when have a vk Resources tool file /class 
			// this function could be added
			/////////////////////////////////
			Resources::CreateImage2D(mVulkanContext, mExtent.width, mExtent.height, depth_format,
				VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mDepth.image, mDepth.memory);

			mDepth.imageView = Resources::CreateImageView2D(mVulkanContext->device, mDepth.image, depth_format, VK_IMAGE_ASPECT_DEPTH_BIT);


			//THE TRANSITION DONT NEED TO BE EXPLICITY 
			//Utils::TransitionImageLayout(mVulkanContext, command_pool, mDepth.image, depth_format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
			Utils::TransitionImageLayout(mVulkanContext, command_pool, mDepth.image, depth_format, Utils::kImageUndefinedToDepthStencilOptimal);
		}


		std::vector<VkFramebuffer> Swapchain::CreateFramebuffers(std::vector<VkImageView> img_views, VkRenderPass render_pass)
		{
			std::vector<VkFramebuffer> temp_framebuffers(img_views.size());

			//create framebuffer for swap chain image views
			for (size_t i = 0; i < img_views.size(); i++)
			{

				std::array<VkImageView, 2> attachments = {
					img_views[i],
					mDepth.imageView
				};

				//std::cout << "Size with Division and first element access: " << tes
				VkFramebufferCreateInfo framebuffer_create_info{};
				framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
				framebuffer_create_info.renderPass = render_pass;
				framebuffer_create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
				framebuffer_create_info.pAttachments = attachments.data();
				framebuffer_create_info.width = mExtent.width;
				framebuffer_create_info.height = mExtent.height;
				framebuffer_create_info.layers = 1;

				if (vkCreateFramebuffer(mVulkanContext->device, &framebuffer_create_info, nullptr, &temp_framebuffers[i]) != VK_SUCCESS)
					throw std::runtime_error("Failed to create framebuffer!!!!!!");
			}

			return temp_framebuffers;
		}

		void Swapchain::UnifySwapchainImageResources(std::vector<VkImage> images, std::vector<VkImageView> image_views, std::vector<VkFramebuffer> framebuffers)
		{
			if (images.size() != image_views.size() || images.size() != framebuffers.size())
				throw std::runtime_error("[Swapchain images Resources] --> {Images}{image views}{framebuffer} size not match!!!!");

			mImages.resize(images.size());
			//tracking images in flight
			mImagesInFlight.resize(images.size());

			VkSemaphoreCreateInfo semaphore_create_info{};
			semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			for (size_t i = 0; i < images.size(); i++)
			{
				SwapchainImage swapchain_image{};
				swapchain_image.image = images[i];
				swapchain_image.imageView = image_views[i];
				swapchain_image.framebuffer = framebuffers[i];


				if (vkCreateSemaphore(mVulkanContext->device, &semaphore_create_info, nullptr, &swapchain_image.renderComplete) != VK_SUCCESS)
					throw std::runtime_error("Failed to create semaphore!!!!!!");

				mImages[i] = swapchain_image;
			}
		}
	} //#vulkan
}//#Fluxon