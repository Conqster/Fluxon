#pragma once


#include "Renderer/Mesh.h"
#include "Renderer/PBRMaterial.h"
#include "../Camera.h"

#include "SceneData.h"

namespace Fluxon {

	enum class ESceneComponentType
	{
		SCT_Renderable, 
		SCT_Light,
		SCT_Camera,
		SCT_Transform
	};

	class ISceneComponent
	{
	public:
		uint32_t id = 0; //TODO(Conqster): later have a proper UUID
		virtual ESceneComponentType GetType() const = 0;
	};


	namespace Vulkan {
		struct DescriptorSetSpecification;
		class GraphicsPipeline;
	}
	class RenderableComponent : public ISceneComponent
	{
	public:
		RenderableComponent() {
			std::cout << "Want a Renderable Component!!!!!!!\n";

			//TODO(Conqster): Need to change this arctecture 
			//should not own gpu data just transfer datas
			renderPrimitiveData.pipelineIdx = 0;
			renderPrimitiveData.mesh = mesh;
			renderPrimitiveData.material = material;
			renderPrimitiveData.pushConstantSpecs = {
			{ VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(renderPrimitiveData.GPUModel), sizeof(glm::vec4), (void*)&new_mat_sys_hack},
			{VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(renderPrimitiveData.GPUModel), (void*)&renderPrimitiveData.GPUModel}
			};
		}
		~RenderableComponent() {
			std::cout << "Discaded a Renderable Component!!!!!!!\n";
		}
		//RenderableComponent& operator=(const RenderableComponent& rhs) {
		//	this->mesh = rhs.mesh;
		//	this->material = rhs.material;
		//	this->visible = rhs.visible;
		//	this->new_mat_sys_hack = rhs.new_mat_sys_hack;

		//	this->renderPrimitiveData = rhs.renderPrimitiveData;
		//	this->renderPrimitiveData.pushConstantSpecs = {
		//	{ VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(renderPrimitiveData.GPUModel), sizeof(glm::vec4), (void*)&this->new_mat_sys_hack},
		//	{VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(renderPrimitiveData.GPUModel), (void*)&renderPrimitiveData.GPUModel}
		//	};	
		//	this->pipelineGlobalBucketIdx = rhs.pipelineGlobalBucketIdx;
		//	this->descSetSpecificationGlobalBucketIdx = rhs.descSetSpecificationGlobalBucketIdx;
		//	return *this;
		//}

		RenderableComponent(Vulkan::Mesh* _mesh, Vulkan::PBRMaterial* _material, bool _visiblity = true, bool _new_mat_system = false) {
			mesh = _mesh;
			material = _material;
			visible = _visiblity;
			new_mat_sys_hack = glm::vec4(0.0f, 0.0f, 0.0f, (float)_new_mat_system);

			renderPrimitiveData.pipelineIdx = 0;
			renderPrimitiveData.mesh = mesh;
			renderPrimitiveData.material = material;
			renderPrimitiveData.pushConstantSpecs = {
			{ VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(renderPrimitiveData.GPUModel), sizeof(glm::vec4), (void*)&new_mat_sys_hack},
			{VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(renderPrimitiveData.GPUModel), (void*)&renderPrimitiveData.GPUModel}
			};
		}
		//data points to a buffer of data 
		//like shared resources as multple objcrt could shared data
		Vulkan::Mesh* mesh				= nullptr;
		Vulkan::PBRMaterial* material	= nullptr;
		bool visible					= true;
		//using alpha channel / w as flag for new material in shader
		glm::vec4 new_mat_sys_hack		= glm::vec4(0.0f);

		SceneRenderPrimitive renderPrimitiveData{};
		//GPUStructures::Model renderModelData;

		//backend datas not visible in editor 
		// 
		//Vulkan::GraphicsPipeline* pipelines; 
		//Vulkan::DescriptorSetSpecification* descSetSpecifications; 
		uint32_t pipelineGlobalBucketIdx = -1;
		uint32_t descSetSpecificationGlobalBucketIdx = -1;

		ESceneComponentType GetType() const override {
			return ESceneComponentType::SCT_Renderable;
		}
	};





	class CameraComponent : public ISceneComponent
	{
	public:
		CameraComponent(Camera* p_camera) {
			cameraData = p_camera;
		}

		Camera* cameraData = nullptr;

		ESceneComponentType GetType() const override {
			return ESceneComponentType::SCT_Camera;
		}
	};


	class TransformComponent : public ISceneComponent
	{
	public:
		glm::mat4 transform = glm::mat4(1.0f); 

		ESceneComponentType GetType() const override {
			return ESceneComponentType::SCT_Transform;
		}
	};

} //#Fluxon