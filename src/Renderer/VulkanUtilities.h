#pragma once

#include <vector>
#include <iostream>

#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include "VulkanContext.h"
#include "Swapchain.h"

#include <set>
#include <unordered_map>

namespace Fluxon {
	namespace Vulkan {
		namespace Utils {

			inline constexpr bool HasStencilComponent(VkFormat format) {
				return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
			}

			inline constexpr bool IsDepthFormat(VkFormat format) {
				return format == VK_FORMAT_D32_SFLOAT || HasStencilComponent(format);
			}

			inline constexpr VkImageAspectFlags GetAspectMask(VkFormat format)
			{
				if (HasStencilComponent(format))
					return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
				else if (IsDepthFormat(format))
					return VK_IMAGE_ASPECT_DEPTH_BIT;

				return VK_IMAGE_ASPECT_COLOR_BIT;
			}

			static VkDescriptorSetLayout CreateDummyDescriptorSetLayout(const VulkanContext* vk_ctx)
			{
				VkDescriptorSetLayoutCreateInfo layout_create_info{};
				layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				layout_create_info.bindingCount = 0;
				layout_create_info.pBindings = VK_NULL_HANDLE;

				VkDescriptorSetLayout dummy;
				if (vkCreateDescriptorSetLayout(vk_ctx->device, &layout_create_info, nullptr, &dummy) != VK_SUCCESS)
					throw std::runtime_error("Failed to create descriptor set layout!!!!!!!!");

				return dummy;
			}

			static std::vector<const char*> GetRequiredExtensions(bool enable_validation_layers) {
				uint32_t glfw_extension_count = 0;
				const char** pp_glfw_extensions;
				pp_glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

				std::vector<const char*> extensions(pp_glfw_extensions, pp_glfw_extensions + glfw_extension_count);

				if (enable_validation_layers)
					extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

				std::cout << "\n\nrequired extension count: " << extensions.size() << std::endl;
				for (const auto& extension : extensions)
					std::cout << '\t' << extension << '\n';

				return extensions;
			}

			static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& create_info, PFN_vkDebugUtilsMessengerCallbackEXT debug_callback) {
				create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
				create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
					VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
					VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
				create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
					VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
					VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
				//function call back 
				create_info.pfnUserCallback = debug_callback;
				create_info.pUserData = nullptr;
			}

			static bool CheckDeviceExtensionSupport(VkPhysicalDevice device, std::vector<const char*> device_extension) {
				uint32_t extension_count;
				vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);
				std::vector<VkExtensionProperties> available_extensions(extension_count);
				vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

		/*		available_extensions.clear();
				available_extensions.push_back({ VK_KHR_SWAPCHAIN_EXTENSION_NAME , 1 });
				available_extensions.push_back({ "VK_KHR_seperate_depth_stencil_layouts" , 1 });*/

				std::cout << "\n\n\033[1;32m device available extension count: \033[0m" << extension_count << std::endl;
				for (const auto& available_extension : available_extensions)
					std::cout << '\t' << available_extension.extensionName << '\n';

				std::set<std::string> required_extensions(device_extension.begin(), device_extension.end());
				for (const auto& extension : available_extensions)
					required_extensions.erase(extension.extensionName);

				return required_extensions.empty();
			}

			static bool CheckValidationLayerSupport(std::vector<const char*> validation_layers) {
				uint32_t layer_count;
				vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

				std::vector<VkLayerProperties> avaliable_layers(layer_count);
				vkEnumerateInstanceLayerProperties(&layer_count, &avaliable_layers[0]);

				std::cout << "\n\navaliable layer count: " << layer_count << std::endl;
				std::cout << "available layers: \n";
				for (const auto& layer : avaliable_layers)
					std::cout << '\t' << layer.layerName << '\n';


				//check our required layer from available layer
				for (const auto& layer_name : validation_layers)
				{
					bool layer_found = false;
					for (const auto& avaliable_layer_prop : avaliable_layers)
					{
						if (strcmp(layer_name, avaliable_layer_prop.layerName) == 0)
						{
							layer_found = true;
							break;
						}
					}

					if (!layer_found)
						return false;
				}

				return true;
			}


			static SwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
			{
				SwapchainSupportDetails details;

				vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

				uint32_t format_count;
				vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);
				if (format_count != 0)
				{
					details.formats.resize(format_count);
					vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, details.formats.data());
				}

				uint32_t present_mode_count;
				vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, nullptr);
				if (present_mode_count != 0)
				{
					details.presentModes.resize(present_mode_count);
					vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, details.presentModes.data());
				}

				return details;
			}


			static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
				QueueFamilyIndices indices{};

				uint32_t queue_family_count = 0;
				vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

				std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
				vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

				int i = 0;
				for (const auto& queue_family : queue_families)
				{
					if ((queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
						(queue_family.queueFlags & VK_QUEUE_COMPUTE_BIT))
						indices.graphicsAndComputeFamily = i;

					//look for a queue family which has the capablity of presenting 
					//to our window surface
					VkBool32 present_support = false;
					vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_support);
					if (present_support)
						indices.presentFamily = i;

					if (indices.IsRequiredQueuesComplete())
						break;

					i++;
				}

				return indices;
			}
			//TODO (Conqster): Proper Utilty function !!!.
			static QueueFamilyIndices FindDeviceQueueFamilies(const VulkanContext& vk_ctx) {
				return FindQueueFamilies(vk_ctx.physicalDevice, vk_ctx.surface);
			}


			static VkFormat FindSupportedFormat(const VkPhysicalDevice& physical_device, const std::vector<VkFormat>& candidates, VkImageTiling req_img_tiling, VkFormatFeatureFlags required_features) {

				for (auto format : candidates)
				{
					VkFormatProperties props;
					vkGetPhysicalDeviceFormatProperties(physical_device, format, &props);

					if (req_img_tiling == VK_IMAGE_TILING_LINEAR &&
						(props.linearTilingFeatures & required_features) == required_features)
						return format;
					else if (req_img_tiling == VK_IMAGE_TILING_OPTIMAL &&
						(props.optimalTilingFeatures & required_features) == required_features)
						return format;
				}

				throw std::runtime_error("Failed to find supported format!!!!!!");
			}

			//utility
			static VkFormat  FindDepthFormat(const VkPhysicalDevice& physical_device) {
				return FindSupportedFormat(
					physical_device,
					{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, //<<-- CANDIDATES FORMAT FOR DEPTH TESTING
					VK_IMAGE_TILING_OPTIMAL,
					VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
				);
			}


			static uint32_t FindMemoryType(VkPhysicalDevice physical_device, uint32_t type_filter, VkMemoryPropertyFlags properties) {

				VkPhysicalDeviceMemoryProperties mem_properties;
				vkGetPhysicalDeviceMemoryProperties(physical_device, &mem_properties);

				for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
				{
					if ((type_filter & (1 << i)) &&
						(mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
						return i;
				}


				throw std::runtime_error("Failed to find suitable memeory type!!!");
			}




			static VkCommandBuffer BeginSingleTimeCommand(VkDevice device, VkCommandPool command_pool) {
				VkCommandBufferAllocateInfo alloc_info{};
				alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				alloc_info.commandPool = command_pool;
				alloc_info.commandBufferCount = 1;

				VkCommandBuffer command_buffer;
				vkAllocateCommandBuffers(device, &alloc_info, &command_buffer);

				VkCommandBufferBeginInfo command_begin_info{};
				command_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				command_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

				vkBeginCommandBuffer(command_buffer, &command_begin_info);
				return command_buffer;
			}

			static void EndSingleTimeCommand(const VulkanContext* vk_ctx, VkCommandPool command_pool, VkCommandBuffer command_buffer) {
				vkEndCommandBuffer(command_buffer);

				VkSubmitInfo submit_info{};
				submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
				submit_info.commandBufferCount = 1;
				submit_info.pCommandBuffers = &command_buffer;

				vkQueueSubmit(vk_ctx->graphicsQueue, 1, &submit_info, VK_NULL_HANDLE);
				vkQueueWaitIdle(vk_ctx->graphicsQueue);
				vkFreeCommandBuffers(vk_ctx->device, command_pool, 1, &command_buffer);
			}




			//Later support multiple /batching multiple transitions 
			struct LayoutTransitionInfo
			{
				VkImageLayout oldLayout					= VK_IMAGE_LAYOUT_UNDEFINED;
				VkImageLayout newLayout					= VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;


				//vulkan pipeline, stage for layout transition
				//'scrStageMask' -> stage where the image was previous used
				VkPipelineStageFlags srcStageMask		= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				//'dstStageMask' -> stage where the layout will be used next 
				VkPipelineStageFlags dstStageMask		= VK_PIPELINE_STAGE_TRANSFER_BIT;

				//memory access mask for the barrier
				//'srcAccess' -> previous access type (e.g read, write, shader read, attachment write etc)
				VkAccessFlags srcAccess					= 0;
				//'dstAccess' -> future access type after transition (e.g read, write, shader read, attachment write etc)
				VkAccessFlags dstAccess					= VK_ACCESS_TRANSFER_WRITE_BIT;

				//Subrource info 
				//no of array layers to transition (e.g for image2DArray)
				uint32_t subRangeLayerCount				= 1;
			};


			constexpr LayoutTransitionInfo kImageUndefinedToDstOptimal = {
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT, 

				0,
				VK_ACCESS_TRANSFER_WRITE_BIT, 
				
				1
			};
			constexpr LayoutTransitionInfo kImageDstOptimalToShaderOptimalRead = {
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,

				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,

				VK_ACCESS_TRANSFER_WRITE_BIT,
				VK_ACCESS_SHADER_READ_BIT,
				
				1
			};
			constexpr LayoutTransitionInfo kImageUndefinedToDepthStencilOptimal = {
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,

				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,

				0,
				VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
				
				1
			};
			constexpr LayoutTransitionInfo kImageUndefinedToColourAttachmentOptimal = {
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,

				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,

				0,
				VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				
				1
			};


	
			/// <summary>
			/// for now, the pipeline command barrier is called per function call
			/// later in the future, a batch system would be implemented based 
			/// on the growth in transition call and if multiple transition can be pair as 
			/// a group. 
			/// then the function only batch and grp similar transtions togerther to a deferred 
			/// cmd call.
			/// </summary>
			/// <param name="vk_ctx"></param>
			/// <param name="command_pool"></param>
			/// <param name="image"></param>
			/// <param name="format"></param>
			/// <param name="transition_info"></param>
			static void TransitionImageLayout(const VulkanContext* vk_ctx, VkCommandPool command_pool,
				VkImage image, VkFormat format, LayoutTransitionInfo transition_info) {

				VkCommandBuffer command_buffer = BeginSingleTimeCommand(vk_ctx->device, command_pool);

				VkImageMemoryBarrier memory_barrier{};
				memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				memory_barrier.oldLayout = transition_info.oldLayout;
				memory_barrier.newLayout = transition_info.newLayout;
				memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				memory_barrier.image = image;

				memory_barrier.srcAccessMask = transition_info.srcAccess;
				memory_barrier.dstAccessMask = transition_info.dstAccess;

				//sub resources
				memory_barrier.subresourceRange.aspectMask =  GetAspectMask(format);
				memory_barrier.subresourceRange.baseMipLevel = 0;
				memory_barrier.subresourceRange.levelCount = 1;
				memory_barrier.subresourceRange.baseArrayLayer = 0;
				memory_barrier.subresourceRange.layerCount = transition_info.subRangeLayerCount; // 1;

				vkCmdPipelineBarrier(
					command_buffer,
					transition_info.srcStageMask, transition_info.dstStageMask,
					0,
					0, nullptr,
					0, nullptr,
					1, &memory_barrier
				);

				EndSingleTimeCommand(vk_ctx, command_pool, command_buffer);
			}



			static std::unordered_map<VkBufferUsageFlagBits, std::string> VK_BufferUsageName =
			{
					{VK_BUFFER_USAGE_TRANSFER_SRC_BIT, "VK_BUFFER_USAGE_TRANSFER_SRC_BIT"},
					{VK_BUFFER_USAGE_TRANSFER_DST_BIT, "VK_BUFFER_USAGE_TRANSFER_DST_BIT"},
					{VK_BUFFER_USAGE_INDEX_BUFFER_BIT, "VK_BUFFER_USAGE_INDEX_BUFFER_BIT"},
					{VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, "VK_BUFFER_USAGE_VERTEX_BUFFER_BIT"},
					{VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT,"VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT"},
					//{VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT = 0x00000008,},
					//{VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT = 0x00000010,},
					//{VK_BUFFER_USAGE_STORAGE_BUFFER_BIT = 0x00000020,},
					//{VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT = 0x00000100,},
					//{VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT = 0x00020000,},
					//{VK_BUFFER_USAGE_VIDEO_DECODE_SRC_BIT_KHR = 0x00002000,},
					//{VK_BUFFER_USAGE_VIDEO_DECODE_DST_BIT_KHR = 0x00004000,},
					//{VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_BUFFER_BIT_EXT = 0x00000800,},
					//{VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_COUNTER_BUFFER_BIT_EXT = 0x00001000,},
					//{VK_BUFFER_USAGE_CONDITIONAL_RENDERING_BIT_EXT = 0x00000200,},
					//{VK_BUFFER_USAGE_EXECUTION_GRAPH_SCRATCH_BIT_AMDX = 0x02000000,},
					//{VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR = 0x00080000,},
					//{VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR = 0x00100000,},
					//{VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR = 0x00000400,},
					//{VK_BUFFER_USAGE_VIDEO_ENCODE_DST_BIT_KHR = 0x00008000,},
					//{VK_BUFFER_USAGE_VIDEO_ENCODE_SRC_BIT_KHR = 0x00010000,},
					//{VK_BUFFER_USAGE_SAMPLER_DESCRIPTOR_BUFFER_BIT_EXT = 0x00200000,},
					//{VK_BUFFER_USAGE_RESOURCE_DESCRIPTOR_BUFFER_BIT_EXT = 0x00400000,},
					//{VK_BUFFER_USAGE_PUSH_DESCRIPTORS_DESCRIPTOR_BUFFER_BIT_EXT = 0x04000000,},
					//{VK_BUFFER_USAGE_MICROMAP_BUILD_INPUT_READ_ONLY_BIT_EXT = 0x00800000,},
					//{VK_BUFFER_USAGE_MICROMAP_STORAGE_BIT_EXT = 0x01000000,},
					//{VK_BUFFER_USAGE_TILE_MEMORY_QCOM = 0x08000000,},
					//{VK_BUFFER_USAGE_RAY_TRACING_BIT_NV = VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR,},
					//{VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_EXT = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,},
					//{VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_KHR = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,},
					//{VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF},
			};

			static std::string VK_BufferUsageFlagString(VkBufferUsageFlagBits usage) {

				std::string temp = "|";
				bool found = false;
				for (const auto& [flag, name] : VK_BufferUsageName)
				{
					if (usage & flag)
					{
						found = true;
						temp += name + "|";
					}
				}

				if (!found)
					return "\033[1;31mUnknown property!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\033[0m";

				return temp;
			}



			static std::unordered_map<VkMemoryPropertyFlagBits, std::string> VK_MemoryPropertyFlagNames = 
			{
					{VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, "VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT"},
					{VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, "VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT"},
					{VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, "VK_MEMORY_PROPERTY_HOST_COHERENT_BIT"},
					{VK_MEMORY_PROPERTY_HOST_CACHED_BIT, "VK_MEMORY_PROPERTY_HOST_CACHED_BIT"},
					{VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT, "VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT"},
					{VK_MEMORY_PROPERTY_PROTECTED_BIT , "VK_MEMORY_PROPERTY_PROTECTED_BIT"},
					{VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD , "VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD"},
					{VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD , "VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD"},
					{VK_MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV, "VK_MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV"},
					{VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM, "VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM"},
			};
			static std::string VK_MemoryPropertyFlagString(VkMemoryPropertyFlagBits property) {


				std::string temp = "|";
				bool found = false;
				for (const auto& [flag, name] : VK_MemoryPropertyFlagNames)
				{
					if (property & flag)
					{
						found = true;
						temp += name + "|";
					}
				}

				if(!found)
					return "\033[1;31mUnknown property!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\033[0m";

				return temp;

				//switch (property)
				//{
				//case VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT:
				//	return "VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT ";
				//	break;
				//case VK_MEMORY_PROPERTY_HOST_COHERENT_BIT:
				//	return "VK_MEMORY_PROPERTY_HOST_COHERENT_BIT";
				//case VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT:
				//	return "VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT";
				//}

				//return "\033[1;31mUnknown property!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\033[0m";
			}

		}// # Utils
	} //#vulkan
}//#Fluxon