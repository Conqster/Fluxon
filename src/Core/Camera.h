#pragma once


//#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <array>

#include "Core.h"

#include "Enums.h"


namespace Fluxon {

	/// <summary>
	/// base camera for static light weight effect 
	/// 
	/// good for shadow mapping and static view (to aim specific target render with 
	/// offset as position to target object direction) 
	/// view based on position & forward 
	/// </summary>
	class Camera
	{
	public:
		Camera() = default;
		Camera(const ECameraType& type, const float near, const float far, const float fovy_scale) :
			mType(type), mNearZ(near), mFarZ(far), mFovY(fovy_scale){ }

		glm::mat4 const GetView() const;
		glm::mat4 const GetProjection(float width, float height) const;
		glm::mat4 const GetProjection(float aspect) const;

		virtual void SetLookTarget(const glm::vec3& target);

		inline glm::mat4 const GetPerspectiveProjection(float aspect) const;
		inline glm::mat4 const GetOrthoProjection(float aspect) const;


		inline ECameraType& GetType() { return mType; }
		inline void SetType(const ECameraType& type) { mType = type; }
		inline bool IsOrthographic() { return mType == ECameraType::Orthographic; }
		inline bool IsPerspective() { return mType == ECameraType::Perspective; }


		//getters & setters 
		const glm::vec3& GetPosition() const { return mPosition; }

		const glm::vec3& GetForward() const { return mForward; }

		inline float GetNearZ() { return mNearZ; }
		inline float GetFarZ() { return mFarZ; }

		inline float GetFovY() { return mFovY; }
		inline float GetScale() { return mScale; }

		inline void SetPosition(const glm::vec3 pos) { mPosition = pos; }
		inline void SetForward(const glm::vec3 forward) { mForward = forward; }

		inline void SetNearZ(const float near) { mNearZ = near; }
		inline void SetFarZ(const float far) { mFarZ = far; }

		inline void SetFovY(const float fovy) { mFovY = fovy; }
		inline void SetScale(const float scale) { mScale = scale; }



	protected:
		MAKE_PROPERTY_SERIALISABLE(Camera)
		//MAKE_PROPERTY_UI_TRAIT(Camera)


		glm::vec3 mPosition = glm::vec3(-0.93f, 1.60f, 8.55f);

		//camera vector basis
		glm::vec3 mForward = glm::vec3(-1.0f);


		float mNearZ = 0.1f;
		float mFarZ = 100.0f;

		union {
			float mFovY = 45.0f;
			struct {
				float mScale;
			};
		};

		ECameraType mType = ECameraType::Perspective;
	};


	/// <summary>
	/// support programmatic rotations
	/// future support animation, interpolation 
	/// where basis vector would be replaced with quaternion
	/// 
	/// No direction input support / speed rate / delta rate
	/// </summary>
	class DynamicCamera : public Camera
	{
	public:
		DynamicCamera() = default;

		DynamicCamera(const ECameraType& type, const float near, const float far, const float fovy_scale) : 
			Camera(type, near, far, fovy_scale){ }

		const glm::vec3& GetUp() const { return mUp; } //<-- better 
		const glm::vec3& GetRight() const { return mRight; } //<--- out dated const usage

		virtual void SetLookTarget(const glm::vec3& target) override;

	protected:

		void UpdateUPRTBasisVector();

		MAKE_PROPERTY_SERIALISABLE(DynamicCamera)
		//MAKE_PROPERTY_UI_TRAIT(DynamicCamera)

		// camera vector basis to maintain rotation axes
		glm::vec3 mUp = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 mRight = glm::vec3(1.0f, 0.0f, 0.0f);
	};



	/// <summary>
	/// support editor camera view
	/// for scene inspection, debubing, tooling
	/// mouse driven rotation, translation and speed control 
	/// </summary>
	class EditorCamera : public DynamicCamera
	{
	public:
		EditorCamera() = default;

		EditorCamera(const ECameraType& type, const float near, const float far, const float fovy_scale) :
			DynamicCamera(type, near, far, fovy_scale) {}

		//getters & setters 
		float GetMoveSpeed() { return mMoveSpeed; }
		float GetRotSpeed() { return mRotSpeed; }

		void SetMoveSpeed(float speed) { mMoveSpeed = speed; }
		void SetRotSpeed(float speed) { mRotSpeed = speed; }

		void Translate(const glm::vec3 dir, float dt) {
			mPosition += dir * mMoveSpeed * dt;
		}

		void RotateByMouseMovement(float dt_x, float dt_y) {
			if (dt_x != 0.0f || dt_y != 0.0f)
			{
				float yaw = dt_x * mRotSpeed * 0.005f;
				float pitch = dt_y * mRotSpeed * 0.005f;


				glm::quat pitch_quat = glm::angleAxis(pitch, mRight);
				glm::quat yaw_quat = glm::angleAxis(-yaw, glm::vec3(0.0f, 1.0f, 0.0f));
				mForward = glm::rotate(glm::normalize(yaw_quat * pitch_quat), mForward);

				mForward = glm::normalize(mForward);

				UpdateUPRTBasisVector();
			}
		}

	protected:
		MAKE_PROPERTY_SERIALISABLE(EditorCamera)
		//MAKE_PROPERTY_UI_TRAIT(EditorCamera)

		float mMoveSpeed = 5.0f;
		float mRotSpeed = 0.3f;
	};
}//#Fluxon
