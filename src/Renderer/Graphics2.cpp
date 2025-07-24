#include "Graphics2.h"

#include "Core/Utilities.h"
#include "Resources/ModelLoader.h"

#include "Core/InputSystem.h"

namespace Fluxon {
	void Vulkan::BeginFrame(VulkanContext& vk_render_ctx, FrameResources& frame_resources, uint32_t& o_img_idx, const Platform::Window& window, VkRenderPass render_pass, UI_System* ui_in_use)
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
			WindowResizedRecreateResource(vk_render_ctx, window, frame_resources.commandPool, render_pass);
			return;
		}
		else if (acquire_nxt_img_result != VK_SUCCESS && acquire_nxt_img_result != VK_SUBOPTIMAL_KHR)
			std::cout << "\033[1;31m [DRAW FRAME]\033[0m: " << "Failed to Acquire Swap Chain Image" << std::endl;


		//check if the acquired swapchain image is in use, wait for it
		//auto& swapchain_img = mSwapchain->GetImages()[img_index];
		//if (mSwapchain->mImagesInFlight[img_index] != VK_NULL_HANDLE)
		//	vkWaitForFences(mDevice.device, 1, &mSwapchain->mImagesInFlight[img_index], VK_TRUE, UINT64_MAX);

		vkResetCommandPool(vk_render_ctx.device, frame_resources.commandPool, 0);

		if (ui_in_use)
			ui_in_use->StartFrame();

		VkCommandBufferBeginInfo begin_info{};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = 0; //VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		begin_info.pInheritanceInfo = nullptr;


		if (vkBeginCommandBuffer(frame_resources.commandBuffer, &begin_info) != VK_SUCCESS)
			throw std::runtime_error("Failed to begin recording command buffer!!!!!");
	}

	void Vulkan::EndFrame(Vulkan::FrameResources& frame_resources, VulkanContext& vk_render_ctx, uint32_t img_idx, Platform::Window& window, VkRenderPass render_pass)
	{
		if (vkEndCommandBuffer(frame_resources.commandBuffer) != VK_SUCCESS)
			throw std::runtime_error("Failed to record command buffer!!!!");

		VkSubmitInfo submit_info{};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitSemaphores = &frame_resources.imageAvailable;
		submit_info.pWaitDstStageMask = wait_stages;

		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &frame_resources.commandBuffer;
		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = &vk_render_ctx.swapchain->GetImages()[img_idx].renderComplete;

		if (vkQueueSubmit(vk_render_ctx.graphicsQueue, 1, &submit_info, frame_resources.inFlight) != VK_SUCCESS)
			throw std::runtime_error("Failed to submit draw command buffer!!!!");

		//Matrk image as in use by this frame 
		//mSwapchain->mImagesInFlight[img_index] = frame.inFlight;

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
			WindowResizedRecreateResource(vk_render_ctx, window, frame_resources.commandPool, render_pass);
		}
		else if (presenting_queue_result != VK_SUCCESS)
			std::cout << "\033[1;31m [DRAW FRAME]\033[0m: " << "Failed to Present Image" << std::endl;

		vk_render_ctx.frameIndex = (vk_render_ctx.frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void Vulkan::WindowResizedRecreateResource(VulkanContext& vk_render_ctx, const Platform::Window& window, VkCommandPool cmd_pool, VkRenderPass render_pass)
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



	namespace Vulkan {

		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
			VkDebugUtilsMessageTypeFlagsEXT message_type,
			const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
			void* p_user_data) {

			//std::cerr << "Validation layer: " << p_callback_data->pMessage << std::endl;
			std::cerr << "\033[1;34m [Validation layer]\033[0m: " << p_callback_data->pMessage << std::endl;
			return VK_FALSE;
		}


		void Renderer::Init(Platform::Window& window, Camera* app_cam_ref)
		{
			mWindow = &window;
			if (app_cam_ref)
				mCamera = app_cam_ref;

			mVulkanContext = Vulkan::CreateVulkanContext(&window, true, DebugCallback);
			//mDevice = Vulkan::InitDevice(&window, true, DebugCallback);

			//mDevice.instance = mVulkanContext.instance;t
			//mDevice.physicalDevice = mVulkanContext.physicalDevice;
			//mDevice.device = mVulkanContext.device;
			//mDevice.surface = mVulkanContext.surface;
			//mDevice.graphicsQueue = mVulkanContext.graphicsQueue;
			//mDevice.presentQueue = mVulkanContext.presentQueue;
			//mDevice.queueFamilyIndices = mVulkanContext.queueFamilyIndices;
			//mDevice.debugMessager = mVulkanContext.debugMessager;

			//CreateForwardRenderPassPipeline();
			mForwardRenderPass.Init(&mVulkanContext);
			mFrames.resize(Vulkan::MAX_FRAMES_IN_FLIGHT);
			Vulkan::CreateFrameObjects(&mVulkanContext, mFrames);

			//TODO : later move framebuffer/renderpass out of swap chain 
			//so it would depend on renderpass 
			//mVulkanContext.swapchain = std::make_unique<Vulkan::Swapchain>(&mDevice, window, mFrames[0].commandPool, mForwardRenderPass.renderPass);
			mVulkanContext.swapchain = std::make_unique<Vulkan::Swapchain>(&mVulkanContext, window, mFrames[0].commandPool, mForwardRenderPass.GetRenderPassHandle());

			//TODO(Conqster-Uniformbuffer): have functionm to create new description set layout dynamically 
			CreateSkyboxDescriptionSetLayout();
			CreateModelSceneDescriptionSetLayout();

			std::array<const char*, Vulkan::SHADER_PBR_MATERIAL_MAX_MAPS> old_pbr_mat_paths =
			{
				"assets/textures/rusty_metal/rusty_metal_diff.jpg",
				"assets/textures/rusty_metal/rusty_metal_nor.jpg",
				"assets/textures/rusty_metal/rusty_metal_arm_not2.jpg",
			};


			std::array<const char*, Vulkan::SHADER_PBR_MATERIAL_MAX_MAPS> pbr_mat_paths =
			{
				"assets/textures/worn_bronze_metal/2K/worn_bronze_metal_base_colour.jpg",
				"assets/textures/worn_bronze_metal/2K/worn_bronze_metal_normal.jpg",
				"assets/textures/worn_bronze_metal/2K/worn_bronze_metal_orm.jpg",
			};
			mRustyMetalPBRMat = Vulkan::PBRMaterial::CreatePBRMaterialMaps(&mVulkanContext, mFrames[0].commandPool, pbr_mat_paths);
			//Vulkan::PBRMaterial::CreatePBRMaterialMaps(&mVulkanContext, mRustyMetalPBRMat, mFrames[0].commandPool, pbr_mat_paths);
			mMaterialDescriptorLayout = Vulkan::PBRMaterial::GenerateMaterialDecriptorSetLayoutSpecification(&mVulkanContext, mRustyMetalPBRMat, 0, 0);
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

			GraphicsPipelineSpecification model_gfx_pipeline_spec{};
			model_gfx_pipeline_spec.vertexShaderPath = "assets/shaders/spirvs/model_shading.vert.spv";
			model_gfx_pipeline_spec.fragmentShaderPath = "assets/shaders/spirvs/model_shading.frag.spv";
			model_gfx_pipeline_spec.renderPass = &mForwardRenderPass;
			model_gfx_pipeline_spec.descriptorSetLayouts = {
				mMaterialDescriptorLayout.layoutHandle,
				mModelSceneDescriptorLayout.layoutHandle,
				mSkyboxDescriptorLayout.layoutHandle };
			model_gfx_pipeline_spec.pushConstantRanges = { push_constant_range, push_constant_range2 };
			////leave remain data as default 
			mModelGraphicsPipeline.Init(&mVulkanContext, model_gfx_pipeline_spec);


			GraphicsPipelineSpecification skybox_gfx_pipeline_spec{};
			skybox_gfx_pipeline_spec.vertexShaderPath = "assets/shaders/spirvs/skybox.vert.spv";
			skybox_gfx_pipeline_spec.fragmentShaderPath = "assets/shaders/spirvs/skybox.frag.spv";
			skybox_gfx_pipeline_spec.renderPass = &mForwardRenderPass;
			skybox_gfx_pipeline_spec.descriptorSetLayouts = {
				mMaterialDescriptorLayout.layoutHandle,
				mModelSceneDescriptorLayout.layoutHandle,
				mSkyboxDescriptorLayout.layoutHandle };
			skybox_gfx_pipeline_spec.pushConstantRanges = { };

			skybox_gfx_pipeline_spec.rasterisationCullMode = VK_CULL_MODE_FRONT_BIT;
			skybox_gfx_pipeline_spec.enableDepthWrite = false;
			skybox_gfx_pipeline_spec.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
			////leave remain data as default 
			mSkyboxGraphicsPipeline.Init(&mVulkanContext, skybox_gfx_pipeline_spec);


			CreateTestSceneData(&mVulkanContext, mFrames[0].commandPool);

			std::vector<DescriptorSetSpecification> desc_set_layout_spec = { mModelSceneDescriptorLayout, mSkyboxDescriptorLayout, mMaterialDescriptorLayout };
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
			Vulkan::Resources::CreateImageSamplerTexture(&mVulkanContext,
				mFrames[0].commandPool,
				//"textures/Manchester_512.png",
				"assets/textures/melady_square.jpg",
				tex_image_format,
				mTextureImageSampler,
				mUseSamplingAnisotropy);
			Vulkan::DescriptorBinding texture_binding = Vulkan::DescriptorBinding::MakeImageSampler(1, 1, mTextureImageSampler);

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
				mSkyboxSamplerCubemap,
				mUseSamplingAnisotropy);
			Vulkan::DescriptorBinding skybox_cube_tex_binding = Vulkan::DescriptorBinding::MakeImageSampler(2, 0, mSkyboxSamplerCubemap);

			auto pbr_material_tex_binding = PBRMaterial::GetDescriptorSetBindings(mRustyMetalPBRMat, 0);

			for (auto& frame : mFrames)
			{
				for (auto& pbr_mat_binding : pbr_material_tex_binding)
					frame.AppendDescriptorBinding(pbr_mat_binding);
				frame.AppendDescriptorBinding(texture_binding);
				frame.AppendDescriptorBinding(skybox_cube_tex_binding);
				frame.WritePendingDescriptorBinding(mVulkanContext.device);
			}
		}


		void Renderer::CreateModelSceneDescriptionSetLayout()
		{
			//to create a ubo this function could take in the binding id
			mModelSceneDescriptorLayout.set = 1;
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


			//group bindings 
			std::array<VkDescriptorSetLayoutBinding, 3> bindings = { ubo_layout_binding, sampler_layout_binding, directional_lighting_layout_binding };
			VkDescriptorSetLayoutCreateInfo layout_create_info{};
			layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layout_create_info.bindingCount = static_cast<uint32_t>(bindings.size());
			layout_create_info.pBindings = bindings.data();

			if (vkCreateDescriptorSetLayout(mVulkanContext.device, &layout_create_info, nullptr, &mModelSceneDescriptorLayout.layoutHandle) != VK_SUCCESS)
				throw std::runtime_error("Failed to create descriptor set layout!!!!!!!!");
		}

		void Renderer::CreateSkyboxDescriptionSetLayout()
		{

			VkDescriptorSetLayoutBinding skybox_samplercube_layout_binding{};
			skybox_samplercube_layout_binding.binding = 0;
			skybox_samplercube_layout_binding.descriptorCount = 1;
			skybox_samplercube_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			skybox_samplercube_layout_binding.pImmutableSamplers = nullptr;
			skybox_samplercube_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

			VkDescriptorSetLayoutBinding skybox_ubo_layout_binding{};
			skybox_ubo_layout_binding.binding = 1;
			skybox_ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			skybox_ubo_layout_binding.descriptorCount = 1;
			skybox_ubo_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			skybox_ubo_layout_binding.pImmutableSamplers = nullptr;

			//group bindings 
			std::array<VkDescriptorSetLayoutBinding, 2> bindings = { skybox_samplercube_layout_binding, skybox_ubo_layout_binding };
			VkDescriptorSetLayoutCreateInfo layout_create_info{};
			layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layout_create_info.bindingCount = static_cast<uint32_t>(bindings.size());
			layout_create_info.pBindings = bindings.data();

			mSkyboxDescriptorLayout.set = 2;
			if (vkCreateDescriptorSetLayout(mVulkanContext.device, &layout_create_info, nullptr, &mSkyboxDescriptorLayout.layoutHandle) != VK_SUCCESS)
				throw std::runtime_error("Failed to create descriptor set layout!!!!!!!!");
		}



		void Renderer::CreateDummyDescriptionSetLayout(Vulkan::VulkanContext* vk_ctx, DescriptorSetSpecification& description_set_spec, uint32_t set)
		{
			description_set_spec.set = set;

			VkDescriptorSetLayoutCreateInfo layout_create_info{};
			layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layout_create_info.bindingCount = 0;
			layout_create_info.pBindings = VK_NULL_HANDLE;

			if (vkCreateDescriptorSetLayout(vk_ctx->device, &layout_create_info, nullptr, &description_set_spec.layoutHandle) != VK_SUCCESS)
				throw std::runtime_error("Failed to create descriptor set layout!!!!!!!!");
		}


		VkShaderModule Renderer::CreateShaderModule(const VulkanContext& vk_ctx, const std::vector<char>& code)
		{
			VkShaderModuleCreateInfo shader_module_create_info{};
			shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			shader_module_create_info.codeSize = code.size();
			shader_module_create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

			VkShaderModule shader_module;
			if (vkCreateShaderModule(vk_ctx.device, &shader_module_create_info, nullptr, &shader_module) != VK_SUCCESS)
				throw std::runtime_error("Failed to create shader module!!!!!!");
			return shader_module;
		}

		void Renderer::CreateTestSceneData(const Vulkan::VulkanContext* vk_ctx, VkCommandPool cmd_pool)
		{
			mMeshes.resize(4);
			std::vector<GPUStructures::Vertex> vertices_data;
			std::vector<uint16_t> indices_data;
			Fluxon::Utils::GetUnitCubeVertices(vertices_data, indices_data);
			mMeshes[0].Generate(vk_ctx, cmd_pool, vertices_data, indices_data);

			vertices_data.clear();
			indices_data.clear();
			Fluxon::Utils::GenerateSphereVertices(vertices_data, indices_data);
			mMeshes[1].Generate(vk_ctx, cmd_pool, vertices_data, indices_data);
			mModelDatas[1].transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.63f, 0.28f, 5.47f));

			ModelLoader quick_loader;
			auto [model_vertices, model_indices] = quick_loader.LoadAsVerticesData("assets/models/model.dae", true, false);
			mMeshes[2].Generate(vk_ctx, cmd_pool, model_vertices, model_indices);
			mModelDatas[2].transform = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, -1.56, 1.19f)) *
				glm::scale(glm::mat4(1.0f), glm::vec3(0.02f));

			vertices_data.clear();
			indices_data.clear();
			Fluxon::Utils::GetUnitQuadVertices(vertices_data, indices_data);
			mMeshes[3].Generate(vk_ctx, cmd_pool, vertices_data, indices_data);
			mModelDatas[3].transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.5f, 0.0f)) *
				glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
				glm::scale(glm::mat4(1.0f), glm::vec3(10.0f));


			if (mCamera)
				mCamera->SetLookTarget(mModelDatas[1].transform[3]);
		}

		void Renderer::DrawScene(Camera* camera, glm::vec3 translate_model, glm::vec3 rotate_axis)
		{
			mCamera = camera;
			uint32_t swapchain_img_idx;
			auto& curr_frame = mFrames[mVulkanContext.frameIndex];
			auto& curr_cmd_buff = curr_frame.commandBuffer;
			//Vulkan::BeginFrame(mVulkanContext, curr_frame, swapchain_img_idx, mWindow, mForwardRenderPass.renderPass, &mImGuiUI);

			Vulkan::BeginFrame(mVulkanContext, curr_frame, swapchain_img_idx, *mWindow, mForwardRenderPass.GetRenderPassHandle(), &mImGuiUI);


			UpdateGlobalFrameDataUBO(curr_frame, translate_model, rotate_axis, mWindow->width, mWindow->height);

			//forward render pass
			//RecordRenderPass(curr_frame.commandBuffer, mForwardRenderPass, curr_frame, swapchain_img_idx, mVulkanContext.swapchain->GetExtent(), &mImGuiUI);
			mForwardRenderPass.Begin(&mVulkanContext, curr_cmd_buff, swapchain_img_idx);

			mModelGraphicsPipeline.Bind(curr_cmd_buff);
			mModelGraphicsPipeline.BindDescriptorSets(curr_cmd_buff, &curr_frame.GetGlobalDescriptorSet(mModelSceneDescriptorLayout), mModelSceneDescriptorLayout.set);
			mModelGraphicsPipeline.BindDescriptorSets(curr_cmd_buff, &curr_frame.GetGlobalDescriptorSet(mSkyboxDescriptorLayout), mSkyboxDescriptorLayout.set);
			mModelGraphicsPipeline.BindDescriptorSets(curr_cmd_buff, &curr_frame.GetGlobalDescriptorSet(mMaterialDescriptorLayout), mMaterialDescriptorLayout.set);
			//TODO(COnqster): might combine/add "vkCmdBindDescriptorSets" to GrpahicsPipeline
			//vkCmdBindDescriptorSets(curr_frame.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mModelGraphicsPipeline.GetLayoutHandle(), 0, 1, &curr_frame.globalDescriptorSet, 0, nullptr);

			for (size_t i = 0; i < mMeshes.size(); i++)
			{
				glm::vec4 cam_and_new_material_sys = glm::vec4(mCamera->position, (i == 1) ? 1 : 0);
				mModelGraphicsPipeline.PushConstant(curr_cmd_buff, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(mModelDatas[0]), sizeof(glm::vec4), (void*)&cam_and_new_material_sys);
				mModelGraphicsPipeline.PushConstant(curr_cmd_buff, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mModelDatas[i]), (void*)&mModelDatas[i]);
				mMeshes[i].Draw(curr_cmd_buff);
			}

			//sky box
			mSkyboxGraphicsPipeline.Bind(curr_cmd_buff);
			//mSkyboxGraphicsPipeline.BindDescriptorSets(curr_cmd_buff, &curr_frame.GetGlobalDescriptorSet(mModelSceneDescriptorLayout), mModelSceneDescriptorLayout.set);
			//mSkyboxGraphicsPipeline.BindDescriptorSets(curr_cmd_buff, &curr_frame.GetGlobalDescriptorSet(mSkyboxDescriptorLayout), mSkyboxDescriptorLayout.set);
			std::vector<VkDescriptorSet> desc_set_layout = { curr_frame.GetGlobalDescriptorSet(mModelSceneDescriptorLayout),  curr_frame.GetGlobalDescriptorSet(mSkyboxDescriptorLayout) };
			uint32_t first_set = glm::min(mModelSceneDescriptorLayout.set, mSkyboxDescriptorLayout.set);
			mSkyboxGraphicsPipeline.BindDescriptorSets(curr_cmd_buff, desc_set_layout.data(), first_set, desc_set_layout.size());

			//mesh idx 0 is a cube
			mMeshes[0].Draw(curr_cmd_buff);

			/////////////////////////////////////
			// IMGUI
			/////////////////////////////////////
			//ImGui::ShowDemoWindow();
			SceneUIWindow();
			DebugInputKeyUI();
			mImGuiUI.RenderFrame(curr_cmd_buff);

			mForwardRenderPass.End(curr_cmd_buff);

			Vulkan::EndFrame(curr_frame, mVulkanContext, swapchain_img_idx, *mWindow, mForwardRenderPass.GetRenderPassHandle());
		}

		void Renderer::UpdateGlobalFrameDataUBO(Vulkan::FrameResources& frame_resources, glm::vec3 translate_model, glm::vec3 rotate_axis, float win_width, float win_height)
		{
			GPUStructures::MVP_UniformBufferObject MVP;
			//mModelMatrix = glm::translate(glm::mat4(1.0f), translate_model) * glm::rotate(glm::mat4(1.0f), glm::radians(glm::length(rotate_axis)), glm::normalize(rotate_axis));

			if (glm::length(rotate_axis) > 0.0f)
				mModelDatas[0].transform = glm::rotate(mModelDatas[0].transform, glm::radians(glm::length(rotate_axis)), glm::normalize(rotate_axis));
			mModelDatas[1].transform = glm::translate(mModelDatas[1].transform, translate_model);

			if (!mCamera)
				return;
			MVP.view = Camera::GetView(*mCamera);
			MVP.projection = Camera::GetProjection(*mCamera, win_width, win_height);
			MVP.position = mCamera->position;

			Vulkan::CopyToMappedMemory(frame_resources.globalCameraUniformBuffer, &MVP, sizeof(MVP));

			//update light descriptor uniform buffer 
			Vulkan::CopyToMappedMemory(frame_resources.globalDirectionalLightUniformBuffer, &mDirLight, sizeof(mDirLight));

			//update skybox descriptor uniform buffer 
			Vulkan::CopyToMappedMemory(frame_resources.globalSkyboxUniformBuffer, &mSkybox, sizeof(mSkybox));

			//update material 
#if DEBUG_MAT_HACK
			Vulkan::CopyToMappedMemory(mRustyMetalPBRMat.hackDebugMaterialGPUBuffer, &mRustyMetalPBRMat.hackDebugMaterial, sizeof(mRustyMetalPBRMat.hackDebugMaterial));
#endif // DEBUG_MAT_HACK

		}


		void Renderer::SceneUIWindow()
		{
			bool ui_trying_to_hook = false;
			ImGui::Begin("Test Scene Modification Window");

			for (auto& model : mModelDatas)
			{
				ImGui::PushID(&model);
				ImGui::SeparatorText("Model");
				Model_Transform_Editor_Panel_UI(model.transform);
				ImGui::ColorEdit3("colour", &model.colour[0]);
				ImGui::PopID();
			}

			ImGui::Spacing();
			bool window_cursor_lock = Platform::IsCursorLocked(mWindow);
			ImGui::Checkbox("Mouse Cursor", &window_cursor_lock);
			ImGui::Spacing();

			ImGui::SeparatorText("Camera Properties");
			if (mCamera)
			{
				auto cam_pos = (*mCamera).position;
				ImGui::Text("Position x: %f, y: %f, z: %f", cam_pos.x, cam_pos.y, cam_pos.z);
				auto cam_forward = mCamera->forward;
				ImGui::Text("Forward x: %f, y: %f, z: %f", cam_forward.x, cam_forward.y, cam_forward.z);
				ImGui::SliderFloat("Move Speed", &mCamera->moveSpeed, 0.01f, 10.0f);
				ImGui::SliderFloat("Rot Speed", &mCamera->rotSpeed, 0.01f, 1.0f);
				ImGui::SliderFloat("FOV", &mCamera->fov, 0.0f, 179.0f, "%.1f");
				ImGui::DragFloat("Near", &mCamera->nearPlane, 0.1f, 0.01f, 50.0f, "%.1f");
				ImGui::DragFloat("Far", &mCamera->farPlane, 0.1f, 0.0f, 500.0f, "%.1f");
			}
			else
				ImGui::Text("No vaild Camera !!!!!!");


			ImGui::SeparatorText("Renderer Properties");
			ImGui::ColorEdit3("clear colour", &mForwardRenderPass.mClearColour[0]);
			ImGui::SeparatorText("Directional Light");
			bool enable = static_cast<bool>(mDirLight.enable);
			if (ImGui::Checkbox("enable light", &enable))
				mDirLight.enable = enable;
			ImGui::DragFloat3("light direction", &mDirLight.direction[0], 0.1f, -2.0f, 2.0f);
			ImGui::ColorEdit3("light colour", &mDirLight.colour[0]);
			ImGui::SliderInt("model shinness", &mDirLight.shinness, 1, 128);
			ImGui::SeparatorText("Skybox");
			enable = static_cast<bool>(mSkybox.enable);
			if (ImGui::Checkbox("enable skybox", &enable))
				mSkybox.enable = enable;
			ImGui::SliderFloat("skybox influence", &mSkybox.skyboxInfluencity, 0.0f, 1.0f, "%.1f");
			ImGui::SliderFloat("skybox reflect", &mSkybox.objectReflectivity, 0.0f, 1.0f, "%.1f");
			ImGui::SliderInt("hack _padding for switch material map", &mSkybox._padding, 0, 10);

			//test hack material 
#if DEBUG_MAT_HACK
			ImGui::SeparatorText("Hack Material");
			ImGui::ColorEdit3("hack colour", &mRustyMetalPBRMat.hackDebugMaterial.colour[0]);
			ImGui::SliderFloat("hack metallic", &mRustyMetalPBRMat.hackDebugMaterial.metallic, 0.0f, 1.0f);
			ImGui::SliderFloat("hack roughness", &mRustyMetalPBRMat.hackDebugMaterial.roughness, 0.0f, 1.0f);
			ImGui::SliderFloat("hack ao", &mRustyMetalPBRMat.hackDebugMaterial.ao, 0.0f, 1.0f);
#endif // DEBUG_MAT_HACK

			ImGuiIO ui_io = ImGui::GetIO();
			ui_trying_to_hook = ui_io.WantTextInput || ui_io.WantCaptureMouse;
			//ui_trying_to_hook = ImGui::IsAnyItemActive();
			ImGui::Checkbox("UI Trying to use cursor", &ui_trying_to_hook);
			ImGui::Text("UI Mouse drag threshold: %f", ui_io.MouseDragThreshold);
			double x_pos, y_pos;
			glfwGetCursorPos(mWindow->glfwWindowHandle, &x_pos, &y_pos);
			int width, height;
			glfwGetWindowSize(mWindow->glfwWindowHandle, &width, &height);
			ImGui::Text("Mouse cursor pos: %f, %f", x_pos, y_pos);
			ImGui::Text("Window size: %f, %f", width, height);
			

			ImGui::End();
		}

		void Renderer::Cleanup()
		{
			vkDeviceWaitIdle(mVulkanContext.device);

			mImGuiUI.Shutdown();

			Vulkan::DestroyFrameObjects(&mVulkanContext, mFrames);

			mModelGraphicsPipeline.Cleanup(&mVulkanContext);
			mSkyboxGraphicsPipeline.Cleanup(&mVulkanContext);

			mForwardRenderPass.Cleanup(&mVulkanContext);
			mVulkanContext.swapchain->Cleanup();

			for (auto& mesh : mMeshes)
				mesh.Cleanup(mVulkanContext.device);


			Vulkan::Resources::DestoryTextureImageSampler(mVulkanContext.device, mTextureImageSampler);
			Vulkan::Resources::DestoryTextureImageSampler(mVulkanContext.device, mSkyboxSamplerCubemap);

			PBRMaterial::DestoryPBRMaterial(&mVulkanContext, mRustyMetalPBRMat);

			//vkDestroyDescriptorSetLayout(mVulkanContext.device, mDescriptorSetLayout, nullptr);
			vkDestroyDescriptorSetLayout(mVulkanContext.device, mModelSceneDescriptorLayout.layoutHandle, nullptr);
			vkDestroyDescriptorSetLayout(mVulkanContext.device, mSkyboxDescriptorLayout.layoutHandle, nullptr);
			vkDestroyDescriptorSetLayout(mVulkanContext.device, mMaterialDescriptorLayout.layoutHandle, nullptr);

			//Vulkan::DestroyDevice(&mDevice, true);
			Vulkan::DestroyVulkanContext(&mVulkanContext, true);
		}

		bool Renderer::Model_Transform_Editor_Panel_UI(glm::mat4& transform)
		{

			auto& translate = transform[3];
			glm::vec3 euler;
			glm::vec3 scale;
			glm::vec3 hack;
			Fluxon::Utils::DecomposeTransform(transform, hack, euler, scale);

			bool update = ImGui::DragFloat3("Translation", &translate[0], 0.01f, (0.0f), (0.0f), "%.2f");
			//= ImGui::IsItemDeactivatedAfterEdit();//ImGui::IsItemEdited();
			//if (translate_update)
			//	transform.invMatrix = glm::inverse(transform.matrix);

			update = ImGui::DragFloat3("Euler", &euler[0], 0.01f, (0.0f), (0.0f), "%.2f");
			update |= ImGui::DragFloat3("Scale", &scale[0], 0.01f, (0.0f), (0.0f), "%.2f");
			if (update)
			{
				transform = glm::translate(glm::mat4(1.0f), static_cast<glm::vec3>(translate)) *
					//glm::toMat4(glm::quat(glm::radians(euler))) *
					glm::mat4_cast(glm::quat(glm::radians(euler))) *
					glm::scale(glm::mat4(1.0f), scale);
			}

			return update;
		}

		void Renderer::DebugInputKeyUI()
		{
			Fluxon::EventSystem::Input input = Fluxon::EventSystem::Input::Instance();
			if(ImGui::Begin("Debug Input System Events"))
			{
				static const char true_text[] = "True";
				static const char false_text[] = "False";

				static const ImVec4 true_col = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
				static const ImVec4 false_col = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

				auto& SetImguiKeyTextState = [&](const char* text, bool state)
				{

					char text_fmt[64];
					snprintf(text_fmt, sizeof(text_fmt), "%s: %s", text,
							(state) ? true_text : false_text);
					ImGui::TextColored((state) ? true_col : false_col, text_fmt);
					
				};


				auto& SetImguiKeyTextStatePressedTypes = [&](const char* text, const int key_code)
				{
					bool held_state = input.GetKey(key_code);
					bool pressed = input.GetKeyDown(key_code);
					bool released = input.GetKeyUp(key_code);

					SetImguiKeyTextState(text, held_state);

					ImGui::SameLine();
					char text_fmt[64];
					snprintf(text_fmt, sizeof(text_fmt), "pressed: %s",
								(pressed) ? true_text : false_text);
					ImGui::TextColored((pressed) ? true_col : false_col, text_fmt);

					ImGui::SameLine();
					snprintf(text_fmt, sizeof(text_fmt), "relesed: %s",
						(released) ? true_text : false_text);
					ImGui::TextColored((released) ? true_col : false_col, text_fmt);
				};

				SetImguiKeyTextStatePressedTypes("Key T", GLFW_KEY_T);
				SetImguiKeyTextStatePressedTypes("Key Q", GLFW_KEY_Q);
				SetImguiKeyTextStatePressedTypes("Key A", GLFW_KEY_A);
				SetImguiKeyTextStatePressedTypes("Key Z", GLFW_KEY_Z);
				SetImguiKeyTextStatePressedTypes("Key P", GLFW_KEY_P);
				SetImguiKeyTextStatePressedTypes("Key J", GLFW_KEY_J);
				SetImguiKeyTextStatePressedTypes("Left Mouse", GLFW_MOUSE_BUTTON_LEFT);
				SetImguiKeyTextStatePressedTypes("Right Mouse", GLFW_MOUSE_BUTTON_RIGHT);
			}
			ImGui::End();

		}



	} //#Vulkan2
}//#Fluxon