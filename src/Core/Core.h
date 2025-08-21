#pragma once
#include <cstdint>
#include <yaml-cpp/yaml.h>

#include "EnableBitMaskOperators.h"

namespace Fluxon {


	using u8 = uint8_t;
	using u16 = uint16_t;
	using u32 = uint32_t;
	using u64 = uint64_t;


	using i8 = int8_t;
	using i16 = int16_t;
	using i32 = int32_t;
	using i64 = int64_t;


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


#define MAKE_PROPERTY_SERIALISABLE(Type) friend struct YAML::convert<Type>;

#define FLUX_VULKAN 1

#define ENABLE_BIT_MASK_OPERATORS(x, is_mask) \
		template<> \
		struct Fluxon::FlagBitMaskTraits<x>{ \
			static constexpr bool isMask = is_mask; \
			static_assert(std::is_enum_v<x>, "EnumBitsType needs to be an enum class!!!!"); \
		}; \


#pragma region BitFlag/BitMask


	template<typename BitsType>
	class Flag
	{
	public:
		static_assert(std::is_enum_v<BitsType>, "BitsType needs to be an enum class!!!!");
		using underlying_type = std::underlying_type_t<BitsType>;

		// constructors
		constexpr Flag() noexcept : mBits(0) {}
		constexpr Flag(BitsType bit) noexcept : mBits(static_cast<underlying_type>(bit)) {}
		constexpr Flag(Flag<BitsType> const& rhs) noexcept = default;
		constexpr explicit Flag(underlying_type flags) noexcept : mBits(flags) {}


		////methods
		//constexpr bool Has(BitsType bit) const noexcept;
		//constexpr void Set(BitsType bit) noexcept;
		//constexpr void Clear(BitsType bit) noexcept;

		//[[nodiscard]] constexpr underlying_type Value() const { return mBits; }
		////constexpr operator underlying_type() const noexcept { return mValue; }
		//explicit operator underlying_type() const = delete;

		////bitwise operations decleration
		//constexpr Flag<BitsType> operator|(Flag<BitsType> const& rhs) const noexcept; //OR
		//constexpr Flag<BitsType> operator&(Flag<BitsType> const& rhs) const noexcept; //AND
		//constexpr Flag<BitsType> operator^(Flag<BitsType> const& rhs) const noexcept; //XOR
		//constexpr Flag<BitsType> operator~() const noexcept;						  //NOT

		////assignment operators
		//constexpr Flag<BitsType>& operator|=(Flag<BitsType> const& rhs)  noexcept;	  //OR
		//constexpr Flag<BitsType>& operator&=(Flag<BitsType> const& rhs) noexcept;	  //AND
		//constexpr Flag<BitsType>& operator^=(Flag<BitsType> const& rhs) noexcept;	  //XOR

		[[nodiscard]] constexpr bool Contains(BitsType bit) const noexcept {
			return (mBits & static_cast<underlying_type>(bit)) != 0;
		}

		[[nodiscard]] constexpr underlying_type Value() const { return mBits; }
		//constexpr operator underlying_type() const noexcept { return mValue; }
		explicit operator underlying_type() const = delete;

		[[nodiscard]] constexpr void AddBit(BitsType& bit)  noexcept {
			mBits |= static_cast<underlying_type>(bit);
		}

		[[nodiscard]] constexpr void RemoveBit(BitsType& bit)  noexcept {
			mBits &= ~static_cast<underlying_type>(bit);
		}

		//bitwise operations decleration
		constexpr Flag operator|(BitsType bit) const noexcept {
			return Flag(mBits | static_cast<underlying_type>(bit));
		}
		constexpr Flag operator|(Flag rhs) const noexcept {
			return Flag(mBits | rhs.mBits);
		}
		constexpr Flag& operator|=(BitsType bit) noexcept {
			mBits |= static_cast<underlying_type>(bit);
			return *this;
		}
		constexpr Flag& operator|=(Flag rhs) noexcept {
			mBits |= rhs.mBits;
			return *this;
		}

	private:
		underlying_type mBits;
	};




	////flag methopds
	//template<typename BitsType>
	//inline constexpr bool Flag<BitsType>::Has(BitsType bit) const noexcept
	//{
	//	return (mBits & static_cast<underlying_type>(bit)) != 0;
	//}

	//template<typename BitsType>
	//inline constexpr void Flag<BitsType>::Set(BitsType bit) noexcept
	//{
	//	mBits |= static_cast<underlying_type>(bit);
	//}

	//template<typename BitsType>
	//inline constexpr void Flag<BitsType>::Clear(BitsType bit) noexcept
	//{
	//	mBits &= ~static_cast<underlying_type>(bit);
	//}

	////flag bitwise operationn
	//template<typename BitsType>
	//inline constexpr Flag<BitsType> Flag<BitsType>::operator|(Flag<BitsType> const& rhs) const noexcept
	//{
	//	return Flag<BitsType>(mBits | rhs.mBits);
	//}

	//template<typename BitsType>
	//inline constexpr Flag<BitsType> Flag<BitsType>::operator&(Flag<BitsType> const& rhs) const noexcept
	//{
	//	return Flag<BitsType>(mBits & rhs.mBits);
	//}


	//template<typename BitsType>
	//inline constexpr Flag<BitsType> Flag<BitsType>::operator^(Flag<BitsType> const& rhs) const noexcept
	//{
	//	return Flag<BitsType>(mBits ^ rhs.mBits);
	//}


	//template<typename BitsType>
	//inline constexpr Flag<BitsType> Flag<BitsType>::operator~() const noexcept
	//{
	//	return Flag<BitsType>(~mBits);
	//}



	////flag assignemnt operators
	//template<typename BitsType>
	//inline constexpr Flag<BitsType>& Flag<BitsType>::operator|=(Flag<BitsType> const& rhs) noexcept
	//{
	//	mBits |= rhs;
	//	return *this;
	//}

	//template<typename BitsType>
	//inline constexpr Flag<BitsType>& Flag<BitsType>::operator&=(Flag<BitsType> const& rhs) noexcept
	//{
	//	mBits &= rhs;
	//	return *this;
	//}

	//template<typename BitsType>
	//inline constexpr Flag<BitsType>& Flag<BitsType>::operator^=(Flag<BitsType> const& rhs) noexcept
	//{
	//	mBits ^= rhs;
	//	return *this;
	//}


	//////Bitwise OR
	//template<typename BitsType>
	//constexpr Flag<BitsType> operator|(BitsType lhs, BitsType rhs) noexcept
	//{
	//	using underlying_type = std::underlying_type_t<BitsType>;
	//	return Flag<BitsType>(static_cast<underlying_type>(lhs) | static_cast<underlying_type>(rhs));
	//}

	//template<typename BitsType>
	//constexpr Flag<BitsType> operator|=(BitsType& lhs, BitsType rhs) noexcept
	//{
	//	lhs = lhs | rhs;
	//	return lhs;
	//}
#pragma endregion //BitFlag/BitMask

	




	class NonCopyable
	{
	public:
		NonCopyable() = default;
		NonCopyable(const NonCopyable&) = delete;
		void operator=(const NonCopyable&) = delete;
	};
} //namespace






