
#include "UISystem.h"

#include <iostream>



#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_vulkan.h>


#include "AssertLog.h"

#include <filesystem>



void CheckVulkanResult(VkResult err)
{
	if (err == 0)return;
	//std::cerr << "\033[1;33m [ImGui][Vulkan]\033[0m Error: " << err << std::endl;
	fprintf(stderr, "[Vulkan] Error: VkResult = %d\n", err);
	if (err < 0)
		abort();
}


namespace Fluxon {

	//TODO (Conqster): Need to clean this up, just a quick hack 
	bool UI_System::UserIsHoveringWindow() {
		ImGuiIO& io = ImGui::GetIO();
	
		return io.WantTextInput || io.WantCaptureMouse;
	}
	
	
	void UI_System::Init(const Vulkan::VulkanContext* vk_ctx, GLFWwindow* glfwWindowHandle, UISystemVulkanSpecification ui_vk_spec)
	{
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		mRenderer.Init(vk_ctx, glfwWindowHandle);

		ImGui::StyleColorsDark();

		mVk_props = ui_vk_spec;


		LoadFonts(io);


		ImGuiStyle* style = &ImGui::GetStyle();
		//SetDefaultSandboxTheme(*style, mVk_props.sandTheme);
		SetupStyle(*style, mVk_props.sandTheme);
	}
	void UI_System::Init(GLFWwindow* glfwWindowHandle, UISystemVulkanSpecification ui_vk_spec)
	{
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui_ImplGlfw_InitForVulkan(glfwWindowHandle, true);
	
		ImGui_ImplVulkan_InitInfo imgui_init_info{};
		imgui_init_info.Instance = ui_vk_spec.vkInstance;
		imgui_init_info.PhysicalDevice = ui_vk_spec.physicalDevice;
		imgui_init_info.Device = ui_vk_spec.device;
		imgui_init_info.QueueFamily = ui_vk_spec.graphicsQueueFamily;
		imgui_init_info.Queue = ui_vk_spec.graphicsQueue;
		imgui_init_info.DescriptorPool = ui_vk_spec.descriptorPool;
		imgui_init_info.MinImageCount = ui_vk_spec.minImageCount;
		imgui_init_info.ImageCount = ui_vk_spec.imageCount;
		imgui_init_info.RenderPass = ui_vk_spec.renderPass;
	
		imgui_init_info.Subpass = ui_vk_spec.subpass;
		imgui_init_info.CheckVkResultFn = CheckVulkanResult;
	
	
		bool successful = ImGui_ImplVulkan_Init(&imgui_init_info);
	
		mVk_props = ui_vk_spec;


		LoadFonts(io);


		ImGuiStyle* style = &ImGui::GetStyle();
		//SetDefaultSandboxTheme(*style, mVk_props.sandTheme);
		SetupStyle(*style, mVk_props.sandTheme);
		//ImVec4* colours = style->Colors;


		//FLUX_DEBUG_LOG_INFO("==============ImGui Colours==============");
		//FLUX_DEBUG_LOG_INFO("Text colour: ", colours[ImGuiCol_Text]);
		//FLUX_DEBUG_LOG_INFO("WindowBg colour: ", colours[ImGuiCol_WindowBg]);
		//FLUX_DEBUG_LOG_INFO("ImGuiCol_Border colour: ", colours[ImGuiCol_Border]);
		//FLUX_DEBUG_LOG_INFO("ImGuiCol_FrameBg colour: ", colours[ImGuiCol_FrameBg]);


		//colours[ImGuiCol_Text] = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
		//colours[ImGuiCol_TextDisabled] = ImVec4(0.171, 0.122, .122, 1.0f);//
		//colours[ImGuiCol_WindowBg] = ImVec4(0.064, 0.066, 0.66, 1.0);//
		//colours[ImGuiCol_ChildBg] = ImVec4(0.280f, 0.280f, 0.280f, 0.000f);
		//colours[ImGuiCol_PopupBg] = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
		//colours[ImGuiCol_Border] = ImVec4(0.266f, 0.266f, 0.266f, 1.000f);
		//colours[ImGuiCol_BorderShadow] = ImVec4(0.000f, 0.000f, 0.000f, 0.000f);
		//colours[ImGuiCol_FrameBg] = ImVec4(0.105, 0.104, 0.104, 1.000f);//
		//colours[ImGuiCol_FrameBgHovered] = ImVec4(0.200f, 0.200f, 0.200f, 1.000f);
		//colours[ImGuiCol_FrameBgActive] = ImVec4(0.280f, 0.280f, 0.280f, 1.000f);
		//colours[ImGuiCol_TitleBg] = ImVec4(0.05, 0.055, 0.055, 1.0);//
		//colours[ImGuiCol_TitleBgActive] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
		//colours[ImGuiCol_TitleBgCollapsed] = ImVec4(0.056f, 0.188f, 0.282f, 1.000f);//
		//colours[ImGuiCol_MenuBarBg] = ImVec4(0.055f, 0.188f, 0.282f, 1.000f);//
		//colours[ImGuiCol_ScrollbarBg] = ImVec4(0.055f, 0.188f, 0.282f, 1.000f);//
		//colours[ImGuiCol_ScrollbarGrab] = ImVec4(0.009f, 0.065f, 0.155f, 1.000f);//
		//colours[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.025f, 0.083f, 0.254f, 1.000f);//
		//colours[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.025f, 0.083f, 0.254f, 1.000f);//
		//colours[ImGuiCol_CheckMark] = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
		//colours[ImGuiCol_SliderGrab] = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
		//colours[ImGuiCol_SliderGrabActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		//colours[ImGuiCol_Button] = ImVec4(1.000f, 1.000f, 1.000f, 0.000f);
		//colours[ImGuiCol_ButtonHovered] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
		//colours[ImGuiCol_ButtonActive] = ImVec4(1.000f, 1.000f, 1.000f, 0.391f);
		//colours[ImGuiCol_Header] = ImVec4(0.025f, 0.083f, 0.254f, 1.000f);//
		//colours[ImGuiCol_HeaderHovered] = ImVec4(0.168f, 0.260f, 0.309f, 1.000f);//
		//colours[ImGuiCol_HeaderActive] = ImVec4(0.059f, 0.533f, 0.639f, 1.000f);//
		//colours[ImGuiCol_Separator] = ImVec4(0.059f, 0.533f, 0.639f, 1.000f);//
		//colours[ImGuiCol_SeparatorHovered] = ImVec4(0.063f, 0.193f, 0.575f, 1.000f);//
		//colours[ImGuiCol_SeparatorActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		//colours[ImGuiCol_ResizeGrip] = ImVec4(0.063f, 0.192f, 0.576f, 1.000f);//
		//colours[ImGuiCol_ResizeGripHovered] = ImVec4(0.059f, 0.533f, 0.639f, 1.000f);//
		//colours[ImGuiCol_ResizeGripActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		//colours[ImGuiCol_Tab] = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
		//colours[ImGuiCol_TabHovered] = ImVec4(0.059f, 0.533f, 0.639f, 1.000f);//
		//colours[ImGuiCol_TabActive] = ImVec4(0.063f, 0.192f, 0.576f, 1.000f);//
		//colours[ImGuiCol_TabUnfocused] = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
		//colours[ImGuiCol_TabUnfocusedActive] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
		////colors[ImGuiCol_DockingPreview] = ImVec4(1.000f, 0.391f, 0.000f, 0.781f);
		////colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
		//colours[ImGuiCol_PlotLines] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
		//colours[ImGuiCol_PlotLinesHovered] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		//colours[ImGuiCol_PlotHistogram] = ImVec4(0.586f, 0.586f, 0.586f, 1.000f);
		//colours[ImGuiCol_PlotHistogramHovered] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		//colours[ImGuiCol_TextSelectedBg] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
		//colours[ImGuiCol_DragDropTarget] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		//colours[ImGuiCol_NavHighlight] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		//colours[ImGuiCol_NavWindowingHighlight] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		//colours[ImGuiCol_NavWindowingDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
		//colours[ImGuiCol_ModalWindowDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);


		//style->ChildRounding = 4.0f;
		//style->FrameBorderSize = 1.0f;
		//style->FrameRounding = 4.0f;
		//style->GrabMinSize = 7.0f;
		//style->PopupRounding = 2.0f;
		//style->ScrollbarRounding = 12.0f;
		//style->ScrollbarSize = 13.0f;
		//style->TabBorderSize = 1.0f;
		//style->TabRounding = 4.0f;
		//style->WindowRounding = 4.0f;
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

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			// Update and Render additional Platform Windows
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}
	
	void UI_System::Shutdown()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	inline ImVec4 ToImGui(const glm::vec4& v)
	{
		return ImVec4(v.x, v.y, v.z, v.w);
	}

	void UI_System::SetDefaultSandboxTheme(ImGuiStyle& style, const ESandboxTheme& sandbox_theme_accent)
	{
		ImVec4* colours = style.Colors;

		switch (sandbox_theme_accent)
		{
		case ESandboxTheme::BlueAccent:
			BlueAccentTheme(style);
			break;
		case ESandboxTheme::TealAccent:
			TealAccentTheme(style);
			break;
		default:
			BlueAccentTheme(style);
			break;
		}


		//Text
		colours[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.0f);
		colours[ImGuiCol_TextDisabled] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

		//Window and Panels
		colours[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.105f, 0.11f, 1.0f);
		colours[ImGuiCol_ChildBg] = ImVec4(0.1f, 0.105f, 0.11f, 1.0f);
		colours[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);

		//Headers /Tabs (Blue accent)
		colours[ImGuiCol_Header] = ImVec4(0.020f, 0.025f, 0.30f, 1.000f);
		//colours[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
		//colours[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.0f);

		colours[ImGuiCol_Tab] = ImVec4(0.15f, 0.18f, 0.22f, 1.0f);
		//colours[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
		//colours[ImGuiCol_TabActive] = ImVec4(0.20f, 0.50f, 0.85f, 1.000f);
		colours[ImGuiCol_TabUnfocused] = ImVec4(0.15f, 0.18f, 0.22f, 1.0f);
		//colours[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.50f, 0.85f, 1.000f);

		//frames (inputs, sliders)
		colours[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.21f, 0.22f, 1.00f);//
		//colours[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
		//colours[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);

		//Button
		colours[ImGuiCol_Button] = ImVec4(0.2f, 0.21f, 0.22f, 1.0f);
		//colours[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
		//colours[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.0f);

		//Scrollbar
		colours[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);//
		colours[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.0f);//
		colours[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.000f);//
		colours[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.000f);//

		//Separator
		colours[ImGuiCol_Separator] = ImVec4(0.39f, 0.39f, 0.39f, 0.62f);//
		colours[ImGuiCol_SeparatorHovered] = ImVec4(0.75f, 0.75f, 0.75f, 1.0f);//
		colours[ImGuiCol_SeparatorActive] = ImVec4(1.000f, 0.40f, 0.000f, 1.000f);

		//Resize Grip
		colours[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);//
		//colours[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);//
		//colours[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);



		colours[ImGuiCol_Border] = ImVec4(0.266f, 0.266f, 0.266f, 1.000f);
		colours[ImGuiCol_BorderShadow] = ImVec4(0.000f, 0.000f, 0.000f, 0.000f);
		colours[ImGuiCol_TitleBg] = ImVec4(0.05, 0.055, 0.055, 1.0);//
		colours[ImGuiCol_TitleBgActive] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
		colours[ImGuiCol_TitleBgCollapsed] = ImVec4(0.056f, 0.188f, 0.282f, 1.000f);//
		colours[ImGuiCol_MenuBarBg] = ImVec4(0.055f, 0.188f, 0.282f, 1.000f);//
		colours[ImGuiCol_CheckMark] = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
		colours[ImGuiCol_SliderGrab] = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
		colours[ImGuiCol_SliderGrabActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		//colors[ImGuiCol_DockingPreview] = ImVec4(1.000f, 0.391f, 0.000f, 0.781f);
		//colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
		colours[ImGuiCol_PlotLines] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
		colours[ImGuiCol_PlotLinesHovered] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		colours[ImGuiCol_PlotHistogram] = ImVec4(0.586f, 0.586f, 0.586f, 1.000f);
		colours[ImGuiCol_PlotHistogramHovered] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		colours[ImGuiCol_TextSelectedBg] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
		colours[ImGuiCol_DragDropTarget] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		colours[ImGuiCol_NavHighlight] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		colours[ImGuiCol_NavWindowingHighlight] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		colours[ImGuiCol_NavWindowingDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
		colours[ImGuiCol_ModalWindowDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);

		colours[ImGuiCol_TableBorderLight] = ImVec4(0.02f, 0.02f, 0.02f, 1.0f);
		colours[ImGuiCol_TableBorderStrong].w = 0.25f;


		style.FrameRounding = 2.0f;
		style.GrabRounding = 4.0f;
		style.TabRounding = 3.0f;

		//style.ChildRounding = 4.0f;
		//style.FrameBorderSize = 1.0f;
		//style.GrabMinSize = 7.0f;
		//style.PopupRounding = 2.0f;
		//style.ScrollbarRounding = 12.0f;
		//style.ScrollbarSize = 13.0f;
		//style.TabBorderSize = 1.0f;
		//style.WindowRounding = 4.0f;
	}

	void UI_System::BlueAccentTheme(ImGuiStyle& style)
	{
		ImVec4* colours = style.Colors;

		//Headers /Tabs
		colours[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
		colours[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.0f);

		colours[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
		colours[ImGuiCol_TabActive] = ImVec4(0.20f, 0.50f, 0.85f, 1.000f);
		colours[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.50f, 0.85f, 1.000f);

		colours[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
		colours[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);

		colours[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
		colours[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.0f);

		colours[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);//
		colours[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	}

	void UI_System::TealAccentTheme(ImGuiStyle& style)
	{
		ImVec4* colours = style.Colors;

		//Headers /Tabs
		colours[ImGuiCol_HeaderHovered] = ImVec4(0.20f, 0.85f, 0.65f, 0.80f);
		colours[ImGuiCol_HeaderActive] = ImVec4(0.20, 0.85, 0.65f, 1.0f);

		colours[ImGuiCol_TabHovered] = ImVec4(0.20f, 0.85f, 0.65f, 0.80f);
		colours[ImGuiCol_TabActive] = ImVec4(0.20f, 0.75f, 0.55f, 1.000f);
		colours[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.75f, 0.55f, 1.000f);

		colours[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.85f, 0.65f, 0.40f);
		colours[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.85f, 0.65f, 0.67f);

		colours[ImGuiCol_ButtonHovered] = ImVec4(0.20f, 0.895f, 0.65f, 0.80f);
		colours[ImGuiCol_ButtonActive] = ImVec4(0.1f, 0.75f, 0.55f, 1.0f);

		colours[ImGuiCol_ResizeGripHovered] = ImVec4(0.20f, 0.85f, 0.65f, 0.67f);//
		colours[ImGuiCol_ResizeGripActive] = ImVec4(0.20f, 0.85f, 0.65f, 0.95f);
	}

	void UI_System::SetupStyle(ImGuiStyle& style, ESandboxTheme theme)
	{
		glm::vec4 accent;/* = ImVec4(0.2f, 0.55f, 0.95f, 1.0f)*/

		switch (theme)
		{
		case Fluxon::ESandboxTheme::BlueAccent:
			accent = glm::vec4(0.2f, 0.55f, 0.95f, 1.0f);
			break;
		case Fluxon::ESandboxTheme::TealAccent:
			accent = glm::vec4(0.20f, 0.85f, 0.65f, 0.80f);
			break;
		default:
			break;
		}

		ImGui::StyleColorsDark(&style);

		style.WindowRounding = 4.0f;
		style.FrameRounding = 3.0f;
		style.ChildRounding = 3.0f;
		style.PopupRounding = 3.0f;
		style.GrabRounding = 3.0f;
		style.ScrollbarRounding = 3.0f;


		style.WindowBorderSize = 1.0f;
		style.FrameBorderSize = 1.0f;
		style.TabBorderSize = 0.0f;

		style.ItemSpacing = ImVec2(8, 4);
		style.ItemInnerSpacing = ImVec2(6, 4);
		style.CellPadding = ImVec2(6, 2);
		style.IndentSpacing = 18.0f;

		ImVec4* colours = style.Colors;

		colours[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.0f);
		colours[ImGuiCol_ChildBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.0f);
		colours[ImGuiCol_Header] = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
		colours[ImGuiCol_HeaderHovered] = ToImGui(accent * glm::vec4(1, 1, 1, 0.6));
		colours[ImGuiCol_HeaderActive] = ToImGui(accent);
		colours[ImGuiCol_Tab] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
		colours[ImGuiCol_TabHovered] = ToImGui(accent * glm::vec4(1, 1, 1, 0.7f));
		colours[ImGuiCol_TabActive] = ToImGui(accent);
		colours[ImGuiCol_FrameBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
		colours[ImGuiCol_FrameBgHovered] = ToImGui(accent * glm::vec4(1, 1, 1, 0.5f));
		colours[ImGuiCol_FrameBgActive] = ToImGui(accent);
		colours[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
		colours[ImGuiCol_ButtonHovered] = ToImGui(accent * glm::vec4(1, 1, 1, 0.5f));
		colours[ImGuiCol_ButtonActive] = ToImGui(accent);
		colours[ImGuiCol_CheckMark] = ToImGui(accent);

		colours[ImGuiCol_TableRowBg] = ImVec4(0.13f, 0.13f, 0.13f, 0.7f); //ImVec4(0.13f, 0.13f, 0.13f, 1.0f);
		colours[ImGuiCol_TableRowBgAlt] = ImVec4(0.16f, 0.16f, 0.16f, 0.7f); // ImVec4(0.16f, 0.16f, 0.16f, 1.0f);



		//colours[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.0f);
		//colours[ImGuiCol_TextDisabled] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
		//colours[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		//colours[ImGuiCol_TabUnfocused] = ImVec4(0.15f, 0.18f, 0.22f, 1.0f);
		//colours[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.21f, 0.22f, 1.00f);
		//colours[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);//
		//colours[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.0f);//
		//colours[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.000f);//
		//colours[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.000f);
		//colours[ImGuiCol_Separator] = ImVec4(0.39f, 0.39f, 0.39f, 0.62f);//
		//colours[ImGuiCol_SeparatorHovered] = ImVec4(0.75f, 0.75f, 0.75f, 1.0f);//
		//colours[ImGuiCol_SeparatorActive] = ImVec4(1.000f, 0.40f, 0.000f, 1.000f);
		//colours[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);//
		//colours[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);//
		//colours[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
		//colours[ImGuiCol_Border] = ImVec4(0.266f, 0.266f, 0.266f, 1.000f);
		//colours[ImGuiCol_BorderShadow] = ImVec4(0.000f, 0.000f, 0.000f, 0.000f);
		//colours[ImGuiCol_TitleBg] = ImVec4(0.05, 0.055, 0.055, 1.0);//
		//colours[ImGuiCol_TitleBgActive] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
		//colours[ImGuiCol_TitleBgCollapsed] = ImVec4(0.056f, 0.188f, 0.282f, 1.000f);//
		//colours[ImGuiCol_MenuBarBg] = ImVec4(0.055f, 0.188f, 0.282f, 1.000f);//
		//colours[ImGuiCol_SliderGrab] = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
		//colours[ImGuiCol_SliderGrabActive] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		//colours[ImGuiCol_DockingPreview] = ImVec4(1.000f, 0.391f, 0.000f, 0.781f);
		//colours[ImGuiCol_DockingEmptyBg] = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
		//colours[ImGuiCol_PlotLines] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
		//colours[ImGuiCol_PlotLinesHovered] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		//colours[ImGuiCol_PlotHistogram] = ImVec4(0.586f, 0.586f, 0.586f, 1.000f);
		//colours[ImGuiCol_PlotHistogramHovered] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		//colours[ImGuiCol_TextSelectedBg] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
		//colours[ImGuiCol_DragDropTarget] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		//colours[ImGuiCol_NavHighlight] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		//colours[ImGuiCol_NavWindowingHighlight] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
		//colours[ImGuiCol_NavWindowingDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
		//colours[ImGuiCol_ModalWindowDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);

		//colours[ImGuiCol_TableBorderLight] = ImVec4(0.02f, 0.02f, 0.02f, 1.0f);
		//colours[ImGuiCol_TableBorderStrong].w = 0.25f;

	}

	void UI_System::LoadFonts(ImGuiIO& imgui_io)
	{

		//ImFont* default_font = nullptr;
		imgui_io.Fonts->AddFontFromFileTTF((mFontDirectory + "/" + mDefaultFont).c_str(), 18.0f);
		for (const auto& entry : std::filesystem::directory_iterator(mFontDirectory))
		{
			if (entry.is_regular_file())
			{
				std::string path = entry.path().filename().string();

				FLUX_DEBUG_LOG_INFO("Font: ", path);
				//if(path == mDefaultFont) //already add Pushfont not allowed
					//default_font = imgui_io.Fonts->AddFontFromFileTTF((mFontDirectory + "/" + path).c_str(), 16.0f);
				//else 
				if(path != mDefaultFont) 
					imgui_io.Fonts->AddFontFromFileTTF((mFontDirectory + "/" + path).c_str(), 18.0f);

			}

		}

		//if (default_font != nullptr)
		//	ImGui::PushFont(default_font);

	}
	

} //namesapce Fluxon