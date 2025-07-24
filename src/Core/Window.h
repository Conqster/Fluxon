#pragma once
////To access native plaform functions 
////to ccreate glfw window surface 
//#define VK_USE_Platform_WIN32_KHR
//#define GLFW_INCLUDE_VULKAN

//#define GLFW_EXPOSE_NATIVE_WIN32
//#include <GLFW/glfw3native.h>

#include <GLFW/glfw3.h>
#include <stdint.h>

#include <string>


namespace Fluxon {
	namespace Platform {

		struct Window
		{
			uint32_t width = 800;
			uint32_t height = 600;

			GLFWwindow* glfwWindowHandle = nullptr;

			std::string title;

			/// <summary>
			/// Cursor mode defines the state of the window 
			/// along the current frame,
			/// but the backend handle (glfw window handle) is only 
			/// set when flushed at the end of the frame. 
			/// </summary>
			enum class CURSOR_MODE : uint32_t {
				CURSOR_NORMAL = GLFW_CURSOR_NORMAL,
				CURSOR_DISABLED = GLFW_CURSOR_DISABLED,
			} cursorMode = CURSOR_MODE::CURSOR_NORMAL;

			bool windowResized = false;
		};


		//TODO (Conqster): Move window function into .cpp file 
		void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
		bool InitialiseGLFWWindow(Window& window);
		void ChangeWindowTitle(Window* window, const char* title);

		/// <summary>
		/// This set the backend handle state for frame
		/// and return window state to default
		/// 
		/// </summary>
		/// <param name="window"></param>
		/// <returns> previous window state </returns>
		Window FlushWindow(Window* window);
		/// <summary>
		/// This state the state of platform window to locked 
		/// but not the backend handle until flushed at end of frame
		/// </summary>
		/// <param name="window"> set state to locked</param>
		void LockCursor(Window* window);
		/// <summary>
		/// This state the state of platform window to unlocked 
		/// but not the backend handle until flushed at end of frame
		/// </summary>
		/// <param name="window"> set state to unlocked</param>
		void UnlockCursor(Window* window);
		void ToggleCursor(Window* window);


		/// <summary>
		/// get if window state has been locked for current frame 
		/// but the backend has not been locked 
		/// 
		/// TODO(Conqster): maybe change to captured 
		/// </summary>
		/// <param name="window"></param>
		/// <returns></returns>
		bool IsCursorLocked(Window* window);
		bool ShouldWindowClose(Window* window);
		void DestroyWindow(Window* window);

	} // #Platform
}//#Fluxon
