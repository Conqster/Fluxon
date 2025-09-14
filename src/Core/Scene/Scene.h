#pragma once
#include "SceneNode.h"
#include <memory>

#include "Renderer/Render Passes/ForwardRenderPass.h"
#include "Renderer/Render Passes/RenderPass.h"

#include <glm/glm.hpp>
#include "Renderer/GPUStrutures.h"

#include "Renderer/Pipelines/GraphicsPipeline.h"
#include "Renderer/Mesh.h"
#include "Resources/MaterialResource.h"

#include "SceneData.h"

#include "Resources/MeshResource.h"

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
		Scene() {
			mName.reserve(128);
		}
		bool HasRoot() { return mRootNode; }
		SceneNode* GetRootNode() {
			return mRootNode;
		}
		SceneNode* GetCameraNode() {
			return mCacheMainCameraNode;
		}
		//SceneNode* GetDirectionalLightNode();

		//Scene::Update generates SceneContext for update nodes 
		void Update(float delta_time, SceneContext& scene_ctx);
		//void Render(Vulkan::VulkanContext& vk_ctx, VkCommandPool cmd_pool);


		void SetNewRootNode(const SceneNode& root_node_data) {
			if (mRootNode)
				printf("BAD BAD; Need to clean up parent_node tree before set a new one ");

			mSceneResources.renderableComponents.reserve(10);

			//mRootNode = std::move(p_node);
			//mRootNode->mScene = this;

			mRootNode = CreateNewNode(*this, root_node_data);
		}


		void SetCameraNode(SceneNode* cam_node) {
			mCacheMainCameraNode = cam_node;
		}

		void SetRootNode(SceneNode* root_node) {
			if (mRootNode)
				printf("BAD BAD; Need to clean up parent_node tree before set a new one ");

			mSceneResources.renderableComponents.reserve(10);

			//mRootNode = std::move(p_node);
			//mRootNode->mScene = this;

			mRootNode = root_node;
		}

		void DestroyScene(Vulkan::VulkanContext* vk_ctx)
		{
			for (auto& pipelines : mGlobalGraphicsPipelines)
				pipelines.Cleanup(vk_ctx);

			mGlobalRenderPass.get()->Cleanup(vk_ctx);

			for (auto& mesh : GetAllocatedMeshResources())
				//mesh->gpuMesh.Cleanup();
				MeshResource::DestroyMeshResource(vk_ctx->device, *mesh);


			for (auto& image_sampler : mGlobalImageSamplers)
				Vulkan::Resources::DestoryTextureImageSampler(vk_ctx->device, image_sampler);
			
			
			for (auto& mat : GetAllocatedMaterialResources())
				MaterialResource::DestroyMaterialResource(vk_ctx, *mat);


			for (auto& desc_set_spec : mGlobalDescriptorSetSpecifications)
				vkDestroyDescriptorSetLayout(vk_ctx->device, desc_set_spec.layoutHandle, nullptr);
		}


		SceneResources mSceneResources;
	private:
		SceneNode* mRootNode = nullptr;




		SceneNode* mCacheMainCameraNode = nullptr;
		SceneNode* mCacheMainDirectionalLight = nullptr;


		//Global datas 
		std::unique_ptr<Vulkan::RenderPass> mGlobalRenderPass;
		std::vector<Vulkan::DescriptorSetSpecification> mGlobalDescriptorSetSpecifications;
		std::vector<Vulkan::GraphicsPipeline> mGlobalGraphicsPipelines;


		std::vector<Vulkan::Resources::ImageSampler> mGlobalImageSamplers;

		//stack allocated memory
		SceneResourceAllocator::AllocatorSlotPool<RenderableComponent, 16> mRenderablePool{};
		//SceneResourceAllocator::AllocatorSlotPool<Vulkan::Mesh, 10> mMeshPool{};
		SceneResourceAllocator::AllocatorSlotPool<MeshResource, 13> mMeshPool{};
		//SceneResourceAllocator::AllocatorSlotPool<Vulkan::PBRMaterial, 2> mPBRMaterialPool;
		SceneResourceAllocator::AllocatorSlotPool<MaterialResource, 5> mPBRMaterialPool;
		SceneResourceAllocator::AllocatorSlotPool<SceneNode, 16> mFlatSceneNode;

		uint32_t AllocateRenderable(const RenderableComponent& src) {

			uint32_t idx = mRenderablePool.Allocate(src);;
			RenderableComponent& comp = mRenderablePool.Get(idx);
			comp.id = idx;
			return idx;
		}

		uint32_t AllocateMesh(const MeshResource& src) {
			return mMeshPool.Allocate(src);
		}

		uint32_t AllocateMaterial(const MaterialResource& src) {
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

		std::string mName = "test test";

		inline RenderableComponent& GetRenderableComponetByIdx(uint32_t idx) {
			return mRenderablePool.Get(idx);
		}
		inline MeshResource* GetMeshByIdx(uint32_t idx){
			return &mMeshPool.Get(idx);
		}
		inline MaterialResource& GetMaterialByIdx(uint32_t idx) {
			return mPBRMaterialPool.Get(idx);
		}
		inline SceneNode& GetSceneNodeByIdx(uint32_t idx) {
			return mFlatSceneNode.Get(idx);
		}


		std::vector<SceneNode*> GetAllocatedSceneNodes(){
			return mFlatSceneNode.GetAllocatedData();
		}

		std::vector<RenderableComponent*> GetAllocatedRenderableComponent() {
			return mRenderablePool.GetAllocatedData();
		}

		std::vector<MeshResource*> GetAllocatedMeshResources() {
			return mMeshPool.GetAllocatedData();
		}

		std::vector<MaterialResource*> GetAllocatedMaterialResources() {
			return mPBRMaterialPool.GetAllocatedData();
		}

		//static uint32_t CreateNewMaterial(Scene& scene, const Vulkan::PBRMaterial& data, SceneNode* node) {
		
		//This allocated the memory for material on in scene, and try to perform gpu loading
		static uint32_t CreateNewMaterial(Scene& scene, SceneNode* node, Vulkan::VulkanContext* vk_ctx, std::array<std::string, Vulkan::SHADER_PBR_MATERIAL_MAX_MAPS> pbr_mat_paths, VkFormat tex_image_format = VK_FORMAT_R8G8B8A8_SRGB, bool enable_anisotropu_sampling = true) {


			VkCommandPool cmd_pool = vk_ctx->transientCmdPool;
			MaterialResource resource;
			resource.materialDesc = {
				-1,
				pbr_mat_paths,
				tex_image_format,
				enable_anisotropu_sampling
			};
			uint32_t idx = scene.AllocateMaterial(resource);

			resource = scene.GetMaterialByIdx(idx);
			resource.materialDesc.id = idx;

			//load gpu data
			if(resource.gpuMaterial.albedo.image == VK_NULL_HANDLE)
				MaterialResource::CreatePBRMaterialResource(vk_ctx, cmd_pool, scene.GetMaterialByIdx(idx));
			if (node)
			{
				if (node->HasRenderableComponent())
				{
					auto& render_comp = *node->GetRenderableComponent();
					//render_comp.material = &scene.GetMaterialByIdx(idx);
					render_comp.material = &scene.GetMaterialByIdx(idx);
				}
				else
				{
					throw std::runtime_error("Couldnt create a renderer component for material without a mesh");
				}
			}
			return idx;
		}

		//static uint32_t CreateNewMaterial(Scene& scene, const MaterialResource& data, SceneNode* node) {
		//	uint32_t idx = scene.AllocateMaterial(data);
		//	if (node)
		//	{
		//		if (node->HasRenderableComponent())
		//		{
		//			auto& render_comp = *node->GetRenderableComponent();
		//			//render_comp.material = &scene.GetMaterialByIdx(idx);
		//			render_comp.material = &scene.GetMaterialByIdx(idx);
		//		}
		//		else
		//		{
		//			throw std::runtime_error("Couldnt create a renderer component for material without a mesh");
		//		}
		//	}
		//	return idx;
		//}

		static uint32_t CreateNewMaterial(Scene& scene, Vulkan::VulkanContext* vk_ctx, VkCommandPool cmd_pool, const MaterialResource& data) {
			uint32_t idx = scene.AllocateMaterial(data);
			MaterialResource::CreatePBRMaterialResource(vk_ctx, cmd_pool, scene.GetMaterialByIdx(idx));
			return idx;
		}

		//static Vulkan::Mesh* CreateNewMesh(Scene& scene, const Vulkan::Mesh& data, SceneNode* node) {
		//	uint32_t idx = scene.AllocateMesh(data);
		//	if (node)
		//	{
		//		if (node->HasRenderableComponent())
		//		{
		//			auto& render_comp = *node->GetRenderableComponent();
		//			render_comp.mesh = scene.GetMeshByIdx(idx);
		//		}
		//		else
		//		{
		//			Vulkan::Mesh* mesh = scene.GetMeshByIdx(idx);
		//			AttachRenderableComponent(scene, *node, RenderableComponent(mesh, {}));
		//		}
		//	}
		//	return scene.GetMeshByIdx(idx);
		//}



		static uint32_t CreateMesh(Scene& scene, const Vulkan::VulkanContext* vk_ctx, const EPrimitiveMeshType primitive_type, const std::string& file_path = "") {

			VkCommandPool cmd_pool = vk_ctx->transientCmdPool;
			switch (primitive_type)
			{
			case Fluxon::EPrimitiveMeshType::None:
				break;
			case Fluxon::EPrimitiveMeshType::ModelMesh:
				return CreateModel(scene, vk_ctx, cmd_pool, file_path);
				break;
			case Fluxon::EPrimitiveMeshType::Cube:
				return CreateCube(scene, vk_ctx, cmd_pool);
				break;
			case Fluxon::EPrimitiveMeshType::Sphere:
				return CreateSphere(scene, vk_ctx, cmd_pool);
				break;
			case Fluxon::EPrimitiveMeshType::Plane:
				return CreatePlane(scene, vk_ctx, cmd_pool);
				break;
			default:
				std::cout << "Invalid / not supported Mesh Primitive Type";
				return -1;
				break;
			}

			std::cout << "Invalid / not supported Mesh Primitive Type";
			return 0;
		}

		static uint32_t CreateCube(Scene& scene, const Vulkan::VulkanContext* vk_ctx, VkCommandPool cmd_pool) {
			uint32_t idx = scene.AllocateMesh({}); // <-- allocates memory
			MeshResource* mesh_reso = scene.GetMeshByIdx(idx);
			mesh_reso->descriptor.id = idx;
			//mesh_reso->CreateNewCube(vk_ctx, cmd_pool, *mesh_reso);
			MeshResource::CreateNewCube(vk_ctx, cmd_pool, *mesh_reso);
			return idx;
		}

		static uint32_t CreateSphere(Scene& scene, const Vulkan::VulkanContext* vk_ctx, VkCommandPool cmd_pool) {
			uint32_t idx = scene.AllocateMesh({}); // <-- allocates memory
			MeshResource* mesh_reso = scene.GetMeshByIdx(idx);
			mesh_reso->descriptor.id = idx;
			//mesh_reso->CreateNewCube(vk_ctx, cmd_pool, *mesh_reso);
			MeshResource::CreateNewSphere(vk_ctx, cmd_pool, *mesh_reso);
			return idx;
		}

		static uint32_t CreatePlane(Scene& scene, const Vulkan::VulkanContext* vk_ctx, VkCommandPool cmd_pool) {
			uint32_t idx = scene.AllocateMesh({}); // <-- allocates memory
			MeshResource* mesh_reso = scene.GetMeshByIdx(idx);
			mesh_reso->descriptor.id = idx;
			//mesh_reso->CreateNewCube(vk_ctx, cmd_pool, *mesh_reso);
			MeshResource::CreateNewPlane(vk_ctx, cmd_pool, *mesh_reso);
			return idx;
		}

		static uint32_t CreateModel(Scene& scene, const Vulkan::VulkanContext* vk_ctx, VkCommandPool cmd_pool, const std::string& file_path) {
			uint32_t idx = scene.AllocateMesh({}); // <-- allocates memory
			MeshResource* mesh_reso = scene.GetMeshByIdx(idx);
			mesh_reso->descriptor.id = idx;
			//mesh_reso->CreateNewCube(vk_ctx, cmd_pool, *mesh_reso);
			MeshResource::CreateNewModel(vk_ctx, cmd_pool, *mesh_reso, file_path);
			return idx;
		}

		static void AttachRenderableComponent(Scene& scene, SceneNode& node, RenderableComponent& data) {
			if (node.mScene != &scene || node.idx == -1)
				std::cout << "Node might not be created this current scene or invalid!!!!!!\n";

			//hack 
			// check renderable component datas(mesh, material)
			if (data.meshIdx.has_value() && data.mesh == nullptr)
				data.mesh = scene.GetMeshByIdx(data.meshIdx.value());
			if (data.materialIdx.has_value())
				data.material = &scene.GetMaterialByIdx(data.materialIdx.value());

			if (data.meshIdx.has_value() || data.materialIdx.has_value())
				data.UpdateGPUParameters();

			uint32_t idx = scene.AllocateRenderable(data);
			node.AttachRenderable(idx);
			scene.GetRenderableComponetByIdx(idx).renderPrimitiveData.GPUModel.transform = node.GetWorldTransform();
		}

		static void CreateNewRenderableComponent(Scene& scene, RenderableComponent& data){
			//scene.AllocateRenderable(data);



			////hack 
			// check renderable component datas(mesh, material)
			if (data.meshIdx.has_value() && data.mesh == nullptr)
				data.mesh = scene.GetMeshByIdx(data.meshIdx.value());
			if (data.materialIdx.has_value())
				data.material = &scene.GetMaterialByIdx(data.materialIdx.value());

			if (data.meshIdx.has_value() || data.materialIdx.has_value())
				data.UpdateGPUParameters();

			uint32_t idx = scene.AllocateRenderable(data);
			//quick hack
			scene.GetRenderableComponetByIdx(idx).renderPrimitiveData.GPUModel.transform = scene.GetSceneNodeByIdx(idx).GetWorldTransform();
		}



		Vulkan::RenderPass* GetForwardRenderPass() {
			return dynamic_cast<Vulkan::RenderPass*>(mGlobalRenderPass.get());
		}
		void SetForwardRenderPass(std::unique_ptr<Vulkan::RenderPass> p_render_pass) {
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

		static SceneNode* CreateNewNode(Scene& scene, uint32_t parent_node, const SceneNode& child_data)
		{
			uint32_t idx = scene.AllocateSceneNode(child_data);
			SceneNode* child = &scene.GetSceneNodeByIdx(idx);
			child->idx = idx;
			child->mScene = &scene;
			//parent_node.mScene = &scene;
			child->mParent = parent_node;
			scene.mSceneResources.renderableComponents.push_back(&child->mTransformComponent);//just a hack for ui remove late

			return child;
		}

		static SceneNode* AddChild(Scene& scene, SceneNode& parent_node, const SceneNode& child_data)
		{
			uint32_t idx = scene.AllocateSceneNode(child_data);
			SceneNode* child = &scene.GetSceneNodeByIdx(idx);
			child->idx = idx;
			//parent_node.mScene = &scene;
			child->mScene = &scene;
			//child->mParent = 
			parent_node.mParent = parent_node.idx;
			scene.mSceneResources.renderableComponents.push_back(&child->mTransformComponent);//just a hack for ui remove late
		
			return child;
		}

		static SceneNode* AddChild(Scene& scene, uint32_t parent_node, const SceneNode& child_data)
		{
			uint32_t idx = scene.AllocateSceneNode(child_data);
			SceneNode* child = &scene.GetSceneNodeByIdx(idx);
			child->idx = idx;
			child->mScene = &scene;
			//parent_node.mScene = &scene;
			child->mParent = parent_node;
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


