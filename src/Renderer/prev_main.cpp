////To access native plaform functions 
////to ccreate glfw window surface 
//#define VK_USE_PLATFORM_WIN32_KHR
//#define GLFW_INCLUDE_VULKAN
//#include <GLFW/glfw3.h>
//#include "Window.h"
//#define GLFW_EXPOSE_NATIVE_WIN32
//#include <GLFW/glfw3native.h>
//
//#include <vulkan/vulkan.h>
//
//
//
//#include <iostream>
//#include <stdexcept>
//#include <cstdlib>
//
//#include <vector>
//#include <map>
//#include <optional>
//#include <set>
//
//#include <cstdint>
//#include <limits>
//#include <algorithm>
//
//#include "Utilities.h"
//
//#include <thread>
//#include <chrono>
//
//
//#include "GPUStrutures.h"
//#define GLM_FORCE_RADIANS
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//
//#include "InputSystem.h"
//
//
//#define STB_IMAGE_IMPLEMENTATION
//#include <stb_image.h>
//
//#include "Camera.h"
//#define IMGUI_IN_USE 1
//#include "UISystem.h"
//
//#include "VulkanUtilities.h"
//#include "Device.h"
//
//class HelloTriangleApp
//{
//public:
//	void Run() {
//		initWindow();
//		InitVulkan();
//		InitSystems();
//		MainLoop();
//		CleanUp();
//	}
//private:
//	const uint32_t WIDTH = 800;
//	const uint32_t HEIGHT = 600;
//
//	const std::vector<const char*> mValidationLayers = {
//		"VK_LAYER_KHRONOS_validation"
//	};
//	const std::vector<const char*> mDeviceExtensions = {
//		VK_KHR_SWAPCHAIN_EXTENSION_NAME
//	};
//#ifdef NDEBUG
//	const bool mEnableValidationLayers = false;
//#else
//	const bool mEnableValidationLayers = true;
//#endif // NDEBUG
//
//	Vulkan::Device mDevice;
//
//	VkSwapchainKHR mSwapchain;
//	std::vector<VkImage> mSwapchainImages;
//	VkFormat mSwapchainImgFormat;
//	VkExtent2D mSwapchainExtent;
//	std::vector<VkImageView> mSwapchainImgViews;
//
//
//	VkRenderPass mRenderPass;
//	VkDescriptorSetLayout mDescriptorSetLayout;
//	VkDescriptorPool mDescriptorPool;
//	std::vector<VkDescriptorSet> mDescriptorSets;
//	VkPipelineLayout mPipelineLayout;
//	VkPipeline mGraphicsPipeline;
//	std::vector<VkFramebuffer> mSwapchainFramebuffers;
//
//	VkCommandPool mCommandPool;
//	//Each frame should have its own command buffer, set of semaphores, and fence
//	std::vector<VkCommandBuffer> mCommandBuffers;
//
//	VkBuffer mVertexBuffer;
//	VkDeviceMemory mVertexBufferMemory;
//
//	VkBuffer mIndexBuffer;
//	VkDeviceMemory mIndexBufferMemory;
//
//	std::vector<VkBuffer> mUniformBuffers;
//	std::vector<VkDeviceMemory> mUniformBuffersMemories;
//	std::vector<void*> mUniformBuffersMapped;
//
//	VkImage mTextureImage;
//	VkDeviceMemory mTextureImageMemory;
//	VkImageView mTextureImageView;
//	VkSampler mTextureSampler;
//	bool mUseSamplingAnisotropy = false;
//
//
//
//	VkImage mDepthImage;
//	VkDeviceMemory mDepthImageMemory;
//	VkImageView mDepthImageView;
//
//	Camera mCamera;
//	UI_System mImGuiUISystem;
//
//	Platform::Window mTestWindow;
//	//in functions 
//	//void CleanUp() {
//	//2 x void RecordCommandBuffer(VkCommandBuffer command_buffer, uint32_t image_idx) {
//	//void InitSystems() {
//
//#define RENDER_CUBE 1
//#if RENDER_CUBE
//
//	//interleaving vertex attributes
//	//for cube 
//	const std::vector<GPUStructures::Vertex> mVertices = {
//
//	//front face (+z)
//	{ { -0.5, -0.5, 0.5}, { 1.0, 0.0, 0.0 },	{1.0f, 0.0f}},
//	{ {  0.5, -0.5, 0.5}, {0.0, 1.0, 0.0},		{0.0f, 0.0f} },
//	{ {  0.5,  0.5, 0.5 }, {1.0, 1.0, 0.0},		{0.0f, 1.0f}},
//	{ { -0.5,  0.5, 0.5}, {0.0, 0.0, 1.0},		{1.0f, 1.0f} },
//
//		// Back face (-z)
//	{ {-0.5, -0.5, -0.5},	{1.0, 0.0, 0.0},	{1.0f, 0.0f}},
//	{ { 0.5 ,-0.5 ,-0.5},	{0.0, 0.0, 1.0},	{0.0f, 0.0f} },
//	{ { 0.5 , 0.5, -0.5},	{1.0, 0.0, 1.0},	{0.0f, 1.0f}},
//	{ {-0.5,  0.5 ,-0.5},	{1.0, 1.0, 1.0},	{1.0f, 1.0f} },
//
//	// Right face (+x)
//	{ {0.5, -0.5, -0.5},	{1.0, 0.0, 0.0},	{1.0f, 0.0f}},
//	{ { 0.5,  0.5, -0.5},	{0.0, 1.0, 0.0},	{0.0f, 0.0f} },
//	{ { 0.5,  0.5,  0.5	},	{1.0, 1.0, 0.0},	{0.0f, 1.0f}},
//	{ { 0.5, -0.5,  0.5},	{0.0, 0.0, 1.0},	{1.0f, 1.0f} },
//	
//	//Left face  (-x)
//	{ {-0.5, -0.5, -0.5},	{1.0, 0.0, 0.0},	{1.0f, 0.0f}},
//	{ {-0.5,  0.5 ,-0.5},	{0.0, 0.0, 1.0},	{0.0f, 0.0f} },
//	{ {-0.5,  0.5 , 0.5 },	{1.0, 0.0, 1.0},	{0.0f, 1.0f}},
//	{ {-0.5, -0.5,  0.5},	{1.0, 1.0, 1.0},	{1.0f, 1.0f} },
//	
//	//Top face (+y)
//	{{-0.5, 0.5, -0.5},		{1.0, 0.0, 0.0},	{1.0f, 0.0f}},
//	{{ 0.5, 0.5, -0.5},		{0.0, 1.0, 0.0},	{0.0f, 0.0f} },
//	{{ 0.5, 0.5,  0.5},		{1.0, 1.0, 0.0},	{0.0f, 1.0f}},
//	{{-0.5, 0.5,  0.5},		{0.0, 0.0, 1.0},	{1.0f, 1.0f} },
//	
//	//Bottom face (-y)
//	{ {-0.5, -0.5, -0.5},	{1.0, 0.0, 0.0},	{1.0f, 0.0f}},
//	  {{0.5, -0.5, -0.5},	{0.0, 0.0, 1.0},	{0.0f, 0.0f} },
//	  {{0.5, -0.5,  0.5},	{1.0, 0.0, 1.0},	{0.0f, 1.0f}},
//	{{-0.5, -0.5,  0.5},	{1.0, 1.0, 1.0},	{1.0f, 1.0f} }
//	};
//
//	const std::vector<uint16_t> mIndices = {
//		//Front face
//		0, 1, 2,
//		0, 2, 3,
//
//		//Back face
//		4, 6, 5,
//		4, 7, 6,
//
//		//Right face
//		8, 9, 10,
//		8, 10, 11,
//
//		//Left face
//		12, 14, 13,
//		12, 15, 14,
//
//		//Top face
//		16, 18, 17,
//		16, 19, 18,
//
//		//Bottom face
//		20, 21, 22,
//		20, 22, 23
//	};
//
//#else
//
//	//interleaving vertex attributes
//	const std::vector<GPUStructures::Vertex> mVertices = {
//	{{-0.5f, -0.5f, 0.0f},	{0.0f, 0.0f, 1.0f},	{1.0f, 0.0f}},
//	{{0.5f, -0.5f, 0.0f},	{0.0f, 1.0f, 0.0f},	{0.0f, 0.0f} },
//	{{0.5f, 0.5f, 0.0f},	{1.0f, 0.0f, 0.0f},	{0.0f, 1.0f}},
//	{{-0.5f, 0.5f, 0.0f},	{0.0f, 0.0f, 1.0f},	{1.0f, 1.0f} }
//	};
//
//	const std::vector<uint16_t> mIndices = {
//		0, 1, 2,
//		2, 3, 0
//	};
//
//#endif // RENDER_CUBE
//
//
//
//
//
//	glm::vec3 mModelPos = glm::vec3(0.0f);
//	glm::vec3 mModelRotateAxis = glm::vec3(0.0f, 0.0f, 1.0f);
//	glm::mat4 mModelMatrix = glm::mat4(1.0f);
//
//	std::vector<VkSemaphore> mImageAvailableSemaphores;
//	//render finsinhed semaphore needs to be image/swap chain image based count 
//	//because nots not based on the frame count/flight count its based  on
//	//is the graphic queue finshed rendering/writing/using the spaw chain image then 
//	//if so it can now be used, not flight based 
//	//Each swapchain image needs its own render-finshed semaphore 
//	//This is because the semaphore is tied to the usage of a specific swapcchain image, 
//	// not to the frame-in-flight index. A semaphore used in vkQueuePresentKHR
//	// may still be in use by the presentation engine even after the GPU work is Reusing 
//	// it too early (e.g, based on frame count) can cause validation errors.
//	// By indexing semaphores by the acquired image index, we ensure that a semaphore
//	// is only reused after the presentation engine has finshed with that image.
//	//
//	std::vector<VkSemaphore> mRenderFinishedSemaphores;
//	std::vector<VkFence> mInFlightFences;
//
//
//	// 2 because we don't want the CPU to get too far ahead of the GPU
//	//With 2 frames in flight, the CPU and the GPU can be working on their own tasks at the same time
//	const int MAX_FRAMES_IN_FLIGHT = 2;
//	uint32_t mCurrentFrame = 0;
//
//	double mCurrFrameDT;
//	int mFrameRate = 0;
//
//	//bool mLockCursor = false;
//	
//
//	void initWindow() {
//
//
//		mTestWindow.width = 1920;// WIDTH;
//		mTestWindow.height = 1080;// HEIGHT;
//		mTestWindow.title = "vulkan";
//
//		Platform::InitialiseGLFWWindow(mTestWindow);
//		//mWindow = mTestWindow.glfwWindowHandle;
//		//glfwSetKeyCallback(mWindow, KeyboardInputCallback);
//		InputSystem::Input::Instance().CreateCallbacks(mTestWindow.glfwWindowHandle);
//	}
//
//
//	void InitVulkan() {
//
//
//		mDevice = Vulkan::InitDevice(&mTestWindow, true, DebugCallback);
//
//		
//		CreateSwapChain();
//		CreateSwapchainImageViews();
//
//		
//		CreateRenderPass();
//		CreateDescriptionSetLayout();
//		//dependacies 
//		//swapchain
//		//descriptionsetlayout
//		//device
//		CreateGraphicsPipeline();
//
//
//		//dependacies 
//		//physical device
//		//device
//		CreateCommandPool();
//
//		//dependacies 
//		// device
//		// swap chain
//		//command pool
//		CreateDepthResources();
//
//		//dependacies 
//		//swapchain image view
//		//depth image view (depth resource) 
//		CreateFramebuffers();
//
//		CreateVertexBuffer();
//		CreateIndexBuffer();
//		VkFormat tex_image_format = VK_FORMAT_R8G8B8A8_SRGB;
//		CreateTextureImage(tex_image_format);
//		CreateTextureImageView(tex_image_format);
//		mTextureSampler = CreateTextureSampler();
//
//		CreateUniformBuffers();
//		CreateDescriptorPool();
//		CreateDescriptorSets();
//
//		CreateCommandBuffers();
//		CreateSyncObjects();
//	}
//	void MainLoop() {
//
//		//glfwGetTime returns time in seconds
//		double last_frame_time = glfwGetTime() * 1000.0f; 
//		//while (!glfwWindowShouldClose(mWindow)) {
//		while (!Platform::ShouldWindowClose(&mTestWindow)) 
//		{
//
//			//glfwGetTime returns time in seconds
//			double curr_frame_time = glfwGetTime() * 1000.0f;
//			mCurrFrameDT = curr_frame_time - last_frame_time;
//			last_frame_time = curr_frame_time;
//
//			glfwPollEvents();
//			DrawFrame();
//			//std::this_thread::sleep_for(std::chrono::seconds(10));
//
//			auto input_sys_inst = InputSystem::Input::Instance();
//
//
//			if (input_sys_inst.GetKey(GLFW_KEY_UP))
//				mModelPos.z += 2.5f * GetFrameTimeInSeconds();
//			if ( input_sys_inst.GetKey(GLFW_KEY_DOWN))
//				mModelPos.z -= 2.5f * GetFrameTimeInSeconds();
//
//			if (input_sys_inst.GetKey(GLFW_KEY_LEFT))
//				mModelPos.x -= 2.5f * GetFrameTimeInSeconds();
//			if (input_sys_inst.GetKey(GLFW_KEY_RIGHT))
//				mModelPos.x += 2.5f * GetFrameTimeInSeconds();
//
//
//
//			TranslateCamera(input_sys_inst, mCamera, GetFrameTimeInSeconds(), glm::vec3(2.5f));
//				
//
//
//			static float rot_increament = 90.0f;
//			if (input_sys_inst.GetKeyDown(GLFW_KEY_Z))
//				rot_increament += 5.0f;
//			else if (input_sys_inst.GetKeyDown(GLFW_KEY_X))
//				rot_increament -= 5.0f;
//			rot_increament = glm::clamp(rot_increament, 0.0f, 720.0f);
//			//update the and store rotation in rotateAxis
//			//converted to radian when use
//			//mModelRotateAxis = glm::vec3(0.0f, 0.0f, 45.0f);
//			float axis_rotation = glm::length(mModelRotateAxis);
//			axis_rotation += rot_increament * GetFrameTimeInSeconds();
//			axis_rotation = (axis_rotation > 360) ? axis_rotation - 360 : axis_rotation;
//			mModelRotateAxis = glm::normalize(mModelRotateAxis) * axis_rotation;
//			//if (axis_rotation > 360)
//				//axis_rotation = axis_rotation - 360;
//
//			static int axis = 0;
//			if (input_sys_inst.GetKeyDown(GLFW_KEY_SPACE))
//			{
//				axis = (axis + 1) % 3;
//				if (axis == 0)
//					mModelRotateAxis = glm::vec3(0.0f, 0.0f, axis_rotation);
//				if (axis == 1)
//					mModelRotateAxis = glm::vec3(axis_rotation, 0.0f, 0.0f);
//				if (axis == 2)
//					mModelRotateAxis = glm::vec3(0.0f, axis_rotation, 0.0f);
//			}
//			
//
//			//Update GLFW window title with frame times
//			static float display_title_update = 0;
//			display_title_update += GetFrameTimeInSeconds();
//			if (display_title_update >= 0.5f)
//			{
//				static char window_title[64];
//				mFrameRate = 1000.0f / mCurrFrameDT;
//				snprintf(window_title, sizeof(window_title), "Vulkan - %.3fms, %d FPS. Current Axis rot: %.1f", mCurrFrameDT, mFrameRate, axis_rotation);
//				Platform::ChangeWindowTitle(&mTestWindow, window_title);
//				display_title_update = 0;
//			}
//
//
//			input_sys_inst.FlushFrameInputs();
//		}
//	}
//	void CleanUp() {
//
//		vkDeviceWaitIdle(mDevice.device);
//
//		mImGuiUISystem.Shutdown();
//
//		//vkQueueWaitIdle(mPresentQueue /*mGraphicsQueue*/);
//
//		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
//		{
//			vkDestroySemaphore(mDevice.device, mImageAvailableSemaphores[i], nullptr);
//			vkDestroyFence(mDevice.device, mInFlightFences[i], nullptr);
//		}
//
//		for (auto& render_finished_semaphore : mRenderFinishedSemaphores)
//			vkDestroySemaphore(mDevice.device, render_finished_semaphore, nullptr);
//
//		vkDestroyCommandPool(mDevice.device, mCommandPool, nullptr);
//		vkDestroyPipeline(mDevice.device, mGraphicsPipeline, nullptr);
//		vkDestroyPipelineLayout(mDevice.device, mPipelineLayout, nullptr);
//		vkDestroyRenderPass(mDevice.device, mRenderPass, nullptr);
//
//		CleanupSwapchainAndResources();
//
//		vkDestroyBuffer(mDevice.device, mVertexBuffer, nullptr);
//		vkFreeMemory(mDevice.device, mVertexBufferMemory, nullptr);
//
//		vkDestroyBuffer(mDevice.device, mIndexBuffer, nullptr);
//		vkFreeMemory(mDevice.device, mIndexBufferMemory, nullptr);
//
//		vkDestroySampler(mDevice.device, mTextureSampler, nullptr);
//		vkDestroyImageView(mDevice.device, mTextureImageView, nullptr);
//		vkDestroyImage(mDevice.device, mTextureImage, nullptr);
//		vkFreeMemory(mDevice.device, mTextureImageMemory, nullptr);
//
//		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
//		{
//			vkDestroyBuffer(mDevice.device, mUniformBuffers[i], nullptr);
//			vkFreeMemory(mDevice.device, mUniformBuffersMemories[i], nullptr);
//		}
//		vkDestroyDescriptorSetLayout(mDevice.device, mDescriptorSetLayout, nullptr);
//
//		//vkFreeDescriptorSets(mDevice.device, mDescriptorPool, static_cast<uint32_t>(mDescriptorSets.size()), mDescriptorSets.data());
//		vkDestroyDescriptorPool(mDevice.device, mDescriptorPool, nullptr);
//
//		//if (mEnableValidationLayers)
//		//	DestroyDebugUtilsMessengerEXT(mDevice.instance, mDevice.debugMessager, nullptr);
//		//vkDestroyDevice(mDevice.device, nullptr);
//		//vkDestroySurfaceKHR(mDevice.instance, mDevice.surface, nullptr);
//		//vkDestroyInstance(mDevice.instance, nullptr);
//
//		Vulkan::DestroyDevice(&mDevice, true);
//		Platform::DestroyWindow(&mTestWindow);
//	}
//
//
//	inline float GetFrameTimeInSeconds() {
//		return mCurrFrameDT * 0.001f;
//	}
//
//	void DrawFrame() {
//
//		vkWaitForFences(mDevice.device, 1, &mInFlightFences[mCurrentFrame], VK_TRUE, UINT64_MAX);
//
//		uint32_t img_index;
//		VkResult acquire_nxt_img_result = vkAcquireNextImageKHR(mDevice.device, mSwapchain, UINT64_MAX, mImageAvailableSemaphores[mCurrentFrame], VK_NULL_HANDLE, &img_index);
//
//		if (acquire_nxt_img_result == VK_ERROR_OUT_OF_DATE_KHR)
//		{
//			std::cout << "\033[1;31m [DRAW FRAME]\033[0m: " << "Need to REcreate image acquire" << std::endl;
//			WindowResizedRecreateResource();
//			return;
//		}
//		else if(acquire_nxt_img_result != VK_SUCCESS && acquire_nxt_img_result != VK_SUBOPTIMAL_KHR)
//			std::cout << "\033[1;31m [DRAW FRAME]\033[0m: " << "Failed to Acquire Swap Chain Image" << std::endl;
//
//		vkResetFences(mDevice.device, 1, &mInFlightFences[mCurrentFrame]);
//
//		//call vkResetCommandBuffer on the command buffer to make sure it is able to be recorded.
//		vkResetCommandBuffer(mCommandBuffers[mCurrentFrame], 0);
//
//		UpdateUniformBuffer(mCurrentFrame, mModelPos, mModelRotateAxis);
//		UpdateUniformBuffer((mCurrentFrame+1)%MAX_FRAMES_IN_FLIGHT, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//
//		RecordCommandBuffer(mCommandBuffers[mCurrentFrame], img_index);
//
//		VkSubmitInfo submit_info{};
//		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//		VkSemaphore  wait_img_write_semaphores[] = { mImageAvailableSemaphores[mCurrentFrame]};
//		VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
//		submit_info.waitSemaphoreCount = 1;
//		submit_info.pWaitSemaphores = wait_img_write_semaphores;
//		submit_info.pWaitDstStageMask = wait_stages;
//
//		submit_info.commandBufferCount = 1;
//		submit_info.pCommandBuffers = &mCommandBuffers[mCurrentFrame];
////
////#if MULTI_FINISHED_SEMAPHORES
////		VkSemaphore render_finished_semaphores = mRenderFinishedSemaphores[img_index];
////		VkSemaphore signal_semaphores[] = { render_finished_semaphores };
////#else
////		VkSemaphore signal_semaphores[] = { mRenderFinishedSemaphore };
////#endif // MULTI_FINISHED_SEMAPHORES
//
//		//use img_index because semaphore to image been used in swap chain
//		VkSemaphore signal_img_write_complete_present_semaphores[] = { mRenderFinishedSemaphores[img_index]};
//		submit_info.signalSemaphoreCount = 1;
//		submit_info.pSignalSemaphores = signal_img_write_complete_present_semaphores;
//
//		if (vkQueueSubmit(mDevice.graphicsQueue, 1, &submit_info, mInFlightFences[mCurrentFrame]) != VK_SUCCESS)
//			throw std::runtime_error("Failed to submit draw command buffer!!!!");
//
//		VkPresentInfoKHR present_info{};
//		present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
//		present_info.waitSemaphoreCount = 1;
//		present_info.pWaitSemaphores = signal_img_write_complete_present_semaphores;
//
//		VkSwapchainKHR swap_chains[] = { mSwapchain };
//		present_info.swapchainCount = 1;
//		present_info.pSwapchains = swap_chains;
//		present_info.pImageIndices = &img_index;
//		present_info.pResults = nullptr;
//
//		VkResult presenting_queue_result = vkQueuePresentKHR(mDevice.presentQueue, &present_info);
//
//		if (presenting_queue_result == VK_ERROR_OUT_OF_DATE_KHR || presenting_queue_result == VK_SUBOPTIMAL_KHR || mTestWindow.windowResized)
//		{
//			std::cout << "\033[1;31m [DRAW FRAME]\033[0m: " << "Need to Recreate image acquire, for Presentation" << std::endl;
//			mTestWindow.windowResized = false;
//			WindowResizedRecreateResource();
//		}
//		else if (presenting_queue_result != VK_SUCCESS)
//			std::cout << "\033[1;31m [DRAW FRAME]\033[0m: " << "Failed to Present Image" << std::endl;
//
//		mCurrentFrame = (mCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
//	}
//
//	void UpdateUniformBuffer(uint32_t current_image, glm::vec3 model_pos, glm::vec3 rotate_axis) {
//		static auto start_time = std::chrono::high_resolution_clock::now();
//
//		auto curr_time = std::chrono::high_resolution_clock::now();
//		float time = std::chrono::duration<float, std::chrono::seconds::period>(curr_time - start_time).count();
//
//		GPUStructures::MVP_UniformBufferObject MVP;
//		//MVP.model = glm::translate(glm::mat4(1.0f), mModelPos) * glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), mModelRotateAxis);
//		mModelMatrix = glm::translate(glm::mat4(1.0f), model_pos) * glm::rotate(glm::mat4(1.0f), glm::radians(glm::length(rotate_axis)), glm::normalize(rotate_axis));
//		//mModelMatrix = glm::translate(mModelMatrix, mModelPos) * glm::rotate(mModelMatrix, glm::radians(glm::length(rotate_axis)), glm::normalize(rotate_axis));
//		MVP.model = mModelMatrix;
//
//		//MVP.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
//
//
//		MVP.view = Camera::GetView(mCamera);
//		MVP.projection = Camera::GetProjection(mCamera, static_cast<float>(mSwapchainExtent.width), static_cast<float>(mSwapchainExtent.height));
//
//		//MVP.view = glm::lookAt(position, position + forward, glm::vec3(0.0f, 1.0f, 0.0f));
//		//MVP.projection = glm::perspective(glm::radians(45.0f), mSwapchainExtent.width / (float)mSwapchainExtent.height, 0.1f, 10.0f);
//
//		//MVP.projection[1][1] *= -1;
//
//		memcpy(mUniformBuffersMapped[current_image], &MVP, sizeof(MVP));
//	}
//
//
//	//void CreateVKInstance() {
//	//	if (mEnableValidationLayers && !CheckValidationLayerSupport())
//	//		throw std::runtime_error("\n\nVaildation layers requested, but not available!!!");
//
//
//	//	VkApplicationInfo app_info{};
//	//	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
//	//	app_info.pApplicationName = "Hello Triangle";
//	//	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
//	//	app_info.pEngineName = "No Engine";
//	//	app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
//	//	app_info.apiVersion = VK_API_VERSION_1_0;
//
//
//
//	//	VkInstanceCreateInfo create_info{};
//	//	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
//	//	create_info.pApplicationInfo = &app_info;
//
//	//	auto req_extensions = GetRequiredExtensions();
//	//	create_info.enabledExtensionCount = static_cast<uint32_t>(req_extensions.size());
//	//	create_info.ppEnabledExtensionNames = req_extensions.data();
//
//	//	VkDebugUtilsMessengerCreateInfoEXT debug_utils_create_info{};
//	//	if (mEnableValidationLayers)
//	//	{
//	//		create_info.enabledLayerCount = static_cast<uint32_t>(mValidationLayers.size());
//	//		create_info.ppEnabledLayerNames = mValidationLayers.data();
//
//	//		PopulateDebugMessengerCreateInfo(debug_utils_create_info);
//	//		create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_utils_create_info;
//	//	}
//	//	else
//	//	{
//	//		create_info.enabledLayerCount = 0;
//	//		create_info.pNext = nullptr;
//	//	}
//
//
//	//	// TODO (Conqster): This is not required 
//	//	uint32_t extension_count = 0;
//	//	vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
//	//	std::vector<VkExtensionProperties> extensions(extension_count);
//	//	vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());
//
//	//	std::cout << "\n\navaliable extension count: " << extension_count << std::endl;
//	//	std::cout << "available extensions: \n";
//	//	for (const auto& extension : extensions)
//	//		std::cout << '\t' << extension.extensionName << '\n';
//
//	//	if (vkCreateInstance(&create_info, nullptr, &mVkInstance) != VK_SUCCESS)
//	//		throw std::runtime_error("Failed to create vulkan instance!!!!!!!");
//	//}
//
//
//
//	bool CheckValidationLayerSupport() {
//		uint32_t layer_count;
//		vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
//
//		std::vector<VkLayerProperties> avaliable_layers(layer_count);
//		vkEnumerateInstanceLayerProperties(&layer_count, &avaliable_layers[0]);
//
//		std::cout << "\n\navaliable layer count: " << layer_count << std::endl;
//		std::cout << "available layers: \n";
//		for (const auto& layer : avaliable_layers)
//			std::cout << '\t' << layer.layerName << '\n';
//
//
//		//check our required layer from available layer
//		for (const auto& layer_name : mValidationLayers)
//		{
//			bool layer_found = false;
//			for (const auto& avaliable_layer_prop : avaliable_layers)
//			{
//				if (strcmp(layer_name, avaliable_layer_prop.layerName) == 0)
//				{
//					layer_found = true;
//					break;
//				}
//			}
//
//			if (!layer_found)
//				return false;
//		}
//
//		return true;
//	}
//
//	std::vector<const char*> GetRequiredExtensions() {
//		uint32_t glfw_extension_count = 0;
//		const char** pp_glfw_extensions;
//		pp_glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
//
//		std::vector<const char*> extensions(pp_glfw_extensions, pp_glfw_extensions + glfw_extension_count);
//
//		if (mEnableValidationLayers)
//			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
//		
//		std::cout << "\n\nrequired extension count: " << extensions.size() << std::endl;
//		for (const auto& extension : extensions)
//			std::cout << '\t' << extension << '\n';
//
//		return extensions;
//	}
//
//
//
//	/////////////////////////////////////////
//	// vk_GLFW_WINDOW_SURFACE
//	/////////////////////////////////////////
//#pragma region vk_GLFW_WINDOW_SURFACE
//	//void CreateSurface() {
//	//	VkWin32SurfaceCreateInfoKHR surface_create_info{};
//	//	surface_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
//	//	surface_create_info.hwnd = glfwGetWin32Window(mWindow);
//	//	surface_create_info.hinstance = GetModuleHandle(nullptr);
//
//	//	if (vkCreateWin32SurfaceKHR(mVkInstance, &surface_create_info, nullptr, &mSurface) != VK_SUCCESS)
//	//		throw std::runtime_error("Failed to create window surface!!!!");
//
//
//	//	//OR
//	//	//if (glfwCreateWindowSurface(mVkInstance, mWindow, nullptr, &mSurface) != VK_SUCCESS)
//	//		//throw std::runtime_error("Failed to create window surface!!!!!");
//	//}
//
//	//
//
//#pragma endregion //#vk_GLFW_WINDOW_SURFACE
//
//
//	/////////////////////////////////////////
//	// vk_DEBUG_UTILS
//	/////////////////////////////////////////
//#pragma region vk_DEBUG_UTILS
//	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
//		VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
//		VkDebugUtilsMessageTypeFlagsEXT message_type,
//		const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
//		void* p_user_data) {
//
//		//std::cerr << "Validation layer: " << p_callback_data->pMessage << std::endl;
//		std::cerr << "\033[1;34m [Validation layer]\033[0m: " << p_callback_data->pMessage << std::endl;
//		return VK_FALSE;
//	}
//
//#pragma endregion //# vk_DEBUG_UTILS
//
//
//
//
//#pragma region vk_SWAPCHAIN
//	struct SwapChainSupportDetails {
//		VkSurfaceCapabilitiesKHR capabilities;
//		std::vector<VkSurfaceFormatKHR> formats;
//		std::vector<VkPresentModeKHR> presentModes;
//	};
//	
//	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
//		SwapChainSupportDetails details;
//
//		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
//
//		uint32_t format_count;
//		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);
//		if (format_count != 0)
//		{
//			details.formats.resize(format_count);
//			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, details.formats.data());
//		}
//
//		uint32_t present_mode_count;
//		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, nullptr);
//		if (present_mode_count != 0)
//		{
//			details.presentModes.resize(present_mode_count);
//			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, details.presentModes.data());
//		}
//
//		return details;
//	}
//	bool CheckDeviceExtensionSupport(VkPhysicalDevice device) {
//		uint32_t extension_count;
//		vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);
//		std::vector<VkExtensionProperties> available_extensions(extension_count);
//		vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());
//
//		std::cout << "\n\n\033[1;32m device available extension count: \033[0m" << extension_count << std::endl;
//		for (const auto& available_extension : available_extensions)
//			std::cout << '\t' << available_extension.extensionName << '\n';
//
//		std::set<std::string> required_extensions(mDeviceExtensions.begin(), mDeviceExtensions.end());
//		for (const auto& extension : available_extensions)
//			required_extensions.erase(extension.extensionName);
//
//		return required_extensions.empty();
//	}
//
//
//	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats) {
//		
//		for (const auto& available_format : available_formats)
//			if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB &&
//				available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
//				return available_format;
//
//		return available_formats[0];
//	}
//
//	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes) {
//		
//		for (const auto& available_present_mode : available_present_modes)
//			if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
//				return available_present_mode;
//
//		return VK_PRESENT_MODE_FIFO_KHR;
//	}
//
//	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
//		if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)()) {
//			return capabilities.currentExtent;
//		}
//		else {
//			int width, height;
//			glfwGetFramebufferSize(mTestWindow.glfwWindowHandle, &width, &height);
//
//			VkExtent2D actualExtent = {
//				static_cast<uint32_t>(width),
//				static_cast<uint32_t>(height)
//			};
//
//			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
//			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
//
//			return actualExtent;
//		}
//	}
//
//	void CreateSwapChain() {
//		//SwapChainSupportDetails swap_chain_support = QuerySwapChainSupport(mPhysicalDevice);
//		SwapChainSupportDetails swap_chain_support = QuerySwapChainSupport(mDevice.physicalDevice, mDevice.surface);
//
//		VkSurfaceFormatKHR surface_format = ChooseSwapSurfaceFormat(swap_chain_support.formats);
//		VkPresentModeKHR present_mode = ChooseSwapPresentMode(swap_chain_support.presentModes);
//		VkExtent2D extent = ChooseSwapExtent(swap_chain_support.capabilities);
//
//		// +1 
//		//because the min means we might sometimes have to wait on the driver to complete
//		//internal operations before we can acquire another imgae to render
//		uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;
//
//		//maxImageCount = 0, special case if no maximum
//		if (swap_chain_support.capabilities.maxImageCount > 0 && image_count > swap_chain_support.capabilities.maxImageCount)
//			image_count = swap_chain_support.capabilities.maxImageCount;
//
//
//		VkSwapchainCreateInfoKHR swapchain_create_info{};
//		swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
//		swapchain_create_info.surface = mDevice.surface;
//
//		swapchain_create_info.minImageCount = image_count;
//		swapchain_create_info.imageFormat = surface_format.format;
//		swapchain_create_info.imageColorSpace = surface_format.colorSpace;
//		swapchain_create_info.imageExtent = extent;
//		swapchain_create_info.imageArrayLayers = 1;
//		swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
//
//		//QueueFamilyIndices indices = FindQueueFamilies(mPhysicalDevice);
//		Vulkan::QueueFamilyIndices indices = Vulkan::Utils::FindDeviceQueueFamilies(mDevice);
//		uint32_t queue_family_indices[] = {
//			indices.graphicsFamily.value(),
//			indices.presentFamily.value()
//		};
//
//		if (indices.graphicsFamily != indices.presentFamily)
//		{
//			swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
//			swapchain_create_info.queueFamilyIndexCount = 2;
//			swapchain_create_info.pQueueFamilyIndices = queue_family_indices;
//		}
//		else
//		{
//			swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
//			swapchain_create_info.queueFamilyIndexCount = 0;
//			swapchain_create_info.pQueueFamilyIndices = nullptr;
//		}
//
//
//		swapchain_create_info.preTransform = swap_chain_support.capabilities.currentTransform;
//		swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
//		swapchain_create_info.presentMode = present_mode;
//		swapchain_create_info.clipped = VK_TRUE;
//
//		swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;
//
//
//		if (vkCreateSwapchainKHR(mDevice.device, &swapchain_create_info, nullptr, &mSwapchain) != VK_SUCCESS)
//			throw std::runtime_error("Failed to create swap chain!!!!");
//
//
//		vkGetSwapchainImagesKHR(mDevice.device, mSwapchain, &image_count, nullptr);
//		mSwapchainImages.resize(image_count);
//		vkGetSwapchainImagesKHR(mDevice.device, mSwapchain, &image_count, mSwapchainImages.data());
//
//
//		mSwapchainImgFormat = surface_format.format;
//		mSwapchainExtent = extent;
//
//	}
//#pragma endregion
//
//#pragma region vk_IMAGE_VIEWs
//	void CreateSwapchainImageViews() {
//		mSwapchainImgViews.resize(mSwapchainImages.size());
//
//		for (size_t i = 0; i < mSwapchainImages.size(); i++)
//			mSwapchainImgViews[i] = CreateImageView(mSwapchainImages[i], mSwapchainImgFormat, VK_IMAGE_ASPECT_COLOR_BIT);
//	}
//#pragma endregion //#vk_IMAGE_VIEWs
//
//#pragma region vk_RENDER_PASSES
//	void CreateRenderPass() {
//		VkAttachmentDescription colour_attachment{};
//		colour_attachment.format = mSwapchainImgFormat;
//		colour_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
//
//		
//		//VK_ATTACHMENT_LOAD_OP_LOAD: Preserve the existing contents of the attachment
//		//VK_ATTACHMENT_LOAD_OP_CLEAR : Clear the values to a constant at the start
//		//VK_ATTACHMENT_LOAD_OP_DONT_CARE : Existing contents are undefined; we don't care about them
//		//VK_ATTACHMENT_STORE_OP_STORE: Rendered contents will be stored in memoryand can be read later
//		//VK_ATTACHMENT_STORE_OP_DONT_CARE : Contents of the framebuffer will be undefined after the rendering operation
//		colour_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//		colour_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
//
//		colour_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//		colour_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//		colour_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//		colour_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
//
//
//		//Depth attachment
//		VkAttachmentDescription depth_attachment{};
//		depth_attachment.format = FindDepthFormat();
//		depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
//		depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//		depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//
//		depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//		depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//		depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//		depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
//
//
//
//
//		//SUBPASSESS & ATTACHMENT REFERENCES
//		VkAttachmentReference colour_attachment_ref{};
//		colour_attachment_ref.attachment = 0; //Our array consists of a single VkAttachmentDescription,->colour_attachment
//		colour_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//
//		//depth
//		VkAttachmentReference depth_attachment_ref{};
//		depth_attachment_ref.attachment = 1;
//		depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
//
//
//		VkSubpassDescription subpass{};
//		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
//		subpass.colorAttachmentCount = 1;
//		subpass.pColorAttachments = &colour_attachment_ref;
//		subpass.pDepthStencilAttachment = &depth_attachment_ref;
//
//
//		VkSubpassDependency dependency{};
//		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
//		dependency.dstSubpass = 0;
//
//		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
//		dependency.srcAccessMask = 0;
//
//		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
//		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
//
//		//The following other types of attachments can be referenced by a subpass :
//		//pInputAttachments: Attachments that are read from a shader
//		//pResolveAttachments : Attachments used for multisampling color attachments
//		//pDepthStencilAttachment : Attachment for depthand stencil data
//		//pPreserveAttachments : Attachments that are not used by this subpass, but for which the data must be preserved
//
//		std::array<VkAttachmentDescription, 2> attachments = { colour_attachment, depth_attachment };
//		//Render pass 
//		VkRenderPassCreateInfo renderpass_create_info{};
//		//renderpass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
//		//renderpass_create_info.attachmentCount = 1;
//		////renderpass_create_info.pAttachments = &colour_attachment;
//		//renderpass_create_info.pAttachments = attachments.data();
//		//renderpass_create_info.subpassCount = 1;
//		//renderpass_create_info.pSubpasses = &subpass;
//		//renderpass_create_info.dependencyCount = 1;
//		//renderpass_create_info.pDependencies = &dependency;
//
//		renderpass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
//		renderpass_create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
//		renderpass_create_info.pAttachments = attachments.data();
//		renderpass_create_info.subpassCount = 1;
//		renderpass_create_info.pSubpasses = &subpass;
//		renderpass_create_info.dependencyCount = 1;
//		renderpass_create_info.pDependencies = &dependency;
//
//		if (vkCreateRenderPass(mDevice.device, &renderpass_create_info, nullptr, &mRenderPass) != VK_SUCCESS)
//			throw std::runtime_error("Failed to create render pass!!!!!!");
//	}
//
//
//#pragma endregion //#vk_RENDER_PASSES
//
//
//#pragma region vk_GRAPHICS_PIPELINE
//	void CreateGraphicsPipeline() {
//		auto vert_shader_code = Utils::ReadFile("shaders/tri_vert.spv");
//		auto frag_shader_code = Utils::ReadFile("shaders/tri_frag.spv");
//
//
//		std::cout << "vert shader bytes size: " << vert_shader_code.size() << std::endl;
//		std::cout << "frag shader bytes size: " << frag_shader_code.size() << std::endl;
//
//		VkShaderModule vert_shader_module = CreateShaderModule(vert_shader_code);
//		VkShaderModule frag_shader_module = CreateShaderModule(frag_shader_code);
//
//		/// <summary>
//		/// shader stage creation
//		/// </summary>
//#pragma region vk_SHADER_STAGE_CREATION_INFO
//		VkPipelineShaderStageCreateInfo vert_shader_stage_info{};
//		vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//		vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
//		vert_shader_stage_info.module = vert_shader_module;
//		vert_shader_stage_info.pName = "main";
//
//		VkPipelineShaderStageCreateInfo frag_shader_stage_info{};
//		frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//		frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
//		frag_shader_stage_info.module = frag_shader_module;
//		frag_shader_stage_info.pName = "main";
//
//#pragma endregion //#vk_SHADER_STAGE_CREATION_INFO
//		VkPipelineShaderStageCreateInfo shader_stages[] = { vert_shader_stage_info, frag_shader_stage_info };
//
//
//		////////////////////////////////////////////////
//		// FIXED FUNCTONS (NOT PROGRAMMABLE VERTEX/FRAG)
//		////////////////////////////////////////////////
//#pragma region vk_FIXED_PIPELINE_FUNCTIONs
//
//		//VERTEX INPUT
//#pragma region vk_VERTEX_INPUT
//		VkPipelineVertexInputStateCreateInfo vertex_input_create_info{};
//		vertex_input_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
//		auto binding_description = GPUStructures::Vertex::GetBindingDescription();
//		auto attribute_description = GPUStructures::Vertex::GetAttributeDescriptions();
//		vertex_input_create_info.vertexBindingDescriptionCount = 1;
//		vertex_input_create_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_description.size());
//		vertex_input_create_info.pVertexBindingDescriptions = &binding_description;
//		vertex_input_create_info.pVertexAttributeDescriptions = attribute_description.data();
//#pragma endregion //#vk_VERTEX_INPUT
//
//		//INPUT ASSRMBLY
//#pragma region vk_INPUT_ASSEMBLY
//		VkPipelineInputAssemblyStateCreateInfo input_assembly_create_info{};
//		input_assembly_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
//		input_assembly_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
//		input_assembly_create_info.primitiveRestartEnable = VK_FALSE;
//#pragma endregion //#vk_INPUT_ASSEMBLY
//
//#pragma region vk_VIEWPORT_AND_SCISSOR_STATE
//		/// <summary>
//		/// I THINK THE VIEWPORT & SCISSOR ARE SET HERE FOR STATIC STATE
//		/// 
//		/// viewport represents the tranformation of rendered image to the framebuffer
//		/// defines the image origin to framebuffer 
//		/// if viewport is half on y of the screen the output framebuffer image 
//		/// would be rendered to half of the screen 
//		/// 
//		/// while
//		/// scissor is more like a mask 
//		/// scissor rectangles define in which regions pixels will actually be stored
//		/// pixels outside the rectangle will be discared by the rasteriser
//		/// </summary>
//		VkViewport viewport{};
//		viewport.x = 0.0f;
//		viewport.y = 0.0f;
//		viewport.width = static_cast<float>(mSwapchainExtent.width);
//		viewport.height = static_cast<float>(mSwapchainExtent.height);
//		viewport.minDepth = 0.0f;
//		viewport.maxDepth = 1.0f;
//
//		VkRect2D scissor{};
//		scissor.offset = { 0,0 };
//		scissor.extent = mSwapchainExtent;
//
//		VkPipelineViewportStateCreateInfo viewport_state_create_info{};
//		viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
//		viewport_state_create_info.viewportCount = 1;
//		//viewport_state_create_info.pViewports = &viewport;
//		viewport_state_create_info.scissorCount = 1;
//		//viewport_state_create_info.pScissors = &scissor;
//
//#pragma endregion //#vk_VIEWPORT_AND_SCISSOR_STATE
//
//		//RASTERISER
//		/// <summary>
//		/// Rasteriser takes the geometry shaped by the vertices from verytex shader 
//		/// into fragment to be coloured (by fragment shader) 
//		/// performs (depth testing, face culling and the scissor test)
//		/// </summary>
//		VkPipelineRasterizationStateCreateInfo rasteriser_create_info{};
//		rasteriser_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
//		rasteriser_create_info.depthClampEnable = VK_FALSE;
//		rasteriser_create_info.rasterizerDiscardEnable = VK_FALSE;
//		//VK_POLYGON_MODE_FILL: fill the area of the polygon with fragments
//		//VK_POLYGON_MODE_LINE: polygon edges are drawn as lines
//		//VK_POLYGON_MODE_POINT : polygon vertices are drawn as points
//		rasteriser_create_info.polygonMode = VK_POLYGON_MODE_FILL;
//		rasteriser_create_info.lineWidth = 1.0f;
//		rasteriser_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
//		rasteriser_create_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
//		rasteriser_create_info.depthBiasEnable = VK_FALSE;
//		rasteriser_create_info.depthBiasConstantFactor = 0.0f; // Optional
//		rasteriser_create_info.depthBiasClamp = 0.0f; // Optional
//		rasteriser_create_info.depthBiasSlopeFactor = 0.0f; // Optional
//		
//		//MULTISAMPLING
//#pragma region vk_MULTISAMPLING
//		VkPipelineMultisampleStateCreateInfo multisampling_create_info{};
//		multisampling_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
//		multisampling_create_info.sampleShadingEnable = VK_FALSE;
//		multisampling_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
//		multisampling_create_info.minSampleShading = 1.0f;
//		multisampling_create_info.pSampleMask = nullptr;
//		multisampling_create_info.alphaToCoverageEnable = VK_FALSE;
//		multisampling_create_info.alphaToOneEnable = VK_FALSE;
//#pragma endregion //#vk_MULTISAMPLING
//
//		//DEPTH & STENCIL TESTING
//#pragma region vk_DEPTH_STENCIL_STATE
//		VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info{};
//		depth_stencil_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
//		depth_stencil_state_create_info.depthTestEnable = VK_TRUE;
//		depth_stencil_state_create_info.depthWriteEnable = VK_TRUE;
//		depth_stencil_state_create_info.depthCompareOp = VK_COMPARE_OP_LESS;
//		depth_stencil_state_create_info.depthBoundsTestEnable = VK_FALSE;
//		depth_stencil_state_create_info.minDepthBounds = 0.0f; // Optional
//		depth_stencil_state_create_info.maxDepthBounds = 1.0f; // Optional
//		depth_stencil_state_create_info.stencilTestEnable = VK_FALSE;
//		depth_stencil_state_create_info.front = {}; // Optional
//		depth_stencil_state_create_info.back = {}; // Optional
//#pragma endregion //#vk_DEPTH_STENCIL_STATE
//
//		//COLOUR BLENDING
//#pragma region vk_COLOUR_BLEND_ATTACHMENT
//		VkPipelineColorBlendAttachmentState colour_blend_attachement{};
//		colour_blend_attachement.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
//			VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
//		bool enable_alpha_blend = false;
//		colour_blend_attachement.blendEnable = static_cast<VkBool32>(enable_alpha_blend);
//		if (!enable_alpha_blend)
//		{
//			colour_blend_attachement.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
//			colour_blend_attachement.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
//			colour_blend_attachement.colorBlendOp = VK_BLEND_OP_ADD; // Optional
//			colour_blend_attachement.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
//			colour_blend_attachement.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
//			colour_blend_attachement.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
//		}
//		else
//		{
//			colour_blend_attachement.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
//			colour_blend_attachement.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
//			colour_blend_attachement.colorBlendOp = VK_BLEND_OP_ADD;
//			colour_blend_attachement.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
//			colour_blend_attachement.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
//			colour_blend_attachement.alphaBlendOp = VK_BLEND_OP_ADD;
//		}
//
//		VkPipelineColorBlendStateCreateInfo colour_blending_create_info{};
//		colour_blending_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
//		colour_blending_create_info.logicOpEnable = static_cast<VkBool32>(enable_alpha_blend);
//		if (!enable_alpha_blend)
//			colour_blending_create_info.logicOp = VK_LOGIC_OP_COPY; //optional since its not enabled
//		else
//			colour_blending_create_info.logicOp = VK_LOGIC_OP_COPY;
//		colour_blending_create_info.attachmentCount = 1;
//		colour_blending_create_info.pAttachments = &colour_blend_attachement;
//		colour_blending_create_info.blendConstants[0] = 0.0f; //optional
//		colour_blending_create_info.blendConstants[1] = 0.0f; //optional
//		colour_blending_create_info.blendConstants[2] = 0.0f; //optional
//		colour_blending_create_info.blendConstants[3] = 0.0f; //optional
//#pragma endregion //#vk_COLOUR_BLEND_ATTACHMENT
//		
//
//		//DYNAMIC STATE
//#pragma region vk_DYNAMIC_STATES
//		std::vector<VkDynamicState> dynamic_states = {
//			VK_DYNAMIC_STATE_VIEWPORT,
//			VK_DYNAMIC_STATE_SCISSOR
//		};
//
//		VkPipelineDynamicStateCreateInfo dynamic_state_create_info{};
//		dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
//		dynamic_state_create_info.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
//		dynamic_state_create_info.pDynamicStates = dynamic_states.data();
//#pragma endregion //#vk_DYNAMIC_STATES
//
//
//#pragma region vk_PIPELINE_LAYOUT
//		VkPipelineLayoutCreateInfo pipeline_layout_create_info{};
//		pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
//		pipeline_layout_create_info.setLayoutCount = 1;
//		pipeline_layout_create_info.pSetLayouts = &mDescriptorSetLayout; //<--- ubo decription layout sample MVP UBO 
//		pipeline_layout_create_info.pushConstantRangeCount = 0;
//		pipeline_layout_create_info.pPushConstantRanges = nullptr;
//
//		if (vkCreatePipelineLayout(mDevice.device, &pipeline_layout_create_info, nullptr, &mPipelineLayout) != VK_SUCCESS)
//			throw std::runtime_error("Failed to create pipelines layout!!!!");
//#pragma endregion //#vk_PIPELINE_LAYOUT
//
//#pragma endregion //#vk_FIXED_PIPELINE_FUNCTIONs
//
//		// TODO(Conqster): could later have an abstraction that the the information like 
//		//vertex input state 
//		//shader stages 
//		//input assembly state
//		//multisampling 
//		//etc are function array returns etc
//		VkGraphicsPipelineCreateInfo pipeline_create_info{};
//		pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
//		pipeline_create_info.stageCount = 2;
//		pipeline_create_info.pStages = shader_stages;
//
//		pipeline_create_info.pVertexInputState = &vertex_input_create_info;
//		pipeline_create_info.pInputAssemblyState = &input_assembly_create_info;
//		pipeline_create_info.pViewportState = &viewport_state_create_info;
//		pipeline_create_info.pRasterizationState = &rasteriser_create_info;
//		pipeline_create_info.pMultisampleState = &multisampling_create_info;
//		pipeline_create_info.pDepthStencilState = nullptr;
//		pipeline_create_info.pColorBlendState = &colour_blending_create_info;
//		pipeline_create_info.pDepthStencilState = &depth_stencil_state_create_info;
//		pipeline_create_info.pDynamicState = &dynamic_state_create_info;
//
//		pipeline_create_info.layout = mPipelineLayout;
//		pipeline_create_info.renderPass = mRenderPass;
//		pipeline_create_info.subpass = 0;
//
//		pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
//		pipeline_create_info.basePipelineIndex = -1;
//
//		if (vkCreateGraphicsPipelines(mDevice.device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &mGraphicsPipeline) != VK_SUCCESS)
//			throw std::runtime_error("Failed to Create Graphics Pipeline!!!!!");
//
//		vkDestroyShaderModule(mDevice.device, vert_shader_module, nullptr);
//		vkDestroyShaderModule(mDevice.device, frag_shader_module, nullptr);
//	}
//
//
//	VkShaderModule CreateShaderModule(const std::vector<char>& code) {
//		VkShaderModuleCreateInfo shader_module_create_info{};
//		shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
//		shader_module_create_info.codeSize = code.size();
//		shader_module_create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());
//
//		VkShaderModule shader_module;
//		if (vkCreateShaderModule(mDevice.device, &shader_module_create_info, nullptr, &shader_module) != VK_SUCCESS)
//			throw std::runtime_error("Failed to create shader module!!!!!!");
//		return shader_module;
//	}
//#pragma endregion //#vk_GRAPHICS_PIPELINE
//
//
//#pragma region vk_FRAMEBUFFERS
//	void CreateFramebuffers() {
//		mSwapchainFramebuffers.resize(mSwapchainImgViews.size());
//
//		//create framebuffer for swap chain image views
//		for (size_t i = 0; i < mSwapchainImgViews.size(); i++)
//		{
//
//			std::array<VkImageView, 2> attachments = {
//				mSwapchainImgViews[i],
//				mDepthImageView
//			};
//
//			//std::cout << "Size with Division and first element access: " << tes
//			VkFramebufferCreateInfo framebuffer_create_info{};
//			framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
//			framebuffer_create_info.renderPass = mRenderPass;
//			framebuffer_create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
//			framebuffer_create_info.pAttachments = attachments.data();
//			framebuffer_create_info.width = mSwapchainExtent.width;
//			framebuffer_create_info.height = mSwapchainExtent.height;
//			framebuffer_create_info.layers = 1;
//
//			if (vkCreateFramebuffer(mDevice.device, &framebuffer_create_info, nullptr, &mSwapchainFramebuffers[i]) != VK_SUCCESS)
//				throw std::runtime_error("Failed to create framebuffer!!!!!!");
//		}
//	}
//#pragma endregion
//
//
//#pragma region vk_COMMAND_BUFFERs
//	void CreateCommandPool() {
//		Vulkan::QueueFamilyIndices queue_family_indices = Vulkan::Utils::FindDeviceQueueFamilies(mDevice);
//
//		VkCommandPoolCreateInfo command_pool_create_info{};
//		command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
//		command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
//		command_pool_create_info.queueFamilyIndex = queue_family_indices.graphicsFamily.value();
//
//		if (vkCreateCommandPool(mDevice.device, &command_pool_create_info, nullptr, &mCommandPool) != VK_SUCCESS)
//			throw std::runtime_error("Failed to create command pools!!!!");
//	}
//
//	void CreateCommandBuffers() {
//		mCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
//
//		VkCommandBufferAllocateInfo allocation_info{};
//		allocation_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//		allocation_info.commandPool = mCommandPool;
//		allocation_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//		allocation_info.commandBufferCount = static_cast<uint32_t>(mCommandBuffers.size());
//
//		if (vkAllocateCommandBuffers(mDevice.device, &allocation_info, mCommandBuffers.data()) != VK_SUCCESS)
//			throw std::runtime_error("Failed to allocate command buffers!!!!!!");
//	}
//
//
//	////////////////////////////
//	// Probably have a vk Utility tool file / class as well 
//	// to use as an utility 
//	////////////////////////////
//	VkCommandBuffer BeginSingleTimeCommand() {
//		VkCommandBufferAllocateInfo alloc_info{};
//		alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//		alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//		alloc_info.commandPool = mCommandPool;
//		alloc_info.commandBufferCount = 1;
//
//		VkCommandBuffer command_buffer;
//		vkAllocateCommandBuffers(mDevice.device, &alloc_info, &command_buffer);
//
//		VkCommandBufferBeginInfo command_begin_info{};
//		command_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//		command_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//
//		vkBeginCommandBuffer(command_buffer, &command_begin_info);
//		return command_buffer;
//	}
//
//	void EndSingleTimeCommand(VkCommandBuffer command_buffer) {
//		vkEndCommandBuffer(command_buffer);
//
//		VkSubmitInfo submit_info{};
//		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//		submit_info.commandBufferCount = 1;
//		submit_info.pCommandBuffers = &command_buffer;
//
//		vkQueueSubmit(mDevice.graphicsQueue, 1, &submit_info, VK_NULL_HANDLE);
//		vkQueueWaitIdle(mDevice.graphicsQueue);
//		vkFreeCommandBuffers(mDevice.device, mCommandPool, 1, &command_buffer);
//	}
//
//
//	void RecordCommandBuffer(VkCommandBuffer command_buffer, uint32_t image_idx) {
//		VkCommandBufferBeginInfo begin_info{};
//		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//		begin_info.flags = 0;
//		begin_info.pInheritanceInfo = nullptr;
//
//		/////////////////////////////////////
//		// IMGUI
//		/////////////////////////////////////
//		mImGuiUISystem.StartFrame();
//
//
//		if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS)
//			throw std::runtime_error("Failed to begin recording command buffer!!!!!");
//
//
//		std::array<VkClearValue, 2> clear_values{};
//		clear_values[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
//		//if (image_idx == 1)
//		//	clear_colour = { {{1.0f, 0.0f, 1.0f, 1.0f}} };
//		//else if (image_idx == 2)
//		//	clear_colour = { {{1.0f, 1.0f, 0.0f, 1.0f}} };
//		//clear_colour = { {{1.0f, 0.0f, 1.0f, 1.0f}} };
//		clear_values[1].depthStencil = {1.0f, 0};
//
//		VkRenderPassBeginInfo render_pass_info{};
//		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
//		render_pass_info.renderPass = mRenderPass;
//		render_pass_info.framebuffer = mSwapchainFramebuffers[image_idx];
//		render_pass_info.renderArea.offset = { 0, 0 };
//		render_pass_info.renderArea.extent = mSwapchainExtent;
//		render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
//		render_pass_info.pClearValues = clear_values.data();
//
//		vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
//
//		vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mGraphicsPipeline);
//
//		VkViewport viewport{};
//		viewport.x = 0.0f;
//		viewport.y = 0.0f;
//		viewport.width = static_cast<float>(mSwapchainExtent.width);
//		viewport.height = static_cast<float>(mSwapchainExtent.height);
//		viewport.minDepth = 0.0f;
//		viewport.maxDepth = 1.0f;
//		vkCmdSetViewport(command_buffer, 0, 1, &viewport);
//
//		VkRect2D scissor{};
//		scissor.offset = { 0, 0 };
//		scissor.extent = mSwapchainExtent;
//		vkCmdSetScissor(command_buffer, 0, 1, &scissor);
//
//		//vkCmdDraw(command_buffer, 3, 1, 0, 0);
//		VkBuffer vertex_buffers[] = { mVertexBuffer };
//		VkDeviceSize offsets[] = { 0 };
//		vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);
//		//VkBuffer vertex_buffers[] = { mVertexBuffer, mIndexBuffer };
//		//VkBuffer index_buffers[] = { mIndexBuffer };
//		//VkDeviceSize index_offsets[] = { 0 }; //later size_of vertex size
//		vkCmdBindIndexBuffer(command_buffer, mIndexBuffer, 0, VK_INDEX_TYPE_UINT16);
//
//		vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1, &mDescriptorSets[(mCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT], 0, nullptr);
//		vkCmdDrawIndexed(command_buffer, static_cast<uint32_t>(mIndices.size()), 1, 0, 0, 0);
//
//
//		//vkCmdDraw(command_buffer, static_cast<uint32_t>(mVertices.size()), 1, 0, 0);
//		vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1, &mDescriptorSets[mCurrentFrame], 0, nullptr);
//		vkCmdDrawIndexed(command_buffer, static_cast<uint32_t>(mIndices.size()), 1, 0, 0, 0);
//
//		//vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);
//		//vkCmdBindIndexBuffer(command_buffer, mIndexBuffer, 0, VK_INDEX_TYPE_UINT16);
//		// 
//		// 
//		//UpdateUniformBuffer(mCurrentFrame, glm::vec3(1.0f, 0.0f, 0.0f));
//		//vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);
//		//vkCmdBindIndexBuffer(command_buffer, mIndexBuffer, 0, VK_INDEX_TYPE_UINT16);
//		//vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1, &mDescriptorSets[mCurrentFrame], 0, nullptr);
//		//vkCmdDrawIndexed(command_buffer, static_cast<uint32_t>(mIndices.size()), 1, 0, 0, 0);
//
//
//
//		/////////////////////////////////////
//		// IMGUI
//		/////////////////////////////////////
//		ImGui::ShowDemoWindow();
//		SceneUIWindow();
//		mImGuiUISystem.RenderFrame(command_buffer);
//
//		vkCmdEndRenderPass(command_buffer);
//
//		if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS)
//			throw std::runtime_error("Failed to record command buffer!!!!");
//
//
//
//	}
//#pragma endregion //#vk_COMMAND_BUFFERs
//
//
//#pragma region vk_SEMAPHORE
//	void CreateSyncObjects() {
//		mImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
//		mRenderFinishedSemaphores.resize(mSwapchainImages.size());
//		mInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
//
//		VkSemaphoreCreateInfo semaphore_create_info{};
//		semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
//
//		VkFenceCreateInfo fence_create_info{};
//		fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
//		fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
//
//		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
//		{
//		if (vkCreateSemaphore(mDevice.device, &semaphore_create_info, nullptr, &mImageAvailableSemaphores[i]) != VK_SUCCESS ||
//			vkCreateFence(mDevice.device, &fence_create_info, nullptr, &mInFlightFences[i]) != VK_SUCCESS)
//			throw std::runtime_error("Failed to create semaphore!!!!!!");
//
//		}
//
//		mRenderFinishedSemaphores.resize(mSwapchainImages.size());
//		for(auto& render_finished_semaphore : mRenderFinishedSemaphores)
//			if(vkCreateSemaphore(mDevice.device, &semaphore_create_info, nullptr, &render_finished_semaphore) != VK_SUCCESS)
//				throw std::runtime_error("Failed to create semaphore!!!!!!");
//	}
//#pragma endregion  //#vk_SEMAPHORE
//
//#pragma region vk_ELEMENT_RECONSTRUCTION(SWAPCHAIN/FRAMEBUFFER{COLOUR/DEPTH})
//
//	void WindowResizedRecreateResource() {
//		int width = 0;
//		int height = 0;
//		glfwGetFramebufferSize(mTestWindow.glfwWindowHandle, &width, &height);
//		while (width == 0 || height == 0)
//		{
//			glfwGetFramebufferSize(mTestWindow.glfwWindowHandle, &width, &height);
//			glfwWaitEvents();
//		}
//
//
//		//vkDeviceWaitIdle(mDevice);
//		vkQueueWaitIdle(mDevice.graphicsQueue); //<-- only wait when writing not necessary when present
//
//
//		CleanupSwapchainAndResources();
//
//
//		CreateSwapChain();
//		CreateSwapchainImageViews();
//		CreateDepthResources();
//		CreateFramebuffers();
//
//	}
//
//	void CleanupSwapchainAndResources() {
//
//		vkDestroyImage(mDevice.device, mDepthImage, nullptr);
//		vkFreeMemory(mDevice.device, mDepthImageMemory, nullptr);
//		vkDestroyImageView(mDevice.device, mDepthImageView, nullptr);
//
//
//		for (auto framebuffer : mSwapchainFramebuffers)
//			vkDestroyFramebuffer(mDevice.device, framebuffer, nullptr);
//
//		for (auto image_view : mSwapchainImgViews)
//			vkDestroyImageView(mDevice.device, image_view, nullptr);
//
//		vkDestroySwapchainKHR(mDevice.device, mSwapchain, nullptr);
//	}
//#pragma endregion //#vk_ELEMENT_RECONSTRUCTION(SWAPCHAIN/FRAMEBUFFER{COLOUR/DEPTH})
//
//
//
//#pragma region vk_BUFFER_CREATION
//	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& buffer_memory) {
//		VkBufferCreateInfo buffer_info{};
//		buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//		buffer_info.size = size;
//		buffer_info.usage = usage;
//		buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//
//		std::cout << "\n\n\033[1;32m ======================BUFFER CREATION==========================\033[0m\n";
//		std::cout << "\trequested size: " << size << "\n";
//		std::cout << "\tusage: " << Utils::VK_BufferUsageFlagString(static_cast<VkBufferUsageFlagBits>(usage)) << "\n";
//		std::cout << "\tproperties: " << Utils::VK_MemoryPropertyFlagString(static_cast<VkMemoryPropertyFlagBits>(properties)) << "\n";
//
//		if (vkCreateBuffer(mDevice.device, &buffer_info, nullptr, &buffer) != VK_SUCCESS)
//			throw std::runtime_error("Failed to create vertex buffer!!!!!!");
//
//
//		VkMemoryRequirements mem_requirements;
//		vkGetBufferMemoryRequirements(mDevice.device, buffer, &mem_requirements);
//
//		std::cout << "\t\033[1;33mMemory Requirement for buffer\033[0m\n";
//		std::cout << "\t\t Size: " << mem_requirements.size << "\n";
//		std::cout << "\t\t Alignment: " << mem_requirements.alignment << "\n";
//		std::cout << "\t\t Memory type bits: " << mem_requirements.memoryTypeBits << "\n";
//
//		VkMemoryAllocateInfo alloc_info{};
//		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//		alloc_info.allocationSize = mem_requirements.size;
//		alloc_info.memoryTypeIndex = FindMemoryType(mem_requirements.memoryTypeBits, properties);
//
//		if (vkAllocateMemory(mDevice.device, &alloc_info, nullptr, &buffer_memory) != VK_SUCCESS)
//			throw std::runtime_error("Failed to allocate vertex buffer memeory!!!!!!");
//
//		vkBindBufferMemory(mDevice.device, buffer, buffer_memory, 0);
//	}
//
//
//	void CopyBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size) {
//
//		VkCommandBuffer command_buffer = BeginSingleTimeCommand();
//
//		VkBufferCopy copy_region{};
//		copy_region.srcOffset = 0;
//		copy_region.dstOffset = 0;
//		copy_region.size = size;
//		vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);
//
//		EndSingleTimeCommand(command_buffer);
//	}
//#pragma endregion //#vk_BUFFER_CREATION
//
//
//	uint32_t FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties) {
//
//		VkPhysicalDeviceMemoryProperties mem_properties;
//		vkGetPhysicalDeviceMemoryProperties(mDevice.physicalDevice, &mem_properties);
//
//		for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
//		{
//			if ((type_filter & (1 << i)) && 
//				(mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
//				return i;
//		}
//
//
//		throw std::runtime_error("Failed to find suitable memeory type!!!");
//	}
//
//	void CreateVertexBuffer() {
//
//		VkDeviceSize buffer_size = sizeof(mVertices[0]) * mVertices.size();
//
//		VkBuffer staging_buffer;
//		VkDeviceMemory staging_buffer_memory;
//		CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
//			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
//			staging_buffer, staging_buffer_memory);
//
//		void* data;
//		vkMapMemory(mDevice.device, staging_buffer_memory, 0, buffer_size, 0, &data);
//		memcpy(data, mVertices.data(), static_cast<size_t>(buffer_size));
//		vkUnmapMemory(mDevice.device, staging_buffer_memory);
//
//		CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
//			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
//			mVertexBuffer, mVertexBufferMemory);
//
//		CopyBuffer(staging_buffer, mVertexBuffer, buffer_size);
//
//		vkDestroyBuffer(mDevice.device, staging_buffer, nullptr);
//		vkFreeMemory(mDevice.device, staging_buffer_memory, nullptr);
//	}
//
//	void CreateIndexBuffer() {
//
//		VkDeviceSize buffer_size = sizeof(mIndices[0]) * mIndices.size();
//
//		VkBuffer staging_buffer;
//		VkDeviceMemory staging_buffer_memory;
//		CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
//			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
//			staging_buffer, staging_buffer_memory);
//
//		void* data;
//		vkMapMemory(mDevice.device, staging_buffer_memory, 0, buffer_size, 0, &data);
//		memcpy(data, mIndices.data(), static_cast<size_t>(buffer_size));
//		vkUnmapMemory(mDevice.device, staging_buffer_memory);
//
//		CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
//			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
//			mIndexBuffer, mIndexBufferMemory);
//
//		CopyBuffer(staging_buffer, mIndexBuffer, buffer_size);
//
//		vkDestroyBuffer(mDevice.device, staging_buffer, nullptr);
//		vkFreeMemory(mDevice.device, staging_buffer_memory, nullptr);
//	}
//
//
//#pragma region vk_DESCRIPTION_LAYOUT_AND_UNIFORM_BUFFER_OBJ
//	void CreateDescriptionSetLayout() {
//
//		//to create a ubo this function could take in the binding id
//		VkDescriptorSetLayoutBinding ubo_layout_binding{};
//		ubo_layout_binding.binding = 0;
//		ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//		ubo_layout_binding.descriptorCount = 1;
//		ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
//		ubo_layout_binding.pImmutableSamplers = nullptr;
//
//		VkDescriptorSetLayoutBinding sampler_layout_binding{};
//		sampler_layout_binding.binding = 1;
//		sampler_layout_binding.descriptorCount = 1;
//		sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//		sampler_layout_binding.pImmutableSamplers = nullptr;
//		sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
//
//
//		//group bindings 
//		std::array<VkDescriptorSetLayoutBinding, 2> bindings = { ubo_layout_binding, sampler_layout_binding };
//		VkDescriptorSetLayoutCreateInfo layout_create_info{};
//		layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
//		layout_create_info.bindingCount = static_cast<uint32_t>(bindings.size());
//		layout_create_info.pBindings = bindings.data();
//
//		if (vkCreateDescriptorSetLayout(mDevice.device, &layout_create_info, nullptr, &mDescriptorSetLayout) != VK_SUCCESS)
//			throw std::runtime_error("Failed to create descriptor set layout!!!!!!!!");
//	}
//
//	void CreateUniformBuffers() {
//		VkDeviceSize buffer_size = sizeof(GPUStructures::MVP_UniformBufferObject);
//
//		mUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
//		mUniformBuffersMemories.resize(MAX_FRAMES_IN_FLIGHT);
//		mUniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);
//
//		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
//		{
//			CreateBuffer(buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
//				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
//				mUniformBuffers[i], mUniformBuffersMemories[i]);
//
//			vkMapMemory(mDevice.device, mUniformBuffersMemories[i], 0, buffer_size, 0, &mUniformBuffersMapped[i]);
//		}
//	}
//
//	void CreateDescriptorPool() {
//		//descriptor pool for uniform buffer
//		VkDescriptorPoolSize ubo_pool_size{};
//		ubo_pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//#if IMGUI_IN_USE
//		ubo_pool_size.descriptorCount = 1000;
//#else
//	ubo_pool_size.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
//#endif // IMGUI_IN_USE
//
//		
//		
//		//descriptor pool for image sampler
//		VkDescriptorPoolSize sampler_pool_size{};
//		sampler_pool_size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//#if IMGUI_IN_USE
//		sampler_pool_size.descriptorCount = 1000;
//#else
//		sampler_pool_size.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
//#endif // IMGUI_IN_USE
//
//
//		//group pool size
//		std::array<VkDescriptorPoolSize, 2> grp_pool_sizes = { ubo_pool_size, sampler_pool_size };
//
//		VkDescriptorPoolCreateInfo pool_create_info{};
//		pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
//		pool_create_info.poolSizeCount = static_cast<uint32_t>(grp_pool_sizes.size());
//		pool_create_info.pPoolSizes = grp_pool_sizes.data();
//
//#if IMGUI_IN_USE
//		//when using imgui because during shut down imgui 
//		//would attempt to free descriptor set 
//		pool_create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
//
//		pool_create_info.maxSets = 1000 * grp_pool_sizes.size();
//#else
//		pool_create_info.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
//#endif // IMGUI_IN_USE
//
//
//		if (vkCreateDescriptorPool(mDevice.device, &pool_create_info, nullptr, &mDescriptorPool) != VK_SUCCESS)
//			throw std::runtime_error("Failed to Create DEscriptor Pool!!!!!!!");
//	}
//
//	void CreateDescriptorSets() {
//		std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, mDescriptorSetLayout);
//		VkDescriptorSetAllocateInfo alloc_info{};
//		alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//		alloc_info.descriptorPool = mDescriptorPool;
//		alloc_info.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
//		alloc_info.pSetLayouts = layouts.data();
//
//	
//		mDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
//		if (vkAllocateDescriptorSets(mDevice.device, &alloc_info, mDescriptorSets.data()) != VK_SUCCESS)
//			throw std::runtime_error("Failed to allocate descriptor sets !!!!!!!");
//
//		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
//			
//			//ubo uniformbuffer for mvp uses buffer, 
//			//while the image sampler is transfer from buffer storage to image storage.
//
//			VkDescriptorBufferInfo buffer_info{};
//			buffer_info.buffer = mUniformBuffers[i];
//			buffer_info.offset = 0;
//			buffer_info.range = sizeof(GPUStructures::MVP_UniformBufferObject);
//
//
//			VkDescriptorImageInfo image_info{};
//			image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//			image_info.imageView = mTextureImageView;
//			image_info.sampler = mTextureSampler;
//
//
//			std::array<VkWriteDescriptorSet, 2> descriptor_writes{};
//			descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//			descriptor_writes[0].dstSet = mDescriptorSets[i];
//			descriptor_writes[0].dstBinding = 0;
//			descriptor_writes[0].dstArrayElement = 0;
//			descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//			descriptor_writes[0].descriptorCount = 1;
//			descriptor_writes[0].pBufferInfo = &buffer_info;
//
//			descriptor_writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//			descriptor_writes[1].dstSet = mDescriptorSets[i];
//			descriptor_writes[1].dstBinding = 1;
//			descriptor_writes[1].dstArrayElement = 0;
//			descriptor_writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//			descriptor_writes[1].descriptorCount = 1;
//			descriptor_writes[1].pImageInfo = &image_info;
//
//			vkUpdateDescriptorSets(mDevice.device, static_cast<uint32_t>(descriptor_writes.size()), descriptor_writes.data(), 0, nullptr);
//		}
//	}
//#pragma endregion //#vk_DESCRIPTION_LAYOUT_AND_UNIFORM_BUFFER_OBJ
//
//
//#pragma region vk_IMAGE_RESOURCE_CREATION
//	void CreateTextureImage(VkFormat format) {
//		int tex_width; 
//		int tex_height;
//		int tex_channels;
//
//		int required_channels = static_cast<int>(STBI_rgb_alpha);
//		stbi_uc* pixels = stbi_load("textures/Manchester_512.png", &tex_width, &tex_height, &tex_channels, required_channels);
//		VkDeviceSize image_size = static_cast<VkDeviceSize>(tex_width) * 
//								  static_cast<VkDeviceSize>(tex_height) * 
//								  static_cast<VkDeviceSize>(required_channels);
//
//		if (!pixels)
//			throw std::runtime_error("Failed to load texture image !!!!!!");
//
//		VkBuffer tex_staging_buffer;
//		VkDeviceMemory tex_staging_buffer_memory;
//
//		CreateBuffer(image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
//			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
//			tex_staging_buffer, tex_staging_buffer_memory);
//
//		void* data;
//		vkMapMemory(mDevice.device, tex_staging_buffer_memory, 0, image_size, 0, &data);
//		memcpy(data, pixels, static_cast<size_t>(image_size));
//		vkUnmapMemory(mDevice.device, tex_staging_buffer_memory);
//
//		stbi_image_free(pixels);
//
//
//
//		CreateImage(static_cast<uint32_t>(tex_width),
//			static_cast<uint32_t>(tex_height),
//			format,
//			VK_IMAGE_TILING_OPTIMAL,
//			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
//			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
//			mTextureImage,
//			mTextureImageMemory);
//
//
//		TransitionImageLayout(mTextureImage,
//			format,
//			VK_IMAGE_LAYOUT_UNDEFINED,
//			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
//
//		CopyBufferToImage(tex_staging_buffer,
//			mTextureImage,
//			static_cast<uint32_t>(tex_width),
//			static_cast<uint32_t>(tex_height));
//
//		TransitionImageLayout(mTextureImage,
//			format,
//			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
//			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
//
//
//		vkDestroyBuffer(mDevice.device, tex_staging_buffer, nullptr);
//		vkFreeMemory(mDevice.device, tex_staging_buffer_memory, nullptr);
//	}
//
//	////////////////////////////
//	// Probably have a vk Utility Resource file / class 
//	// to generate resource like this 
//	////////////////////////////
//	VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspect_flags) {
//		VkImageViewCreateInfo img_view_create_info{};
//		img_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
//		img_view_create_info.image = image;
//
//		img_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
//		img_view_create_info.format = format;
//
//		img_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
//		img_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
//		img_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
//		img_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
//
//		img_view_create_info.subresourceRange.aspectMask = aspect_flags;
//		img_view_create_info.subresourceRange.baseMipLevel = 0;
//		img_view_create_info.subresourceRange.levelCount = 1;
//		img_view_create_info.subresourceRange.baseArrayLayer = 0;
//		img_view_create_info.subresourceRange.layerCount = 1;
//
//		VkImageView temp_image_view;
//		if (vkCreateImageView(mDevice.device, &img_view_create_info, nullptr, &temp_image_view) != VK_SUCCESS)
//			throw std::runtime_error("Failed to create image view!!");
//
//		return temp_image_view;
//	}
//
//	void CreateTextureImageView(VkFormat format) {
//
//		mTextureImageView = CreateImageView(mTextureImage, format, VK_IMAGE_ASPECT_COLOR_BIT);
//	}
//
//	VkSampler CreateTextureSampler() {
//		//later pass in unique parameters 
//		//like 
//		//filters 
//		//repeat mode
//		//border colour etc
//		//properties like 
//		//max anisotropy is determined by support 
//		VkSamplerCreateInfo sampler_create_info{};
//		sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
//		sampler_create_info.magFilter = VK_FILTER_LINEAR;
//		sampler_create_info.minFilter = VK_FILTER_LINEAR;
//		sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
//		sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
//		sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
//
//		//could have this cache during load time 
//		//to use for data recration when need without the need 
//		//for not always having to get physical device multiple time during load 
//		VkPhysicalDeviceProperties retrived_physical_device_prop{};
//		vkGetPhysicalDeviceProperties(mDevice.physicalDevice, &retrived_physical_device_prop);
//
//		sampler_create_info.anisotropyEnable = static_cast<VkBool32>(mUseSamplingAnisotropy);
//		sampler_create_info.maxAnisotropy = (mUseSamplingAnisotropy) ? retrived_physical_device_prop.limits.maxSamplerAnisotropy : 1.0f;
//		sampler_create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
//		sampler_create_info.unnormalizedCoordinates = VK_FALSE;
//		sampler_create_info.compareEnable = VK_FALSE;
//		sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS;
//
//		sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
//		sampler_create_info.mipLodBias = 0.0f;
//		sampler_create_info.minLod = 0.0f;
//		sampler_create_info.maxLod = 0.0f;
//
//		VkSampler temp_sampler;
//		if (vkCreateSampler(mDevice.device, &sampler_create_info, nullptr, &temp_sampler) != VK_SUCCESS)
//			throw std::runtime_error("Failed to Create TExture Sampler!!!!!!!");
//
//		return temp_sampler;
//	}
//
//	void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, 
//		VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& image_memory) {
//
//		VkImageCreateInfo image_create_info{};
//		image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
//		image_create_info.imageType = VK_IMAGE_TYPE_2D;
//		image_create_info.extent.width = width;
//		image_create_info.extent.height = height;
//		image_create_info.extent.depth = 1;
//		image_create_info.mipLevels = 1;
//		image_create_info.arrayLayers = 1;
//		image_create_info.format = format;
//		image_create_info.tiling = tiling;
//		image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//		image_create_info.usage = usage;
//		image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//		image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
//		image_create_info.flags = 0;
//
//		if (vkCreateImage(mDevice.device, &image_create_info, nullptr, &image) != VK_SUCCESS)
//			throw std::runtime_error("Failed to Create Image!!!!!!");
//
//		VkMemoryRequirements memRequirements;
//		vkGetImageMemoryRequirements(mDevice.device, image, &memRequirements);
//
//		VkMemoryAllocateInfo alloc_info{};
//		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//		alloc_info.allocationSize = memRequirements.size;
//		alloc_info.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);
//
//		if (vkAllocateMemory(mDevice.device, &alloc_info, nullptr, &image_memory) != VK_SUCCESS) {
//			throw std::runtime_error("failed to allocate image memory!");
//		}
//
//
//		vkBindImageMemory(mDevice.device, image, image_memory, 0);
//	}
//
//
//	void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout) {
//		VkCommandBuffer command_buffer = BeginSingleTimeCommand();
//
//		VkPipelineStageFlags source_stage;
//		VkPipelineStageFlags destination_stage;
//
//		VkImageMemoryBarrier memory_barrier{};
//		memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
//		memory_barrier.oldLayout = old_layout; //VK_IMAGE_LAYOUT_UNDEFINED is we do not care about the image existing content
//		memory_barrier.newLayout = new_layout;
//		memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//		memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//		memory_barrier.image = image;
//
//		//sub resources
//		if (new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
//		{
//			memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
//			if (HasStencilComponent(format))
//				memory_barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
//		}
//		else
//			memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//		memory_barrier.subresourceRange.baseMipLevel = 0;
//		memory_barrier.subresourceRange.levelCount = 1;
//		memory_barrier.subresourceRange.baseArrayLayer = 0;
//		memory_barrier.subresourceRange.layerCount = 1;
//
//		//old layout undefinied/irrelevant 
//		//trying to writing to writing to transfer buffer
//		if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
//		{
//			memory_barrier.srcAccessMask = 0; //<-- Specifies the  type of access that must happen-before the barrier
//			memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT; //<-- Specifies the  type of access that must wait until-after the barrier
//			
//			source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//			destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//		}
//		//if old layout used as transfer destination 
//		else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
//		{
//			memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//			memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//			
//			source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//			destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//		}
//		else if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
//		{
//			memory_barrier.srcAccessMask = 0;
//			memory_barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
//
//			source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//			destination_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
//		}
//		else
//			throw std::invalid_argument("Unsupported image layout transition!!!!!!!!!");
//
//		vkCmdPipelineBarrier(
//			command_buffer,
//			source_stage, destination_stage,
//			0,
//			0, nullptr,
//			0, nullptr,
//			1, &memory_barrier
//		);
//
//		EndSingleTimeCommand(command_buffer);
//	}
//
//	void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
//		VkCommandBuffer command_buffer = BeginSingleTimeCommand();
//
//		VkBufferImageCopy region{};
//		region.bufferOffset = 0; 
//		region.bufferRowLength = 0; 
//		region.bufferImageHeight = 0;
//		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//		region.imageSubresource.mipLevel = 0;
//		region.imageSubresource.baseArrayLayer = 0; 
//		region.imageSubresource.layerCount = 1;
//
//		region.imageOffset = { 0,0,0 };
//		region.imageExtent = {
//			width,
//			height,
//			1
//		};
//
//		vkCmdCopyBufferToImage(
//			command_buffer,
//			buffer,
//			image,
//			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
//			1,
//			&region
//		);
//
//
//
//		EndSingleTimeCommand(command_buffer);
//	}
//#pragma endregion //# vk_IMAGE_RESOURCE_CREATION
//
//#pragma region vk_DEPTH_RESOURCES_CREATION
//	/////////////////////////////////
//	// when have a vk Utility tool file /class 
//	// this function could be added
//	/////////////////////////////////
//	VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling req_img_tiling, VkFormatFeatureFlags required_features) {
//
//		for (auto format : candidates)
//		{
//			VkFormatProperties props;
//			vkGetPhysicalDeviceFormatProperties(mDevice.physicalDevice, format, &props);
//
//			if (req_img_tiling == VK_IMAGE_TILING_LINEAR &&
//				(props.linearTilingFeatures & required_features) == required_features)
//				return format;
//			else if (req_img_tiling == VK_IMAGE_TILING_OPTIMAL &&
//				(props.optimalTilingFeatures & required_features) == required_features)
//				return format;
//		}
//
//		throw std::runtime_error("Failed to find supported format!!!!!!");
//	}
//
//	/////////////////////////////////
//	// when have a vk Utility tool file /class 
//	// this function could be added
//	/////////////////////////////////
//	VkFormat  FindDepthFormat() {
//		return FindSupportedFormat(
//			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, //<<-- CANDIDATES FORMAT FOR DEPTH TESTING
//			VK_IMAGE_TILING_OPTIMAL,
//			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
//		);
//	}
//
//	/////////////////////////////////
//	// when have a vk Utility tool file /class 
//	// this function could be added
//	/////////////////////////////////
//	bool HasStencilComponent(VkFormat format) {
//		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
//	}
//
//	void CreateDepthResources() {
//		VkFormat depth_format = FindDepthFormat();
//
//
//		/////////////////////////////////
//		// when have a vk Resources tool file /class 
//		// this function could be added
//		/////////////////////////////////
//		CreateImage(mSwapchainExtent.width, mSwapchainExtent.height,
//			depth_format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
//			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mDepthImage, mDepthImageMemory);
//
//		mDepthImageView = CreateImageView(mDepthImage, depth_format, VK_IMAGE_ASPECT_DEPTH_BIT);
//
//
//		//THE TRANSITION DONT NEED TO BE EXPLICITY 
//		TransitionImageLayout(mDepthImage, depth_format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
//	}
//
//#pragma endregion //#vk_DEPTH_RESOURCES_CREATION
//
//
//
//
//	uint32_t mSubpassPipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
//	void InitSystems() {
//
//		mCamera.SetLookTarget(glm::vec3(0.0f, 0.0f, 0.5f));
//
//		//TODO (Conqster): Now that device is a structure of vulkann info, i can pass device to imgui
//		//
//		Vulkan_Engine_Prop vk_imgui_props{};
//		vk_imgui_props.vkInstance = mDevice.instance;
//		vk_imgui_props.physicalDevice = mDevice.physicalDevice;
//		vk_imgui_props.device = mDevice.device;
//		Vulkan::QueueFamilyIndices queue_families = Vulkan::Utils::FindDeviceQueueFamilies(mDevice);
//		if (!queue_families.graphicsFamily.has_value())
//			throw std::runtime_error("Faild to find graphics queue fgamily for ImGui!!!!!!");
//		vk_imgui_props.graphicsQueueFamily = queue_families.graphicsFamily.value();
//		vk_imgui_props.graphicsQueue = mDevice.graphicsQueue;
//		vk_imgui_props.descriptorPool = mDescriptorPool;
//		SwapChainSupportDetails swap_chain_support = QuerySwapChainSupport(mDevice.physicalDevice, mDevice.surface);
//		// +1 
//		//because the min means we might sometimes have to wait on the driver to complete
//		//internal operations before we can acquire another imgae to render
//		vk_imgui_props.minImageCount = swap_chain_support.capabilities.minImageCount + 1;
//		vk_imgui_props.imageCount = mSwapchainImages.size();
//		vk_imgui_props.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
//		vk_imgui_props.subpass = mSubpassPipelineBindPoint;
//		vk_imgui_props.renderPass = mRenderPass;
//
//		mImGuiUISystem.Init(mTestWindow.glfwWindowHandle, vk_imgui_props);
//
//	}
//
//
//
//	void SceneUIWindow() {
//		
//		ImGui::Begin("Test Scene Modification Window");
//
//		ImGui::DragFloat3("Model Position", &mModelPos[0], 0.01f);
//
//		ImGui::Spacing();
//		bool window_cursor_lock = Platform::IsCursorLocked(&mTestWindow);
//		ImGui::Checkbox("Mouse Cursor", &window_cursor_lock);
//		ImGui::Spacing();
//
//		ImGui::SeparatorText("Camera Properties");
//		auto cam_pos = mCamera.position;
//		ImGui::Text("Position x: %f, y: %f, z: %f", cam_pos.x, cam_pos.y, cam_pos.z);
//		auto cam_forward = mCamera.forward;
//		ImGui::Text("Forward x: %f, y: %f, z: %f", cam_forward.x, cam_forward.y, cam_forward.z);
//		ImGui::SliderFloat("Move Speed", &mCamera.moveSpeed, 0.01f, 10.0f);
//		ImGui::SliderFloat("Rot Speed", &mCamera.rotSpeed, 0.01f, 1.0f);
//		ImGui::SliderFloat("FOV", &mCamera.fov, 0.0f, 179.0f, "%.1f");
//		ImGui::DragFloat("Near", &mCamera.nearPlane, 0.1f, 0.01f, 50.0f, "%.1f");
//		ImGui::DragFloat("Far", &mCamera.farPlane, 0.1f, 0.0f, 500.0f, "%.1f");
//
//		ImGui::End();
//	}
//
//
//	void TranslateCamera(InputSystem::Input& input_sys_inst, Camera& cam, float delta_sec, glm::vec3 translate_by = glm::vec3(1.0f)) {
//
//		if (mImGuiUISystem.UserIsHoveringWindow() && !Platform::IsCursorLocked(&mTestWindow))
//			return;
//
//		if (input_sys_inst.GetKeyDown(GLFW_MOUSE_BUTTON_LEFT))
//			Platform::ToggleCursor(&mTestWindow);
//
//		if(Platform::IsCursorLocked(&mTestWindow))
//			cam.RotateByMouseMovement(input_sys_inst.GetMouseAxisFloatDt(0), input_sys_inst.GetMouseAxisFloatDt(1));
//
//		glm::vec3 displacement = glm::vec3(0.0f);
//
//		if (input_sys_inst.GetKey(GLFW_KEY_W))
//			cam.Translate(cam.forward, delta_sec);
//		if (input_sys_inst.GetKey(GLFW_KEY_S))
//			cam.Translate(-cam.forward, delta_sec);
//
//		if (input_sys_inst.GetKey(GLFW_KEY_A))
//			cam.Translate(-cam.right, delta_sec);
//		if (input_sys_inst.GetKey(GLFW_KEY_D))
//			cam.Translate(cam.right, delta_sec);
//
//
//		if (input_sys_inst.GetKey(GLFW_KEY_Q))
//			cam.Translate(cam.up, delta_sec);
//		if (input_sys_inst.GetKey(GLFW_KEY_E))
//			cam.Translate(-cam.up, delta_sec);
//
//	}
//
//
//};
//
//
//
//
//
//
//int main()
//{
//	HelloTriangleApp app;
//
//	try {
//		app.Run();
//	}
//	catch (const std::exception& e) {
//		std::cerr << e.what() << std::endl;
//		return EXIT_FAILURE;
//	}
//	return EXIT_SUCCESS;
//}