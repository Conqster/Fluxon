#include "SceneRuntime.h"
#include "Scene/ISceneComponent.h"

#include "Renderer/Render Passes/ForwardRenderPass.h"

#include "Utilities.h"
#include "Resources/ModelLoader.h"

#include "Editor/MainSceneEditor.h"

#include "Renderer/Render Passes/ComputePass.h"

#include "Renderer/Render Passes/RenderPass.h"

#define LOAD_SCENE_FROM_FILE 1


namespace Fluxon {


	void SceneRuntime::Init(Vulkan::VulkanContext* gfx_ctx, Camera* app_cam_ref)
	{
		mVulkanContext = gfx_ctx;
		
		mVulkanContext->swapchain.get()->RegisterOnResizeEvent([&](uint32 w, uint32 h) {ResizeFunction(w, h); });

		mWindow = gfx_ctx->pMainPlatformWindow;

		if (app_cam_ref)
			mCamera = app_cam_ref;


		//Vulkan::VulkanContextSpecification vk_ctx_spec{};
		//vk_ctx_spec.bEnableFragmentStoresAndAtomics = true;
		//vk_ctx_spec.debug_callback = Vulkan::Utils::DebugCallback;
		//mVulkanContext = Vulkan::VulkanContext::CreateVulkanContext(&window, vk_ctx_spec);


		/////////////////////
		// TEST NEW RENDERPASS 
		/////////////////////
		Vulkan::RenderPassSpecification renderpass_spec = Vulkan::GenerateForwardPassSpecification(gfx_ctx, mWindow->width, mWindow->height);
		//Vulkan::RenderPass forward_pass;


		mScene.SetForwardRenderPass(std::make_unique<Vulkan::RenderPass>());
		Vulkan::RenderPass* forward_render_pass = mScene.GetForwardRenderPass();
		//forward_render_pass->Init(mVulkanContext);
		forward_render_pass->Init(gfx_ctx, renderpass_spec);
		//forward_render_pass->Init(mVulkanContext, mVulkanContext->transientCmdPool, mWindow->width, mWindow->height);

		mShadowGfxPass.Init(mVulkanContext, mVulkanContext->transientCmdPool);

		mSceneCollsionVoxelPass.Init(mVulkanContext, mVulkanContext->transientCmdPool, mCamera);





#pragma region New DescriptorSet

#pragma endregion //New DescriptorSet

		


		

#if LOAD_SCENE_FROM_FILE
		//mScene.mSceneResources.camera = static_cast<EditorCamera*>(app_cam_ref);
		////quick hack for loading
		//mScene.mSceneResources.directionalLight = &mDirLight;
		//SceneSerialiser scene_serialiser;
		//scene_serialiser.LoadScene(mVulkanContext, mFrames[0].commandPool, "assets/scenes/experiment.fluxon", mScene);
		//mScene.mSceneResources.instantSceneName = &mScene.mName;
		LoadSceneFromFile(mVulkanContext, "assets/scenes/experiment.fluxon", mScene, app_cam_ref);
#else
		CreateSceneFromCode();
#endif // LOAD_SCENE_FROM_FILE



		//TODO(Conqster-Uniformbuffer): have functionm to create new description set layout dynamically 
		uint32_t model_desc_set_idx  = mScene.AddDescriptorSetSpecification(Vulkan::DescriptorSetSpecification());
		uint32_t skybox_desc_set_idx = mScene.AddDescriptorSetSpecification(Vulkan::DescriptorSetSpecification());
		uint32_t matertial_desc_set_idx;// = mScene.AddDescriptorSetSpecification({});
		CreateSkyboxDescriptionSetLayout(mVulkanContext->device, mScene.GetDescriptorSetSpecificationByIdx(skybox_desc_set_idx));
		CreateModelSceneDescriptionSetLayout(mVulkanContext, mScene.GetDescriptorSetSpecificationByIdx(model_desc_set_idx));

		mSceneModelDescSetIdx = model_desc_set_idx;

		std::array<const char*, Vulkan::SHADER_PBR_MATERIAL_MAX_MAPS> old_pbr_mat_paths =
		{
			"assets/textures/rusty_metal/rusty_metal_diff.jpg",
			"assets/textures/rusty_metal/rusty_metal_nor.jpg",
			"assets/textures/rusty_metal/rusty_metal_arm_not2.jpg",
		};


		std::array<std::string, Vulkan::SHADER_PBR_MATERIAL_MAX_MAPS> pbr_mat_paths =
		{
			"assets/textures/worn_bronze_metal/2K/worn_bronze_metal_base_colour.jpg",
			"assets/textures/worn_bronze_metal/2K/worn_bronze_metal_normal.jpg",
			"assets/textures/worn_bronze_metal/2K/worn_bronze_metal_orm.jpg",
		};


#if LOAD_SCENE_FROM_FILE
		uint32_t pbr_mat_idx = 0;
#else
		//uint32_t pbr_mat_idx = Scene::CreateNewMaterial(mScene, Vulkan::PBRMaterial::CreatePBRMaterialMaps(mVulkanContext, mFrames[0].commandPool, pbr_mat_paths), nullptr);
		//uint32_t pbr_mat_idx = Scene::CreateNewMaterial(mScene, MaterialResource::CreatePBRMaterialResource(mVulkanContext, mFrames[0].commandPool, pbr_mat_paths), nullptr);
		uint32_t pbr_mat_idx = Scene::CreateNewMaterial(mScene, nullptr, mVulkanContext, mFrames[0].commandPool, pbr_mat_paths);
#endif // LOAD_SCENE_FROM_FILE

		//Vulkan::PBRMaterial::CreatePBRMaterialMaps(mVulkanContext, mRustyMetalPBRMat, mFrames[0].commandPool, pbr_mat_paths);
		//matertial_desc_set_idx = mScene.AddDescriptorSetSpecification(Vulkan::PBRMaterial::GenerateMaterialDecriptorSetLayoutSpecification(mVulkanContext, mScene.GetPBRMaterialByIdx(pbr_mat_idx), 0, 0));
		//matertial_desc_set_idx = mScene.AddDescriptorSetSpecification(Vulkan::PBRMaterial::GenerateMaterialDecriptorSetLayoutSpecification(mVulkanContext, mScene.GetMaterialByIdx(pbr_mat_idx), 0, 0));
		matertial_desc_set_idx = mScene.AddDescriptorSetSpecification(Vulkan::PBRMaterial::GenerateMaterialDecriptorSetLayoutSpecification(mVulkanContext, mScene.GetMaterialByIdx(pbr_mat_idx).gpuMaterial, 0, 0));
		//CreateDummyDescriptionSetLayout(mVulkanContext, mMaterialDescriptorLayout, 0);
		//CreateGraphicsPipeline();






#pragma region New DescriptorSet
		//this is common data descriptor
		Vulkan::DescriptorBindingInfo camera_ubo{};
		camera_ubo.name = "camera_ubo";
		camera_ubo.binding = 0;
		camera_ubo.stageFlag = VK_SHADER_STAGE_VERTEX_BIT;
		camera_ubo.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		camera_ubo.descType = Vulkan::EDescriptorType::UniformBuffer;
		camera_ubo.bufferSize = static_cast<uint32>(sizeof(GPUStructures::MVP_UniformBufferObject));

		Vulkan::DescriptorBindingInfo image_sampler{};
		image_sampler.name = "image_sampler";
		image_sampler.binding = 1;
		image_sampler.stageFlag = VK_SHADER_STAGE_FRAGMENT_BIT;
		image_sampler.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		image_sampler.descType = Vulkan::EDescriptorType::CombinedImageSampler;

		Vulkan::TextureSpecification image_sampler_spec{};
		image_sampler_spec.format = VK_FORMAT_R8G8B8A8_SRGB;
		image_sampler_spec.usage = ETextureUsageBits::Sampled;
		image_sampler_spec.imageDiscPaths = { "assets/textures/melady_square.jpg" };
		image_sampler_spec.bEnableSamplerAnisotropy = true;
		image_sampler_spec.imageAspect = VK_IMAGE_ASPECT_COLOR_BIT;
		image_sampler.tex_spec = image_sampler_spec;

		Vulkan::DescriptorBindingInfo directional_light_ubo{};
		directional_light_ubo.name = "directional_light_ubo";
		directional_light_ubo.binding = 2;
		directional_light_ubo.stageFlag = VK_SHADER_STAGE_FRAGMENT_BIT;
		directional_light_ubo.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		directional_light_ubo.descType = Vulkan::EDescriptorType::UniformBuffer;
		directional_light_ubo.bufferSize = static_cast<uint32>(sizeof(GPUStructures::DirectionalLight));

		Vulkan::DescriptorBindingInfo shadow_image_sampler_binding{};
		shadow_image_sampler_binding.name = "shadow_image_sampler";
		shadow_image_sampler_binding.binding = 3;
		shadow_image_sampler_binding.stageFlag = VK_SHADER_STAGE_FRAGMENT_BIT;
		shadow_image_sampler_binding.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		shadow_image_sampler_binding.descType = Vulkan::EDescriptorType::ExternalImageSampler;

		Vulkan::GPUImageSampler shadow_image_sampler{};
		Vulkan::RenderTarget& shadow_rt = mShadowGfxPass.GetShadowDepthRenderTarget();

		FLUX_DEBUG_LOG_INFO("Sampler: ",
			"\n\t gpu id: ", shadow_rt.sampler,
			", cpu address: ", &shadow_rt.sampler,"\n");
		shadow_image_sampler.image = shadow_rt.image;
		shadow_image_sampler.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		shadow_image_sampler.imageView = shadow_rt.imageView;
		shadow_image_sampler.sampler = shadow_rt.sampler;
		shadow_image_sampler_binding.externalImageSampler = shadow_image_sampler;// std::move(shadow_image_sampler);
		FLUX_DEBUG_LOG_INFO("Sampler: ",
			"\n\t gpu id: ", shadow_rt.sampler,
			", cpu address: ", &shadow_rt.sampler, "\n");

		//descriptor bindings for skybox 
		Vulkan::DescriptorBindingInfo skybox_cube_image{};
		skybox_cube_image.name = "image_sampler";
		skybox_cube_image.binding = 0;
		skybox_cube_image.stageFlag = VK_SHADER_STAGE_FRAGMENT_BIT;
		skybox_cube_image.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		skybox_cube_image.descType = Vulkan::EDescriptorType::CombinedImageSampler;

		Vulkan::TextureSpecification skybox_cube_texture_spec{};
		skybox_cube_texture_spec.format = VK_FORMAT_R8G8B8A8_SRGB;
		skybox_cube_texture_spec.usage = ETextureUsageBits::Sampled;
		skybox_cube_texture_spec.imageDiscPaths = {
			"assets/textures/default_skybox/right.jpg",	//+X
			"assets/textures/default_skybox/left.jpg",		//-X
			"assets/textures/default_skybox/top.jpg",		//+Y
			"assets/textures/default_skybox/bottom.jpg",	//-Y
			"assets/textures/default_skybox/front.jpg",	//+Z
			"assets/textures/default_skybox/back.jpg",		//-Z
		};
		skybox_cube_texture_spec.bEnableSamplerAnisotropy = true;
		skybox_cube_texture_spec.imageAspect = VK_IMAGE_ASPECT_COLOR_BIT;
		skybox_cube_texture_spec.type = ETextureType::TextureCube;
		skybox_cube_image.tex_spec = skybox_cube_texture_spec;// GPUStructures::SkyboxProperties



		Vulkan::DescriptorBindingInfo skybox_ubo_param{};
		skybox_ubo_param.name = "skybox_ubo_param";
		skybox_ubo_param.binding = 1;
		skybox_ubo_param.stageFlag = VK_SHADER_STAGE_FRAGMENT_BIT;
		skybox_ubo_param.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		skybox_ubo_param.descType = Vulkan::EDescriptorType::UniformBuffer;
		skybox_ubo_param.bufferSize = static_cast<uint32>(sizeof(GPUStructures::SkyboxProperties));

		/// 0 camera_ubo 0
		// 1 image_sampler (melady image) 1 inTexSampler
		// 2 directional_light_ubo 2 DirectionalLight
		// 3 shadow_image_sampler_binding 3 uShadowMap

		// 4 skybox_cube_image 0
		// 5 skybox_ubo_param 1

		//LAYOUTS 
		//a layout for a set
		Vulkan::DescriptorLayoutInfo model_desc_layout_info{};
		model_desc_layout_info.name = "model_layout";
		model_desc_layout_info.requiredBindings = { camera_ubo, image_sampler, directional_light_ubo, shadow_image_sampler_binding };
		//model_desc_layout_info.requiredDescriptorDataByIdx = { 0, 1, 2, 3 };

		Vulkan::DescriptorLayoutInfo skybox_desc_layout_info{};
		skybox_desc_layout_info.name = "skybox_layout";
		skybox_desc_layout_info.requiredBindings = { skybox_cube_image, skybox_ubo_param };
		//skybox_desc_layout_info.requiredDescriptorDataByIdx = { 4, 5 };

		Vulkan::DescriptorSetInfo model_desc_set_info{};
		model_desc_set_info.name = "CamAndModelSet";
		model_desc_set_info.set = 1;
		model_desc_set_info.requiredLayouts = { "model_layout" };

		Vulkan::DescriptorSetInfo skybox_desc_set_info{};
		skybox_desc_set_info.name = "SkyboxSet";
		skybox_desc_set_info.set = 2;
		skybox_desc_set_info.requiredLayouts = { "skybox_layout" };


		Vulkan::DescriptorSpecification desc_set_spec{};
		desc_set_spec.frameInFlight = 2;
		desc_set_spec.bTightFitDescriporPool = false;
		//desc_set_spec.descriptors = { camera_ubo, image_sampler, directional_light_ubo, shadow_image_sampler_binding, skybox_cube_image, skybox_ubo_param };
		desc_set_spec.layoutInfos = { model_desc_layout_info, skybox_desc_layout_info };
		desc_set_spec.setsInfos = { model_desc_set_info,  skybox_desc_set_info };


		mDescriptorSet.Create(mVulkanContext, desc_set_spec);

		mDescriptorSet.PushDescriptorSet("MaterialSet", 0, Vulkan::EDescriptorBuffering::Double);
		Vulkan::PBRMaterial& mat = mScene.GetMaterialByIdx(pbr_mat_idx).gpuMaterial;
		mDescriptorSet.PushTextureBinding("albedoMap", "MaterialSet", 0, mat.albedo, mat.imagelayout, EShaderStageBit::Fragment, Vulkan::EDescriptorType::ExternalImageSampler);
		mDescriptorSet.PushTextureBinding("normalMap", "MaterialSet", 1, mat.normal, mat.imagelayout, EShaderStageBit::Fragment, Vulkan::EDescriptorType::ExternalImageSampler);
		mDescriptorSet.PushTextureBinding("aoRMMap", "MaterialSet", 2, mat.aoRM, mat.imagelayout, EShaderStageBit::Fragment, Vulkan::EDescriptorType::ExternalImageSampler);
		mDescriptorSet.BakePendingWrites();

#pragma endregion //New DescriptorSet

		





#pragma region Generate pipelines from New vulkan Descriptor Set
		////push constant for model 
		VkPushConstantRange push_constant_range{};
		push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		push_constant_range.offset = 0; //<- i am presumiing that this is used if there is more than one push constant
		push_constant_range.size = sizeof(GPUStructures::Model);

		VkPushConstantRange push_constant_range2{};
		push_constant_range2.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		push_constant_range2.offset = sizeof(GPUStructures::Model);
		push_constant_range2.size = sizeof(glm::vec4);

		Vulkan::GraphicsPipelineSpecification model_gfx_pipeline_spec{};
		model_gfx_pipeline_spec.vertexShaderPath = "assets/shaders/spirvs/model_shading.vert.spv";
		model_gfx_pipeline_spec.fragmentShaderPath = "assets/shaders/spirvs/model_shading.frag.spv";
		model_gfx_pipeline_spec.renderPass = forward_render_pass->GetRenderPassHandle();
		model_gfx_pipeline_spec.descriptorSetLayouts = {
			mDescriptorSet.GetSetLayoutHandle("MaterialSet"), //<---  set 0 
			mDescriptorSet.GetSetLayoutHandle("CamAndModelSet"), //<---  set 1
			mDescriptorSet.GetSetLayoutHandle("SkyboxSet"), }; //<---  set 2
		model_gfx_pipeline_spec.pushConstantRanges = { push_constant_range, push_constant_range2 };
		////leave remain data as default 
		uint32_t model_gfx_pipeline_idx = mScene.AddGfxPipeline(Vulkan::GraphicsPipeline());
		mScene.GetGfxPipelineByIdx(model_gfx_pipeline_idx).Init(mVulkanContext, model_gfx_pipeline_spec);

		Vulkan::GraphicsPipelineSpecification skybox_gfx_pipeline_spec{};
		skybox_gfx_pipeline_spec.vertexShaderPath = "assets/shaders/spirvs/skybox.vert.spv";
		skybox_gfx_pipeline_spec.fragmentShaderPath = "assets/shaders/spirvs/skybox.frag.spv";
		skybox_gfx_pipeline_spec.renderPass = forward_render_pass->GetRenderPassHandle();
		skybox_gfx_pipeline_spec.descriptorSetLayouts = {
			mVulkanContext->dummyDescSetLayout, //mScene.GetDescriptorSetSpecificationByIdx(matertial_desc_set_idx).layoutHandle,  //<-------- Need to remove not but a desc set 0 is required so serve as dummy 
			mDescriptorSet.GetSetLayoutHandle("CamAndModelSet"), //<---  set 1
			mDescriptorSet.GetSetLayoutHandle("SkyboxSet"), }; //<---  set 2
		skybox_gfx_pipeline_spec.pushConstantRanges = { };

		skybox_gfx_pipeline_spec.rasterisationCullMode = VK_CULL_MODE_FRONT_BIT;
		skybox_gfx_pipeline_spec.bEnableDepthWrite = false;
		skybox_gfx_pipeline_spec.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		////leave remain data as default 
		uint32_t skybox_gfx_pipeline_idx = mScene.AddGfxPipeline(Vulkan::GraphicsPipeline());
		mScene.GetGfxPipelineByIdx(skybox_gfx_pipeline_idx).Init(mVulkanContext, skybox_gfx_pipeline_spec);

#pragma endregion //Generate pipelines from New vulkan Descriptor Set







		////push constant for model 
		//VkPushConstantRange push_constant_range{};
		//push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		//push_constant_range.offset = 0; //<- i am presumiing that this is used if there is more than one push constant
		//push_constant_range.size = sizeof(GPUStructures::Model);

		//VkPushConstantRange push_constant_range2{};
		//push_constant_range2.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		//push_constant_range2.offset = sizeof(GPUStructures::Model);
		//push_constant_range2.size = sizeof(glm::vec4);

		//Vulkan::GraphicsPipelineSpecification model_gfx_pipeline_spec{};
		//model_gfx_pipeline_spec.vertexShaderPath = "assets/shaders/spirvs/model_shading.vert.spv";
		//model_gfx_pipeline_spec.fragmentShaderPath = "assets/shaders/spirvs/model_shading.frag.spv";
		//model_gfx_pipeline_spec.renderPass = forward_render_pass->GetRenderPassHandle();
		//model_gfx_pipeline_spec.descriptorSetLayouts = {
		//	mScene.GetDescriptorSetSpecificationByIdx(matertial_desc_set_idx).layoutHandle,
		//	mScene.GetDescriptorSetSpecificationByIdx(model_desc_set_idx).layoutHandle,
		//	mScene.GetDescriptorSetSpecificationByIdx(skybox_desc_set_idx).layoutHandle };
		//model_gfx_pipeline_spec.pushConstantRanges = { push_constant_range, push_constant_range2 };
		//////leave remain data as default 
		//uint32_t model_gfx_pipeline_idx = mScene.AddGfxPipeline(Vulkan::GraphicsPipeline());
		//mScene.GetGfxPipelineByIdx(model_gfx_pipeline_idx).Init(mVulkanContext, model_gfx_pipeline_spec);

		//VkDescriptorSetLayout dummy_desc_set_layout = mVulkanContext->dummyDescSetLayout;
		//Vulkan::GraphicsPipelineSpecification skybox_gfx_pipeline_spec{};
		//skybox_gfx_pipeline_spec.vertexShaderPath = "assets/shaders/spirvs/skybox.vert.spv";
		//skybox_gfx_pipeline_spec.fragmentShaderPath = "assets/shaders/spirvs/skybox.frag.spv";
		//skybox_gfx_pipeline_spec.renderPass = forward_render_pass->GetRenderPassHandle();
		//skybox_gfx_pipeline_spec.descriptorSetLayouts = {
		//	dummy_desc_set_layout, //mScene.GetDescriptorSetSpecificationByIdx(matertial_desc_set_idx).layoutHandle,  //<-------- Need to remove not but a desc set 0 is required so serve as dummy 
		//	mScene.GetDescriptorSetSpecificationByIdx(model_desc_set_idx).layoutHandle,
		//	mScene.GetDescriptorSetSpecificationByIdx(skybox_desc_set_idx).layoutHandle };
		//skybox_gfx_pipeline_spec.pushConstantRanges = { };

		//skybox_gfx_pipeline_spec.rasterisationCullMode = VK_CULL_MODE_FRONT_BIT;
		//skybox_gfx_pipeline_spec.bEnableDepthWrite = false;
		//skybox_gfx_pipeline_spec.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		//////leave remain data as default 
		//uint32_t skybox_gfx_pipeline_idx = mScene.AddGfxPipeline(Vulkan::GraphicsPipeline());
		//mScene.GetGfxPipelineByIdx(skybox_gfx_pipeline_idx).Init(mVulkanContext, skybox_gfx_pipeline_spec);
		////mSkyboxGraphicsPipeline.Init(mVulkanContext, skybox_gfx_pipeline_spec);

#if LOAD_SCENE_FROM_FILE
#else
		//CreateTestSceneData(mVulkanContext, mFrames[0].commandPool);
#endif // LOAD_SCENE_FROM_FILE



		mFrames.resize(Vulkan::kMaxFramesInFlight);
		Vulkan::CreateFrameObjects(mVulkanContext, mFrames);

#pragma region old frame r3esource descriptor set setup

		std::vector<Vulkan::DescriptorSetSpecification> desc_set_layout_spec = {
			mScene.GetDescriptorSetSpecificationByIdx(matertial_desc_set_idx),
			mScene.GetDescriptorSetSpecificationByIdx(model_desc_set_idx),
			mScene.GetDescriptorSetSpecificationByIdx(skybox_desc_set_idx) };
		//Vulkan::GenerateFrameObjectDescriptorPoolSet(mVulkanContext->device, desc_set_layout_spec, mFrames);
		//TODO(Conqster-Uniformbuffer): have a system that inserts required buffer bindings
		// 
		//Vulkan::CreateFrameDatasUniformCameraBuffer(mVulkanContext, mFrames);
		//Vulkan::CreateFrameDatasDirectionalLightUniformbuffer(mVulkanContext, mFrames);

		//Scene per frame ubo 
		Vulkan::UniformBufferSpecification cam_ubo_spec{};
		cam_ubo_spec.type = Vulkan::GlobalUboType::Camera_UBO;
		cam_ubo_spec.set = 1;
		cam_ubo_spec.binding = 0;
		cam_ubo_spec.requiredSize = sizeof(GPUStructures::MVP_UniformBufferObject);

		Vulkan::UniformBufferSpecification dir_light_ubo_spec{};
		dir_light_ubo_spec.type = Vulkan::GlobalUboType::DirectionalLight_UBO;
		dir_light_ubo_spec.set = 1;
		dir_light_ubo_spec.binding = 2;
		dir_light_ubo_spec.requiredSize = sizeof(GPUStructures::DirectionalLight);

		Vulkan::UniformBufferSpecification skybox_ubo_spec{};
		skybox_ubo_spec.type = Vulkan::GlobalUboType::Skybox_UBO;
		skybox_ubo_spec.set = 2;
		skybox_ubo_spec.binding = 1;
		skybox_ubo_spec.requiredSize = sizeof(GPUStructures::SkyboxProperties);

		std::vector<Vulkan::UniformBufferSpecification> ubo_specs = { cam_ubo_spec, dir_light_ubo_spec, skybox_ubo_spec };
		//Vulkan::CreateAndAppendFrameDatasUniformBuffers(mVulkanContext, mFrames, ubo_specs);


		VkFormat tex_image_format = VK_FORMAT_R8G8B8A8_SRGB;
		uint32_t tex_image_sampler_idx = mScene.AddImageSampler(Vulkan::Resources::ImageSampler());
		uint32_t skybox_image_sampler_idx = mScene.AddImageSampler(Vulkan::Resources::ImageSampler());
		Vulkan::Resources::CreateImageSamplerTexture(mVulkanContext,
			mVulkanContext->transientCmdPool,
			//"textures/Manchester_512.png",
			"assets/textures/melady_square.jpg",
			tex_image_format,
			mScene.GetImageSamplerByIdx(tex_image_sampler_idx),
			mUseSamplingAnisotropy);


		Vulkan::DescriptorBinding texture_binding = Vulkan::DescriptorBinding::MakeImageSampler(1, 1, mScene.GetImageSamplerByIdx(tex_image_sampler_idx));

		std::vector<const char*> skybox_imgs = {
			"assets/textures/default_skybox/right.jpg",	//+X
			"assets/textures/default_skybox/left.jpg",		//-X
			"assets/textures/default_skybox/top.jpg",		//+Y
			"assets/textures/default_skybox/bottom.jpg",	//-Y
			"assets/textures/default_skybox/front.jpg",	//+Z
			"assets/textures/default_skybox/back.jpg",		//-Z

		};
		Vulkan::Resources::CreateImageSamplerTextureCube(mVulkanContext, mVulkanContext->transientCmdPool,
			skybox_imgs,
			tex_image_format,
			mScene.GetImageSamplerByIdx(skybox_image_sampler_idx),
			mUseSamplingAnisotropy);
		Vulkan::DescriptorBinding skybox_cube_tex_binding = Vulkan::DescriptorBinding::MakeImageSampler(2, 0, mScene.GetImageSamplerByIdx(skybox_image_sampler_idx));

		//auto pbr_material_tex_binding = Vulkan::PBRMaterial::GetDescriptorSetBindings(mScene.GetMaterialByIdx(pbr_mat_idx), 0);
		auto pbr_material_tex_binding = Vulkan::PBRMaterial::GetDescriptorSetBindings(mScene.GetMaterialByIdx(pbr_mat_idx).gpuMaterial, 0);

		Vulkan::DescriptorBinding shadow_map_binding = mShadowGfxPass.GetSamplerBinding(1, 3);


		//for (auto& frame : mFrames)
		//{
		//	for (auto& pbr_mat_binding : pbr_material_tex_binding)
		//		frame.AppendDescriptorBinding(pbr_mat_binding);
		//	frame.AppendDescriptorBinding(texture_binding);
		//	frame.AppendDescriptorBinding(skybox_cube_tex_binding);

		//	frame.AppendDescriptorBinding(shadow_map_binding);

		//	frame.WritePendingDescriptorBinding(mVulkanContext->device);

		//	//append Computer Descriptorsd

		//}

#pragma endregion






		mScene.mSceneResources.shadowPassDepthRT = &mShadowGfxPass.GetShadowDepthRenderTarget();
		mScene.mSceneResources.voxelDepthRT = &mSceneCollsionVoxelPass.GetDepthRenderTarget();
		mComputePass.Init(mVulkanContext, mVulkanContext->transientCmdPool, forward_render_pass->GetRenderPassHandle(), mScene.GetDescriptorSetSpecificationByIdx(model_desc_set_idx).layoutHandle);
		mScene.mSceneResources.particleEmitter = &mComputePass.mEmitter;


		mDebugLines.Init(mVulkanContext, forward_render_pass->GetRenderPassHandle(), mScene.GetDescriptorSetSpecificationByIdx(model_desc_set_idx).layoutHandle);
		//std::vector<VkDescriptorSetLayout>set_layout = { mDescriptorSet.GetSetLayoutHandle("CamAndModelSet") };
		//mDebugLines.Init(mVulkanContext, forward_render_pass->GetRenderPassHandle(), set_layout[0]);




		Vulkan::Resources::ImGui_Texture_Test::CreateTexture(mVulkanContext, mFrames[0].commandPool,
			//"textures/Manchester_512.png",
			"assets/textures/melady_square.jpg",
			VK_FORMAT_R8G8B8A8_SRGB
			/*VK_FORMAT_R8G8B8A8_UNORM*/, experiment_texture,
			mUseSamplingAnisotropy);
		mScene.mSceneResources.experiment_texture = &experiment_texture;
		mScene.mSceneResources.sceneRenderOutput = &mScene.GetForwardRenderPass()->GetOutputRT(0);


		VkPhysicalDeviceLimits device_limits = mVulkanContext->GetPhysicalDeviceLimits();
		std::cout << "max Framebuffer Layers: " <<  device_limits.maxFramebufferLayers <<"\n";
		std::cout << "SSBO storage limits: " <<  device_limits.maxStorageBufferRange <<"\n";
		FLUX_DEBUG_LOG_INFO("Per Stage Descriptor Storage Buffers limits: ", (int)device_limits.maxPerStageDescriptorStorageBuffers);
		FLUX_DEBUG_LOG_INFO("Descriptor Set Storage Buffers limits: ", (int)device_limits.maxDescriptorSetStorageBuffers);
		auto& wGC = device_limits.maxComputeWorkGroupCount;
		FLUX_DEBUG_LOG_INFO("okay, this uis just an info \n haha Compute Work Group \n\t count limit [1]: ", wGC[0], 
		", \n\t count limit [2]: ", wGC[1], 
		",\n\t count limit [3]: ", wGC[2]);
		FLUX_DEBUG_LOG_INFO("Compute Work Group innvocation Limit: ", (int)device_limits.maxComputeWorkGroupInvocations);
		auto& wGS = device_limits.maxComputeWorkGroupSize;
		FLUX_DEBUG_LOG_INFO("Compute Work Group innvocation Limit \n\t [1]: ", wGS[0], ", \n\t [2]: ", wGS[1], ",\n\t [3]: ", wGS[2]);


		Vulkan::RenderTarget& newshadow_rt = mShadowGfxPass.GetShadowDepthRenderTarget();

		FLUX_DEBUG_LOG_INFO("Sampler: ",
			"\n\t gpu id: ", newshadow_rt.sampler,
			", cpu address: ", &newshadow_rt.sampler, "\n");
	}



	void SceneRuntime::CreateModelSceneDescriptionSetLayout(Vulkan::VulkanContext* vk_ctx, Vulkan::DescriptorSetSpecification& description_set_spec)
	{
		//to create a ubo this function could take in the binding id
		description_set_spec.set = 1;
		VkDescriptorSetLayoutBinding ubo_layout_binding{};
		ubo_layout_binding.binding = 0;
		ubo_layout_binding.descriptorCount = 1;
		ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		ubo_layout_binding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutBinding sampler_layout_binding{};
		sampler_layout_binding.binding = 1;
		sampler_layout_binding.descriptorCount = 1;
		sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		sampler_layout_binding.pImmutableSamplers = nullptr;
		sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		//test if possible to combined with other binding line ubo_layout_binding
		//to have similar binding but use in different stages
		VkDescriptorSetLayoutBinding directional_lighting_layout_binding{};
		directional_lighting_layout_binding.binding = 2;
		directional_lighting_layout_binding.descriptorCount = 1;
		directional_lighting_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		directional_lighting_layout_binding.pImmutableSamplers = nullptr;
		directional_lighting_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutBinding shadow_sampler_layout_binding{};
		shadow_sampler_layout_binding.binding = 3;
		shadow_sampler_layout_binding.descriptorCount = 1;
		shadow_sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		shadow_sampler_layout_binding.pImmutableSamplers = nullptr;
		shadow_sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;


		//group bindings 
		std::array<VkDescriptorSetLayoutBinding, 4> bindings = { ubo_layout_binding, sampler_layout_binding, directional_lighting_layout_binding, shadow_sampler_layout_binding };
		VkDescriptorSetLayoutCreateInfo layout_create_info{};
		layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layout_create_info.bindingCount = static_cast<uint32_t>(bindings.size());
		layout_create_info.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(vk_ctx->device, &layout_create_info, nullptr, &description_set_spec.layoutHandle) != VK_SUCCESS)
			throw std::runtime_error("Failed to create descriptor set layout!!!!!!!!");
	}
	void SceneRuntime::CreateSkyboxDescriptionSetLayout(VkDevice d, Vulkan::DescriptorSetSpecification& description_set_spec)
	{
		VkDescriptorSetLayoutBinding skybox_samplercube_layout_binding{}; //<--- used in both model & sybox fragment shader as the Image Cube Map
		skybox_samplercube_layout_binding.binding = 0;
		skybox_samplercube_layout_binding.descriptorCount = 1;
		skybox_samplercube_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		skybox_samplercube_layout_binding.pImmutableSamplers = nullptr;
		skybox_samplercube_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutBinding skybox_ubo_layout_binding{}; //<<---- used in model fragment for samplaing parameters 
		skybox_ubo_layout_binding.binding = 1;
		skybox_ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		skybox_ubo_layout_binding.descriptorCount = 1;
		skybox_ubo_layout_binding.pImmutableSamplers = nullptr;
		skybox_ubo_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		//group bindings 
		std::array<VkDescriptorSetLayoutBinding, 2> bindings = { skybox_samplercube_layout_binding, skybox_ubo_layout_binding };
		VkDescriptorSetLayoutCreateInfo layout_create_info{};
		layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layout_create_info.bindingCount = static_cast<uint32_t>(bindings.size());
		layout_create_info.pBindings = bindings.data();

		description_set_spec.set = 2;
		if (vkCreateDescriptorSetLayout(d, &layout_create_info, nullptr, &description_set_spec.layoutHandle) != VK_SUCCESS)
			throw std::runtime_error("Failed to create descriptor set layout!!!!!!!!");
	}






	void SceneRuntime::Update(float delta_time, Platform::Window* p_window, Camera* p_frame_camera)
	{
		mVulkanContext->frameDT = delta_time;
		auto& curr_frame = mFrames[mVulkanContext->frameIndex];


		/////////////////
		// COMPUTE PASS
		/////////////////
		//Vulkan::BeginComputeFrameCommand(mVulkanContext, curr_frame, &mComputePass);
		vkWaitForFences(mVulkanContext->device, 1, &curr_frame.computeInFlight, VK_TRUE, UINT64_MAX);


		mComputePass.PreBeginFrameSync(mVulkanContext->frameIndex, mVulkanContext->frameDT);
		vkResetFences(mVulkanContext->device, 1, &curr_frame.computeInFlight);

		curr_frame.computeCmdBuffer.Reset();
		curr_frame.computeCmdBuffer.BeginRecord();

		//mComputePass.Begin(mVulkanContext, curr_frame.computeCommandBuffer, 0);
		mComputePass.Begin(mVulkanContext, curr_frame.computeCmdBuffer.GetCommandHandle(), 0);

		//Vulkan::EndComputeFrameCommand(mVulkanContext, curr_frame);
		curr_frame.computeCmdBuffer.EndRecord();
		curr_frame.computeCmdBuffer.SubmitRecording(mVulkanContext->computeQueue, curr_frame.computeInFlight,
			{}, {}, { curr_frame.computeCompleteSemaphore });




		//current passes
		//only one passes required : forward pass
		//later this is build inside Scene
		mSceneContext = {};
		mCamera = p_frame_camera;
		mSceneContext.mainCamera = mCamera;
		mSceneContext.mainWindow = p_window;

		mSceneContext.renderPassBuckets.resize(1);
		mSceneContext.renderPassBuckets.push_back({});

		//RenderPassBucket render_pass_bucket{};
		//render_pass_bucket.type = ERenderPassType::RenderPass_None;
		//render_pass_bucket.pass = mScene

		mScene.mSceneResources.directionalLight = &mDirLight;
		mScene.mSceneResources.skybox = &mSkybox;
		mScene.mSceneResources.currentFrameDT = delta_time;
		mScene.Update(delta_time, mSceneContext);

		//need imgui desc for dshadow target
		auto& shadow_depth = mShadowGfxPass.GetShadowDepthRenderTarget();
		if (!shadow_depth.targetMask.Contains(ERenderTargetBits::ImGuiView))
			Vulkan::RenderTarget::MakeRenderTarget(mVulkanContext, shadow_depth, ERenderTargetBits::ImGuiView);

		///TODOConqster): sort as we instert render Objects 


		//by now the scene context should be updated 
		//uint32_t swapchain_img_idx;
		//const auto& curr_cmd_buff = curr_frame.gfxCmdBuffer.GetCommandHandle(); //curr_frame.commandBuffer;
		//Vulkan::BeginFrame(mVulkanContext, curr_frame, swapchain_img_idx, mWindow, mForwardRenderPass.renderPass, &mImGuiUI);
		//need to fix this function later; render pass is only need to reconstrcution the swapchain if window resize 
		//so this does not define the render pass begine

		LightShadowData& light_shadow_data = mScene.mSceneResources.lightShadowData;



		//glm::mat4 proj = glm::ortho(-light_shadow_data.viewSize, light_shadow_data.viewSize,
		//	-light_shadow_data.viewSize, light_shadow_data.viewSize,
		//	light_shadow_data.near, light_shadow_data.far);
		//proj[1][1] *= -1;
		//glm::vec3 normalised_light_dir = glm::normalize(mDirLight.direction);
		//glm::vec3 light_pos = light_shadow_data.sampleOrigin - (normalised_light_dir * light_shadow_data.sampleShadowDistance);
		////glm::mat4 view = glm::lookAt(light_pos, light_shadow_data.sampleOrigin, light_shadow_data.sampleWorldUp); //world up 0, 1, 0
		//glm::vec3 up = (glm::abs(glm::dot(normalised_light_dir, glm::vec3(0, 1, 0))) > 0.99f) ? glm::vec3(0, 0, 1) : glm::vec3(0, 1, 0);
		//glm::mat4 view = glm::lookAt(light_pos, light_shadow_data.sampleOrigin, up); //world up 0, 1, 0


		light_shadow_data.UpdateCameraPosition(mDirLight.direction);

#pragma region OLd Compute pass

		//Vulkan::BeginComputeFrameCommand(mVulkanContext, curr_frame, &mComputePass);

		////mComputePass.Begin(mVulkanContext, curr_frame.computeCommandBuffer, 0);
		//mComputePass.Begin(mVulkanContext, curr_frame.computeCmdBuffer.GetCommandHandle(), 0);

		//Vulkan::EndComputeFrameCommand(mVulkanContext, curr_frame);

#pragma endregion
		
		//UpdateGlobalFrameDataUBO(curr_frame, mWindow->width, mWindow->height, view, proj);
		//UpdateGlobalFrameDataUBO(curr_frame, mWindow->width, mWindow->height, light_shadow_data.GetView(), light_shadow_data.GetProjection());
		UpdateGlobalFrameDataUBO(curr_frame, mScene.mSceneResources.viewportWidth, mScene.mSceneResources.viewportHeight, light_shadow_data.GetView(), light_shadow_data.GetProjection());

		//std::function<void(u32 w, u32 h)> lamda_resize_func = [&](u32 w, u32 h) {ResizeFunction(w, h); };
		//Vulkan::BeginGfxFrameCommand(mVulkanContext, curr_frame, swapchain_img_idx, *mWindow, mScene.GetForwardRenderPass()->GetRenderPassHandle(), lamda_resize_func/*mImGuiUI*/);
		//Vulkan::BeginGfxFrameCommand(mVulkanContext, curr_frame, swapchain_img_idx, *mWindow, mImGuiUI->mRenderer.GetRenderPassHandle(), lamda_resize_func/*mImGuiUI*/);
		
		vkWaitForFences(mVulkanContext->device, 1, &curr_frame.inFlight, VK_TRUE, UINT64_MAX);
		vkResetFences(mVulkanContext->device, 1, &curr_frame.inFlight);
		uint32 swapchain_img_idx = mVulkanContext->swapchain->AcquireNextImageIdx(mVulkanContext, curr_frame.imageAvailable);
		auto& curr_cmd_buff = curr_frame.gfxCmdBuffer;

		//Vulkan::BeginGfxFrameCommand(curr_frame);
		/////////////////////////////////////
		// GRAPHICS PASS
		/////////////////////////////////////
		curr_cmd_buff.Reset();
		curr_cmd_buff.BeginRecord();


		//shadow pass 
		mShadowGfxPass.depthBiasConstantFactor = mScene.mSceneResources.lightShadowData.depthBiasConstantFactor;
		mShadowGfxPass.depthBiasClamp = mScene.mSceneResources.lightShadowData.depthBiasClamp;
		mShadowGfxPass.depthBiasSlopeFactor = mScene.mSceneResources.lightShadowData.depthBiasSlopeFactor;
		mShadowGfxPass.Begin(mVulkanContext, curr_cmd_buff.GetCommandHandle(), 0);

		//view = Camera::GetView(*mCamera);
		//proj = Camera::GetProjection(*mCamera, mWindow->width, mWindow->height);
		//glm::mat4 light_matrix = proj * view; 
		//light_matrix = Camera::GetProjection(*mCamera, mWindow->width, mWindow->height) * Camera::GetView(*mCamera);
		mShadowGfxPass.UpdateLightMatrixBuffer(light_shadow_data.GetView(), light_shadow_data.GetProjection(), mVulkanContext->frameIndex);
		mShadowGfxPass.UpdateLightMoveBuffer(mScene.mSceneResources.experimentLightMove, mVulkanContext->frameIndex);
		for (auto& render_obj : mSceneContext.renderPassBuckets[0].renderObjects)
		{
			//if ((render_obj.maskLayer & ERenderMask::RM_ShadowCaster) == ERenderMask::RM_ShadowCaster)
			if (render_obj.renderMask.Contains(ERenderBits::ShadowCaster))
			{
				auto& matrix = render_obj.GPUModel.transform;
				mShadowGfxPass.PushModelTransformConstant(curr_cmd_buff.GetCommandHandle(), &render_obj.GPUModel);
				render_obj.mesh->Draw(curr_cmd_buff.GetCommandHandle());
			}
		}
		mShadowGfxPass.End(curr_cmd_buff.GetCommandHandle());



		//voxel scene geometry pass
		mSceneCollsionVoxelPass.Begin(mVulkanContext, curr_cmd_buff.GetCommandHandle(), swapchain_img_idx);
		mSceneCollsionVoxelPass.UpdateCameraData(mCamera);
		mSceneCollsionVoxelPass.UpdateVoxelParamData({});
		for (auto& render_obj : mSceneContext.renderPassBuckets[0].renderObjects)
		{
			auto& matrix = render_obj.GPUModel.transform;
			mSceneCollsionVoxelPass.PushModelTransformConstant(curr_cmd_buff.GetCommandHandle(), &render_obj.GPUModel);
			render_obj.mesh->Draw(curr_cmd_buff.GetCommandHandle());
		}
		mSceneCollsionVoxelPass.End(curr_cmd_buff.GetCommandHandle());

		for (auto& render_passes : mSceneContext.renderPassBuckets)
		{
			if (render_passes.type == ERenderPassType::None)
				continue;

			// render pass
			//Vulkan::BeginFrame2(mVulkanContext, curr_frame, swapchain_img_idx, *mWindow, render_passes.pass->GetRenderPassHandle());/, & mImGuiUI);
			//render_passes.pass->Begin(mVulkanContext, curr_cmd_buff, swapchain_img_idx);
			//mScene.GetForwardRenderPass()->Begin(mVulkanContext, curr_cmd_buff.GetCommandHandle(), swapchain_img_idx);
			mScene.GetForwardRenderPass()->Begin(curr_cmd_buff.GetCommandHandle());

			uint32_t curr_pipeline = -1;
			for (auto& render_obj : render_passes.renderObjects)
			{
				static Vulkan::GraphicsPipeline* gfx_pipeline = nullptr;
				if (curr_pipeline != render_obj.pipelineIdx)
				{
					gfx_pipeline = render_passes.pipelines[render_obj.pipelineIdx].pipeline;
					gfx_pipeline->Bind(curr_cmd_buff.GetCommandHandle());
					//required descriptor sets 
					////later batch descriptor sets 
					//for (auto& desc : render_passes.pipelines[render_obj.pipelineIdx].descSetSpecifications)
					//{
					//	gfx_pipeline->BindDescriptorSets(curr_cmd_buff.GetCommandHandle(), &curr_frame.GetGlobalDescriptorSet(*desc), desc->set, 1);
					//}
					//std::vector<VkDescriptorSet> descriptor_sets;
					//descriptor_sets.reserve(render_passes.pipelines[render_obj.pipelineIdx].descSetSpecifications.size());
					//uint32 first_set = UINT32_MAX;
					//for (auto& desc : render_passes.pipelines[render_obj.pipelineIdx].descSetSpecifications)
					//{
					//	first_set = glm::min(first_set, desc->set);
					//	descriptor_sets.emplace_back(curr_frame.GetGlobalDescriptorSet(*desc));
					//}

					uint32 first_set = 0;
					if(render_obj.renderMask.Contains(ERenderBits::ShadowCaster))
					{
						//MaterialSet  CamAndModelSet  SkyboxSet
						std::vector<VkDescriptorSet> descriptor_sets = {
							mDescriptorSet.GetHandle("MaterialSet"),
							mDescriptorSet.GetHandle("CamAndModelSet"),
							mDescriptorSet.GetHandle("SkyboxSet"),
						};
						gfx_pipeline->BindDescriptorSets(curr_cmd_buff.GetCommandHandle(), descriptor_sets.data(), first_set, (uint32)(descriptor_sets.size()));
					}
					else
					{
						first_set = 1;
						//quick hack obj tracjk their required set 
						//dummy  CamAndModelSet  SkyboxSet
						std::vector<VkDescriptorSet> descriptor_sets = {
							mDescriptorSet.GetHandle("CamAndModelSet"), //idx 1
							mDescriptorSet.GetHandle("SkyboxSet"), //idx 2 
						};
						gfx_pipeline->BindDescriptorSets(curr_cmd_buff.GetCommandHandle(), descriptor_sets.data(), first_set, (uint32)(descriptor_sets.size()));

						//std::vector<VkDescriptorSet> descriptor_sets;
						//descriptor_sets.reserve(render_passes.pipelines[render_obj.pipelineIdx].descSetSpecifications.size());
						//for (auto& desc : render_passes.pipelines[render_obj.pipelineIdx].descSetSpecifications)
						//{
						//	first_set = glm::min(first_set, desc->set);
						//	descriptor_sets.emplace_back(curr_frame.GetGlobalDescriptorSet(*desc));
						//}
						//gfx_pipeline->BindDescriptorSets(curr_cmd_buff.GetCommandHandle(), descriptor_sets.data(), first_set, (uint32)(descriptor_sets.size()));
					}
					curr_pipeline = render_obj.pipelineIdx;
				}

				for (auto& push_const : render_obj.pushConstantSpecs)
					gfx_pipeline->PushConstant(curr_cmd_buff.GetCommandHandle(), push_const.stage_flag, push_const.offset, push_const.size, push_const.pdata);

				render_obj.mesh->Draw(curr_cmd_buff.GetCommandHandle());
			}

			//auto camera_desc_set = &curr_frame.GetGlobalDescriptorSet(mScene.GetDescriptorSetSpecificationByIdx(mSceneModelDescSetIdx));
			auto camera_desc_set = mDescriptorSet.GetHandle("CamAndModelSet");
			//Test: Draw Debug Line 
			//mDebugLinePipeline.Bind(curr_cmd_buff);
			//mDebugLinePipeline.BindDescriptorSets(curr_cmd_buff, camera_desc_set, 1, 1);
			//
			//VkDeviceSize offsets[] = { 0 };
			//vkCmdBindVertexBuffers(curr_cmd_buff, 0, 1, &mDebugVertexBuffer.buffer, offsets);
			//vkCmdDraw(curr_cmd_buff, 4, 1, 0, 0);
			//debug_line[0].position = glm::vec4(0.0f, -5.0f, 0.0, 1.0f);
//debug_line[1].position = glm::vec4(0.0f, 5.0f, 0.0, 1.0f);
//debug_line[2].position = glm::vec4(-5.0f, 0.0f, 0.0, 1.0f);
//debug_line[3].position = glm::vec4(5.0f, 0.0f, 0.0, 1.0f);

			mDebugLines.BeginFrame(mVulkanContext->frameIndex);


#pragma region Emitter debug line bounds
			auto& emitter = mComputePass.mEmitter;
			glm::vec3 bounds_min = /*emitter.worldTransform * */glm::vec4(emitter.boundsMin, 1.0f);
			glm::vec3 bounds_max = /*emitter.worldTransform * */ glm::vec4(emitter.boundsMax, 1.0f);
			std::array<glm::vec3, 24> aabb_edges = Utils::GenerateAABBEdges(emitter.worldTransform, bounds_min, bounds_max);


			for (size_t i = 0; i < aabb_edges.size(); i += 2)
				mDebugLines.PushLine({ glm::vec4(aabb_edges[i], 1.0f), Colour32::sCyan }, { glm::vec4(aabb_edges[i + 1], 1.0f), Colour32::sCyan });


			glm::vec4 localGravity = glm::inverse(emitter.worldTransform) * glm::vec4(0.0f, -9.8f, 0.0f, 0.0f);
			mDebugLines.PushLine({ emitter.worldTransform[3] }, { emitter.worldTransform[3] + localGravity * 4.0f });

			mDebugLines.PushLine({ glm::vec4(10.0f, -5.0f, 0.0, 1.0f) }, { glm::vec4(0.0f, 5.0f, 0.0, 1.0f) });
			mDebugLines.PushLine({ glm::vec4(-5.0f, 0.0f, 0.0, 1.0f) }, { glm::vec4(5.0f, 0.0f, 0.0, 1.0f) });
#pragma endregion //Emitter debug line bounds

#pragma region Dir Light debug line bounds 
			auto& dir_light_aabb = light_shadow_data.GetViewOrthoVolumeAABBBounds();

			glm::mat4 light_world_transform = glm::translate(glm::mat4(1.0f), light_shadow_data.GetViewPosition()) * Utils::DirectionalBasis(mDirLight.direction);

			aabb_edges = Utils::GenerateAABBEdges(light_world_transform, dir_light_aabb);
			for (size_t i = 0; i < aabb_edges.size(); i += 2)
				mDebugLines.PushLine({ glm::vec4(aabb_edges[i], 1.0f), Colour32::sGreen }, { glm::vec4(aabb_edges[i + 1], 1.0f), Colour32::sGreen });

#pragma endregion //Dir Light debug line bounds 



			mDebugLines.DrawDebugLines(mVulkanContext, curr_cmd_buff.GetCommandHandle(), &camera_desc_set);

			mComputePass.HackRenderComputeParticle(mVulkanContext, curr_cmd_buff.GetCommandHandle(), &camera_desc_set);


			//mImGuiUI->StartFrame();
			//Editor::SceneRuntimeUIWindow(mScene.mSceneResources);
			//mEditorUI.RenderUI(mScene.mSceneResources);
			//ImGui::ShowDemoWindow();
			//if(mImGuiUI != nullptr)
			//{
			//	//mImGuiUI->RenderFrame(curr_cmd_buff);
			//	mImGuiUI->mRenderer.RenderFrame(mVulkanContext, curr_cmd_buff);
			//}
			//render_passes.pass->End(curr_cmd_buff);
			mScene.GetForwardRenderPass()->End(curr_cmd_buff.GetCommandHandle());
			//mScene.GetForwardRenderPass()->End(curr_cmd_buff);
			//Vulkan::EndFrame2(curr_frame, mVulkanContext, swapchain_img_idx, *mWindow, render_passes.pass->GetRenderPassHandle());
		}


		//if (mImGuiUI != nullptr)
		//{
		//	mImGuiUI->StartFrame();
		//	mEditorUI.RenderUI(mScene.mSceneResources);
		//	Editor::SceneRuntimeUIWindow(mScene.mSceneResources);
		//	ImGui::ShowDemoWindow();
		//	//mImGuiUI->RenderFrame(curr_cmd_buff);
		//	mImGuiUI->mRenderer.RenderFrame(mVulkanContext, curr_cmd_buff.GetCommandHandle(), swapchain_img_idx);


		//	

		//}

		////////////////
		// hack function 
		// callback to render ui stuffs 
		////////////////
		mOnUIRenderCallback(mVulkanContext, curr_cmd_buff.GetCommandHandle(), swapchain_img_idx);


		//Vulkan::EndGfxFrameCommand(curr_frame, mVulkanContext, mVulkanContext->swapchain->GetAcquiredImage());
		curr_cmd_buff.EndRecord();
		curr_cmd_buff.SubmitRecording(mVulkanContext->graphicsQueue, curr_frame.inFlight,
			{ curr_frame.computeCompleteSemaphore, curr_frame.imageAvailable },
			{ VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT },
			{ mVulkanContext->swapchain->GetAcquiredImage().renderComplete });


		//Vulkan::SwapPresentFrameImage(curr_frame, mVulkanContext, swapchain_img_idx, *mWindow, mScene.GetForwardRenderPass()->GetRenderPassHandle(), lamda_resize_func);
		//Vulkan::SwapPresentFrameImage(curr_frame, mVulkanContext, swapchain_img_idx, *mWindow, mImGuiUI->mRenderer.GetRenderPassHandle(), lamda_resize_func);
		mVulkanContext->swapchain.get()->Present(mVulkanContext);

		//std::function<void(u32 w, u32 h)> lamda_resize_render = [&](u32 w, u32 h)
		//	{
		//		mScene.GetForwardRenderPass()->OnResize(mVulkanContext, mFrames[mVulkanContext->frameIndex].commandPool, w, h);
		//		
		//	};
		//mEditorUI.CheckViewportResize(lamda_resize_render);


		//mEditorUI.CheckViewportResize([&](u32 w, u32 h) {
		//	});
		mVulkanContext->frameIndex = (mVulkanContext->frameIndex + 1) % Vulkan::kMaxFramesInFlight;
	}


	void SceneRuntime::ResizeFunction(uint32 width, uint32 height)
	{

		//mImGuiUI->mRenderer.OnWindowResize(mVulkanContext, width, height);
		mOnSwapchainUICallback(mVulkanContext, width, height);

		//need to get view port size 
		uint32 viewport_width = (uint32)mScene.mSceneResources.viewportWidth;
		uint32 viewport_height = (uint32)mScene.mSceneResources.viewportHeight;
		mScene.GetForwardRenderPass()->OnResize(mVulkanContext, mFrames[mVulkanContext->frameIndex].commandPool, viewport_width, viewport_height);
	}

	void SceneRuntime::OnViewportResize(uint32 width, uint32 height)
	{
		mScene.GetForwardRenderPass()->OnResize(mVulkanContext, mFrames[mVulkanContext->frameIndex].commandPool, width, height);
	}

	void SceneRuntime::UpdateGlobalFrameDataUBO(Vulkan::FrameResources& frame_resources, float win_width, float win_height, glm::mat4 light_view, glm::mat4 light_proj)
	{
		GPUStructures::MVP_UniformBufferObject MVP;
		if (!mCamera)
			return;
		MVP.view = mCamera->GetView();
		MVP.projection = mCamera->GetProjection(win_width, win_height);
		MVP.position = mCamera->GetPosition();
		MVP.lightView = light_view;
		MVP.lightProj = light_proj;
		


		//FLUX_DEBUG_ASSERT(!frame_resources.globalCameraUniformBuffer.IsMapped());
		//Vulkan::GPUBuffer::CopyToMappedMemory(frame_resources.globalCameraUniformBuffer, &MVP, sizeof(MVP));
		mDescriptorSet.UpdateUBOBinding("CamAndModelSet::camera_ubo", &MVP, mVulkanContext->frameIndex);
		mDescriptorSet.UpdateUBOBinding("CamAndModelSet::directional_light_ubo", &mDirLight, mVulkanContext->frameIndex);
		mDescriptorSet.UpdateUBOBinding("SkyboxSet::skybox_ubo_param", &mSkybox, mVulkanContext->frameIndex);

		////update light descriptor uniform buffer 
		//Vulkan::GPUBuffer::CopyToMappedMemory(frame_resources.globalDirectionalLightUniformBuffer, &mDirLight, sizeof(mDirLight));

		////update skybox descriptor uniform buffer 
		//Vulkan::GPUBuffer::CopyToMappedMemory(frame_resources.globalSkyboxUniformBuffer, &mSkybox, sizeof(mSkybox));

		//update material 
#if DEBUG_MAT_HACK
		Vulkan::CopyToMappedMemory(mRustyMetalPBRMat.hackDebugMaterialGPUBuffer, &mRustyMetalPBRMat.hackDebugMaterial, sizeof(mRustyMetalPBRMat.hackDebugMaterial));
#endif // DEBUG_MAT_HACK

	}



	void SceneRuntime::LoadSceneFromFile(Vulkan::VulkanContext* vk_ctx, const char* scene_path, Scene& scene, Camera* cam_ref)
	{
		mScene.mSceneResources.camera = static_cast<EditorCamera*>(cam_ref);
		//quick hack for loading
		mScene.mSceneResources.directionalLight = &mDirLight;
		SceneSerialiser scene_serialiser;
		scene_serialiser.LoadScene(vk_ctx, "assets/scenes/experiment.fluxon", mScene);
		mScene.mSceneResources.instantSceneName = &mScene.mName;
	}


	void SceneRuntime::CreateSceneFromCode(Vulkan::VulkanContext* vk_ctx, Scene& scene, Camera* cam_ref)
	{
		if (!scene.HasRoot())
			mScene.SetNewRootNode(SceneNode("Root-Node", glm::mat4(1.0f)));

		SceneNode* scene_root = mScene.GetRootNode();

		auto& pos = (cam_ref) ? cam_ref->GetPosition() : glm::vec3(0.0f);
		SceneNode* camera_node = Scene::CreateNewNode(mScene, SceneNode("Camera-Node", glm::translate(glm::mat4(1.0f), pos)));
		camera_node->AddComponent(std::make_unique<CameraComponent>(cam_ref));
		//scene_root->AddChild(std::move(camera_node));
		Scene::AttachChild(mScene, *scene_root, *camera_node);
		mScene.SetCameraNode(camera_node);


		VkCommandPool cmd_pool = vk_ctx->transientCmdPool;
		//std::vector<GPUStructures::Vertex> vertices_data;
		//std::vector<uint16_t> indices_data;

		//mScene.mGlobalMeshes.reserve(10);

		///Fluxon::Utils::GetUnitCubeVertices(vertices_data, indices_data);
		//Vulkan::Mesh* mesh0 = mScene.AddMesh(Vulkan::Mesh(Vulkan::Mesh(vk_ctx, cmd_pool, vertices_data, indices_data)));
		//uint32_t mesh0 = Scene::CreateNewMesh(mScene, Vulkan::Mesh(vk_ctx, cmd_pool, vertices_data, indices_data));
		uint32_t mesh0 = Scene::CreateCube(mScene, vk_ctx, cmd_pool);
		std::cout << "Pointer Memory Address: " << &mesh0 << "\n";
		std::cout << "Memory Address: " << mesh0 << "\n";
		//uint32_t mesh1 = Scene::CreateNewMesh(mScene, Vulkan::Mesh());
		uint32_t mesh1 = Scene::CreateSphere(mScene, vk_ctx, cmd_pool);
		//uint32_t mesh2 = Scene::CreateNewMesh(mScene, Vulkan::Mesh());
		uint32_t mesh2 = Scene::CreateModel(mScene, vk_ctx, cmd_pool, "assets/models/model.dae");
		//uint32_t mesh3 = Scene::CreateNewMesh(mScene, Vulkan::Mesh());
		uint32_t mesh3 = Scene::CreateMesh(mScene, vk_ctx, EPrimitiveMeshType::Plane);
		//mesh0->Generate(vk_ctx, cmd_pool, vertices_data, indices_data);

		uint32_t mat = 0;

		SceneNode* mesh_node0 = Scene::CreateNewNode(mScene, SceneNode("Mesh-Node0"));
#if USE_EXPLICIT_COMPONENT
		Scene::AttachRenderableComponent(mScene, *mesh_node0, RenderableComponent(mesh0, mat));
#else
		mesh_node0->AddComponent(std::make_unique<RenderableComponent>(mesh0, mat));
		RenderableComponent* renderable_comp = dynamic_cast<RenderableComponent*>(mesh_node0->GetComponents()[0].get());
		if (renderable_comp && renderable_comp->GetType() == ESceneComponentType::SCT_Renderable)
		{
			std::cout << "Pointer Memory Address: " << &mesh0 << "\n";
			std::cout << "Memory Address: " << mesh0 << "\n";
			std::cout << "Node use pointer mesh Memory Address: " << &renderable_comp->mesh << "\n";
			std::cout << "Node use pointer mesh data Memory Address: " << renderable_comp->mesh << "\n";
		}
#endif // USE_EXPLICIT_COMPONENT
		Scene::AttachChild(mScene, *scene_root, *mesh_node0);





		//SceneNode* mesh_node1 = new SceneNode("Mesh-Node1", glm::translate(glm::mat4(1.0f), glm::vec3(0.63f, 0.28f, 5.47f)));
		SceneNode* mesh_node1 = Scene::CreateNewNode(mScene, SceneNode("Mesh-Node1", glm::translate(glm::mat4(1.0f), glm::vec3(0.63f, 0.28f, 5.47f))));
		if (mCamera && mesh_node1)
		{
			mCamera->SetLookTarget(mesh_node1->GetWorldTransform()[3]);
#if USE_EXPLICIT_COMPONENT
			Scene::AttachRenderableComponent(mScene, *mesh_node1, RenderableComponent(mesh1, mat, true, true));
#else
			mesh_node1->AddComponent(std::make_unique<RenderableComponent>(mesh1, mat, true, true));
#endif // USE_EXPLICIT_COMPONENT
			//scene_root->AddChild(std::move(mesh_node1));
			Scene::AttachChild(mScene, *scene_root, *mesh_node1);
		}



		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, -1.56, 1.19f)) *
			glm::scale(glm::mat4(1.0f), glm::vec3(0.02f));
		SceneNode* mesh_node2 = Scene::CreateNewNode(mScene, SceneNode("Mesh-Node2", transform));
#if USE_EXPLICIT_COMPONENT
		Scene::AttachRenderableComponent(mScene, *mesh_node2, RenderableComponent(mesh2, mat));
#else
		mesh_node2->AddComponent(std::make_unique<RenderableComponent>(mesh2, mat));
#endif // USE_EXPLICIT_COMPONENT
		Scene::AttachChild(mScene, *scene_root, *mesh_node2);




		transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.5f, 0.0f)) *
			glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
			glm::scale(glm::mat4(1.0f), glm::vec3(10.0f));
		SceneNode* mesh_node3 = Scene::CreateNewNode(mScene, SceneNode("Mesh-Node3", transform));
#if USE_EXPLICIT_COMPONENT
		Scene::AttachRenderableComponent(mScene, *mesh_node3, RenderableComponent(mesh3, mat));
#else
		mesh_node3->AddComponent(std::make_unique<RenderableComponent>(mesh3, mat));
#endif // USE_EXPLICIT_COMPONENT
		Scene::AttachChild(mScene, *scene_root, *mesh_node3);

	}

} //#Fluxon