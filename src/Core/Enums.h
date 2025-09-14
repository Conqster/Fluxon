#pragma once
#include "Core.h"

namespace Fluxon {


	enum class ERenderPassType : uint8
	{
		None,

		Forward,
		Shadow,
		Voxelisation
	};

	enum class ERenderBits : uint16
	{
		None			= 0,
		Opaque		= Bit16(0),
		Transparent	= Bit16(1),
		ShadowCaster = Bit16(2),
		Skybox		= Bit16(3),
		Voxel		= Bit16(4), 

		Count			= 5,
	};
	using RenderMasks = Flag<ERenderBits>;
	ENABLE_BIT_MASK_OPERATORS(ERenderBits, true);

	static std::unordered_map<ERenderBits, std::string> RenderMaskBitsLayerNamesMap =
	{
			{ERenderBits::Opaque, "Opaque Layer"},
			{ERenderBits::Transparent, "Transparent Layer"},
			{ERenderBits::ShadowCaster, "Shadow Caster Layer"},
			{ERenderBits::Skybox, "Skybox Layer"},
			{ERenderBits::Voxel, "Voxel Layer"},
	};

	inline constexpr std::array<const char*, static_cast<size_t>(ERenderBits::Count)> RenderMaskBitsLayerNames()
	{
		return { "Opaque Layer", "Transparent Layer", "ShadowCaster Layer", "Skybox Layer", "Voxel Layer"};
	}

	inline std::vector<const char*> RenderMaskBitsLayerNamesVector()
	{
		std::vector<const char*> temp = { 
			"Opaque Layer", 
			"Transparent Layer", 
			"ShadowCaster Layer", 
			"Skybox Layer", 
			"Voxel Layer" 
		};

		return temp;
	}

	enum class ETextureType : uint16
	{
		Texture2D,
		Texture2DArray,
		Texture3D,
		TextureCube,
	};

	enum class ERenderTargetBits : uint32
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


	enum class ETextureUsageBits : uint16
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
	enum class EPBRTextureMapType : uint8
	{
		Albedo,
		Normal,
		aoRM,

		Count
	};


	enum class EMeshType : uint8
	{
		Static
	};

	enum class EPrimitiveMeshType : uint8
	{
		None,
		ModelMesh,			//Loaded from external file
		Cube,
		Sphere,
		Plane
	};



	enum class ECameraType : uint8
	{
		Perspective,
		Orthographic
	};


	constexpr std::array<const char*, 2>GetCameraTypeAsChar()
	{
		return { "Perspective", "Orthographic" };
	}



	enum class ESandboxTheme : uint8
	{
		BlueAccent,
		TealAccent
	};


	enum EDescriptorSetErrorBit : uint8
	{
		None				 = 0,
		InvalidBinding		 = Bit8(0),
		BindingOverlap		 = Bit8(1),
		NoData				 = Bit8(2),
		TypeDoesNotMatchData = Bit8(3),
		TypeDoesNotExist	 = Bit8(4),
	};

	using DescriptorSetErrorFlag = Flag<EDescriptorSetErrorBit>;
	ENABLE_BIT_MASK_OPERATORS(EDescriptorSetErrorBit, true);




	enum class EShaderStageBit : uint8
	{
		Vertex		= Bit8(0),
		Fragment	= Bit8(1),
		Compute		= Bit8(2)
	};
	using ShaderStageFlag = Flag<EShaderStageBit>;
	ENABLE_BIT_MASK_OPERATORS(EShaderStageBit, true);

} //namespace Fluxon