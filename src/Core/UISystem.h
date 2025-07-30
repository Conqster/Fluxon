#pragma once

#include <vulkan/vulkan.h>

//ui vulkan data
struct Vulkan_Engine_Prop
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
};





#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_vulkan.h>

//#include <GLFW/glfw3.h>

//#include <iostream>



//blueprint/declartion
class UI_System
{
private:
	Vulkan_Engine_Prop mVk_props;


public:
	void Init(GLFWwindow* glfwWindowHandle, Vulkan_Engine_Prop vk_backend_setup);
	bool UserIsHoveringWindow();
	void StartFrame();
	void RenderFrame(VkCommandBuffer command_buffer);

	void Shutdown();
};








