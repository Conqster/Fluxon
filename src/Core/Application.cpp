#include "Application.h"

#include "InputSystem.h"

#include "Editor/MainSceneEditor.h"

#include "Renderer/Swapchain.h"


namespace Fluxon {

	Fluxon::Application* Application::sAppInstance = nullptr;

	Application::Application()
	{
		sAppInstance = this;
		Init();
	}

	void Application::Init()
	{
		mWindow.width = 1920;
		mWindow.height = 1080;
		Fluxon::Platform::InitialiseGLFWWindow(mWindow);
		//mWindow = mTestWindow.glfwWindowHandle;
		//glfwSetKeyCallback(mWindow, KeyboardInputCallback);
		InputSystem::EventHandler::Instance().CreateCallbacks(mWindow.glfwWindowHandle);



		//Graphics context 
		Vulkan::VulkanContextSpecification vk_ctx_spec{};
		vk_ctx_spec.bEnableFragmentStoresAndAtomics = true;
		vk_ctx_spec.debug_callback = Vulkan::Utils::DebugCallback;
		mGraphicsContext = Vulkan::VulkanContext::CreateVulkanContext(&mWindow, vk_ctx_spec);

		mGraphicsContext.swapchain = std::make_unique<Vulkan::Swapchain>(&mGraphicsContext, mWindow);

		//mSceneManager.SetIMGUI(/*mUIManager*/);
		mUIManager.Init(&mGraphicsContext, mWindow.glfwWindowHandle, {});

		mSceneManager.Init(&mGraphicsContext, &mCamera);
		mEditorPanel.RegisterOnViewportResize([&](uint32 w, uint32 h) {
			mSceneManager.OnViewportResize(w, h);
			});

		

		mSceneManager.mOnUIRenderCallback = [&](const Vulkan::VulkanContext* vk_ctx, VkCommandBuffer cmd_buff, uint32 img_idx) {
			mUIManager.mRenderer.RenderFrame(vk_ctx, cmd_buff, img_idx);
		};

		mSceneManager.mOnSwapchainUICallback = [&](const Vulkan::VulkanContext* vk_ctx, uint32 w, uint32 h) {
			mUIManager.mRenderer.OnWindowResize(vk_ctx, w, h);
		};

	}

	void Application::Update()
	{
		//glfwGetTime returns time in seconds
		double last_frame_time = glfwGetTime();/// *1000.0f;
		//while (!glfwWindowShouldClose(mWindow)) {
		while (!Platform::ShouldWindowClose(&mWindow))
		{

			if (!mWindow.IsWindowMinimised())
			{
				//glfwGetTime returns time in seconds
				double curr_frame_time = glfwGetTime();// *1000.0f;
				mCurrFrameDT = (curr_frame_time - last_frame_time) * 1000.0f;
				last_frame_time = curr_frame_time;

				mWindow.OnUpdate();


				//all ui stuffs need to be called before scene manager update 
				//as rendering is done in scenee manager 
				mUIManager.StartFrame();
				auto& scene_reso = mSceneManager.GetScene().mSceneResources;
				mEditorPanel.RenderUI(scene_reso);
				Editor::SceneRuntimeUIWindow(scene_reso);
				ImGui::ShowDemoWindow();

				mSceneManager.Update(GetFrameTimeInSeconds(), &mWindow, &mCamera);


				ImGuiIO ui_io = ImGui::GetIO();
				//make window capture mouse 
				if (mUIManager.UserIsHoveringWindow() && ui_io.MouseDown[0])
					Fluxon::Platform::LockCursor(&mWindow);


				//std::this_thread::sleep_for(std::chrono::milliseconds(100));


				LaterHaveCameraControllerTranslateCamera(mCamera, GetFrameTimeInSeconds());




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

			}
			else
				mWindow.OnUpdate();

			mEditorPanel.CheckViewportResize();
			Fluxon::Platform::FlushWindow(&mWindow);
			Fluxon::InputSystem::EventHandler::Instance().FlushFrameInputs();
		}
	}

	void Application::CleanUp()
	{
		vkDeviceWaitIdle(mGraphicsContext.device);

		mSceneManager.PreCleanup();

		mUIManager.mRenderer.Cleanup(&mGraphicsContext);
		mUIManager.Shutdown();

		//quick hack
		if(mUIManager.mRenderer.mDescriptorPool != VK_NULL_HANDLE)
			vkDestroyDescriptorPool(mGraphicsContext.device, mUIManager.mRenderer.mDescriptorPool, nullptr);

		mSceneManager.Cleanup();

		Platform::DestroyWindow(&mWindow);
	}

	void Application::LaterHaveCameraControllerTranslateCamera(EditorCamera& cam, float delta_sec)
	{

		if (Platform::IsCursorLocked(&mWindow))
			return;


		//Fluxon::InputSystem::Input().
		auto input_sys_inst = Fluxon::InputSystem::Input();

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

} //namespace Fluxon