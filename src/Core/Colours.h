#pragma once

#include <glm/glm.hpp>
#include "Core.h"


namespace Fluxon {

	inline uint32 PackRGBA(float r, float g, float b, float a) {
		uint8 _r = static_cast<uint8>(r * 255.0f);
		uint8 _g = static_cast<uint8>(g * 255.0f);
		uint8 _b = static_cast<uint8>(b * 255.0f);
		uint8 _a = static_cast<uint8>(a * 255.0f);
		uint32 result = (_a << 24) | (_b << 16) | (_g << 8) | _r;
		return result;
	}

	inline uint32 PackRGBA(float r, float g, float b) {
		return PackRGBA(r, g, b, 1.0f);
	}

	inline uint32 PackRGBA(glm::vec4 c) {
		return PackRGBA(c.r, c.g, c.b, c.a);
	}

	inline uint32 PackRGBA(glm::vec3 c, float a = 1.0f) {
		return PackRGBA(c.r, c.g, c.b, a);
	}

	inline uint32 PackRGBA(float c) {
		return PackRGBA(c, c, c, c);
	}

	inline glm::vec3 RGBAAsVec3(uint32 rgba) {
		//colour channels 
		uint8 r = (rgba & 0x000000FF);
		uint8 g = (rgba & 0x0000FF00) >> 8;
		uint8 b = (rgba & 0x00FF0000) >> 16;
		glm::vec3 col(r, g, b);
		return col / 255.0f;
	}

	struct NODISCARD Colour32
	{
		union {
			uint32 value;
			struct { 
				//depending on development r might be promoted to most importance
				uint8 r; //Little endiann least significat byte
				uint8 g;
				uint8 b;
				uint8 a; //Most significant byte 
			};
		};


		Colour32() : value(0) {}
		Colour32(float _v) : value(PackRGBA(_v, _v, _v, _v)) {}
		Colour32(float _r, float _g, float _b, float _a = 1.0f) : value(PackRGBA(_r, _g, _b, _a)) {}
		Colour32(const glm::vec3& rgb, float _a = 1.0f) : value(PackRGBA(rgb, _a)) {}
		Colour32(const glm::vec4& rgba) : value(PackRGBA(rgba)) {}

		//Convert to glm
		NODISCARD glm::vec3 ToVec3() const { return glm::vec3(r, g, b) / 255.0f; }
		NODISCARD glm::vec4 ToVec4() const { return glm::vec4(r, g, b, a) / 255.0f; }

		bool operator==(const Colour32& other) const { return value == other.value; }
		bool operator!=(const Colour32& other) const { return value != other.value; }

		//implicit conversions
		// to packed uint32_t
		NODISCARD operator uint8() const { return value; }
		NODISCARD operator glm::vec3() const { return glm::vec3(r, g, b) / 255.0f; }
		NODISCARD operator glm::vec4() const { return glm::vec4(r, g, b, a) / 255.0f; }

		//channel accessing
		//u8 operator ()(int idx) const { return r; }


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

