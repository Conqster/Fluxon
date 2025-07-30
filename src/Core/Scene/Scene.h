#pragma once
#include "SceneNode.h"
#include <memory>

#include "Renderer/Render Passes/ForwardRenderPass.h"

#include <glm/glm.hpp>
#include "Renderer/GPUStrutures.h"

#include "Renderer/Pipelines/GraphicsPipeline.h"
#include "Renderer/Mesh.h"
#include "Renderer/PBRMaterial.h"

#include "SceneData.h"

namespace Fluxon {



	namespace Platform {
		struct Window;
	}
	namespace Vulkan {
		struct VulkanContext;
		struct PBRMaterial;
		struct Mesh;
		class GraphicsPipeline;
		struct DescriptorSetSpecification;
		
		namespace Resources {
			struct ImageSampler;
		}
	}

	namespace SceneResourceAllocator {

		template<typename T, size_t MAX_SIZE>
		struct AllocatorSlotPool
		{
			struct Slot
			{
				T data;
				bool used = false;
			};

			std::array<Slot, MAX_SIZE> pool{};
			std::vector<uint32_t> freeList;
			size_t count = 0;

			uint32_t Allocate(const T& value)
			{
				uint32_t idx;
				if (!freeList.empty())
				{
					idx = freeList.back();
					freeList.pop_back();
				}
				else
				{
					if (count >= MAX_SIZE)
						throw std::runtime_error("memory pool full");
					idx = static_cast<uint32_t>(count++);
				}

				pool[idx].data = value;
				pool[idx].used = true;
				return idx;
			}


			void Free(uint32_t idx)
			{
				if (idx >= MAX_SIZE || !pool[idx].used)
					throw std::runtime_error("[FREE-POOL]: Invalid pool free");
				pool[idx].used = false;
				freeList.push_back(idx);;
			}

			T& Get(uint32_t idx)
			{
				if (idx >= MAX_SIZE || !pool[idx].used)
					throw std::runtime_error("[GET-POOL]: Invalid pool index");
				return pool[idx].data;
			}

			std::vector<T*> GetAllocatedData()
			{
				std::vector<T*> temp;
				temp.reserve(count - freeList.size());
				for (uint32_t i = 0; i < count; i++)
					if (pool[i].used)
						temp.push_back(&pool[i].data);

				return temp;
			}
		};
	}


	class Scene
	{
	public:
		bool HasRoot() { return mRootNode.get(); }
		SceneNode* GetRootNode() {
			return mRootNode.get();
		}
		SceneNode* GetCameraNode();
		SceneNode* GetDirectionalLightNode();

		//Scene::Update generates SceneContext for update nodes 
		void Update(float delta_time, SceneContext& scene_ctx);
		//void Render(Vulkan::VulkanContext& vk_ctx, VkCommandPool cmd_pool);


		void SetRootNode(std::unique_ptr<SceneNode> p_node) {
			if (mRootNode)
				printf("BAD BAD; Need to clean up parent_node tree before set a new one ");

			mSceneResources.renderableComponents.reserve(10);

			mRootNode = std::move(p_node);
			mRootNode->mScene = this;
		}


		SceneResources mSceneResources;
	private:
		std::unique_ptr<SceneNode> mRootNode;




		SceneNode* mCacheMainCameraNode = nullptr;
		SceneNode* mCacheMainDirectionalLight = nullptr;


		//Global datas 
		std::unique_ptr<Vulkan::IRenderPass> mGlobalRenderPass;
		std::vector<Vulkan::DescriptorSetSpecification> mGlobalDescriptorSetSpecifications;
		std::vector<Vulkan::GraphicsPipeline> mGlobalGraphicsPipelines;


		std::vector<Vulkan::Resources::ImageSampler> mGlobalImageSamplers;

		//stack allocated memory
		SceneResourceAllocator::AllocatorSlotPool<RenderableComponent, 16> mRenderablePool{};
		SceneResourceAllocator::AllocatorSlotPool<Vulkan::Mesh, 10> mMeshPool{};
		SceneResourceAllocator::AllocatorSlotPool<Vulkan::PBRMaterial, 2> mPBRMaterialPool;
		SceneResourceAllocator::AllocatorSlotPool<SceneNode, 16> mFlatSceneNode;

		uint32_t AllocateRenderable(const RenderableComponent& src) {

			return mRenderablePool.Allocate(src);
		}

		uint32_t AllocateMesh(const Vulkan::Mesh& src) {
			return mMeshPool.Allocate(src);
		}

		uint32_t AllocateMaterial(const Vulkan::PBRMaterial& src) {
			return mPBRMaterialPool.Allocate(src);
		}

		uint32_t AllocateSceneNode(const SceneNode& src) {
			uint32_t idx = mFlatSceneNode.Allocate(src);
			//quick hack for now
			mSceneResources.sceneNodes = mFlatSceneNode.GetAllocatedData();
			return idx;
		}
		//hack for now; to test refactorise
	public:


		inline RenderableComponent& GetRenderableComponetByIdx(uint32_t idx) {
			return mRenderablePool.Get(idx);
		}
		inline Vulkan::Mesh* GetMeshByIdx(uint32_t idx){
			return &mMeshPool.Get(idx);
		}
		inline Vulkan::PBRMaterial& GetMaterialByIdx(uint32_t idx) {
			return mPBRMaterialPool.Get(idx);
		}
		inline SceneNode& GetSceneNodeByIdx(uint32_t idx) {
			return mFlatSceneNode.Get(idx);
		}

		static uint32_t CreateNewMaterial(Scene& scene, const Vulkan::PBRMaterial& data, SceneNode* node) {
			uint32_t idx = scene.AllocateMaterial(data);
			if (node)
			{
				if (node->HasRenderableComponent())
				{
					auto& render_comp = *node->GetRenderableComponent();
					render_comp.material = &scene.GetMaterialByIdx(idx);
				}
				else
				{
					throw std::runtime_error("Couldnt create a renderer component for material without a mesh");
				}
			}
			return idx;
		}

		static Vulkan::Mesh* CreateNewMesh(Scene& scene, const Vulkan::Mesh& data, SceneNode* node) {
			uint32_t idx = scene.AllocateMesh(data);
			if (node)
			{
				if (node->HasRenderableComponent())
				{
					auto& render_comp = *node->GetRenderableComponent();
					render_comp.mesh = scene.GetMeshByIdx(idx);
				}
				else
				{
					Vulkan::Mesh* mesh = scene.GetMeshByIdx(idx);
					AttachRenderableComponent(scene, *node, RenderableComponent(mesh, {}));
				}
			}
			return scene.GetMeshByIdx(idx);
		}

		static void AttachRenderableComponent(Scene& scene, SceneNode& node, const RenderableComponent& data) {
			if (node.mScene != &scene || node.idx == -1)
				std::cout << "Node might not be created this current scene or invalid!!!!!!\n";
			uint32_t idx = scene.AllocateRenderable(data);
			node.AttachRenderable(idx);
			scene.GetRenderableComponetByIdx(idx).renderPrimitiveData.GPUModel.transform = node.GetWorldTransform();
		}



		Vulkan::ForwardRenderPass* GetForwardRenderPass() {
			return dynamic_cast<Vulkan::ForwardRenderPass*>(mGlobalRenderPass.get());
		}
		void SetForwardRenderPass(std::unique_ptr<Vulkan::IRenderPass> p_render_pass) {
			if (mGlobalRenderPass)
				printf("BAD BAD; Need to clean up parent_node tree before set a new one ");

			mGlobalRenderPass = std::move(p_render_pass);
		}



		uint32_t AddDescriptorSetSpecification(Vulkan::DescriptorSetSpecification& desc_set_spec) {
			mGlobalDescriptorSetSpecifications.push_back(desc_set_spec);
			return static_cast<uint32_t>(mGlobalDescriptorSetSpecifications.size()) - 1;
		}
		Vulkan::DescriptorSetSpecification& GetDescriptorSetSpecificationByIdx(uint32_t idx) {
			return mGlobalDescriptorSetSpecifications[idx];
		}




		uint32_t AddGfxPipeline(Vulkan::GraphicsPipeline& pipelines) {
			mGlobalGraphicsPipelines.push_back(pipelines);
			return static_cast<uint32_t>(mGlobalGraphicsPipelines.size()) - 1;
		}
		Vulkan::GraphicsPipeline& GetGfxPipelineByIdx(uint32_t idx) {
			return mGlobalGraphicsPipelines[idx];
		}



		uint32_t AddImageSampler(Vulkan::Resources::ImageSampler& img_sampler) {
			mGlobalImageSamplers.push_back(img_sampler);
			return static_cast<uint32_t>(mGlobalImageSamplers.size()) - 1;
		}
		Vulkan::Resources::ImageSampler& GetImageSamplerByIdx(uint32_t idx) {
			return mGlobalImageSamplers[idx];
		}



		static SceneNode* CreateNewNode(Scene& scene, const SceneNode& data)
		{
			uint32_t idx = scene.AllocateSceneNode(data);
			//uint32_t idx = scene.AllocateSceneNode(std::move(data));
			SceneNode* node = &scene.GetSceneNodeByIdx(idx);
			node->idx = idx;
			node->mScene = &scene;
			scene.mSceneResources.renderableComponents.push_back(&node->mTransformComponent);//just a hack for ui remove later
			return node;
		}

		static SceneNode* AddChild(Scene& scene, SceneNode& parent_node, const SceneNode& child_data)
		{
			uint32_t idx = scene.AllocateSceneNode(child_data);
			SceneNode* child = &scene.GetSceneNodeByIdx(idx);
			child->idx = idx;
			parent_node.mScene = &scene;
			parent_node.mParent = parent_node.idx;
			scene.mSceneResources.renderableComponents.push_back(&child->mTransformComponent);//just a hack for ui remove late
		
			return child;
		}



		static void AttachChild(Scene& scene, SceneNode& parent, SceneNode& child)
		{
			if (child.mScene != &scene)
				throw std::runtime_error("Child not in this scene!!!!!");

			child.mParent = parent.idx;
			parent.mChildren.push_back(child.idx);
		}


	};







} //# Fluxon


