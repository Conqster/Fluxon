#include "Scene.h"



namespace Fluxon {

	void Scene::Update(float delta_time, SceneContext& scene_ctx)
	{
		//current passes
		//only one passes required : forward pass
		auto& render_pass_bucket = scene_ctx.renderPassBuckets[0];
		render_pass_bucket.type = ERenderPassType::RenderPass_Forward;
		render_pass_bucket.pass = GetForwardRenderPass();

		RenderPassBucket::PassPipeline model_pipeline_pass{};
		model_pipeline_pass.pipeline = &mGlobalGraphicsPipelines[0];
		model_pipeline_pass.descSetSpecifications.push_back(&mGlobalDescriptorSetSpecifications[2]);
		model_pipeline_pass.descSetSpecifications.push_back(&mGlobalDescriptorSetSpecifications[0]);
		model_pipeline_pass.descSetSpecifications.push_back(&mGlobalDescriptorSetSpecifications[1]);
		//render_pass_bucket.pipelines = mGlobalGraphicsPipelines;
		render_pass_bucket.descSetSpecifications = mGlobalDescriptorSetSpecifications;

		//skybox pipeline 
		RenderPassBucket::PassPipeline sky_pipeline_pass{};
		sky_pipeline_pass.pipeline = &mGlobalGraphicsPipelines[1];
		sky_pipeline_pass.descSetSpecifications.push_back(&mGlobalDescriptorSetSpecifications[0]);
		sky_pipeline_pass.descSetSpecifications.push_back(&mGlobalDescriptorSetSpecifications[1]);

		render_pass_bucket.pipelines =
		{
			model_pipeline_pass,
			sky_pipeline_pass
		};

		//gether pipelines, descriptorsetspecification and ovbject
		//for Nodes with RenderableComponets
		//now thinking of it if all renderable componet points to the node 
		//then it would be easy to get the render by just looping through componenets bucket 
		// rather node and find if have
		//if (HasRoot())
		//	mRootNode->Update(scene_ctx);

		for (auto& node : mFlatSceneNode.GetAllocatedData())
		{
			if (node->HasRenderableComponent())
			{
				auto& render_comp = *node->GetRenderableComponent();

				if (render_comp.visible)
				{
					render_comp.renderPrimitiveData.GPUModel.transform = node->GetWorldTransform();


					render_comp.renderPrimitiveData.pushConstantSpecs = {
						{ VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(render_comp.renderPrimitiveData.GPUModel), sizeof(glm::vec4), (void*)&render_comp.new_mat_sys_hack},
						{VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(render_comp.renderPrimitiveData.GPUModel), (void*)&render_comp.renderPrimitiveData.GPUModel}
					};
			
					for (auto& buckets : scene_ctx.renderPassBuckets)
						buckets.renderObjects.push_back(render_comp.renderPrimitiveData);

				}
			}
		}

		//for (auto& render_comp : mRenderablePool.GetAllocatedData())
		//{
		//	//render_comp->renderPrimitiveData.GPUModel.transform = GetWorldTransform();
		//	render_comp->renderPrimitiveData.pushConstantSpecs = {
		//		{ VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(render_comp->renderPrimitiveData.GPUModel), sizeof(glm::vec4), (void*)&render_comp->new_mat_sys_hack},
		//		{VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(render_comp->renderPrimitiveData.GPUModel), (void*)&render_comp->renderPrimitiveData.GPUModel}
		//	};
		//	;
		//	for (auto& buckets : scene_ctx.renderPassBuckets)
		//		buckets.renderObjects.push_back(render_comp->renderPrimitiveData);

		//}


		//Add empty object for skybox  
		SceneRenderPrimitive skybox_render_obj{};
		skybox_render_obj.pipelineIdx = 1; //<---skybox pipelines
		skybox_render_obj.mesh = GetMeshByIdx(0); //<-- get a default cube 
		//skybox_render_obj.mesh = &mGlobalMeshes[0]; //<-- get a default cube 
		//skybox_render_obj.mesh = mGlobalMeshes[0].get(); //<-- get a default cube 
		skybox_render_obj.material = nullptr;
		skybox_render_obj.GPUModel.transform = glm::mat4(1.0f);

		for (auto& buckets : scene_ctx.renderPassBuckets)
			buckets.renderObjects.push_back(skybox_render_obj);


	}
} //# Fluxon