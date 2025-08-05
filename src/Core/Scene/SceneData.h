#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include "Renderer/GPUStrutures.h"

#include "Renderer/Pipelines/GraphicsPipeline.h"
#include "Renderer/Mesh.h"
#include "Renderer/PBRMaterial.h"

namespace Fluxon {

	enum class ERenderPassType
	{
		RenderPass_None,

		RenderPass_Forward,
		RenderPass_Shadow
	};

	enum class ERenderMask : uint32_t
	{
		RM_Opaque			= 1 << 0,
		RM_Transparent		= 1 << 1,
		RM_ShadowCaster		= 1 << 2,
		RM_Skybox			= 1 << 3,

		//hack 
		Count				= 4,
	};

	inline ERenderMask operator|(ERenderMask lhs, ERenderMask rhs) {
		return static_cast<ERenderMask>(static_cast<int32_t>(lhs) |
										static_cast<int32_t>(rhs));
	}

	inline ERenderMask& operator|=(ERenderMask& lhs, ERenderMask rhs) {
		lhs = lhs | rhs;
		return lhs;
	}

	inline ERenderMask operator&(ERenderMask lhs, ERenderMask rhs) {
		return static_cast<ERenderMask>(static_cast<int32_t>(lhs) &
			static_cast<int32_t>(rhs));
	}
	inline ERenderMask& operator&=(ERenderMask& lhs, ERenderMask rhs) {
		lhs = lhs & rhs;
		return lhs;
	}

	inline ERenderMask operator~(ERenderMask v) {
		return static_cast<ERenderMask>(~static_cast<int32_t>(v));
	}

	inline ERenderMask operator^(ERenderMask lhs, ERenderMask rhs) {
		return static_cast<ERenderMask>(static_cast<int32_t>(lhs) ^
			static_cast<int32_t>(rhs));
	}
	inline ERenderMask& operator^=(ERenderMask& lhs, ERenderMask& rhs) {
		lhs = lhs ^ rhs;
		return lhs;
	}


	static std::unordered_map<ERenderMask, std::string> RenderMaskLayerNamesMap =
	{
			{ERenderMask::RM_Opaque, "Opaque Layer"},
			{ERenderMask::RM_Transparent, "Transparent Layer"},
			{ERenderMask::RM_ShadowCaster, "Shadow Caster Layer"},
			{ERenderMask::RM_Skybox, "Skybox Layer"},
	};

	static std::array<const char*, static_cast<size_t>(ERenderMask::Count)> RenderMaskLayerNames()
	{
		return {"Opaque Layer", "Transparent Layer", "ShadowCaster Layer", "Skybox Layer"};
	}


	struct PushConstantSpecification
	{
		VkShaderStageFlagBits stage_flag = VK_SHADER_STAGE_VERTEX_BIT;
		uint32_t offset = 0;
		uint32_t size = 0;
		void* pdata = nullptr;
	};




	/// <summary>
	/// pipelineIdx points to the pipelines in use 
	/// i.e model graphics pipelines etc
	/// same with descSetSpecifications
	/// how render could be handled 
	/// bind RenderPass
	/// bind first pipelines 
	/// might update descSetSpecifications but for current pipelines bind required descSetSpecifications
	/// draw renderObjects in order and condision check pipelineIdx
	/// if pipelineIdx differs, bind the required pipelineIdx must be in sequential order 
	/// i.e current + 1, then its descSetSpecifications
	/// then continue draw renderObject and if the object need to update retrive require data in node.
	/// 
	/// in conclusion renderObjects must be packed in the order of the Required Pipeline and descSetSpecificationa
	/// 
	/// </summary>
	struct SceneRenderPrimitive
	{
		ERenderMask maskLayer = ERenderMask::RM_Opaque | ERenderMask::RM_ShadowCaster;
		uint32_t pipelineIdx = 0;
		uint32_t descSetSpecificationIdx = 0;
		//SceneNode* node = nullptr;

		//hack 

		GPUStructures::Model GPUModel = { };
		Vulkan::Mesh* mesh = nullptr;
		Vulkan::PBRMaterial* material = nullptr;
		std::vector<PushConstantSpecification> pushConstantSpecs;
	};

	struct RenderPassBucket
	{
		ERenderPassType type = ERenderPassType::RenderPass_None;
		Vulkan::IRenderPass* pass = nullptr;
		std::vector<Vulkan::DescriptorSetSpecification> descSetSpecifications; //<-- min 1

		struct PassPipeline
		{
			Vulkan::GraphicsPipeline* pipeline;
			std::vector<Vulkan::DescriptorSetSpecification*> descSetSpecifications; //<-- min 1
		};
		std::vector<PassPipeline> pipelines; //<-- min 1

		std::vector<SceneRenderPrimitive> renderObjects; // infinte 
	};


	struct Camera;
	struct SceneContext
	{
		Camera* mainCamera = nullptr;
		Platform::Window* mainWindow = nullptr;

		//bucket is refilleed every frame
		//0 -> ShadowPass 
		//1 -> ForwardPass
		std::vector<RenderPassBucket> renderPassBuckets;
	};


	class TransformComponent;
	class SceneNode;
	struct SceneResources
	{
		std::string* instantSceneName								= nullptr;

		std::vector<TransformComponent*> renderableComponents;

		std::vector<SceneNode*> sceneNodes;

		GPUStructures::DirectionalLight* directionalLight			= nullptr;
		GPUStructures::Skybox* skybox								= nullptr;

		Camera* camera												= nullptr;
	};

}