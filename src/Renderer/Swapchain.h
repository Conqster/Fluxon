#pragma once
#include <vulkan/vulkan.h>
#include <array>

#include <vector>
#include "Core/Window.h"

#include "Core/Core.h"

#include <functional>

namespace Fluxon {
	namespace Vulkan {

		struct SwapchainImage
		{
			VkImage image = VK_NULL_HANDLE;
			VkImageView imageView = VK_NULL_HANDLE;

			//gpu semaphore to signal when its okay to present image
			VkSemaphore renderComplete = VK_NULL_HANDLE;
		};



		struct SwapchainSupportDetails
		{
			VkSurfaceCapabilitiesKHR capabilities		= {};
			std::vector<VkSurfaceFormatKHR> formats		= {};
			std::vector<VkPresentModeKHR> presentModes	= {};

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
			/// <summary>
			/// still kept vk_ctx and window seperate 
			/// for swap chain creation, as might waant to teh graphics details 
			/// but different windows 
			/// </summary>
			/// <param name="vk_ctx"></param>
			/// <param name="window"></param>
			Swapchain(VulkanContext* vk_ctx, const Platform::Window& window);
			
			
			bool Init(VulkanContext* vk_ctx, const Platform::Window& window);
			//Swapchain(const Vulkan2::VulkanContext* vk_render_ctx, const Platform::Window& window, VkCommandPool command_pool, VkRenderPass render_pass);
			~Swapchain();

			void Cleanup(const VulkanContext* vk_ctx);

			VkSwapchainKHR GetHandle() const { return mSwapchain; }
			VkFormat GetFormat() const { return mImageFormat; }
			VkExtent2D GetExtent() const { return mExtent; }

			const std::vector<SwapchainImage>& GetImages() const { return mImages; }
			const uint32 GetImageCount() const { return mImages.size(); }

			static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats);


			uint32 AcquireNextImageIdx(VulkanContext* vk_ctx, VkSemaphore frame_img_avail_semaphore);
			SwapchainImage& AcquireNextImage(VulkanContext* vk_ctx, VkSemaphore frame_img_avail_semaphore);
			
			const uint32 GetAcquiredImageIdx() const { return mAcquiredImageIdx; }
			const SwapchainImage& GetAcquiredImage() const { return mImages[mAcquiredImageIdx]; }

			void Present(VulkanContext* vk_ctx);

			void OnResize(VulkanContext* vk_ctx);

			void RegisterOnResizeEvent(const std::function<void(uint32 width, uint32 height)>& on_resize) {
				mRegisteryResize.emplace_back(on_resize);
			}
		private:
			VulkanContext* mVulkanContextRef = nullptr;


			bool CreateSwapchain(const VulkanContext* vk_ctx, const Platform::Window& window);

			bool GetSwapchainVkImagesKHR(VkDevice vk_ctx_device, uint32 counts, std::vector<VkImage>& out_swapchain_vkimages);
			bool CreateSwapchainImageResources(VkDevice vk_ctx_device, std::vector<VkImage>& images, std::vector<SwapchainImage>& out_swapchain_images);
			bool UpdateSwapchainImageResources(VkDevice vk_ctx_device, std::vector<VkImage>& images, std::vector<SwapchainImage>& out_swapchain_images);

			VkSwapchainKHR mSwapchain = VK_NULL_HANDLE;
			VkSwapchainKHR mOldSwapchain = VK_NULL_HANDLE;

			VkFormat mImageFormat{};
			VkExtent2D mExtent{};

			std::vector<SwapchainImage> mImages;

			std::vector<std::function<void(uint32 width, uint32 height)>> mRegisteryResize;

			uint32 mAcquiredImageIdx = 0;


			void CheckVkResultImageOutOfDate(const VkResult& result,VulkanContext* vk_ctx, const char* label);
		};
	} //#Vulkan
}//#Fluxon