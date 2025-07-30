#pragma once

#include <imgui/imgui.h>
#include "../Scene/SceneData.h"
#include "../Scene/ISceneComponent.h"

#include "Core/Utilities.h"
#include <glm/glm.hpp>

#include "Core/Scene/SceneNode.h"

namespace Fluxon {
	namespace Editor {


		static bool Model_Transform_Editor_Panel_UI(glm::mat4& transform)
		{

			auto& translate = transform[3];
			glm::vec3 euler;
			glm::vec3 scale;
			glm::vec3 hack;
			Utils::DecomposeTransform(transform, hack, euler, scale);

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

		static bool RenderableComponentPanel(RenderableComponent& render_comp) {
			bool modified = false;
			ImGui::SeparatorText("Renderable Component");
			ImGui::Checkbox("Visible", &render_comp.visible);

			ERenderMask& mask_layer = render_comp.renderPrimitiveData.maskLayer;
			if (ImGui::TreeNode("Render Mask Layer"))
			{

				//static bool selected[3] = { false, false, false };

				//std::array<bool, 4> selected;//(static_cast<size_t>(ERenderMask::Count));
				//bool selected;//(static_cast<size_t>(ERenderMask::Count));
				auto& layers_name = RenderMaskLayerNames();
				for (uint32_t i = 0; i < static_cast<uint32_t>(ERenderMask::Count); ++i)
				{
					ERenderMask mask = static_cast<ERenderMask>(1 << i);
					//selected[i] = (mask_layer & mask) == mask;
					bool selected = (mask_layer & mask) == mask;
					if (ImGui::Selectable(layers_name[i], &selected))
					{
						if (selected) //add mask
							mask_layer |= mask;
							//mask_layer |= mask;
						else //remove mask
							mask_layer &= ~mask;
							 //mask_layer ^= mask;
					}

				}
				//for(size_t i = 0; i < layers_name.size(); i++)
				//	ImGui::Selectable(layers_name[i], &selected[i]);// ImGui::SameLine();
				//ImGui::Selectable("Hello.cpp", &selected[1]);// ImGui::SameLine();
				//ImGui::Selectable("Hello.h", &selected[2]); //ImGui::SameLine();
				ImGui::TreePop();
			}
			return modified;
		}

		static void MainSceneUIWindow(SceneResources& scene_resources)
		{
			bool ui_trying_to_hook = false;
			ImGui::Begin("Test Scene Modification Window");

			//for (auto& model : scene_resources.renderableComponents)
			//{
			//	ImGui::PushID(&model);
			//	ImGui::SeparatorText("Model");
			//	Model_Transform_Editor_Panel_UI(model->transform);
			//	ImGui::PopID();
			//}

			if (ImGui::BeginTabBar("MainTabBar"))
			{
				if (ImGui::BeginTabItem("Renderer"))
				{
					ImGui::Text("Renderer settings go here.");
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Post-Processing"))
				{
					ImGui::Text("Post-processing controls and effects.");
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();

			}

			for (auto& node : scene_resources.sceneNodes)
			{
				ImGui::PushID(&node);
				ImGui::SeparatorText(node->Name().data());
				ImGui::Indent(20.0f);
				ImGui::SeparatorText("Transform");
				Model_Transform_Editor_Panel_UI(node->GetTransformComponent()->transform);
				if (node->HasRenderableComponent())
					RenderableComponentPanel(*node->GetRenderableComponent());

				ImGui::Unindent(20.0f);
				ImGui::PopID();
				ImGui::Spacing();
				ImGui::Spacing();
			}

			

//			ImGui::Spacing();
//			bool window_cursor_lock = Platform::IsCursorLocked(mWindow);
//			ImGui::Checkbox("Mouse Cursor", &window_cursor_lock);
//			ImGui::Spacing();
//
//			ImGui::SeparatorText("Camera Properties");
//			if (mCamera)
//			{
//				auto cam_pos = (*mCamera).position;
//				ImGui::Text("Position x: %f, y: %f, z: %f", cam_pos.x, cam_pos.y, cam_pos.z);
//				auto cam_forward = mCamera->forward;
//				ImGui::Text("Forward x: %f, y: %f, z: %f", cam_forward.x, cam_forward.y, cam_forward.z);
//				ImGui::SliderFloat("Move Speed", &mCamera->moveSpeed, 0.01f, 10.0f);
//				ImGui::SliderFloat("Rot Speed", &mCamera->rotSpeed, 0.01f, 1.0f);
//				ImGui::SliderFloat("FOV", &mCamera->fov, 0.0f, 179.0f, "%.1f");
//				ImGui::DragFloat("Near", &mCamera->nearPlane, 0.1f, 0.01f, 50.0f, "%.1f");
//				ImGui::DragFloat("Far", &mCamera->farPlane, 0.1f, 0.0f, 500.0f, "%.1f");
//			}
//			else
//				ImGui::Text("No vaild Camera !!!!!!");
//
//
//			ImGui::SeparatorText("Renderer Properties");
//			ImGui::ColorEdit3("clear colour", &mForwardRenderPass.mClearColour[0]);

			if (scene_resources.directionalLight)
			{
				auto& dir_light = *scene_resources.directionalLight;
				ImGui::SeparatorText("Directional Light");
				bool enable = static_cast<bool>(dir_light.enable);
				if (ImGui::Checkbox("enable light", &enable))
					dir_light.enable = enable;
				ImGui::DragFloat3("light direction", &dir_light.direction[0], 0.1f, -2.0f, 2.0f);
				ImGui::ColorEdit3("light colour", &dir_light.colour[0]);
				ImGui::SliderInt("model shinness", &dir_light.shinness, 1, 128);
			}

			if (scene_resources.skybox)
			{
				auto& skybox = *scene_resources.skybox;
				ImGui::SeparatorText("Skybox");
				bool enable = static_cast<bool>(skybox.enable);
				if (ImGui::Checkbox("enable skybox", &enable))
					skybox.enable = enable;
				ImGui::SliderFloat("skybox influence", &skybox.skyboxInfluencity, 0.0f, 1.0f, "%.1f");
				ImGui::SliderFloat("skybox reflect", &skybox.objectReflectivity, 0.0f, 1.0f, "%.1f");
				ImGui::SliderInt("hack _padding for switch material map", &skybox._padding, 0, 10);

			}

//			//test hack material 
//#if DEBUG_MAT_HACK
//			ImGui::SeparatorText("Hack Material");
//			ImGui::ColorEdit3("hack colour", &mRustyMetalPBRMat.hackDebugMaterial.colour[0]);
//			ImGui::SliderFloat("hack metallic", &mRustyMetalPBRMat.hackDebugMaterial.metallic, 0.0f, 1.0f);
//			ImGui::SliderFloat("hack roughness", &mRustyMetalPBRMat.hackDebugMaterial.roughness, 0.0f, 1.0f);
//			ImGui::SliderFloat("hack ao", &mRustyMetalPBRMat.hackDebugMaterial.ao, 0.0f, 1.0f);
//#endif // DEBUG_MAT_HACK
//
//			ImGuiIO ui_io = ImGui::GetIO();
//			ui_trying_to_hook = ui_io.WantTextInput || ui_io.WantCaptureMouse;
//			//ui_trying_to_hook = ImGui::IsAnyItemActive();
//			ImGui::Checkbox("UI Trying to use cursor", &ui_trying_to_hook);
//			ImGui::Text("UI Mouse drag threshold: %f", ui_io.MouseDragThreshold);
//			double x_pos, y_pos;
//			glfwGetCursorPos(mWindow->glfwWindowHandle, &x_pos, &y_pos);
//			int width, height;
//			glfwGetWindowSize(mWindow->glfwWindowHandle, &width, &height);
//			ImGui::Text("Mouse cursor pos: %f, %f", x_pos, y_pos);
//			ImGui::Text("Window size: %f, %f", width, height);


			ImGui::End();
		}

	} //# Editor

} //# Fluxon

