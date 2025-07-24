#pragma once

#include <vulkan/vulkan.h>
#include "Swapchain.h"

#include <memory>
#include <optional>


namespace Fluxon {
	namespace Vulkan {

		struct QueueFamilyIndices
		{
			std::optional<uint32_t> graphicsFamily;
			std::optional<uint32_t> presentFamily;

			bool IsRequiredQueuesComplete() {
				return graphicsFamily.has_value() &&
					presentFamily.has_value();
			}
		};

		//TODO (Conqster): Hack Hack for now 
		const std::vector<const char*> mValidationLayers = {
		"VK_LAYER_KHRONOS_validation"
		};
		const std::vector<const char*> mDeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};



		struct VulkanContext
		{
			VkInstance instance = VK_NULL_HANDLE;
			VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
			VkDevice device = VK_NULL_HANDLE;
			VkSurfaceKHR surface = VK_NULL_HANDLE;

			VkQueue graphicsQueue = VK_NULL_HANDLE;
			VkQueue presentQueue = VK_NULL_HANDLE;

			//TODO (Conqster): This "QueueFamilyIndices" is not set yet
			QueueFamilyIndices queueFamilyIndices{};

			VkDebugUtilsMessengerEXT debugMessager = VK_NULL_HANDLE;

			std::unique_ptr<Swapchain> swapchain;

			uint32_t frameIndex = 0;
		};

		VulkanContext CreateVulkanContext(const Platform::Window* window, bool enable_validation_layers, PFN_vkDebugUtilsMessengerCallbackEXT debug_callback);
		void CreateVkInstance(VulkanContext* vk_ctx, const Platform::Window* window, bool enable_validation_layers, PFN_vkDebugUtilsMessengerCallbackEXT debug_callback);
		void CreateSurface(VulkanContext* vk_ctx, const Platform::Window* window);
		void CreateLogicalDevice(VulkanContext* vk_ctx);

		void DestroyVulkanContext(VulkanContext* vk_ctx, bool has_debugger);

		void InitDebugMessenger(VulkanContext* vk_ctx, PFN_vkDebugUtilsMessengerCallbackEXT debug_callback);


	}//#Vulkan
}//#Fluxon