#pragma once


//#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#define CAM_FOR_VULKAN 1

namespace Fluxon {
	struct Camera
	{
		glm::vec3 position = glm::vec3(-0.93f, 1.60f, 8.55f);

		glm::vec3 forward = glm::vec3(-1.0f);
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);

		float fov = 45.0f;
		float nearPlane = 0.1f;
		float farPlane = 100.0f;

		float moveSpeed = 5.0f;
		float rotSpeed = 0.3f;


		//utility
		static glm::mat4 GetView(const Camera& cam) {
			return glm::lookAt(cam.position, cam.position + cam.forward, glm::vec3(0.0f, 1.0f, 0.0f));
		}

		static glm::mat4 GetProjection(const Camera& cam, float view_width, float view_height) {
			glm::mat4 temp_proj = glm::perspective(glm::radians(cam.fov), view_width / view_height, cam.nearPlane, cam.farPlane);
#if CAM_FOR_VULKAN
			temp_proj[1][1] *= -1;
			return temp_proj;
#else
			return temp_proj;
#endif // CAM_FOR_VULKAN

		}

		//TODO(Conqster - Camera): part of pre frame passs, as its used in pre render for object visibility
		void ComputeAndCacheCameraFrusturm(const Camera& cam) {

		}

		void SetLookTarget(const glm::vec3 target) {
			forward = glm::normalize(target - position);

			// TODO(Conqster): Need to update right and up
			UpdateAxis();
		}

		void Translate(const glm::vec3 dir, float dt) {
			position += dir * moveSpeed * dt;
		}

		void RotateByMouseMovement(float dt_x, float dt_y) {
			if (dt_x != 0.0f || dt_y != 0.0f)
			{
				float yaw = dt_x * rotSpeed * 0.005f;
				float pitch = dt_y * rotSpeed * 0.005f;

				glm::quat pitch_quat = glm::angleAxis(pitch, right);
				glm::quat yaw_quat = glm::angleAxis(-yaw, glm::vec3(0.0f, 1.0f, 0.0f));
				forward = glm::rotate(glm::normalize(yaw_quat * pitch_quat), forward);

				UpdateAxis();
			}
		}


		void UpdateAxis() {
			right = glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f));
			up = glm::cross(right, forward);
		}
	};
}//#Fluxon