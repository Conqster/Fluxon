#pragma once
#include <cstdint>
#include <yaml-cpp/yaml.h>

#include "EnumBitsFlag.h"

namespace Fluxon {


	using u8 = uint8_t;
	using u16 = uint16_t;
	using u32 = uint32_t;
	using u64 = uint64_t;

	using i8 = int8_t;
	using i16 = int16_t;
	using i32 = int32_t;
	using i64 = int64_t;

	//to prevent truncation of 64-bit pointer to 32-bit
	using intptr = intptr_t; //<-- sweet for bitwise ops (address math, masking, hashing)
	using uintptr = uintptr_t; //<-- sweet for numerical interpretation (differences, offsets).
	
	using f32 = float;
	using f64 = double;


	template<typename T>
	inline constexpr T Bit(unsigned x) { 
		return static_cast<T>(T(1) << x); 
	}
	
	inline constexpr u32 Bit32(unsigned x) {
		return Bit<u32>(x);
	}

	inline constexpr u16 Bit16(unsigned x) {
		return Bit<u16>(x);
	}

#define NODISCARD [[nodiscard]]

#define MAKE_PROPERTY_SERIALISABLE(Type) friend struct YAML::convert<Type>;

#define FLUX_VULKAN 1
#define FLUX_ENABLE_CONSOLE_LOGGING 1
#define FLUX_SHOW_DEBUG_SEVERITY_LVL 1
#if FLUX_SHOW_DEBUG_SEVERITY_LVL
	static constexpr bool kFluxShowDebugSeverityLvl = true;
#else
	static constexpr bool kFluxShowDebugSeverityLvl = false;
#endif // FLUX_SHOW_DEBUG_SEVERITY_LVL
#define FLUX_ONLY_DEBUG_HIGH_SEVERITY_LOG 0


#define ENABLE_BIT_MASK_OPERATORS(x, is_mask) \
		template<> \
		struct Fluxon::FlagBitMaskTraits<x>{ \
			static constexpr bool isMask = is_mask; \
			static_assert(std::is_enum_v<x>, "EnumBitsType needs to be an enum class!!!!"); \
		}; 



	class NonCopyable
	{
	public:
		NonCopyable() = default;
		NonCopyable(const NonCopyable&) = delete;
		void operator=(const NonCopyable&) = delete;
	};
} //namespace






