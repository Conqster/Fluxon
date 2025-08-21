#pragma once
#include "Core.h"

namespace Fluxon {

	enum class ETextureType : u16
	{
		Texture2D,
		Texture2DArray,
		Texture3D,
		TextureCube,
	};

	enum class ERenderTargetBits : u32
	{
		None		= 0,
		Write		= Bit<u32>(0),
		Sample		= Bit32(1),
		Cube		= Bit32(2),
		Array2D		= Bit32(3),

		ImGuiView	= Bit32(4)
	};

	using RenderTargetFlag = Flag<ERenderTargetBits>;
	ENABLE_BIT_MASK_OPERATORS(ERenderTargetBits, true);


	enum class ETextureUsageBits : u16
	{
		None				= 0,
		DepthAttachment		= Bit16(0),
		ColourAttachment	= Bit16(1),
		Sampled				= Bit16(2),
		Storage				= Bit16(3),
		ImguiView			= Bit16(4)
	};

	using TextureUsageFlags = Flag<ETextureUsageBits>;
	ENABLE_BIT_MASK_OPERATORS(ETextureUsageBits, true)



} //namespace Fluxon