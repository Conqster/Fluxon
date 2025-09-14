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

#include "Serialisation/SceneSerialisation.h"

#include "Renderer/Render Passes/ShadowRenderPass.h"
#include "Renderer/Render Passes/ComputePass.h"

#include "Renderer/DebugLine.h"
#include "Renderer/Render Passes/ViewVoxelPass.h"

#include "Core.h"

#include "Editor/EditorPanel.h"

#include "Renderer/VulkanDescriptorSet.h"

#include <functional>

namespace Fluxon {

	class SceneRuntime : public NonCopyable
	{
	private:
		Vulkan::VulkanContext* mVulkanContext;

		//TODO(Conqster) : This class should not manage window nor camera
		Platform::Window* mWindow = nullptr;
		Camera* mCamera = nullptr;

		std::vector<Vulkan::FrameResources> mFrames;

		Vulkan::VulkanDescriptorSet mDescriptorSet;



		//VkDescriptorSetLayout mDescriptorSetLayout;
		bool mUseSamplingAnisotropy = false;

		GPUStructures::DirectionalLight mDirLight;
		GPUStructures::SkyboxProperties mSkybox;

		//UI_System* mImGuiUI;
		//UI::EditorUI mEditorUI;

		Scene mScene;

		SceneContext mSceneContext;

		Vulkan::Resources::ImGui_Texture_Test experiment_texture;

		Vulkan::ShadowRenderPass mShadowGfxPass;
		Vulkan::ComputePass mComputePass;

		Vulkan::SceneVoxelPass mSceneCollsionVoxelPass;

		uint32_t mSceneModelDescSetIdx;


		//Vulkan::GraphicsPipeline mDebugLinePipeline;
		//Vulkan::GPUBuffer mDebugVertexBuffer;

		Vulkan::DebugLine mDebugLines;
	public:
		void Init(Vulkan::VulkanContext* gfx_ctx, Camera* app_cam_ref = nullptr);

		void SetIMGUI(/*UI_System& ui*/) {
			//mImGuiUI = &ui;

			Vulkan::Resources::ImGui_Texture_Test::CreateTexture(mVulkanContext, mFrames[0].commandPool,
				//"textures/Manchester_512.png",
				"assets/textures/melady_square.jpg",
				VK_FORMAT_R8G8B8A8_SRGB
				/*VK_FORMAT_R8G8B8A8_UNORM*/, experiment_texture,
				mUseSamplingAnisotropy);
			mScene.mSceneResources.experiment_texture = &experiment_texture;

			auto& voxelRT = mSceneCollsionVoxelPass.GetDepthRenderTarget();
			if (!voxelRT.targetMask.Contains(ERenderTargetBits::ImGuiView))
				Vulkan::RenderTarget::MakeRenderTarget(mVulkanContext, voxelRT, ERenderTargetBits::ImGuiView);



			//Vulkan::ForwardRenderPass* forward_render_pass =;
			//auto& sceneRT = mScene.GetForwardRenderPass()->GetColourRenderTarget();
			auto& sceneRT = mScene.GetForwardRenderPass()->GetOutputRT(0);
			if (!sceneRT.targetMask.Contains(ERenderTargetBits::ImGuiView))
				Vulkan::RenderTarget::MakeRenderTarget(mVulkanContext, sceneRT, ERenderTargetBits::ImGuiView);

			mScene.mSceneResources.sceneRenderOutput = &sceneRT;
		}

		std::vector<Vulkan::FrameResources>& GetFrames() { return mFrames; }
		//ForwardRenderPass& GetForwardRenderPass() { return mForwardRenderPass; }
		Vulkan::VulkanContext& GetVulkanContext() { return *mVulkanContext; }
		//Device& GetDeprecatedDevice() { return mDevice; }

		Scene& GetScene() { return mScene; }

		void CreateModelSceneDescriptionSetLayout(Vulkan::VulkanContext* vk_ctx, Vulkan::DescriptorSetSpecification& description_set_spec);
		void CreateSkyboxDescriptionSetLayout(VkDevice d, Vulkan::DescriptorSetSpecification& description_set_spec);



		void Update(float delta_time, Platform::Window* p_window, Camera* p_frame_camera);
		//void DrawScene2(Camera* camera, glm::vec3 translate_model, glm::vec3 rotate_axis);

		void ResizeFunction(uint32 width, uint32 height);
		void OnViewportResize(uint32 width, uint32 height);


		std::function<void(const Vulkan::VulkanContext* vk_ctx, VkCommandBuffer cmd_buff, uint32 swapchain_img_idx)> mOnUIRenderCallback;
		std::function<void(const Vulkan::VulkanContext* vk_ctx, uint32 w, uint32 h)> mOnSwapchainUICallback;

		void UpdateGlobalFrameDataUBO(Vulkan::FrameResources& frame_resources, float win_width, float win_height, glm::mat4 light_view, glm::mat4 light_proj);
		//void RecordRenderPass(VkCommandBuffer cmd_buff, ForwardRenderPass& pass, const FrameResources& frame_resources, uint32_t img_idx, VkExtent2D render_area_extent, UI_System* ui_in_use = nullptr);

		void PreCleanup() {
			//SerialiseScene("assets/scenes/experiment.fluxon", mScene);
			SceneSerialiser scene_serialiser;
			scene_serialiser.SerialiseScene("assets/scenes/experiment.fluxon", mScene);
			//SerialiseScene("experiment.fluxon", mScene);

			Vulkan::Resources::ImGui_Texture_Test::Destroy(mVulkanContext->device, experiment_texture);
		}

		void Cleanup() {

			vkDeviceWaitIdle(mVulkanContext->device);

			mDebugLines.Cleanup(mVulkanContext);

			
			mDescriptorSet.Destory();

			Vulkan::DestroyFrameObjects(mVulkanContext, mFrames);

			mShadowGfxPass.Cleanup(mVulkanContext);
			mComputePass.Cleanup(mVulkanContext);
			mSceneCollsionVoxelPass.Cleanup(mVulkanContext);

			mScene.DestroyScene(mVulkanContext);

			mVulkanContext->swapchain->Cleanup(mVulkanContext);

			//Vulkan::DestroyDevice(&mDevice, true);
			mVulkanContext->DestroyVulkanContext(true);
		}

		bool Model_Transform_Editor_Panel_UI(glm::mat4& transform);

		void DebugInputKeyUI();


		void LoadSceneFromFile(Vulkan::VulkanContext* vk_ctx, const char* scene_path, Scene& scene, Camera* cam_ref);
		void CreateSceneFromCode(Vulkan::VulkanContext* vk_ctx, Scene& scene, Camera* cam_ref);
	};
} //#Fluxon