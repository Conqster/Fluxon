#pragma once

#include <vulkan/vulkan.h>
#include "Swapchain.h"

#include <memory>
#include <optional>

#include "GraphicsAssertLog.h"

#include <optional>
#include "VulkanCommandBuffer.h"


namespace Fluxon {
	namespace Vulkan {

		static constexpr int kMaxFramesInFlight = 2;

		struct QueueFamilyIndices
		{
			std::optional<uint32_t> graphicsAndComputeFamily;
			std::optional<uint32_t> presentFamily;

			bool IsRequiredQueuesComplete() {
				return graphicsAndComputeFamily.has_value() &&
					presentFamily.has_value();
			}
		};

		//TODO (Conqster): Hack Hack for now 
		const std::vector<const char*> mValidationLayers = {
		"VK_LAYER_KHRONOS_validation"
		};
		const std::vector<const char*> mDeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		"VK_KHR_separate_depth_stencil_layouts", //<-- required for VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL because is not part of core for vulkan below Vulkan 1.2
		"VK_KHR_create_renderpass2" //<-- required by device for "VK_KHR_separate_depth_stencil_layouts"
		};




		struct VulkanContextSpecification
		{
			//vulkan app 
			uint32 applicationVersion					= VK_API_VERSION_1_0;
			uint32 apiVersion							= VK_MAKE_VERSION(1, 0, 0);


			bool bEnableSamplerAnisotropy			= true;
			bool bEnableSeparateDepthStencil		= true;
			bool bEnableGeometryShader				= false;
			bool bEnableFragmentStoresAndAtomics	= false;

			bool bEnableValidationLayer				= true;
			PFN_vkDebugUtilsMessengerCallbackEXT debug_callback;

			bool bCreateTransientCommandBuffer		= true;

			//Not implemented yet
			bool bImageCubeArray					= false;
			//Not implemented yet
			bool bEnableMultiViewport				= false;
			//Not implemented yet
			bool bVertexPipelineStoresAndAtomics	= false;
			//Not implemented yet
			bool bEnableDepthClamp					= false;
			//Not implemented yet
			bool bEnableDepthBiasClamp				= false;
			//Not implemented yet
			bool bEnableTessellationShader			= false;
		};



		struct VulkanContext
		{
			VkInstance instance										= VK_NULL_HANDLE;
			VkPhysicalDevice physicalDevice							= VK_NULL_HANDLE;
			VkDevice device											= VK_NULL_HANDLE;
			VkSurfaceKHR surface									= VK_NULL_HANDLE;

			VkQueue graphicsQueue									= VK_NULL_HANDLE;
			VkQueue computeQueue									= VK_NULL_HANDLE;
			VkQueue presentQueue									= VK_NULL_HANDLE;

			//TODO (Conqster): This "QueueFamilyIndices" is not set yet
			QueueFamilyIndices queueFamilyIndices{};

			VkDebugUtilsMessengerEXT debugMessager					= VK_NULL_HANDLE;

			std::unique_ptr<Swapchain> swapchain;
			Platform::Window* pMainPlatformWindow					= nullptr;

			VkCommandPool transientCmdPool			= VK_NULL_HANDLE;
			VulkanCommandBuffer transientCmdBuffer = {};
			VkFence transientCmdFence				= VK_NULL_HANDLE;
			

			uint32_t frameIndex										= 0;
			//quick hack 
			double frameDT											= 0.0;
			VkDescriptorSetLayout dummyDescSetLayout				= VK_NULL_HANDLE;

			//static VulkanContext CreateVulkanContext(Platform::Window* window, bool enable_validation_layers, PFN_vkDebugUtilsMessengerCallbackEXT debug_callback);
			static VulkanContext CreateVulkanContext(Platform::Window* window, const VulkanContextSpecification& ctx_spec);
			void DestroyVulkanContext(bool has_debugger);

			VkPhysicalDeviceLimits GetPhysicalDeviceLimits()
			{
				FLUX_DEBUG_ASSERT_CRITICAL(physicalDevice != VK_NULL_HANDLE, "No Physical available!!!!!!");
				VkPhysicalDeviceProperties device_props;
				vkGetPhysicalDeviceProperties(physicalDevice, &device_props);
				return device_props.limits;
			}


			VkPhysicalDeviceProperties GetPhysicalDevicePropertices()
			{
				FLUX_DEBUG_ASSERT_CRITICAL(physicalDevice != VK_NULL_HANDLE, "No Physical available!!!!!!");
				VkPhysicalDeviceProperties device_props;
				vkGetPhysicalDeviceProperties(physicalDevice, &device_props);
				return device_props;
			}


		private:
			//parameter data extraction 
			void CreateVkInstance(const Platform::Window* window, const VulkanContextSpecification& ctx_spec, PFN_vkDebugUtilsMessengerCallbackEXT debug_callback);
			void CreateSurface(const Platform::Window* window);
			void CreateLogicalDevice(const VulkanContextSpecification& ctx_spec);

			void CreateTrasientCommand();


			void InitDebugMessenger(PFN_vkDebugUtilsMessengerCallbackEXT debug_callback);
		};


	}//#Vulkan
}//#Fluxon