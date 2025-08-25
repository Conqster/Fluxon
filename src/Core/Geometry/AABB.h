#pragma once
#include "glm/glm.hpp"

#include "Core/Core.h"

namespace Fluxon {

	class NODISCARD AABB
	{
	public:
		//Constructor
		AABB() : mMin(glm::vec3(-0.5f)), mMax(glm::vec3(0.5f)) {}
		AABB(const float size) : mMin(glm::vec3(-1.0f)* size), mMax(glm::vec3(1.0f)* size) {}
		AABB(const glm::vec3& point) : mMin(point), mMax(point) {} //used if only a point is defines
		AABB(const glm::vec3& min, const glm::vec3& max) : mMin(min), mMax(max) {}

		NODISCARD glm::vec3 GetCenter() const
		{
			return (mMin + mMax) * 0.5f;
		}

		NODISCARD glm::vec3 GetSize() const
		{
			return mMax - mMin;
		}

		NODISCARD glm::vec3 GetExtent() const
		{
			return (mMax - mMin) * 0.5f;
		}

		NODISCARD bool IsValid() const {
			return mMin.x <= mMax.x &&
				mMin.y <= mMax.y &&
				mMin.z <= mMax.z;
		}

		void Translate(const glm::vec3& translation)
		{
			mMin += translation;
			mMax += translation;
		}


		/// <summary>
		/// To work with meshes use as 
		/// temp.Scale((scale - glm::vec3(1.0f)) * glm::vec3(0.5f));
		/// as normal scale would be glm::vec3(1.0f) and using scale 
		/// as raw would scale AABB by 1 unnessary unit & the half glm::vec3(0.5f) 
		/// is to compensate that AABB is scaled in both min and max
		/// </summary>
		/// <param name="scale"></param>
		void Expand(const glm::vec3& scale)
		{
			mMin -= scale;
			mMax += scale;
		}

		void Expand(const float scale)
		{
			mMin -= scale;
			mMax += scale;
		}

		void Reset()
		{
			mMin = glm::vec3(FLT_MAX);
			mMax = glm::vec3(-FLT_MAX);
		}



		//OBB to AABB
		AABB Transformed(const glm::mat4& transform) const
		{
			//New AABB center to be in tranform pos
			glm::vec3 temp_min = glm::vec3(transform[3]);
			glm::vec3 temp_max = temp_min;


			for (unsigned int i = 0; i < 3; i++)
			{
				glm::vec3 column = glm::vec3(transform[i]);

				glm::vec3 a = column * mMin[i];
				glm::vec3 b = column * mMax[i];

				temp_min += (glm::min)(a, b);
				temp_max += (glm::max)(a, b);
			}

			return AABB(temp_min, temp_max);
		}

		void Encapsulate(const glm::vec3& pos)
		{
			mMin = (glm::min)(mMin, pos);
			mMax = (glm::max)(mMax, pos);
		}

		void Encapsulate(const AABB& rhs)
		{
			mMin = (glm::min)(mMin, rhs.mMin);
			mMax = (glm::max)(mMax, rhs.mMax);
		}

		NODISCARD std::array<glm::vec3, 8> GetCorners() const
		{
			return {
			   glm::vec3(mMin.x, mMin.y, mMin.z),
			   {mMax.x, mMin.y, mMin.z},
			   {mMax.x, mMax.y, mMin.z},
			   {mMin.x, mMax.y, mMin.z},
			   {mMin.x, mMin.y, mMax.z},
			   {mMax.x, mMin.y, mMax.z},
			   {mMax.x, mMax.y, mMax.z},
			   {mMin.x, mMax.y, mMax.z}
			};
		}

		glm::vec3 mMin = glm::vec3(0.0f);
		glm::vec3 mMax = glm::vec3(0.0f);
	};
} //namespace Fluxon

