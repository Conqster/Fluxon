#include "SceneNode.h"
#include "ISceneComponent.h"

#include "Scene.h"
namespace Fluxon {
	void SceneNode::AddComponent(std::unique_ptr<ISceneComponent> p_comp)
	{
		mComponents.push_back(std::move(p_comp.get()));
		//mComponents[compCount++] = std::move(p_comp);
	}
	//void SceneNode::AddChild(std::unique_ptr<SceneNode> p_child)
	void SceneNode::AddChild(SceneNode* p_child)
	{



		p_child->mParent = idx;
		p_child->mScene = mScene;
		//mScene->mSceneResources.renderableComponents.push_back(&p_child->mTransformComponent);
		mScene->mSceneResources.renderableComponents.push_back(&p_child->mTransformComponent);
		mChildren.push_back(p_child->idx);
	}
	void SceneNode::Update(SceneContext& scene_ctx)
	{
		//RenderableComponet
#if USE_EXPLICIT_COMPONENT
		if (HasRenderableComponent())
		{
			auto render_comp = GetRenderableComponent();
			if (render_comp)
			{
				if (render_comp->mesh->GetVertexBuffer().buffer == VK_NULL_HANDLE ||
					render_comp->mesh->GetVertexBuffer().memory == VK_NULL_HANDLE ||
					render_comp->mesh->GetIndexBuffer().memory == VK_NULL_HANDLE ||
					render_comp->mesh->GetIndexBuffer().buffer == VK_NULL_HANDLE)
					return;

				render_comp->renderPrimitiveData.GPUModel.transform = GetWorldTransform();
				render_comp->renderPrimitiveData.pushConstantSpecs = {
					{ VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(render_comp->renderPrimitiveData.GPUModel), sizeof(glm::vec4), (void*)&render_comp->new_mat_sys_hack},
					{VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(render_comp->renderPrimitiveData.GPUModel), (void*)&render_comp->renderPrimitiveData.GPUModel}
				};

				for (auto& buckets : scene_ctx.renderPassBuckets)
					buckets.renderObjects.push_back(render_comp->renderPrimitiveData);

			}
			else
				std::cout << "Has renderable not cant access!!!!!!\n";
		}
#else
		for (auto& component : mComponents)
		{
			//if (component->GetType() == ESceneComponentType::SCT_Camera)
			//{
			//	auto cam_comp = dynamic_cast<CameraComponent*>(component.get());
			//	if (cam_comp)
			//		cam_comp->cameraData->position = GetWorldTransform()[3];
			//}



			auto render_comp = dynamic_cast<RenderableComponent*>(component.get());
			if (render_comp && render_comp->visible && render_comp->mesh)
			{
				//add gfx pipelines, if not exist 
				//add desc_set_spec, if not exist 
				//add not

				if (render_comp->mesh->GetVertexBuffer().buffer == VK_NULL_HANDLE ||
					render_comp->mesh->GetVertexBuffer().memory == VK_NULL_HANDLE ||
					render_comp->mesh->GetIndexBuffer().memory == VK_NULL_HANDLE ||
					render_comp->mesh->GetIndexBuffer().buffer == VK_NULL_HANDLE)
					return;


				//render_comp->renderModelData.transform = GetWorldTransform();
				//render_comp->renderModelData.colour = glm::vec4(0.0f, 0.0f, 0.0f, (bool)render_comp->new_mat_sys);

				render_comp->renderData.GPUModel.transform = GetWorldTransform();
				//render_comp->renderPrimitiveData.GPUModel.colour = glm::vec3(0.3f);

				//SceneRenderPrimitive render_obj{};// = render_comp->renderPrimitiveData;
				//render_obj.pipelineIdx = 0; //<---model pipelines
				//render_obj.mesh = render_comp->mesh;
				//render_obj.material = render_comp->material;
				//render_obj.GPUModel.transform = GetWorldTransform();
				////hack 
				//render_obj.GPUModel.colour = glm::vec4(0.0f,0.0f, 0.0f, (bool)render_comp->new_mat_sys);
				//render_obj.pushConstantSpecs = {
				//	{ VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(render_obj.GPUModel), sizeof(glm::vec4), (void*)&render_comp->renderModelData.colour},
				//	{VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(render_obj.GPUModel), (void*)&render_comp->renderModelData}
				//};
				for (auto& buckets : scene_ctx.renderPassBuckets)
					buckets.renderObjects.push_back(render_comp->renderData);
			}
		}
#endif // USE_EXPLICIT_COMPONENT

		





		//for (auto& child : mChildren)
			//child->Update(scene_ctx);
	}
	RenderableComponent* SceneNode::GetRenderableComponent()
	{
		if (!mScene || !mRenderableComponentIdx.has_value())
			return nullptr;
		return &mScene->GetRenderableComponetByIdx(mRenderableComponentIdx.value());
	}
}///# Fl;uxon