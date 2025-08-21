#include "Camera.h"



namespace Fluxon {


	glm::mat4 const Camera::GetView() const
	{
		return glm::lookAt(mPosition, mPosition + mForward, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::mat4 const Camera::GetProjection(float width, float height) const
	{
		switch (mType)
		{
		case Fluxon::ECameraType::Perspective:
			return GetPerspectiveProjection(width / height);
			break;
		case Fluxon::ECameraType::Orthographic:
			return GetOrthoProjection(width / height);
			break;
		default:
			break;
		}
	}


	glm::mat4 const Camera::GetProjection(float aspect) const
	{
		switch (mType)
		{
		case Fluxon::ECameraType::Perspective:
			return GetPerspectiveProjection(aspect);
			break;
		case Fluxon::ECameraType::Orthographic:
			return GetOrthoProjection(aspect);
			break;
		default:
			break;
		}
	}

	inline glm::mat4 const Camera::GetPerspectiveProjection(float aspect) const
	{
#if FLUX_VULKAN
		glm::mat4 temp_proj = glm::perspective(glm::radians(mFovY), aspect, mNearZ, mFarZ);
		temp_proj[1][1] *= -1;
		return temp_proj;
#else
		return glm::perspective(glm::radians(mFovY), width / height, mNearZ, mFarZ);
#endif // CAM_FOR_VULKAN
	}

	inline glm::mat4 const Camera::GetOrthoProjection(float aspect) const
	{
#if FLUX_VULKAN
		glm::mat4 temp_proj = glm::ortho(aspect  * -mScale, aspect * mScale, -mScale, mScale, mNearZ, mFarZ);
		temp_proj[1][1] *= -1;
		return temp_proj;
#else
		return glm::ortho(-half_width, half_width, -half_height, half_height);
#endif // CAM_FOR_VULKAN
	}


	void Camera::SetLookTarget(const glm::vec3& target)
	{
		mForward = glm::normalize(target - mPosition);
	}


	void DynamicCamera::SetLookTarget(const glm::vec3& target)
	{
		Camera::SetLookTarget(target);

		UpdateUPRTBasisVector();
	}

	void DynamicCamera::UpdateUPRTBasisVector()
	{
		mRight = glm::cross(mForward, glm::vec3(0.0f, 1.0f, 0.0f));
		mUp = glm::cross(mRight, mForward);
	}
} //namespace Fluxon