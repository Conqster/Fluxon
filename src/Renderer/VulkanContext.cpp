#include "VulkanContext.h"
#include "VulkanUtilities.h"

#include "Core/Window.h"

namespace Fluxon {
	namespace Vulkan {

		static uint32_t RatePhysicalDeviceSuitablity(VkPhysicalDevice physical_device, VkSurfaceKHR surface)
		{
			VkPhysicalDeviceProperties device_properties;
			VkPhysicalDeviceFeatures device_features;
			vkGetPhysicalDeviceProperties(physical_device, &device_properties);
			vkGetPhysicalDeviceFeatures(physical_device, &device_features);

			int score = 0;

			auto queue_indices = Utils::FindQueueFamilies(physical_device, surface);
			if (!queue_indices.IsRequiredQueuesComplete())
				return 0;

			if (!Utils::CheckDeviceExtensionSupport(physical_device, mDeviceExtensions))
				return 0;

			auto swapchain_support = Utils::QuerySwapchainSupport(physical_device, surface);
			if (!swapchain_support.IsSupportSufficent())
				return 0;

			//TODO (Conqster): extra support, later move unless it is high priority requirment 
			VkPhysicalDeviceFeatures supported_features;
			vkGetPhysicalDeviceFeatures(physical_device, &supported_features);
			if (!supported_features.samplerAnisotropy)
				return 0;


			//Discrete GPUs have a signigicant performance advantage
			if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				score += 1000;

			//Maximum possible size texture affects graphics quality
			score += device_properties.limits.maxImageDimension2D;




			//Applicatin can't function without geometry shaders
			//if (!device_features.geometryShader)
				//return 0;

			return score;
		}

		static void EnumerateOptimalPhysicalDeviceToUse(VulkanContext* vk_ctx)
		{
			uint32_t device_count = 0;
			vkEnumeratePhysicalDevices(vk_ctx->instance, &device_count, nullptr);

			if (device_count == 0)
				throw std::runtime_error("Failed to find GPUs with Vulkan supports!!!!!!");

			std::vector<VkPhysicalDevice> physical_devices(device_count);
			vkEnumeratePhysicalDevices(vk_ctx->instance, &device_count, physical_devices.data());

			std::cout << "\n\navailable physical vk_ctx count: " << device_count << std::endl;
			//for (const auto& device : physical_devices)
			//{
			//	VkPhysicalDeviceProperties device_properties;
			//	vkGetPhysicalDeviceProperties(device, &device_properties);
			//	std::cout << '\t' << device_properties.deviceName << '\n';
			//}


			uint32_t optimal_score = 0;
			VkPhysicalDevice optimal_device = VK_NULL_HANDLE;
			for (const auto& physical_device : physical_devices)
			{
				uint32_t physical_device_score = RatePhysicalDeviceSuitablity(physical_device, vk_ctx->surface);
				if (physical_device_score > optimal_score)
				{
					optimal_score = physical_device_score;
					optimal_device = physical_device;
				}
			}

			if (optimal_score == 0 && optimal_device == VK_NULL_HANDLE)
				throw std::runtime_error("No suitable GPU found!!!!!!");

			vk_ctx->physicalDevice = optimal_device;
			//TODO (Conqster): maybe later cache optimal device properties
		}

		VulkanContext CreateVulkanContext(const Platform::Window* window, bool enable_validation_layers, PFN_vkDebugUtilsMessengerCallbackEXT debug_callback)
		{
			//check validation layer is required and supported 
			if (enable_validation_layers && !Utils::CheckValidationLayerSupport(mValidationLayers))
				throw std::runtime_error("\n\nVaildation layers requested, but not available!!!");

			VulkanContext vk_ctx;
			CreateVkInstance(&vk_ctx, window, enable_validation_layers, debug_callback);

			if (enable_validation_layers)
				InitDebugMessenger(&vk_ctx, debug_callback);

			CreateSurface(&vk_ctx, window);

			EnumerateOptimalPhysicalDeviceToUse(&vk_ctx);

			CreateLogicalDevice(&vk_ctx);

			return vk_ctx;
		}

		void CreateVkInstance(VulkanContext* vk_ctx, const Platform::Window* window, bool enable_validation_layers, PFN_vkDebugUtilsMessengerCallbackEXT debug_callback)
		{
			VkApplicationInfo app_info{};
			app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			app_info.pApplicationName = window->title.c_str();
			app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			app_info.pEngineName = "No Engine";
			app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			app_info.apiVersion = VK_API_VERSION_1_0;

			VkInstanceCreateInfo create_info{};
			create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			create_info.pApplicationInfo = &app_info;

			auto req_extensions = Utils::GetRequiredExtensions(enable_validation_layers);
			create_info.enabledExtensionCount = static_cast<uint32_t>(req_extensions.size());
			create_info.ppEnabledExtensionNames = req_extensions.data();

			VkDebugUtilsMessengerCreateInfoEXT debug_utils_create_info{};
			if (enable_validation_layers)
			{
				create_info.enabledLayerCount = static_cast<uint32_t>(mValidationLayers.size());
				create_info.ppEnabledLayerNames = mValidationLayers.data();

				Utils::PopulateDebugMessengerCreateInfo(debug_utils_create_info, debug_callback);
				create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_utils_create_info;
			}
			else
			{
				create_info.enabledLayerCount = 0;
				create_info.pNext = nullptr;
			}

			if (vkCreateInstance(&create_info, nullptr, &vk_ctx->instance) != VK_SUCCESS)
				throw std::runtime_error("Failed to create vulkan instance!!!!!!!");
		}

		void CreateSurface(VulkanContext* vk_ctx, const Platform::Window* window)
		{
			//OR
			if (glfwCreateWindowSurface(vk_ctx->instance, window->glfwWindowHandle, nullptr, &vk_ctx->surface) != VK_SUCCESS)
				throw std::runtime_error("Failed to create window surface!!!!!");
		}

		void CreateLogicalDevice(VulkanContext* vk_ctx)
		{
			QueueFamilyIndices queue_indices = Utils::FindDeviceQueueFamilies(*vk_ctx);

			std::vector<VkDeviceQueueCreateInfo> queue_create_infos;

			//TODO (Conqser): std::vector<uint32_t> unique_queue_families = {
			// TODO (Conqster): This "std::set" might hvae worked because graphic & present family has the same value
			std::set<uint32_t> unique_queue_families = {
				//std::vector<uint32_t> unique_queue_families = {
					queue_indices.graphicsFamily.value(),
					queue_indices.presentFamily.value()
			};

			float queue_priority = 1.0f;
			for (size_t i = 0; i < unique_queue_families.size(); i++)
			{
				VkDeviceQueueCreateInfo queue_create_info{};
				queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				//queue_create_info.queueFamilyIndex = unique_queue_families[i];
				// TODO (Conqster): This "queue_indices.graphicsFamily.value()" might hvae worked because 
				//graphics & present has the same index;
				queue_create_info.queueFamilyIndex = queue_indices.graphicsFamily.value();
				queue_create_info.queueCount = 1;
				queue_create_info.pQueuePriorities = &queue_priority;
				queue_create_infos.push_back(queue_create_info);
			}



			//FEATURES for devices like geomtry/tessellation shader support etc
			VkPhysicalDeviceFeatures device_features{};
			device_features.samplerAnisotropy = VK_TRUE;

			VkDeviceCreateInfo device_create_info{};
			device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			device_create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
			device_create_info.pQueueCreateInfos = queue_create_infos.data();
			device_create_info.pEnabledFeatures = &device_features;

			device_create_info.enabledExtensionCount = static_cast<uint32_t>(mDeviceExtensions.size());
			device_create_info.ppEnabledExtensionNames = mDeviceExtensions.data();

			////Below is not ncessary as previous Vulkan implementation made distinction btw 
			////vulkan instance and device specific valiadation layers but this is no longer the case
			//if (true)
			//{
			//	device_create_info.enabledLayerCount = static_cast<uint32_t>(mValidationLayers.size());
			//	device_create_info.ppEnabledLayerNames = mValidationLayers.data();
			//}

			if (vkCreateDevice(vk_ctx->physicalDevice, &device_create_info, nullptr, &vk_ctx->device) != VK_SUCCESS)
				throw std::runtime_error("Failed to create logical vk_ctx!!!!!!!");

			vkGetDeviceQueue(vk_ctx->device, queue_indices.graphicsFamily.value(), 0, &vk_ctx->graphicsQueue);
			vkGetDeviceQueue(vk_ctx->device, queue_indices.presentFamily.value(), 0, &vk_ctx->presentQueue);
		}

		void DestroyVulkanContext(VulkanContext* vk_ctx, bool has_debugger)
		{
			if (has_debugger)
			{
				auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vk_ctx->instance, "vkDestroyDebugUtilsMessengerEXT");
				if (func != nullptr)
					func(vk_ctx->instance, vk_ctx->debugMessager, nullptr);
			}
			vkDestroyDevice(vk_ctx->device, nullptr);
			vkDestroySurfaceKHR(vk_ctx->instance, vk_ctx->surface, nullptr);
			vkDestroyInstance(vk_ctx->instance, nullptr);
		}


		void InitDebugMessenger(VulkanContext* vk_ctx, PFN_vkDebugUtilsMessengerCallbackEXT debug_callback)
		{
			VkDebugUtilsMessengerCreateInfoEXT debug_utils_create_info{};
			Utils::PopulateDebugMessengerCreateInfo(debug_utils_create_info, debug_callback);


			auto vk_debug_func = ((PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vk_ctx->instance, "vkCreateDebugUtilsMessengerEXT"));
			//TODO (Conqseter): Later have a better asserting & logging system
			if (vk_debug_func == nullptr)
				throw std::runtime_error("Failed to load vkCreateDebugUtilsMessengerEXT!!!");

			if (vk_debug_func(vk_ctx->instance, &debug_utils_create_info, nullptr, &vk_ctx->debugMessager) != VK_SUCCESS)
				throw std::runtime_error("failed to set up debug messenger!!!!");
		}

	} //#Vulkan
}//#Fluxon
