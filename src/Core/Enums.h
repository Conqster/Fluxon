#pragma once
#include "Core.h"

namespace Fluxon {


	enum class ERenderPassType : u8
	{
		None,

		Forward,
		Shadow,
		Voxelisation
	};

	enum class ERenderBits : u16
	{
		None			= 0,
		OpaqueBit		= Bit16(0),
		TransparentBit	= Bit16(1),
		ShadowCasterBit = Bit16(2),
		SkyboxBit		= Bit16(3),
		VoxelBit		= Bit16(4), 

		Count			= 4,
	};
	using RenderMasks = Flag<ERenderBits>;
	ENABLE_BIT_MASK_OPERATORS(ERenderBits, true);

	static std::unordered_map<ERenderBits, std::string> RenderMaskBitsLayerNamesMap =
	{
			{ERenderBits::OpaqueBit, "Opaque Layer"},
			{ERenderBits::TransparentBit, "Transparent Layer"},
			{ERenderBits::ShadowCasterBit, "Shadow Caster Layer"},
			{ERenderBits::SkyboxBit, "Skybox Layer"},
	};

	inline constexpr std::array<const char*, static_cast<size_t>(ERenderBits::Count)> RenderMaskBitsLayerNames()
	{
		return { "Opaque Layer", "Transparent Layer", "ShadowCaster Layer", "Skybox Layer" };
	}

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
		Write		= Bit32(0),
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
		ImguiView			= Bit16(4),
	};

	using TextureUsageFlags = Flag<ETextureUsageBits>;
	ENABLE_BIT_MASK_OPERATORS(ETextureUsageBits, true);


	//could have this as a bit which defines what a pbr material
	//has material texture to.
	enum class EPBRTextureMapType : u8
	{
		Albedo,
		Normal,
		aoRM,

		Count
	};


	enum class EMeshType : u8
	{
		Static
	};

	enum class EPrimitiveMeshType : u8
	{
		None,
		ModelMesh,			//Loaded from external file
		Cube,
		Sphere,
		Plane
	};



	enum class ECameraType : u8
	{
		Perspective,
		Orthographic
	};


	constexpr std::array<const char*, 2>GetCameraTypeAsChar()
	{
		return { "Perspective", "Orthographic" };
	}


} //namespace Fluxon