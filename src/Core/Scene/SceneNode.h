#pragma once
#include <glm/glm.hpp>
#include <memory>

#include <vector>
#include <vulkan/vulkan.h>

#include "ISceneComponent.h"

#include <string>


#define USE_EXPLICIT_COMPONENT 1

namespace Fluxon {

	struct SceneContext;
	class ISceneComponent;

	class SceneNode
	{
	public:
		SceneNode(std::string name = "unk") {
			mName = name;
			AddComponent(std::make_unique<TransformComponent>(mTransformComponent));
			mChildren.reserve(10);
		}
		~SceneNode() {}
		SceneNode(std::string name, glm::mat4& world_transform, int _idx = -1) {
			mName = name;
			mTransformComponent.transform = world_transform;
			AddComponent(std::make_unique<TransformComponent>(mTransformComponent));
			mChildren.reserve(10);
			idx = _idx;
		}
		virtual glm::mat4 GetWorldTransform() const {
			return mTransformComponent.transform;
		}
		 void AddComponent(std::unique_ptr<ISceneComponent> p_comp);
		 //void AddChild(std::unique_ptr<SceneNode> p_child);
		 void AddChild(SceneNode* p_child);
		 std::vector<ISceneComponent*>& GetComponents() {
			 return mComponents;
		 }

		 void Update(SceneContext& scene_ctx);
	
		class Scene* mScene = nullptr;

		bool HasRenderableComponent() const {
			return mRenderableComponentIdx.has_value();
		}
		RenderableComponent* GetRenderableComponent();

		TransformComponent* GetTransformComponent() {
			return &mTransformComponent;
		}

		const TransformComponent* GetTransformComponent() const{
			return &mTransformComponent;
		}

		void AttachRenderable(uint32_t idx) {
			mRenderableComponentIdx = idx;
		}

		const std::string& Name() const {
			return mName;
		}

		 std::string& Name()  {
			return mName;
		}

		int32_t GetIdx() const {
			return idx;
		}

		int32_t GetParentIdx() const {
			return mParent;
		}

		void SetParentIdx(uint32_t idx) {
			mParent = idx;
		}

		const std::vector<uint32_t>& GetChildren() const {
			return mChildren;
		}

		void AddChild(uint32_t idx) {
			mChildren.push_back(idx);
		}

		const int32_t GetRenderableComponentIdx() const {
			return (HasRenderableComponent()) ? mRenderableComponentIdx.value() : -1;
		}
	private:
		std::string mName;
		uint32_t idx = -1;
		TransformComponent mTransformComponent;
		uint32_t mParent = -1;
		std::optional<uint32_t> mRenderableComponentIdx;

		friend Scene;

		//std::vector<std::unique_ptr<ISceneComponent>> mComponents;
		std::vector<ISceneComponent*> mComponents;
		std::vector<uint32_t> mChildren;

	};

} //# Fluxon