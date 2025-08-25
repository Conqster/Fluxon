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
	};


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
	inline constexpr EnumBitsType operator~(EnumBitsType& v)
	{
		printf("using bit not operator!!\n");
		using underlying_type = std::underlying_type_t<EnumBitsType>;
		return static_cast<EnumBitsType>(~static_cast<underlying_type>(v));
	}



	template<typename EnumBitsType, std::enable_if_t<FlagBitMaskTraits<EnumBitsType>::isMask, bool> = true>
	inline constexpr bool HasFlag(EnumBitsType v, EnumBitsType flag)
	{
		using underlying_type = std::underlying_type_t<EnumBitsType>;
		return (static_cast<underlying_type>(v) & static_cast<underlying_type>(flag)) == static_cast<underlying_type>(flag);
	}


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

		//methods
		constexpr bool Contains(const BitsType& bit) const noexcept {
			return (mBits & static_cast<underlying_type>(bit)) != 0;
		}

		constexpr void AddBit(const BitsType& bit)  noexcept {
			mBits |= static_cast<underlying_type>(bit);
		}

		constexpr void RemoveBit(const BitsType& bit)  noexcept {
			mBits &= ~static_cast<underlying_type>(bit);
		}


		//operations
		constexpr underlying_type Value() const { return mBits; }
		//implicit interop with other underlying type not allowed as 
		//Bits flag does not/might mot mapp well with each for instance
		//Flag types in sandbox does not map to vulkan enum for space optimisation
		//vulkan flag are uint32 (4 bytes) where in sandbox could be varinat of 
		//u8 (1 bytes), u16 (2 bytes), u32 (4bytes) even u64 (8bytes) depending on use case
		explicit operator underlying_type() const = delete;

		//bitwise operations 
		constexpr Flag operator|(const BitsType& bit) const noexcept { //OR
			return Flag(mBits | static_cast<underlying_type>(bit));
		}
		constexpr Flag operator|(const Flag& rhs) const noexcept {	//OR
			return Flag(mBits | rhs.mBits);
		}

		constexpr Flag operator&(const BitsType& bit) const noexcept { //AND
			return Flag(mBits & static_cast<underlying_type>(bit));
		}
		constexpr Flag operator&(const Flag& rhs) const noexcept {	//AND
			return Flag(mBits & rhs.mBits);
		}


		constexpr Flag operator^(const BitsType& bit) const noexcept { //XOR
			return Flag(mBits ^ static_cast<underlying_type>(bit));
		}
		constexpr Flag operator^(const Flag& rhs) const noexcept {	//XOR
			return Flag(mBits ^ rhs.mBits);
		}


		[[nodiscard]] constexpr Flag operator~() noexcept {	//NOT
			return Flag(~mBits);
		}

		[[nodiscard]] bool operator==(const Flag& rhs) const noexcept {
			return mBits == rhs.mBits;
		}

		[[nodiscard]] bool operator==(const BitsType& bit) const noexcept {
			return mBits == static_cast<underlying_type>(bit);
		}

		[[nodiscard]] bool operator!=(const Flag& rhs) const noexcept {
			return mBits != rhs.mBits;
		}

		[[nodiscard]] bool operator!=(const BitsType& bit) const noexcept {
			return mBits != static_cast<underlying_type>(bit);
		}



		//bitwise assignment 
		constexpr Flag& operator|=(const BitsType& bit) noexcept {	//OR
			mBits |= static_cast<underlying_type>(bit);
			return *this;
		}

		constexpr Flag& operator|=(const Flag& rhs) noexcept {	//OR
			mBits |= rhs.mBits;
			return *this;
		}


		constexpr Flag& operator&=(const BitsType& bit) noexcept {	//AND
			mBits &= static_cast<underlying_type>(bit);
			return *this;
		}

		constexpr Flag& operator&=(const Flag& rhs) noexcept {	//AND
			mBits &= rhs.mBits;
			return *this;
		}


		constexpr Flag& operator^=(const BitsType& bit) noexcept {	//XOR
			mBits ^= static_cast<underlying_type>(bit);
			return *this;
		}

		constexpr Flag& operator^=(const Flag& rhs) noexcept {	//XOR
			mBits ^= rhs.mBits;
			return *this;
		}


	private:
		union{
			underlying_type mBits;
			BitsType mTypeDebugValue;
		};
	};


	template<typename EnumBitsType, std::enable_if_t<FlagBitMaskTraits<EnumBitsType>::isMask, bool> = true>
	inline constexpr Flag<EnumBitsType> operator~(const EnumBitsType& bit) noexcept {	//NOT
		//printf("using bit not operator, but implicit convert to Flag<>!!\n");
		using underlying_type = std::underlying_type_t<EnumBitsType>;
		return Flag<EnumBitsType>(~static_cast<underlying_type>(bit));
	}


#pragma endregion //BitFlag/BitMask





}
