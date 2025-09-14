#pragma once

#include "Window.h"
#include "Core/UISystem.h"
#include "Core/SceneRuntime.h"

#include "Renderer/VulkanContext.h"


int main();


namespace Fluxon {


	class Application
	{
	public:
		Application();
		~Application() {

		}

		void Init();


		void CleanUp();

		inline static Application* Get() { return sAppInstance; }
	private:


		void Launch() { //<refleching mapler
			Update();
		}

		void Update();


		Platform::Window mWindow;
		SceneRuntime mSceneManager;
		UI_System mUIManager;
		UI::EditorUI mEditorPanel;

		uint32_t mCurrentFrame = 0;


		//deparcated
		Vulkan::VulkanContext mGraphicsContext;
		EditorCamera mCamera;
		double mCurrFrameDT;
		int mFrameRate = 0;

		inline float GetFrameTimeInSeconds() {
			return mCurrFrameDT * 0.001f;
		}


		void LaterHaveCameraControllerTranslateCamera(EditorCamera& cam, float delta_sec);
	
		friend int ::main();
		static Application* sAppInstance;
	};

	Application* CreateApplication();

} //namespace Fluxon