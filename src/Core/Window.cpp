
#include "Window.h"



namespace Fluxon {
	namespace Platform {

		//TODO (Conqster): Move window function into .cpp file 
		void FramebufferResizeCallback(GLFWwindow* window, int width, int height)
		{
			auto win = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));

			win->windowMinimised = (width == 0 || height == 0);

			win->width = width;
			win->height = height;
			win->windowResized = true;
		}

		 bool InitialiseGLFWWindow(Window& window)
		{
			int result = glfwInit();

			if (result == GLFW_FALSE)
				return false;

			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

			window.glfwWindowHandle = glfwCreateWindow(window.width, window.height, window.title.c_str(), nullptr, nullptr);

			glfwSetWindowUserPointer(window.glfwWindowHandle, &window);
			glfwSetFramebufferSizeCallback(window.glfwWindowHandle, /*Platform::*/FramebufferResizeCallback);
			return true;
		}

		 void ChangeWindowTitle(Window* window, const char* title)
		{
			glfwSetWindowTitle(window->glfwWindowHandle, title);
		}



		/// <summary>
		/// This set the backend handle state for frame
		/// and return window state to default
		/// 
		/// </summary>
		/// <param name="window"></param>
		/// <returns> previous window state </returns>
		 Window FlushWindow(Window* window)
		{
			// TODO (Conqster): Need to create an Assert system
			if (!window) return {};

			switch (window->cursorMode)
			{
			case Window::CURSOR_MODE::CURSOR_NORMAL:
				glfwSetInputMode(window->glfwWindowHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				break;
			case Window::CURSOR_MODE::CURSOR_DISABLED:
				glfwSetInputMode(window->glfwWindowHandle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				break;
			default:
				// TODO (Conqster): Need to create an Logging system
				printf("[Window::CURSOR_MODE]: Mode not supported!!!!!!!!\n");
				break;
			}

			Window prev_win = *window;
			//reset window frame
			window->cursorMode = Window::CURSOR_MODE::CURSOR_NORMAL;
			return prev_win;
		}

		/// <summary>
		/// This state the state of platform window to locked 
		/// but not the backend handle until flushed at end of frame
		/// </summary>
		/// <param name="window"> set state to locked</param>
		 void LockCursor(Window* window)
		{
			// TODO (Conqster): Need to create an Assert system
			if (!window)return;

			if (window->cursorMode != Window::CURSOR_MODE::CURSOR_DISABLED)
			{
				//glfwSetInputMode(window->glfwWindowHandle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				//glfwSetInputMode(window->glfwWindowHandle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				window->cursorMode = Window::CURSOR_MODE::CURSOR_DISABLED;
			}
		}

		/// <summary>
	/// This state the state of platform window to unlocked 
	/// but not the backend handle until flushed at end of frame
	/// </summary>
	/// <param name="window"> set state to unlocked</param>
		 void UnlockCursor(Window* window)
		{
			// TODO (Conqster): Need to create an Assert system
			if (!window)return;

			if (window->cursorMode != Window::CURSOR_MODE::CURSOR_NORMAL)
			{
				//glfwSetInputMode(window->glfwWindowHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				window->cursorMode = Window::CURSOR_MODE::CURSOR_NORMAL;
			}
		}

		 void ToggleCursor(Window* window)
		{
			// TODO (Conqster): Need to create an Assert system
			if (!window)return;

			switch (window->cursorMode)
			{
			case Window::CURSOR_MODE::CURSOR_NORMAL:
				LockCursor(window);
				break;
			case Window::CURSOR_MODE::CURSOR_DISABLED:
				UnlockCursor(window);
				break;
			default:
				// TODO (Conqster): Need to create an Logging system
				printf("[Window::CURSOR_MODE]: Mode not supported!!!!!!!!\n");
				break;
			}
		}


		/// <summary>
		/// get if window state has been locked for current frame 
		/// but the backend has not been locked 
		/// 
		/// TODO(Conqster): maybe change to captured 
		/// </summary>
		/// <param name="window"></param>
		/// <returns></returns>
		 bool IsCursorLocked(Window* window)
		{
			// TODO (Conqster): Need to create an Assert system
			if (!window) return false;

			return window->cursorMode == Window::CURSOR_MODE::CURSOR_DISABLED;
		}

		bool ShouldWindowClose(Window* window)
		{
			// TODO (Conqster): Need to create an Assert system
			if (!window) return true;

			return glfwWindowShouldClose(window->glfwWindowHandle);
		}

		void DestroyWindow(Window* window)
		{
			// TODO (Conqster): Need to create an Assert system
			if (!window)return;

			glfwDestroyWindow(window->glfwWindowHandle);
			glfwTerminate();
		}

	} //#Platform
} //# Platform