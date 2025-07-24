
#include "UISystem.h"

#include <iostream>


void CheckVulkanResult(VkResult err)
{
	if (err == 0)return;
	//std::cerr << "\033[1;33m [ImGui][Vulkan]\033[0m Error: " << err << std::endl;
	fprintf(stderr, "[Vulkan] Error: VkResult = %d\n", err);
	if (err < 0)
		abort();
}




///////////////////////////////////////
// IMPLEMENTATION
///////////////////////////////////////
//TODO (Conqster): Need to clean this up, just a quick hack 
bool UI_System::UserIsHoveringWindow() {
	ImGuiIO& io = ImGui::GetIO();

	return io.WantTextInput || io.WantCaptureMouse;
}


void UI_System::Init(GLFWwindow* glfwWindowHandle, Vulkan_Engine_Prop vk_backend_setup)
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfw_InitForVulkan(glfwWindowHandle, true);

	ImGui_ImplVulkan_InitInfo imgui_init_info{};
	imgui_init_info.Instance = vk_backend_setup.vkInstance;
	imgui_init_info.PhysicalDevice = vk_backend_setup.physicalDevice;
	imgui_init_info.Device = vk_backend_setup.device;
	imgui_init_info.QueueFamily = vk_backend_setup.graphicsQueueFamily;
	imgui_init_info.Queue = vk_backend_setup.graphicsQueue;
	imgui_init_info.DescriptorPool = vk_backend_setup.descriptorPool;
	imgui_init_info.MinImageCount = vk_backend_setup.minImageCount;
	imgui_init_info.ImageCount = vk_backend_setup.imageCount;
	imgui_init_info.RenderPass = vk_backend_setup.renderPass;

	imgui_init_info.Subpass = vk_backend_setup.subpass;
	imgui_init_info.CheckVkResultFn = CheckVulkanResult;


	bool successful = ImGui_ImplVulkan_Init(&imgui_init_info);

	mVk_props = vk_backend_setup;

}

void UI_System::StartFrame()
{
	// Start the Dear ImGui frame
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void UI_System::RenderFrame(VkCommandBuffer command_buffer)
{
	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command_buffer);
}

void UI_System::Shutdown()
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}


