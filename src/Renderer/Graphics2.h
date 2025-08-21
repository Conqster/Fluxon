#pragma once

#include <vulkan/vulkan.h>
#include "Swapchain.h"

#include "Core/Camera.h"
#include "FrameResources.h"
#include "Mesh.h"

#include <functional>
#include "Core/UISystem.h"

#include "VulkanContext.h"
#include "Render Passes/ForwardRenderPass.h"

#include "Pipelines/GraphicsPipeline.h"

#include "PBRMaterial.h"
#include "Core/Window.h"


namespace Fluxon {
	namespace Vulkan {

		//Propably move this into vulkanContext struct/class 
		inline void BeginFrame(VulkanContext& vk_render_ctx, Vulkan::FrameResources& frame_data, uint32_t& o_img_idx, const Platform::Window& window, VkRenderPass render_pass, UI_System* ui_in_use = nullptr);
		inline void EndFrame(FrameResources& frame_data, VulkanContext& vk_render_ctx, uint32_t img_idx, Platform::Window& window, VkRenderPass render_pass);
		inline void WindowResizedRecreateResource(VulkanContext& vk_render_ctx, const Platform::Window& window, VkCommandPool cmd_pool, VkRenderPass render_pass);





		class Renderer
		{
		private:
			VulkanContext mVulkanContext;

			//TODO(Conqster) : This class should not manage window nor camera
			Platform::Window* mWindow = nullptr;
			EditorCamera* mCamera = nullptr;


			//std::vector<ForwardRenderPass> mPasses;
			ForwardRenderPass mForwardRenderPass;



			std::vector<FrameResources> mFrames;

			Vulkan::Resources::ImageSampler mTextureImageSampler;
			Vulkan::Resources::ImageSampler mSkyboxSamplerCubemap;

			Vulkan::PBRMaterial mRustyMetalPBRMat;


			//VkDescriptorSetLayout mDescriptorSetLayout;
			DescriptorSetSpecification mModelSceneDescriptorLayout;
			DescriptorSetSpecification mSkyboxDescriptorLayout;
			DescriptorSetSpecification mMaterialDescriptorLayout;

			std::vector<Mesh> mMeshes;
			bool mUseSamplingAnisotropy = false;

			glm::mat4 mModelMatrix = glm::mat4(1.0f);
			std::array<GPUStructures::Model, 4> mModelDatas;

			GPUStructures::DirectionalLight mDirLight;
			GPUStructures::Skybox mSkybox;

			UI_System mImGuiUI;

			//depercate util for testing 
			//Vulkan::Device mDevice;
			Vulkan::GraphicsPipeline mModelGraphicsPipeline;
			Vulkan::GraphicsPipeline mSkyboxGraphicsPipeline;




		public:
			void Init(Platform::Window& window, EditorCamera* app_cam_ref = nullptr);

			void SetIMGUI(UI_System& ui) {
				mImGuiUI = ui;
			}

			std::vector<FrameResources>& GetFrames() { return mFrames; }
			ForwardRenderPass& GetForwardRenderPass() { return mForwardRenderPass; }
			VulkanContext& GetVulkanContext() { return mVulkanContext; }
			//Device& GetDeprecatedDevice() { return mDevice; }

			void CreateModelSceneDescriptionSetLayout();
			void CreateSkyboxDescriptionSetLayout();
			//probably need a system to fill set minimymum - 1 towards zero 
			//becaasuse vulkan required set from 0 sequencly up 
			void CreateDummyDescriptionSetLayout(Vulkan::VulkanContext* vk_ctx, DescriptorSetSpecification& description_set_spec, uint32_t set);


			VkShaderModule CreateShaderModule(const VulkanContext& vk_ctx, const std::vector<char>& code);


			//Create Test resources 
			void CreateTestSceneData(const Vulkan::VulkanContext* vk_ctx, VkCommandPool cmd_pool);


			void DrawScene(EditorCamera* camera, glm::vec3 translate_model, glm::vec3 rotate_axis);
			void UpdateGlobalFrameDataUBO(FrameResources& frame_resources, glm::vec3 model_pos, glm::vec3 rotate_axis, float win_width, float win_height);
			//void RecordRenderPass(VkCommandBuffer cmd_buff, ForwardRenderPass& pass, const FrameResources& frame_resources, uint32_t img_idx, VkExtent2D render_area_extent, UI_System* ui_in_use = nullptr);

			void SceneUIWindow();

			void Cleanup();

			bool Model_Transform_Editor_Panel_UI(glm::mat4& transform);

			void DebugInputKeyUI();
		};
	} //#Vulkan
}//#Fluxon