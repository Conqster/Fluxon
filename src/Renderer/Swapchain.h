#pragma once
#include <vulkan/vulkan.h>
#include <array>

#include <vector>
#include "Core/Window.h"

namespace Fluxon {
	namespace Vulkan {

		struct SwapchainImage
		{
			VkImage image = VK_NULL_HANDLE;
			VkImageView imageView = VK_NULL_HANDLE;
			VkFramebuffer framebuffer = VK_NULL_HANDLE;

			//gpu semaphore to signal when its okay to present image
			VkSemaphore renderComplete = VK_NULL_HANDLE;
		};

		struct DepthResource
		{
			VkImage image = VK_NULL_HANDLE;
			VkDeviceMemory memory = VK_NULL_HANDLE;
			VkImageView imageView = VK_NULL_HANDLE;
		};


		struct SwapchainSupportDetails
		{
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats = {};
			std::vector<VkPresentModeKHR> presentModes = {};

			bool IsSupportSufficent() {
				return !formats.empty() &&
					!presentModes.empty();
			}
		};


		struct VulkanContext;
		class Swapchain
		{
		public:
			Swapchain() {};
			Swapchain(VulkanContext* vk_ctx, const Platform::Window& window, VkCommandPool command_pool, VkRenderPass render_pass);
			//Swapchain(const Vulkan2::VulkanContext* vk_render_ctx, const Platform::Window& window, VkCommandPool command_pool, VkRenderPass render_pass);
			~Swapchain();

			void Cleanup();

			VkSwapchainKHR GetHandle() const { return mSwapchain; }
			VkFormat GetFormat() const { return mImageFormat; }
			VkExtent2D GetExtent() const { return mExtent; }
			const std::vector<SwapchainImage>& GetImages() const { return mImages; }
			const DepthResource& GetDepthResource() const { return mDepth; }

			std::vector<VkFence> mImagesInFlight;

			static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats);
		private:
			void CreateSwapchain(const VulkanContext* vk_ctx, const Platform::Window& window);
			std::vector<VkImage> CreateImages();
			std::vector<VkImageView> CreateImageViews(uint32_t counts, std::vector<VkImage> images);

			void CreateDepthResources(VkCommandPool command_pool);

			std::vector<VkFramebuffer> CreateFramebuffers(std::vector<VkImageView> img_views, VkRenderPass render_pass);

			void UnifySwapchainImageResources(std::vector<VkImage> images, std::vector<VkImageView> image_views, std::vector<VkFramebuffer> framebuffers);


			VulkanContext* mVulkanContext = nullptr;
			VkSwapchainKHR mSwapchain = VK_NULL_HANDLE;

			VkFormat mImageFormat{};
			VkExtent2D mExtent{};

			std::vector<SwapchainImage> mImages;

			DepthResource mDepth{};
		};
	} //#Vulkan
}//#Fluxon