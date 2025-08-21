#pragma once

#include <glm/glm.hpp>
#include "Core.h"


namespace Fluxon {

	inline u32 PackRGBA(float r, float g, float b, float a) {
		u8 _r = static_cast<u8>(r * 255.0f);
		u8 _g = static_cast<u8>(g * 255.0f);
		u8 _b = static_cast<u8>(b * 255.0f);
		u8 _a = static_cast<u8>(a * 255.0f);
		u32 result = (_a << 24) | (_b << 16) | (_g << 8) | _r;
		return result;
	}

	inline u32 PackRGBA(float r, float g, float b) {
		return PackRGBA(r, g, b, 1.0f);
	}

	inline u32 PackRGBA(glm::vec4 c) {
		return PackRGBA(c.r, c.g, c.b, c.a);
	}

	inline u32 PackRGBA(glm::vec3 c, float a = 1.0f) {
		return PackRGBA(c.r, c.g, c.b, a);
	}

	inline u32 PackRGBA(float c) {
		return PackRGBA(c, c, c, c);
	}

	inline glm::vec3 RGBAAsVec3(u32 rgba) {
		//colour channels 
		u8 r = (rgba & 0x000000FF);
		u8 g = (rgba & 0x0000FF00) >> 8;
		u8 b = (rgba & 0x00FF0000) >> 16;
		glm::vec3 col(r, g, b);
		return col / 255.0f;
	}


	struct Colour32
	{
		union {
			u32 value;
			struct { 
				//depending on development r might be promoted to most importance
				u8 r; //Little endiann least significat byte
				u8 g;
				u8 b;
				u8 a; //Most significant byte 
			};
		};


		Colour32() : value(0) {}
		Colour32(float _v) : value(PackRGBA(_v, _v, _v, _v)) {}
		Colour32(float _r, float _g, float _b, float _a = 1.0f) : value(PackRGBA(_r, _g, _b, _a)) {}
		Colour32(const glm::vec3& rgb, float _a = 1.0f) : value(PackRGBA(rgb, _a)) {}
		Colour32(const glm::vec4& rgba) : value(PackRGBA(rgba)) {}

		//Convert to glm
		glm::vec3 ToVec3() const {
			return glm::vec3(r, g, b) / 255.0f;
		}

		glm::vec4 ToVec4() const {
			return glm::vec4(r, g, b, a) / 255.0f;
		}

		bool operator==(const Colour32& other) const { return value == other.value; }
		bool operator!=(const Colour32& other) const { return value != other.value; }

		//implicit conversions
		// to packed uint32_t
		operator u32() const { return value; }
		operator glm::vec3() const { return glm::vec3(r, g, b) / 255.0f; }
		operator glm::vec4() const { return glm::vec4(r, g, b, a) / 255.0f; }


		static const Colour32 sWhite;
		static const Colour32 sBlack;
		static const Colour32 sRed;
		static const Colour32 sGreen;
		static const Colour32 sBlue;
		static const Colour32 sYellow;
		static const Colour32 sCyan;
		static const Colour32 sMagenta;
	};



	//future if needed
	//struct ColourRGBA16
	//{
	//	union {
	//		uint64_t value;
	//		struct {
	//			//depending on development r might be promoted to most importance
	//			uint16_t r; //Little endiann least significat byte
	//			uint16_t g;
	//			uint16_t b;
	//			uint16_t a; //Most significant byte 
	//		};
	//	};


	//	ColourRGBA16() : value(0) {}
	//};



	//template<typename T>
	//struct ColourRGBA
	//{
	//	static_assert(std::is_integral<T>::value, "T must be an integral type.");
	//	static_assert(sizeof(T) == 1 || sizeof(T) == 2, "T must be 8-bit or 16-bit.");

	//	union {
	//		uint64_t value;
	//		struct {
	//			//depending on development r might be promoted to most importance
	//			T r; //Little endiann least significat byte
	//			T g;
	//			T b;
	//			T a; //Most significant byte 
	//		};
	//		typename std::conditional<sizeof(T) == 1, uint32_t, uint64_t>::type value;
	//	};
	//};

	//using ColourRGBA16 = ColourRGBA<uint16_t>;
	//using ColourRGBA8 = ColourRGBA<uint8_t>;
} //namespace Fluxon

