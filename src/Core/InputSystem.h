#pragma once


#include <GLFW/glfw3.h>


namespace Fluxon {
	namespace EventSystem {

		constexpr int NUM_ACTIONS = 2;
		constexpr int NUM_KEYS = 1024;

#define IRELEASE 0
#define IPRESS 1

		class Input
		{
		public:
			static inline Input& Instance() {
				static Input inst;
				return inst;
			}
			void CreateCallbacks(GLFWwindow* window);

			static void FlushFrameInputs();

			//helpers
			bool GetKeyDown(int key_code) {
				return mKeys[key_code][IPRESS];
			}
			bool GetKeyUp(int key_code) {
				return mKeys[key_code][IRELEASE];
			}
			bool GetKey(int key_code) {
				return mHeldKey[key_code];
			}
			float GetMouseAxisFloatDt(int axis_code) {
				return mMouseDt[axis_code];
			}
		private:
			bool mKeys[NUM_KEYS][NUM_ACTIONS] = { false };
			//utility key repeating key over frame
			bool mHeldKey[NUM_KEYS / 2] = { false };
			float mMouseDt[2] = { 0.0f };

			static void KeyboardInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
			static void MouseCursorInputCallback(GLFWwindow* window, double x_pos, double y_pos);
			static void MouseButtonInputCallback(GLFWwindow* window, int button, int action, int mods);
		};

	} //#EventSystem

}//#Fluxon