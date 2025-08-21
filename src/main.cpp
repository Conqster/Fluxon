#include "Core/Window.h"
#include "Core/InputSystem.h"
#include "Core/Camera.h"
#include "Core/UISystem.h"

#include "Renderer/GPUStrutures.h"
#include "Renderer/Graphics2.h"

#define USE_NEW_SCENE_RUNTIME 1

#if USE_NEW_SCENE_RUNTIME
#include "Core/SceneRuntime.h"
#endif // USE_NEW_SCENE_RUNTIME


//#include <chrono>
//#include <thread>

#define IMGUI_IN_USE 1

class Application
{
public:
	void Run() {
		Init();
		MainLoop();
		CleanUp();
	}
private:
	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;

	
	std::array<Fluxon::GPUStructures::Model, 2> mModelDatas;
	Fluxon::EditorCamera mCamera;
	UI_System mImGuiUISystem;

	Fluxon::Platform::Window mWindow;

	glm::vec3 mTranslateModelBy = glm::vec3(0.0f);
	glm::vec3 mModelRotateAxis = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::mat4 mModelMatrix = glm::mat4(1.0f);

#if USE_NEW_SCENE_RUNTIME
	Fluxon::SceneRuntime mSceneRuntime;
#else
	Fluxon::Vulkan::Renderer mRenderer;
#endif // USE_NEW_SCENE_RUNTIME

	uint32_t mCurrentFrame = 0;

	double mCurrFrameDT;
	int mFrameRate = 0;


	int mUI_is_capturing_cursor = false;


	void Init() {


		mWindow.width = 1920;
		mWindow.height = 1080;
		Fluxon::Platform::InitialiseGLFWWindow(mWindow);
		//mWindow = mTestWindow.glfwWindowHandle;
		//glfwSetKeyCallback(mWindow, KeyboardInputCallback);
		Fluxon::InputSystem::EventHandler::Instance().CreateCallbacks(mWindow.glfwWindowHandle);

#if USE_NEW_SCENE_RUNTIME
		mSceneRuntime.Init(mWindow, &mCamera);
		InitSystems(mSceneRuntime.GetVulkanContext(), mSceneRuntime.GetFrames()[0].globalDescriptorPool, mSceneRuntime.GetScene().GetForwardRenderPass()->GetRenderPassHandle());
		mSceneRuntime.SetIMGUI(mImGuiUISystem);
#else
		mRenderer.Init(mWindow, &mCamera);
		InitSystems(mRenderer.GetVulkanContext(), mRenderer.GetFrames()[0].globalDescriptorPool, mRenderer.GetForwardRenderPass().GetRenderPassHandle());
#endif // USE_NEW_SCENE_RUNTIME

	}


	void MainLoop() {

		//glfwGetTime returns time in seconds
		double last_frame_time = glfwGetTime();/// *1000.0f;
		//while (!glfwWindowShouldClose(mWindow)) {
		while (!Fluxon::Platform::ShouldWindowClose(&mWindow))
		{

			//glfwGetTime returns time in seconds
			double curr_frame_time = glfwGetTime();// *1000.0f;
			mCurrFrameDT = (curr_frame_time - last_frame_time) * 1000.0f;
			last_frame_time = curr_frame_time;

			glfwPollEvents();

#if USE_NEW_SCENE_RUNTIME
			mSceneRuntime.Update(GetFrameTimeInSeconds(), &mWindow, &mCamera);
			//mSceneRuntime.Update(mCurrFrameDT, &mWindow, &mCamera);
#else
			mRenderer.DrawScene(&mCamera, mTranslateModelBy, mModelRotateAxis);
#endif // USE_NEW_SCENE_RUNTIME


			ImGuiIO ui_io = ImGui::GetIO();
			//make window capture mouse 
			if (mImGuiUISystem.UserIsHoveringWindow() && ui_io.MouseDown[0])
			{
				Fluxon::Platform::LockCursor(&mWindow);
				mUI_is_capturing_cursor = true;
			}
			else
			{
				//Fluxon::Platform::UnlockCursor(&mWindow);
				mUI_is_capturing_cursor = false;
				//mMouseCursorCaputedInCurrentFrame = false;
			}


			mTranslateModelBy = glm::vec3(0.0f);

			//std::this_thread::sleep_for(std::chrono::milliseconds(100));
			//Fluxon::InputSystem::Input().
			auto input_sys_inst = Fluxon::InputSystem::Input();


			if (input_sys_inst.GetKey(Fluxon::InputSystem::IKeyCode::UpArrow))
				mTranslateModelBy.z += 2.5f * GetFrameTimeInSeconds();
			if (input_sys_inst.GetKey(Fluxon::InputSystem::IKeyCode::DownArrow))
				mTranslateModelBy.z -= 2.5f * GetFrameTimeInSeconds();

			if (input_sys_inst.GetKey(Fluxon::InputSystem::IKeyCode::LeftArrow))
				mTranslateModelBy.x -= 2.5f * GetFrameTimeInSeconds();
			if (input_sys_inst.GetKey(Fluxon::InputSystem::IKeyCode::RightArrow))
				mTranslateModelBy.x += 2.5f * GetFrameTimeInSeconds();



			TranslateCamera(input_sys_inst, mCamera, GetFrameTimeInSeconds(), glm::vec3(2.5f));



			static float rot_increament = 90.0f;
			if (input_sys_inst.GetKeyDown(Fluxon::InputSystem::IKeyCode::Z))
				rot_increament += 10.0f;
			else if (input_sys_inst.GetKeyDown(Fluxon::InputSystem::IKeyCode::X))
				rot_increament -= 10.0f;
			rot_increament = glm::clamp(rot_increament, 0.0f, 720.0f);

			//update the and store rotation in rotateAxis
			//converted to radian when use
			//mModelRotateAxis = glm::vec3(0.0f, 0.0f, 45.0f);
			float axis_rotation = glm::length(mModelRotateAxis);
			axis_rotation = rot_increament * GetFrameTimeInSeconds();
			axis_rotation = (axis_rotation > 360) ? axis_rotation - 360 : axis_rotation;
			mModelRotateAxis = glm::normalize(mModelRotateAxis) * axis_rotation;
			//if (axis_rotation > 360)
				//axis_rotation = axis_rotation - 360;

			static int axis = 0;
			if (input_sys_inst.GetKeyDown(Fluxon::InputSystem::IKeyCode::Space))
			{
				axis = (axis + 1) % 3;
				if (axis == 0)
					mModelRotateAxis = glm::vec3(0.0f, 0.0f, axis_rotation);
				if (axis == 1)
					mModelRotateAxis = glm::vec3(axis_rotation, 0.0f, 0.0f);
				if (axis == 2)
					mModelRotateAxis = glm::vec3(0.0f, axis_rotation, 0.0f);
			}


			//Update GLFW window title with frame times
			static float display_title_update = 0;
			display_title_update += GetFrameTimeInSeconds();
			if (display_title_update >= 0.5f)
			{
				static char window_title[64];
				mFrameRate = 1000.0f / mCurrFrameDT;
				//snprintf(window_title, sizeof(window_title), "Fluxon Engine Editor {Vulkan API} - %.3fms, %d FPS. Current Axis rot: %.1f", mCurrFrameDT, mFrameRate, axis_rotation);
				snprintf(window_title, sizeof(window_title), "Fluxon Engine {Vulkan API} - %.3fms, %d FPS.", mCurrFrameDT, mFrameRate);
				Fluxon::Platform::ChangeWindowTitle(&mWindow, window_title);
				display_title_update = 0;
			}

			Fluxon::Platform::FlushWindow(&mWindow);
			Fluxon::InputSystem::EventHandler::Instance().FlushFrameInputs();
		}
	}
	void CleanUp() {

		//vkDeviceWaitIdle(mDevice.device);
#if USE_NEW_SCENE_RUNTIME
		mSceneRuntime.Cleanup();
#else
		mRenderer.Cleanup();
#endif // USE_NEW_SCENE_RUNTIME


		Fluxon::Platform::DestroyWindow(&mWindow);
	}


	inline float GetFrameTimeInSeconds() {
		return mCurrFrameDT * 0.001f;
	}



	uint32_t mSubpassPipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	void InitSystems(Fluxon::Vulkan::VulkanContext& d, VkDescriptorPool desc_pool, VkRenderPass render_pass) {

		//mCamera.SetLookTarget(glm::vec3(0.0f, 0.0f, 0.5f));

		//TODO (Conqster): Now that device is a structure of vulkann info, i can pass device to imgui
		//auto& d = renderer.GetDeprecatedDevice();
		Vulkan_Engine_Prop vk_imgui_props{};
		vk_imgui_props.vkInstance = d.instance;
		vk_imgui_props.physicalDevice = d.physicalDevice;
		vk_imgui_props.device = d.device;
		Fluxon::Vulkan::QueueFamilyIndices queue_families = Fluxon::Vulkan::Utils::FindDeviceQueueFamilies(d);
		if (!queue_families.graphicsAndComputeFamily.has_value())
			throw std::runtime_error("Faild to find graphics queue fgamily for ImGui!!!!!!");
		vk_imgui_props.graphicsQueueFamily = queue_families.graphicsAndComputeFamily.value();
		vk_imgui_props.graphicsQueue = d.graphicsQueue;
		//vk_imgui_props.descriptorPool = mDescriptorPool;
		vk_imgui_props.descriptorPool = desc_pool;
		Fluxon::Vulkan::SwapchainSupportDetails swap_chain_support = Fluxon::Vulkan::Utils::QuerySwapchainSupport(d.physicalDevice, d.surface);
		// +1 
		//because the min means we might sometimes have to wait on the driver to complete
		//internal operations before we can acquire another imgae to render
		vk_imgui_props.minImageCount = swap_chain_support.capabilities.minImageCount + 1;
		//vk_imgui_props.imageCount = mSwapchainImages.size();
		vk_imgui_props.imageCount = d.swapchain->GetImages().size();
		vk_imgui_props.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		vk_imgui_props.subpass = mSubpassPipelineBindPoint;
		vk_imgui_props.renderPass = render_pass;

		mImGuiUISystem.Init(mWindow.glfwWindowHandle, vk_imgui_props);


#if USE_NEW_SCENE_RUNTIME
#else
		mRenderer.SetIMGUI(mImGuiUISystem);
#endif // USE_NEW_SCENE_RUNTIME

	}





	void TranslateCamera(Fluxon::InputSystem::Input& input_sys_inst, Fluxon::EditorCamera& cam, float delta_sec, glm::vec3 translate_by = glm::vec3(1.0f)) {

		
		if (Fluxon::Platform::IsCursorLocked(&mWindow))
			return;

	

		if (input_sys_inst.GetKey(Fluxon::InputSystem::IKeyCode::MouseRightButton)) //Fluxon::Platform::ToggleCursor(&mWindow);
		{
			Fluxon::Platform::LockCursor(&mWindow);

			cam.RotateByMouseMovement(input_sys_inst.GetMouseAxisFloat(Fluxon::InputSystem::IAxis::Vertical), input_sys_inst.GetMouseAxisFloat(Fluxon::InputSystem::IAxis::Horizontal));

			glm::vec3 displacement = glm::vec3(0.0f);

			if (input_sys_inst.GetKey(Fluxon::InputSystem::IKeyCode::W))
				cam.Translate(cam.GetForward(), delta_sec);
			if (input_sys_inst.GetKey(Fluxon::InputSystem::IKeyCode::S))
				cam.Translate(-cam.GetForward(), delta_sec);

			if (input_sys_inst.GetKey(Fluxon::InputSystem::IKeyCode::A))
				cam.Translate(-cam.GetRight(), delta_sec);
			if (input_sys_inst.GetKey(Fluxon::InputSystem::IKeyCode::D))
				cam.Translate(cam.GetRight(), delta_sec);


			if (input_sys_inst.GetKey(Fluxon::InputSystem::IKeyCode::Q))
				cam.Translate(cam.GetUp(), delta_sec);
			if (input_sys_inst.GetKey(Fluxon::InputSystem::IKeyCode::E))
				cam.Translate(-cam.GetUp(), delta_sec);

		}

	}


};






int main()
{

	//hack for now 
	Application app;


	try {
		app.Run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}