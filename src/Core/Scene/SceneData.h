#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include "Renderer/GPUStrutures.h"

#include "Renderer/Pipelines/GraphicsPipeline.h"
#include "Renderer/Mesh.h"
#include "Renderer/PBRMaterial.h"

#include "Core/Enums.h"

#include "Core/Geometry/AABB.h"

namespace Fluxon {


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
		//ERenderMask maskLayer = ERenderMask::RM_Opaque | ERenderMask::RM_ShadowCaster;
		RenderMasks renderMask = ERenderBits::Opaque | ERenderBits::ShadowCaster;
		uint32_t pipelineIdx = 0;
		uint32_t descSetSpecificationIdx = 0;
		//SceneNode* node = nullptr;

		//hack 

		GPUStructures::Model GPUModel = { };
		Vulkan::Mesh* mesh = nullptr;
		Vulkan::PBRMaterial* material = nullptr;
		std::vector<PushConstantSpecification> pushConstantSpecs;
	};


	namespace Vulkan {
		class RenderPass;
	}
	struct RenderPassBucket
	{
		ERenderPassType type = ERenderPassType::None;
		//Vulkan::IRenderPass* pass = nullptr;
		Vulkan::RenderPass* pass = nullptr;
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


	struct LightShadowData
	{

		Camera camera = Camera(ECameraType::Orthographic, 0.1f, 70.0f, 7.0f);

		float GetNearZ() {
			return camera.GetNearZ();
		}

		float GetFarZ() {
			return camera.GetFarZ();
		}

		float GetViewScale() {
			return camera.GetScale();
		}

		const glm::vec3& GetSampleOrigin() const {
			return camera.GetPosition();
		}

		const glm::vec3& GetViewPosition() const {
			return camera.GetPosition();
		}

		glm::mat4 const GetView() const {
			return camera.GetView();
		}

		glm::mat4 const GetProjection() const {
			return camera.GetOrthoProjection(1.0f); //<-- aspect ratio 1.0 equal width and height
		}

		void UpdateCameraPosition(glm::vec3 light_dir) {
			glm::vec3 normalised_light_dir = glm::normalize(light_dir);
			camera.SetForward(normalised_light_dir);

			glm::vec3 light_pos = sampleOrigin - (normalised_light_dir * sampleShadowDistance);
			camera.SetPosition(light_pos);
		}


		glm::vec3 GetViewOrthoVolumeBoundsMin() {
			return { -camera.GetScale(), -camera.GetScale(), camera.GetNearZ() };
		}

		glm::vec3 GetViewOrthoVolumeBoundsMax() {
			return { camera.GetScale(), camera.GetScale(), camera.GetFarZ() };
		}

		AABB GetViewOrthoVolumeAABBBounds() {
			return
			{
				{ -camera.GetScale(), -camera.GetScale(), camera.GetNearZ() },
				{ camera.GetScale(), camera.GetScale(), camera.GetFarZ() }
			};
		}

		glm::vec3 sampleOrigin = glm::vec3(0.0f);
		//glm::vec3 sampleWorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
		float sampleShadowDistance = 54.0f;

		float depthBiasConstantFactor = 1.75f; // Optional
		float depthBiasClamp = 0.0f; // Optional
		float depthBiasSlopeFactor = 1.25f; // Optional
	};

	class TransformComponent;
	class SceneNode;
	namespace Vulkan {
		struct RenderTarget;
	}
	struct SceneResources
	{
		std::string* instantSceneName								= nullptr;

		std::vector<TransformComponent*> renderableComponents;

		std::vector<SceneNode*> sceneNodes;

		GPUStructures::DirectionalLight* directionalLight			= nullptr;
		GPUStructures::SkyboxProperties* skybox								= nullptr;

		EditorCamera* camera										= nullptr;

		Vulkan::Resources::ImGui_Texture_Test* experiment_texture	= nullptr;
		Vulkan::RenderTarget* shadowPassDepthRT						= nullptr;
		Vulkan::RenderTarget* voxelDepthRT							= nullptr;

		Vulkan::RenderTarget* sceneRenderOutput = nullptr;


		LightShadowData lightShadowData{};

		GPUStructures::EmitterUBO* particleEmitter					= nullptr;

		double currentFrameDT = 0.0f;

		//just for testing 
		float viewportWidth;
		float viewportHeight;

		glm::vec4 experimentLightMove = glm::vec4(0.0f);

		//void* pScene												= nullptr;
	};

	//scene
		//scene level

}