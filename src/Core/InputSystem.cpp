#include "InputSystem.h"

#include <algorithm>

#include <glfw/glfw3.h>


namespace Fluxon {
	namespace InputSystem {

		void EventHandler::CreateCallbacks(GLFWwindow* window)
		{
			glfwSetKeyCallback(window, KeyboardInputCallback);
			glfwSetCursorPosCallback(window, MouseCursorInputCallback);
			glfwSetMouseButtonCallback(window, MouseButtonInputCallback);
		}

		void EventHandler::FlushFrameInputs()
		{
			auto& key_inst = Instance().mKeys;
			std::fill(&key_inst[0][0], &key_inst[0][0] + NUM_KEYS * NUM_ACTIONS, false);
			auto& mouse_dt_inst = Instance().mMouseDt;
			std::fill(std::begin(mouse_dt_inst), std::end(mouse_dt_inst), 0.0f);
		}

		void EventHandler::KeyboardInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			if (key >= 0 && key < NUM_KEYS && action >= 0 && action < NUM_ACTIONS)
			{
				Instance().mKeys[key][action] = true;

				//for held (GLFW internal repeat) immediate responsiveness too slow due this arcitecture for use case
				if (action == IPRESS)
					Instance().mHeldKey[key] = true;
				else if (action == IRELEASE)
					Instance().mHeldKey[key] = false;
			}
		}

		void EventHandler::MouseCursorInputCallback(GLFWwindow* window, double x_pos, double y_pos)
		{
			//first call update
			static float prev[2]{ float(x_pos), float(y_pos) };

			auto& instance = Instance();
			instance.mMouseDt[0] += float(x_pos) - prev[0];			//<-- x 
			instance.mMouseDt[1] += prev[1] - float(y_pos);			//<-- y

			prev[0] = float(x_pos);
			prev[1] = float(y_pos);
		}

		void EventHandler::MouseButtonInputCallback(GLFWwindow* window, int button, int action, int mods)
		{
			if (button >= 0 && button < NUM_KEYS && action >= 0 && action < NUM_ACTIONS)
			{
				//Instance().mMouseButton[button][action] = true;
				Instance().mKeys[button][action] = true;




				//for held (GLFW internal repeat) immediate responsiveness too slow due this arcitecture for use case
				if (action == IPRESS)
					Instance().mHeldKey[button] = true;
				else if (action == IRELEASE)
					Instance().mHeldKey[button] = false;

				//if (key == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_REPEAT)
				//	Instance().mHeldKey[key] = true;
				////if(key == GLFW_MOUSE_BUTTON_LEFT && action != GLFW_REPEAT)
				////	Instance().mHeldKey[key] = false;
			}
		}



		bool InputSystem::Input::GetKeyDown(IKeyCode key)
		{
			return EventHandler::Instance().mKeys[int(key)][IPRESS];
		}

		bool InputSystem::Input::GetKeyUp(IKeyCode key)
		{
			return EventHandler::Instance().mKeys[int(key)][IRELEASE];
		}

		bool InputSystem::Input::GetKey(IKeyCode key)
		{
			return EventHandler::Instance().mHeldKey[int(key)];
		}

		bool InputSystem::Input::GetMousePressed(IKeyCode key)
		{
			//return EventHandler::Instance().mMouseButton[static_cast<int>(key)][GLFW_PRESS];
			return EventHandler::Instance().mKeys[int(key)][IPRESS];
		}

		float InputSystem::Input::GetMouseAxisFloat(IAxis axis)
		{
			return EventHandler::Instance().mMouseDt[int(axis)];
		}

	} //#InputSystem
}//#Fluxon