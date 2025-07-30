#pragma once

#include <vulkan/vulkan.h>
#include "Renderer/Swapchain.h"

#include "Core/Camera.h"
#include "Renderer/FrameResources.h"
#include "Renderer/Mesh.h"

#include <functional>
#include "Core/UISystem.h"

#include "Renderer/VulkanContext.h"
#include "Renderer/Render Passes/ForwardRenderPass.h"

#include "Renderer/Pipelines/GraphicsPipeline.h"

#include "Renderer/PBRMaterial.h"
#include "Core/Window.h"

#include "Scene/Scene.h"
#include "Scene/SceneData.h"

namespace Fluxon {

	namespace Vulkan {
	
		//Propably move this into vulkanContext struct/class 
		inline void BeginFrame2(VulkanContext& vk_render_ctx, Vulkan::FrameResources& frame_data, uint32_t& o_img_idx, const Platform::Window& window, VkRenderPass render_pass, UI_System* ui_in_use = nullptr);
		inline void EndFrame2(FrameResources& frame_data, VulkanContext& vk_render_ctx, uint32_t img_idx, Platform::Window& window, VkRenderPass render_pass);
		inline void WindowResizedRecreateResource2(VulkanContext& vk_render_ctx, const Platform::Window& window, VkCommandPool cmd_pool, VkRenderPass render_pass);
	
	}
	


	class SceneRuntime
	{
	private:
		Vulkan::VulkanContext mVulkanContext;

		//TODO(Conqster) : This class should not manage window nor camera
		Platform::Window* mWindow = nullptr;
		Camera* mCamera = nullptr;

		std::vector<Vulkan::FrameResources> mFrames;

		//VkDescriptorSetLayout mDescriptorSetLayout;
		bool mUseSamplingAnisotropy = false;

		GPUStructures::DirectionalLight mDirLight;
		GPUStructures::Skybox mSkybox;

		UI_System mImGuiUI;

		Scene mScene;

		SceneContext mSceneContext;


	public:
		void Init(Platform::Window& window, Camera* app_cam_ref = nullptr);

		void SetIMGUI(UI_System& ui) {
			mImGuiUI = ui;
		}

		std::vector<Vulkan::FrameResources>& GetFrames() { return mFrames; }
		//ForwardRenderPass& GetForwardRenderPass() { return mForwardRenderPass; }
		Vulkan::VulkanContext& GetVulkanContext() { return mVulkanContext; }
		//Device& GetDeprecatedDevice() { return mDevice; }

		Scene& GetScene() { return mScene; }

		void CreateModelSceneDescriptionSetLayout(Vulkan::VulkanContext* vk_ctx, Vulkan::DescriptorSetSpecification& description_set_spec);
		void CreateSkyboxDescriptionSetLayout(VkDevice d, Vulkan::DescriptorSetSpecification& description_set_spec);
		//probably need a system to fill set minimymum - 1 towards zero 
		//becaasuse vulkan required set from 0 sequencly up 
		void CreateDummyDescriptionSetLayout(Vulkan::VulkanContext* vk_ctx, Vulkan::DescriptorSetSpecification& description_set_spec, uint32_t set);


		//Create Test resources 
		void CreateTestSceneData(const Vulkan::VulkanContext* vk_ctx, VkCommandPool cmd_pool);

		void Update(float delta_time, Platform::Window* p_window, Camera* p_frame_camera);
		//void DrawScene2(Camera* camera, glm::vec3 translate_model, glm::vec3 rotate_axis);

		void UpdateGlobalFrameDataUBO(Vulkan::FrameResources& frame_resources, float win_width, float win_height);
		//void RecordRenderPass(VkCommandBuffer cmd_buff, ForwardRenderPass& pass, const FrameResources& frame_resources, uint32_t img_idx, VkExtent2D render_area_extent, UI_System* ui_in_use = nullptr);

		void SceneUIWindow();

		void Cleanup() {

		}

		bool Model_Transform_Editor_Panel_UI(glm::mat4& transform);

		void DebugInputKeyUI();
	};
} //#Fluxon