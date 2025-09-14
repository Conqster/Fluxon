#pragma once

#include <vulkan/vulkan.h>
#include "Enums.h"



#include "Renderer/VkImGuiRenderer.h"



struct GLFWwindow;
struct ImGuiStyle;
struct ImGuiIO;
struct ImVec4;
namespace Fluxon {

	//ui vulkan data
	struct UISystemVulkanSpecification
	{
		//current imgui uses main render syste/pipelines 
		VkInstance					vkInstance;
		VkPhysicalDevice			physicalDevice;
		VkDevice					device;
		uint32_t					graphicsQueueFamily;
		VkQueue						graphicsQueue;
		VkDescriptorPool			descriptorPool;
		uint32_t					minImageCount;
		uint32_t					imageCount;
		VkSampleCountFlagBits		MSAASamples;
	
		// (Optional)
		//VkPipelineCache             pipelineCache;
		uint32_t                    subpass;
	
		VkRenderPass				renderPass;

		ESandboxTheme				sandTheme = ESandboxTheme::BlueAccent;
	};

	
	
	//blueprint/declartion
	class UI_System
	{
	private:
		UISystemVulkanSpecification mVk_props;
	
		const std::string mFontDirectory = "assets/Fonts";
		const std::string mDefaultFont = "Inter_18pt-Regular.ttf";

	public:

		VkImGuiRenderer mRenderer = {};
		void Init( GLFWwindow* glfwWindowHandle, UISystemVulkanSpecification ui_vk_spec);
		void Init(const Vulkan::VulkanContext* vk_ctx, GLFWwindow* glfwWindowHandle, UISystemVulkanSpecification ui_vk_spec);
		bool UserIsHoveringWindow();

		void StartFrame();

		void RenderFrame(VkCommandBuffer command_buffer);
	
		void Shutdown();

		VkExtent2D GetViewport() {

		}


	private:
		void SetDefaultSandboxTheme(ImGuiStyle& style, const ESandboxTheme& sandbox_theme_accent);
		void BlueAccentTheme(ImGuiStyle& style);
		void TealAccentTheme(ImGuiStyle& style);

		void SetupStyle(ImGuiStyle& style, ESandboxTheme theme = ESandboxTheme::BlueAccent);

		void LoadFonts(ImGuiIO& imgui_io);
	};

} //namespace Fluxon









//NEXT  STOP RENDER MAIN SCENE OR FORWARED PASS TO A TEXTURE INSTEAD OF PRESENT 
//THEN SEND THE TEXTURE TO THE EDITOR (IMGUI TO PRESENT IN VIEW PORT) 


//MIGHT NEED TO GET THE VIEWPORT SIZE TO ADJUST THE SIZE OF THE RENDER TARGET 

//WHAT CHANGES TEH RENDERTARGET FRAMEBUFFER SIZE 
// 1 WINDOW ON RESIZE 
// 2 PANEL ADJUSTMENT 
//