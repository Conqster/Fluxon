#pragma once

#include "Core.h"
#include <type_traits>



//use this for some crazy shit in Imgui conversion "std::underlying_type_t<T>"

namespace Fluxon
{
	template<typename EnumBitsType>
	struct FlagBitMaskTraits
	{
		static constexpr bool isMask = false;

		static_assert(std::is_enum_v<EnumBitsType>, "EnumBitsType needs to be an enum class!!!!");
	};
	

	template<typename EnumBitsType>
	constexpr bool kIsEnableFlagBitMaskTraits = FlagBitMaskTraits<EnumBitsType>::isMask && std::is_enum_v<EnumBitsType>;
	
	
	//template<typename BitsType>
	//constexpr std::enable_if_t<EnableBitMaskOpV<BitsType>, Flag<BitsType>>
	//	operator|(BitsType lhs, BitsType rhs) noexcept
	//{
	//	using underlying_type = std::underlying_type_t<BitsType>;
	//	return Flag<BitsType>(static_cast<underlying_type>(lhs) | static_cast<underlying_type>(rhs));
	//}

	//using underlying_type = std::underlying_type_t<BitsType>;
	//using MaskType = typename std::underlying_type<BitType>::type;
	//Bitwise OR
	template<typename EnumBitsType, std::enable_if_t<FlagBitMaskTraits<EnumBitsType>::isMask, bool> = true>
	inline constexpr EnumBitsType operator|(EnumBitsType lhs, EnumBitsType rhs)
	{
		using underlying_type = std::underlying_type_t<EnumBitsType>;
		return static_cast<EnumBitsType>(static_cast<underlying_type>(lhs) | static_cast<underlying_type>(rhs));
	}

	template<typename EnumBitsType, std::enable_if_t<FlagBitMaskTraits<EnumBitsType>::isMask, bool> = true>
	constexpr EnumBitsType& operator|=(EnumBitsType& lhs, EnumBitsType rhs)
	{
		lhs = lhs | rhs;
		return lhs;
	}


	//Bitwise AND
	template<typename EnumBitsType, std::enable_if_t<FlagBitMaskTraits<EnumBitsType>::isMask, bool> = true>
	constexpr EnumBitsType operator&(EnumBitsType lhs, EnumBitsType rhs)
	{
		using underlying_type = std::underlying_type_t<EnumBitsType>;
		return static_cast<EnumBitsType>(static_cast<underlying_type>(lhs) & static_cast<underlying_type>(rhs));
	}

	template<typename EnumBitsType, std::enable_if_t<FlagBitMaskTraits<EnumBitsType>::isMask, bool> = true>
	inline constexpr EnumBitsType& operator&=(EnumBitsType& lhs, EnumBitsType rhs)
	{
		lhs = lhs & rhs;
		return lhs;
	}


	//Bitwise XOR
	template<typename EnumBitsType, std::enable_if_t<FlagBitMaskTraits<EnumBitsType>::isMask, bool> = true>
	inline constexpr EnumBitsType operator^(EnumBitsType lhs, EnumBitsType rhs)
	{
		using underlying_type = std::underlying_type_t<EnumBitsType>;
		return static_cast<EnumBitsType>(static_cast<underlying_type>(lhs) ^ static_cast<underlying_type>(rhs));
	}

	template<typename EnumBitsType, std::enable_if_t<FlagBitMaskTraits<EnumBitsType>::isMask, bool> = true>
	inline constexpr EnumBitsType& operator^=(EnumBitsType& lhs, EnumBitsType rhs)
	{
		lhs = lhs ^ rhs;
		return lhs;
	}


	//Bitwise NOT
	template<typename EnumBitsType, std::enable_if_t<FlagBitMaskTraits<EnumBitsType>::isMask, bool> = true>
	inline constexpr EnumBitsType operator~(EnumBitsType v)
	{
		using underlying_type = std::underlying_type_t<EnumBitsType>;
		return static_cast<EnumBitsType>(~static_cast<underlying_type>(v));
	}


	template<typename EnumBitsType, std::enable_if_t<FlagBitMaskTraits<EnumBitsType>::isMask, bool> = true>
	inline constexpr bool HasFlag(EnumBitsType v, EnumBitsType flag)
	{
		using underlying_type = std::underlying_type_t<EnumBitsType>;
		return (static_cast<underlying_type>(v) & static_cast<underlying_type>(flag)) == static_cast<underlying_type>(flag);
	}

}
