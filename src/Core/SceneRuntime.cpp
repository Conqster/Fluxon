#include "SceneRuntime.h"
#include "Scene/ISceneComponent.h"

#include "Renderer/Render Passes/ForwardRenderPass.h"

#include "Utilities.h"
#include "Resources/ModelLoader.h"

#include "Editor/MainSceneEditor.h"

#include "Renderer/Render Passes/ComputePass.h"

#define LOAD_SCENE_FROM_FILE 1


namespace Fluxon {


	void Vulkan::BeginGfxFrameCommand(VulkanContext& vk_render_ctx, FrameResources& frame_resources, uint32_t& o_img_idx, 
						const Platform::Window& window, VkRenderPass render_pass, UI_System* ui_in_use)
	{
		//wait for frames prvious GPU work finish 
		//everything is contained in frame
		vkWaitForFences(vk_render_ctx.device, 1, &frame_resources.inFlight, VK_TRUE, UINT64_MAX);
		vkResetFences(vk_render_ctx.device, 1, &frame_resources.inFlight);

		VkResult acquire_nxt_img_result = vkAcquireNextImageKHR(
			vk_render_ctx.device,
			vk_render_ctx.swapchain->GetHandle(),
			UINT64_MAX,
			frame_resources.imageAvailable,
			VK_NULL_HANDLE,
			&o_img_idx);


		if (acquire_nxt_img_result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			std::cout << "\033[1;31m [DRAW FRAME]\033[0m: " << "Need to REcreate image acquire" << std::endl;
			WindowResizedRecreateResource2(vk_render_ctx, window, frame_resources.commandPool, render_pass);
			return;
		}
		else if (acquire_nxt_img_result != VK_SUCCESS && acquire_nxt_img_result != VK_SUBOPTIMAL_KHR)
			std::cout << "\033[1;31m [DRAW FRAME]\033[0m: " << "Failed to Acquire Swap Chain Image" << std::endl;


		//check if the acquired swapchain image is in use, wait for it
		//auto& swapchain_img = mSwapchain->GetImages()[img_index];
		//if (mSwapchain->mImagesInFlight[img_index] != VK_NULL_HANDLE)
		//	vkWaitForFences(mDevice.device, 1, &mSwapchain->mImagesInFlight[img_index], VK_TRUE, UINT64_MAX);

		//vkResetCommandPool(vk_render_ctx.device, frame_resources.commandPool, 0);
		vkResetCommandBuffer(frame_resources.commandBuffer, 0);


		if (ui_in_use)
			ui_in_use->StartFrame();

		VkCommandBufferBeginInfo begin_info{};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = 0; //VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		begin_info.pInheritanceInfo = nullptr;


		if (vkBeginCommandBuffer(frame_resources.commandBuffer, &begin_info) != VK_SUCCESS)
			throw std::runtime_error("Failed to begin recording command buffer!!!!!");
	}

	void Vulkan::SwapPresentFrameImage(FrameResources& frame_reso, VulkanContext& vk_render_ctx, uint32_t img_idx, Platform::Window& window, VkRenderPass render_pass)
	{
		//Present
		VkPresentInfoKHR present_info{};
		present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		present_info.waitSemaphoreCount = 1;
		present_info.pWaitSemaphores = &vk_render_ctx.swapchain->GetImages()[img_idx].renderComplete;

		//is there are grps
		//std::vector<VkSwapchainKHR> grp_swap_chainsgrp_swap_chains = { mSwapchain->GetHandle() };
		//present_info.swapchainCount = static_cast<uint32_t>(grp_swap_chainsgrp_swap_chains.size());
		//present_info.pSwapchains = grp_swap_chainsgrp_swap_chains.data();
		present_info.swapchainCount = 1;
		VkSwapchainKHR swapchain_handle = vk_render_ctx.swapchain->GetHandle();
		present_info.pSwapchains = &swapchain_handle;
		present_info.pImageIndices = &img_idx;
		present_info.pResults = nullptr;

		VkResult presenting_queue_result = vkQueuePresentKHR(vk_render_ctx.presentQueue, &present_info);
		if (presenting_queue_result == VK_ERROR_OUT_OF_DATE_KHR || presenting_queue_result == VK_SUBOPTIMAL_KHR || window.windowResized)
		{
			std::cout << "\033[1;31m [DRAW FRAME]\033[0m: " << "Need to Resize image acquire, for Presentation" << std::endl;
			window.windowResized = false;
			WindowResizedRecreateResource2(vk_render_ctx, window, frame_reso.commandPool, render_pass);
		}
		else if (presenting_queue_result != VK_SUCCESS)
			std::cout << "\033[1;31m [DRAW FRAME]\033[0m: " << "Failed to Present Image" << std::endl;
	}

	void Vulkan::BeginComputeFrameCommand(VulkanContext& vk_render_ctx, FrameResources& frame_resources, ComputePass* compute_pass)
	{
		//compute 
		if (compute_pass != nullptr)
		{
			vkWaitForFences(vk_render_ctx.device, 1, &frame_resources.computeInFlight, VK_TRUE, UINT64_MAX);

			//need to remove this "compute_pass->PreBeginFrameSync(vk_render_ctx.frameIndex, vk_render_ctx.frameDT);" 
			//it createa restriction on custom vcompute pass/it ties compute pass with beginning command buffer
			compute_pass->PreBeginFrameSync(vk_render_ctx.frameIndex, vk_render_ctx.frameDT);
			vkResetFences(vk_render_ctx.device, 1, &frame_resources.computeInFlight);

			//might have to remove vkResetCommandPool(vk_render_ctx.device, frame_resources.commandPool, 0);
			//below becasue both compute & gfx cmd buffer are in the same pool.
			//so by reseting the command pool it might reset all cmd buffers 
			vkResetCommandBuffer(frame_resources.computeCommandBuffer, 0);


			VkCommandBufferBeginInfo begin_info{};
			begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			begin_info.flags = 0; //VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			begin_info.pInheritanceInfo = nullptr;


			if (vkBeginCommandBuffer(frame_resources.computeCommandBuffer, &begin_info) != VK_SUCCESS)
				throw std::runtime_error("Failed to begin recording compute command buffer!!!!!");
		}
	}

	void Vulkan::EndComputeFrameCommand(VulkanContext& vk_render_ctx, FrameResources& frame_reso)
	{
		if (vkEndCommandBuffer(frame_reso.computeCommandBuffer) != VK_SUCCESS)
			throw std::runtime_error("Failed to record compute command buffer!!!!");

		VkSubmitInfo submit_info{};

		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &frame_reso.computeCommandBuffer;
		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = &frame_reso.computeCompleteSemaphore;

		if (vkQueueSubmit(vk_render_ctx.computeQueue, 1, &submit_info, frame_reso.computeInFlight) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit compute command buffer!");
		};
	}

	void Vulkan::EndGfxFrameCommand(Vulkan::FrameResources& frame_reso, VulkanContext& vk_render_ctx, uint32_t img_idx)
	{
		if (vkEndCommandBuffer(frame_reso.commandBuffer) != VK_SUCCESS)
			throw std::runtime_error("Failed to record compute command buffer!!!!");

		VkSemaphore waitSemaphores[] = { frame_reso.computeCompleteSemaphore, frame_reso.imageAvailable };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSubmitInfo submit_info{};

		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.waitSemaphoreCount = 2;
		submit_info.pWaitSemaphores = waitSemaphores;
		submit_info.pWaitDstStageMask = waitStages;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &frame_reso.commandBuffer;
		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = &vk_render_ctx.swapchain->GetImages()[img_idx].renderComplete;

		if (vkQueueSubmit(vk_render_ctx.graphicsQueue, 1, &submit_info, frame_reso.inFlight) != VK_SUCCESS)
			throw std::runtime_error("Failed to submit draw command buffer!!!!");


		//VkSubmitInfo submit_info{};
		//submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		//VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		//submit_info.waitSemaphoreCount = 1;
		//submit_info.pWaitSemaphores = &frame_resources.imageAvailable;
		//submit_info.pWaitDstStageMask = wait_stages;

		//submit_info.commandBufferCount = 1;
		//submit_info.pCommandBuffers = &frame_resources.commandBuffer;
		//submit_info.signalSemaphoreCount = 1;
		//submit_info.pSignalSemaphores = &vk_render_ctx.swapchain->GetImages()[img_idx].renderComplete;

		//if (vkQueueSubmit(vk_render_ctx.graphicsQueue, 1, &submit_info, frame_resources.inFlight) != VK_SUCCESS)
		//	throw std::runtime_error("Failed to submit draw command buffer!!!!");

		//Matrk image as in use by this frame 
		//mSwapchain->mImagesInFlight[img_index] = frame.inFlight;
	}

	void Vulkan::WindowResizedRecreateResource2(VulkanContext& vk_render_ctx, const Platform::Window& window, VkCommandPool cmd_pool, VkRenderPass render_pass)
	{
		int width = 0;
		int height = 0;
		glfwGetFramebufferSize(window.glfwWindowHandle, &width, &height);
		while (width == 0 || height == 0)
		{
			glfwGetFramebufferSize(window.glfwWindowHandle, &width, &height);
			glfwWaitEvents();
		}

		//vkDeviceWaitIdle(vk_render_ctx.device);
		vkDeviceWaitIdle(vk_render_ctx.device);
		vk_render_ctx.swapchain->Cleanup();
		//TODO(Conqster): Need to fix REsizing
		vk_render_ctx.swapchain = std::make_unique<Vulkan::Swapchain>(&vk_render_ctx, window, cmd_pool, render_pass);
	}



	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
		VkDebugUtilsMessageTypeFlagsEXT message_type,
		const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
		void* p_user_data) {

		//std::cerr << "Validation layer: " << p_callback_data->pMessage << std::endl;
		std::cerr << "\033[1;34m [Validation layer]\033[0m: " << p_callback_data->pMessage << std::endl;
		return VK_FALSE;
	}


	void SceneRuntime::Init(Platform::Window& window, Camera* app_cam_ref)
	{
		mWindow = &window;
		if (app_cam_ref)
			mCamera = app_cam_ref;


		Vulkan::VulkanContextSpecification vk_ctx_spec{};
		vk_ctx_spec.bEnableFragmentStoresAndAtomics = true;
		mVulkanContext = Vulkan::VulkanContext::CreateVulkanContext(&window, vk_ctx_spec, DebugCallback);




		mScene.SetForwardRenderPass(std::make_unique<Vulkan::ForwardRenderPass>());
		Vulkan::ForwardRenderPass* forward_render_pass = mScene.GetForwardRenderPass();
		forward_render_pass->Init(&mVulkanContext);
		mFrames.resize(Vulkan::kMaxFramesInFlight);
		Vulkan::CreateFrameObjects(&mVulkanContext, mFrames);

		mShadowGfxPass.Init(&mVulkanContext, mFrames[0].commandPool);

		mSceneCollsionVoxelPass.Init(&mVulkanContext, mFrames[0].commandPool, mCamera);
		//TODO : later move framebuffer/renderpass out of swap chain 
		//so it would depend on renderpass 
		//mVulkanContext.swapchain = std::make_unique<Vulkan::Swapchain>(&mDevice, window, mFrames[0].commandPool, forward_render_pass.renderPass);
		mVulkanContext.swapchain = std::make_unique<Vulkan::Swapchain>(&mVulkanContext, window, mFrames[0].commandPool, forward_render_pass->GetRenderPassHandle());

		
		

#if LOAD_SCENE_FROM_FILE
		mScene.mSceneResources.camera = static_cast<EditorCamera*>(app_cam_ref);
		//quick hack for loading
		mScene.mSceneResources.directionalLight = &mDirLight;
		SceneSerialiser scene_serialiser;
		scene_serialiser.LoadScene(&mVulkanContext, mFrames[0].commandPool, "assets/scenes/experiment.fluxon", mScene);
		mScene.mSceneResources.instantSceneName = &mScene.mName;
#else
		if (!mScene.HasRoot())
			mScene.SetRootNode(SceneNode("Root-Node", glm::mat4(1.0f)));

		mScene.mSceneResources.renderableComponents.reserve(10);

		SceneNode* scene_root = mScene.GetRootNode();

		auto& pos = (app_cam_ref) ? app_cam_ref->position : glm::vec3(0.0f);
		SceneNode* camera_node = Scene::CreateNewNode(mScene, SceneNode("Camera-Node", glm::translate(glm::mat4(1.0f), pos)));
		camera_node->AddComponent(std::make_unique<CameraComponent>(app_cam_ref));
		//scene_root->AddChild(std::move(camera_node));
		Scene::AttachChild(mScene, *scene_root, *camera_node);
		mScene.SetCameraNode(camera_node);
#endif // LOAD_SCENE_FROM_FILE



		//TODO(Conqster-Uniformbuffer): have functionm to create new description set layout dynamically 
		uint32_t model_desc_set_idx  = mScene.AddDescriptorSetSpecification(Vulkan::DescriptorSetSpecification());
		uint32_t skybox_desc_set_idx = mScene.AddDescriptorSetSpecification(Vulkan::DescriptorSetSpecification());
		uint32_t matertial_desc_set_idx;// = mScene.AddDescriptorSetSpecification({});
		CreateSkyboxDescriptionSetLayout(mVulkanContext.device, mScene.GetDescriptorSetSpecificationByIdx(skybox_desc_set_idx));
		CreateModelSceneDescriptionSetLayout(&mVulkanContext, mScene.GetDescriptorSetSpecificationByIdx(model_desc_set_idx));

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
		//uint32_t pbr_mat_idx = Scene::CreateNewMaterial(mScene, Vulkan::PBRMaterial::CreatePBRMaterialMaps(&mVulkanContext, mFrames[0].commandPool, pbr_mat_paths), nullptr);
		//uint32_t pbr_mat_idx = Scene::CreateNewMaterial(mScene, MaterialResource::CreatePBRMaterialResource(&mVulkanContext, mFrames[0].commandPool, pbr_mat_paths), nullptr);
		uint32_t pbr_mat_idx = Scene::CreateNewMaterial(mScene, nullptr, &mVulkanContext, mFrames[0].commandPool, pbr_mat_paths);
#endif // LOAD_SCENE_FROM_FILE

		//Vulkan::PBRMaterial::CreatePBRMaterialMaps(&mVulkanContext, mRustyMetalPBRMat, mFrames[0].commandPool, pbr_mat_paths);
		//matertial_desc_set_idx = mScene.AddDescriptorSetSpecification(Vulkan::PBRMaterial::GenerateMaterialDecriptorSetLayoutSpecification(&mVulkanContext, mScene.GetPBRMaterialByIdx(pbr_mat_idx), 0, 0));
		//matertial_desc_set_idx = mScene.AddDescriptorSetSpecification(Vulkan::PBRMaterial::GenerateMaterialDecriptorSetLayoutSpecification(&mVulkanContext, mScene.GetMaterialByIdx(pbr_mat_idx), 0, 0));
		matertial_desc_set_idx = mScene.AddDescriptorSetSpecification(Vulkan::PBRMaterial::GenerateMaterialDecriptorSetLayoutSpecification(&mVulkanContext, mScene.GetMaterialByIdx(pbr_mat_idx).gpuMaterial, 0, 0));
		//CreateDummyDescriptionSetLayout(&mVulkanContext, mMaterialDescriptorLayout, 0);
		//CreateGraphicsPipeline();

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
			mScene.GetDescriptorSetSpecificationByIdx(matertial_desc_set_idx).layoutHandle,
			mScene.GetDescriptorSetSpecificationByIdx(model_desc_set_idx).layoutHandle,
			mScene.GetDescriptorSetSpecificationByIdx(skybox_desc_set_idx).layoutHandle };
		model_gfx_pipeline_spec.pushConstantRanges = { push_constant_range, push_constant_range2 };
		////leave remain data as default 
		uint32_t model_gfx_pipeline_idx = mScene.AddGfxPipeline(Vulkan::GraphicsPipeline());
		mScene.GetGfxPipelineByIdx(model_gfx_pipeline_idx).Init(&mVulkanContext, model_gfx_pipeline_spec);

		VkDescriptorSetLayout dummy_desc_set_layout = mVulkanContext.dummyDescSetLayout;
		Vulkan::GraphicsPipelineSpecification skybox_gfx_pipeline_spec{};
		skybox_gfx_pipeline_spec.vertexShaderPath = "assets/shaders/spirvs/skybox.vert.spv";
		skybox_gfx_pipeline_spec.fragmentShaderPath = "assets/shaders/spirvs/skybox.frag.spv";
		skybox_gfx_pipeline_spec.renderPass = forward_render_pass->GetRenderPassHandle();
		skybox_gfx_pipeline_spec.descriptorSetLayouts = {
			dummy_desc_set_layout, //mScene.GetDescriptorSetSpecificationByIdx(matertial_desc_set_idx).layoutHandle,  //<-------- Need to remove not but a desc set 0 is required so serve as dummy 
			mScene.GetDescriptorSetSpecificationByIdx(model_desc_set_idx).layoutHandle,
			mScene.GetDescriptorSetSpecificationByIdx(skybox_desc_set_idx).layoutHandle };
		skybox_gfx_pipeline_spec.pushConstantRanges = { };

		skybox_gfx_pipeline_spec.rasterisationCullMode = VK_CULL_MODE_FRONT_BIT;
		skybox_gfx_pipeline_spec.bEnableDepthWrite = false;
		skybox_gfx_pipeline_spec.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		////leave remain data as default 
		uint32_t skybox_gfx_pipeline_idx = mScene.AddGfxPipeline(Vulkan::GraphicsPipeline());
		mScene.GetGfxPipelineByIdx(skybox_gfx_pipeline_idx).Init(&mVulkanContext, skybox_gfx_pipeline_spec);
		//mSkyboxGraphicsPipeline.Init(&mVulkanContext, skybox_gfx_pipeline_spec);

#if LOAD_SCENE_FROM_FILE
#else
		//CreateTestSceneData(&mVulkanContext, mFrames[0].commandPool);
#endif // LOAD_SCENE_FROM_FILE


		std::vector<Vulkan::DescriptorSetSpecification> desc_set_layout_spec = {
			mScene.GetDescriptorSetSpecificationByIdx(matertial_desc_set_idx), 
			mScene.GetDescriptorSetSpecificationByIdx(model_desc_set_idx),
			mScene.GetDescriptorSetSpecificationByIdx(skybox_desc_set_idx) };
		Vulkan::GenerateFrameObjectDescriptorPoolSet(mVulkanContext.device, desc_set_layout_spec, mFrames);
		//TODO(Conqster-Uniformbuffer): have a system that inserts required buffer bindings
		// 
		//Vulkan::CreateFrameDatasUniformCameraBuffer(&mVulkanContext, mFrames);
		//Vulkan::CreateFrameDatasDirectionalLightUniformbuffer(&mVulkanContext, mFrames);

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
		skybox_ubo_spec.requiredSize = sizeof(GPUStructures::Skybox);

		std::vector<Vulkan::UniformBufferSpecification> ubo_specs = { cam_ubo_spec, dir_light_ubo_spec, skybox_ubo_spec };
		Vulkan::CreateAndAppendFrameDatasUniformBuffers(&mVulkanContext, mFrames, ubo_specs);


		VkFormat tex_image_format = VK_FORMAT_R8G8B8A8_SRGB;
		uint32_t tex_image_sampler_idx = mScene.AddImageSampler(Vulkan::Resources::ImageSampler());
		uint32_t skybox_image_sampler_idx = mScene.AddImageSampler(Vulkan::Resources::ImageSampler());
		Vulkan::Resources::CreateImageSamplerTexture(&mVulkanContext,
			mFrames[0].commandPool,
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
		Vulkan::Resources::CreateImageSamplerTextureCube(&mVulkanContext, mFrames[0].commandPool,
			skybox_imgs,
			tex_image_format,
			mScene.GetImageSamplerByIdx(skybox_image_sampler_idx),
			mUseSamplingAnisotropy);
		Vulkan::DescriptorBinding skybox_cube_tex_binding = Vulkan::DescriptorBinding::MakeImageSampler(2, 0, mScene.GetImageSamplerByIdx(skybox_image_sampler_idx));

		//auto pbr_material_tex_binding = Vulkan::PBRMaterial::GetDescriptorSetBindings(mScene.GetMaterialByIdx(pbr_mat_idx), 0);
		auto pbr_material_tex_binding = Vulkan::PBRMaterial::GetDescriptorSetBindings(mScene.GetMaterialByIdx(pbr_mat_idx).gpuMaterial, 0);

		Vulkan::DescriptorBinding shadow_map_binding = mShadowGfxPass.GetSamplerBinding(1, 3);


		for (auto& frame : mFrames)
		{
			for (auto& pbr_mat_binding : pbr_material_tex_binding)
				frame.AppendDescriptorBinding(pbr_mat_binding);
			frame.AppendDescriptorBinding(texture_binding);
			frame.AppendDescriptorBinding(skybox_cube_tex_binding);

			frame.AppendDescriptorBinding(shadow_map_binding);

			frame.WritePendingDescriptorBinding(mVulkanContext.device);

			//append Computer Descriptorsd
			
		}


		mScene.mSceneResources.shadowPassDepthRT = &mShadowGfxPass.GetShadowDepthRenderTarget();
		mScene.mSceneResources.voxelDepthRT = &mSceneCollsionVoxelPass.GetDepthRenderTarget();
		mComputePass.Init(&mVulkanContext, mFrames[0].commandPool, forward_render_pass->GetRenderPassHandle(), mScene.GetDescriptorSetSpecificationByIdx(model_desc_set_idx).layoutHandle);
		mScene.mSceneResources.particleEmitter = &mComputePass.mEmitter;


		mDebugLines.Init(&mVulkanContext, forward_render_pass->GetRenderPassHandle(), mScene.GetDescriptorSetSpecificationByIdx(model_desc_set_idx).layoutHandle);



		VkPhysicalDeviceLimits device_limits = mVulkanContext.GetPhysicalDeviceLimits();
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


	void SceneRuntime::CreateTestSceneData(const Vulkan::VulkanContext* vk_ctx, VkCommandPool cmd_pool)
	{


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
		uint32_t mesh3 = Scene::CreateMesh(mScene, vk_ctx, cmd_pool, EPrimitiveMeshType::Plane);
		//mesh0->Generate(vk_ctx, cmd_pool, vertices_data, indices_data);

		//vertices_data.clear();
		//indices_data.clear();
		//Fluxon::Utils::GenerateSphereVertices(vertices_data, indices_data);
		//mScene.GetMeshByIdx(mesh1)->Generate(vk_ctx, cmd_pool, vertices_data, indices_data);
		//

		//ModelLoader quick_loader;
		//auto [model_vertices, model_indices] = quick_loader.LoadAsVerticesData("assets/models/model.dae", true, false);
		//mScene.GetMeshByIdx(mesh2)->Generate(vk_ctx, cmd_pool, model_vertices, model_indices);


		//vertices_data.clear();
		//indices_data.clear();
		//Fluxon::Utils::GetUnitQuadVertices(vertices_data, indices_data);
		//mScene.GetMeshByIdx(mesh3)->Generate(vk_ctx, cmd_pool, vertices_data, indices_data);


		SceneNode* scene_root = mScene.GetRootNode();
		//Vulkan::PBRMaterial* mat = &mScene.GetMaterialByIdx(0);
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



	void SceneRuntime::Update(float delta_time, Platform::Window* p_window, Camera* p_frame_camera)
	{
		mVulkanContext.frameDT = delta_time;
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
			Vulkan::RenderTarget::MakeRenderTarget(&mVulkanContext, shadow_depth, ERenderTargetBits::ImGuiView);

		///TODOConqster): sort as we instert render Objects 


		//by now the scene context should be updated 
		uint32_t swapchain_img_idx;
		auto& curr_frame = mFrames[mVulkanContext.frameIndex];
		auto& curr_cmd_buff = curr_frame.commandBuffer;
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
		Vulkan::BeginComputeFrameCommand(mVulkanContext, curr_frame, &mComputePass);

		mComputePass.Begin(&mVulkanContext, curr_frame.computeCommandBuffer, 0);

		Vulkan::EndComputeFrameCommand(mVulkanContext, curr_frame);
		
		//UpdateGlobalFrameDataUBO(curr_frame, mWindow->width, mWindow->height, view, proj);
		UpdateGlobalFrameDataUBO(curr_frame, mWindow->width, mWindow->height, light_shadow_data.GetView(), light_shadow_data.GetProjection());
		Vulkan::BeginGfxFrameCommand(mVulkanContext, curr_frame, swapchain_img_idx, *mWindow, mScene.GetForwardRenderPass()->GetRenderPassHandle(), &mImGuiUI);
		


		//shadow pass 
		mShadowGfxPass.depthBiasConstantFactor = mScene.mSceneResources.lightShadowData.depthBiasConstantFactor;
		mShadowGfxPass.depthBiasClamp = mScene.mSceneResources.lightShadowData.depthBiasClamp;
		mShadowGfxPass.depthBiasSlopeFactor = mScene.mSceneResources.lightShadowData.depthBiasSlopeFactor;
		mShadowGfxPass.Begin(&mVulkanContext, curr_cmd_buff, 0);

		//view = Camera::GetView(*mCamera);
		//proj = Camera::GetProjection(*mCamera, mWindow->width, mWindow->height);
		//glm::mat4 light_matrix = proj * view; 
		//light_matrix = Camera::GetProjection(*mCamera, mWindow->width, mWindow->height) * Camera::GetView(*mCamera);
		mShadowGfxPass.UpdateLightMatrixBuffer(light_shadow_data.GetView(), light_shadow_data.GetProjection());
		for (auto& render_obj : mSceneContext.renderPassBuckets[0].renderObjects)
		{
			//if ((render_obj.maskLayer & ERenderMask::RM_ShadowCaster) == ERenderMask::RM_ShadowCaster)
			if (render_obj.renderMask.Contains(ERenderBits::ShadowCasterBit))
			{
				auto& matrix = render_obj.GPUModel.transform;
				mShadowGfxPass.PushModelTransformConstant(curr_cmd_buff, &render_obj.GPUModel);
				render_obj.mesh->Draw(curr_cmd_buff);
			}
		}
		mShadowGfxPass.End(curr_cmd_buff);



		//voxel scene geometry pass
		mSceneCollsionVoxelPass.Begin(&mVulkanContext, curr_cmd_buff, swapchain_img_idx);
		mSceneCollsionVoxelPass.UpdateCameraData(mCamera);
		mSceneCollsionVoxelPass.UpdateVoxelParamData({});
		for (auto& render_obj : mSceneContext.renderPassBuckets[0].renderObjects)
		{
			auto& matrix = render_obj.GPUModel.transform;
			mSceneCollsionVoxelPass.PushModelTransformConstant(curr_cmd_buff, &render_obj.GPUModel);
			render_obj.mesh->Draw(curr_cmd_buff);
		}
		mSceneCollsionVoxelPass.End(curr_cmd_buff);
		
		for (auto& render_passes : mSceneContext.renderPassBuckets)
		{
			if (render_passes.type == ERenderPassType::None)
				continue;

			// render pass
			//Vulkan::BeginFrame2(mVulkanContext, curr_frame, swapchain_img_idx, *mWindow, render_passes.pass->GetRenderPassHandle());/, & mImGuiUI);
			//render_passes.pass->Begin(&mVulkanContext, curr_cmd_buff, swapchain_img_idx);
			mScene.GetForwardRenderPass()->Begin(&mVulkanContext, curr_cmd_buff, swapchain_img_idx);

			uint32_t curr_pipeline = -1;
			for (auto& render_obj : render_passes.renderObjects)
			{
				static Vulkan::GraphicsPipeline* gfx_pipeline = nullptr;
				if (curr_pipeline != render_obj.pipelineIdx)
				{
					gfx_pipeline = render_passes.pipelines[render_obj.pipelineIdx].pipeline;
					gfx_pipeline->Bind(curr_cmd_buff);
					//required descriptor sets 
					//later batch descriptor sets 
					for(auto& desc : render_passes.pipelines[render_obj.pipelineIdx].descSetSpecifications)
						gfx_pipeline->BindDescriptorSets(curr_cmd_buff, &curr_frame.GetGlobalDescriptorSet(*desc), desc->set, 1);

					curr_pipeline = render_obj.pipelineIdx;
				}

				for (auto& push_const : render_obj.pushConstantSpecs)
					gfx_pipeline->PushConstant(curr_cmd_buff, push_const.stage_flag, push_const.offset, push_const.size, push_const.pdata);

				render_obj.mesh->Draw(curr_cmd_buff);
			}

			auto camera_desc_set = &curr_frame.GetGlobalDescriptorSet(mScene.GetDescriptorSetSpecificationByIdx(mSceneModelDescSetIdx));
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

			mDebugLines.BeginFrame(mVulkanContext.frameIndex);


#pragma region Emitter debug line bounds
			auto& emitter = mComputePass.mEmitter;
			glm::vec3 bounds_min = /*emitter.worldTransform * */glm::vec4(emitter.boundsMin, 1.0f);
			glm::vec3 bounds_max = /*emitter.worldTransform * */ glm::vec4(emitter.boundsMax, 1.0f);
			std::array<glm::vec3, 24> aabb_edges = Utils::GenerateAABBEdges(emitter.worldTransform, bounds_min, bounds_max);

		
			for(size_t i = 0; i < aabb_edges.size(); i+=2)
				mDebugLines.PushLine({ glm::vec4(aabb_edges[i], 1.0f), Colour32::sCyan}, { glm::vec4(aabb_edges[i+1], 1.0f), Colour32::sCyan });


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



			mDebugLines.DrawDebugLines(&mVulkanContext, curr_cmd_buff, camera_desc_set);

			mComputePass.HackRenderComputeParticle(&mVulkanContext, curr_cmd_buff, camera_desc_set);
			
			Editor::SceneRuntimeUIWindow(mScene.mSceneResources);
			ImGui::ShowDemoWindow();
			mImGuiUI.RenderFrame(curr_cmd_buff);
			//render_passes.pass->End(curr_cmd_buff);
			mScene.GetForwardRenderPass()->End(curr_cmd_buff);
			//Vulkan::EndFrame2(curr_frame, mVulkanContext, swapchain_img_idx, *mWindow, render_passes.pass->GetRenderPassHandle());
		}


		Vulkan::EndGfxFrameCommand(curr_frame, mVulkanContext, swapchain_img_idx);
		Vulkan::SwapPresentFrameImage(curr_frame, mVulkanContext, swapchain_img_idx, *mWindow, mScene.GetForwardRenderPass()->GetRenderPassHandle());

		mVulkanContext.frameIndex = (mVulkanContext.frameIndex + 1) % Vulkan::kMaxFramesInFlight;
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
		
		Vulkan::GPUBuffer::CopyToMappedMemory(frame_resources.globalCameraUniformBuffer, &MVP, sizeof(MVP));

		//update light descriptor uniform buffer 
		Vulkan::GPUBuffer::CopyToMappedMemory(frame_resources.globalDirectionalLightUniformBuffer, &mDirLight, sizeof(mDirLight));

		//update skybox descriptor uniform buffer 
		Vulkan::GPUBuffer::CopyToMappedMemory(frame_resources.globalSkyboxUniformBuffer, &mSkybox, sizeof(mSkybox));

		//update material 
#if DEBUG_MAT_HACK
		Vulkan::CopyToMappedMemory(mRustyMetalPBRMat.hackDebugMaterialGPUBuffer, &mRustyMetalPBRMat.hackDebugMaterial, sizeof(mRustyMetalPBRMat.hackDebugMaterial));
#endif // DEBUG_MAT_HACK

	}


} //#Fluxon