#pragma once

#include <imgui/imgui.h>
#include "../Scene/SceneData.h"
#include "../Scene/ISceneComponent.h"

#include "Core/Utilities.h"
#include <glm/glm.hpp>

#include "Core/Scene/SceneNode.h"

namespace Fluxon {
	namespace Editor {


		namespace Flux_ImGui_Util {

			static int StringResizeCallback(ImGuiInputTextCallbackData* data)
			{
				if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
				{
					//ImVector<char>* my_str = (ImVector<char>*)data->UserData;
					//IM_ASSERT(my_str->begin() == data->Buf);
					//my_str->resize(data->BufSize); // NB: On resizing calls, generally data->BufSize == data->BufTextLen + 1
					//data->Buf = my_str->begin();

					std::string* my_str = static_cast<std::string*>(data->UserData);
					//char* test = &my_str->data()[0];
					IM_ASSERT(&my_str->data()[0] == data->Buf);
					my_str->resize(data->BufSize); // NB: On resizing calls, generally data->BufSize == data->BufTextLen + 1
					//data->Buf = &my_str->data()[0];
					data->Buf = &(*my_str)[0];
				}
				return 0;
			}

			// Note: Because ImGui:: is a namespace you would typically add your own function into the namespace.
			// For example, you code may declare a function 'ImGui::InputText(const char* label, MyString* my_str)'
			static bool MyInputTextMultiline(const char* label, ImVector<char>* my_str, const ImVec2& size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0)
			{
				IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
				return ImGui::InputTextMultiline(label, my_str->begin(), (size_t)my_str->size(), size, flags | ImGuiInputTextFlags_CallbackResize, StringResizeCallback, (void*)my_str);
			}

			static bool StringInputText(const char* label, std::string* my_str, ImGuiInputTextFlags flags = 0)
			{
				IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
				bool changed =  ImGui::InputText(label, &my_str->data()[0], (size_t)my_str->size(), flags | ImGuiInputTextFlags_CallbackResize, StringResizeCallback, (void*)my_str);

				if (!my_str->empty() && (*my_str)[my_str->size() - 1] == '\0')
					my_str->resize(my_str->size() - 1);
				return changed;
			}

			//
			//SAMPLE
			// 
			// For this demo we are using ImVector as a string container.
			// Note that because we need to store a terminating zero character, our size/capacity are 1 more
			// than usually reported by a typical string class.
			//static ImVector<char> my_str;
			//static std::string my_str;
			////if (my_str.empty())
			////	my_str.push_back(0);
			////Funcs::MyInputTextMultiline("##MyStr", &my_str, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16));
			//Funcs::MyInputText("##MyStr", &my_str);
			//ImGui::Text("Data: %p\nSize: %d\nCapacity: %d", (void*)my_str.data()[0], my_str.size(), my_str.capacity());
	
		} //namespace ImGuiUtil


		static void ApplyEuler(glm::quat& curr_quat, glm::vec3 ui_euler)
		{
			glm::vec3 delta_euler = ui_euler - glm::degrees(glm::eulerAngles(curr_quat));
			
			glm::quat qx = glm::angleAxis(glm::radians(delta_euler.x), glm::vec3(1, 0, 0));
			glm::quat qy = glm::angleAxis(glm::radians(delta_euler.y), glm::vec3(0, 1, 0));
			glm::quat qz = glm::angleAxis(glm::radians(delta_euler.z), glm::vec3(0, 0, 1));

			glm::quat delta_quat = qy * qx * qz;
			curr_quat = glm::normalize(delta_quat * curr_quat);
		}

		static bool Model_Transform_Editor_Panel_UI(glm::mat4& transform)
		{

			glm::quat quat;
			glm::vec3 scale;
			glm::vec3 translate;
			Utils::DecomposeTransform(transform, translate, quat, scale);

			bool update = ImGui::DragFloat3("Translation", &translate[0], 0.01f, (0.0f), (0.0f), "%.2f");
			//= ImGui::IsItemDeactivatedAfterEdit();//ImGui::IsItemEdited();
			//if (translate_update)
			//	transform.invMatrix = glm::inverse(transform.matrix);

			//update = ImGui::DragFloat3("Euler", &euler[0], 0.01f, (0.0f), (0.0f), "%.2f");
			glm::vec3 euler = glm::degrees(glm::eulerAngles(quat));
			if (ImGui::DragFloat3("Euler", &euler[0], 0.05f, 0.0f, (0.0f), "%.2f"))
			{
				ApplyEuler(quat, euler);
				update = true;
			}
			update |= ImGui::DragFloat3("Scale", &scale[0], 0.01f, (0.0f), (0.0f), "%.2f");
			if (update)
				transform = Utils::ComposeTransform(translate, quat, scale);

			return update;
		}

		static bool RenderableComponentPanel(RenderableComponent& render_comp) {
			bool modified = false;
			ImGui::SeparatorText("Renderable Component");
			modified |= ImGui::Checkbox("Visible", &render_comp.visible);

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
						modified = true;
					}

				}
				
				ImGui::TreePop();
			}

			modified |= ImGui::Checkbox("Use new material", (bool*)&render_comp.new_mat_sys_hack.w);
			return modified;
		}


		static void MainSceneUI(SceneResources& scene_resources)
		{
			for (auto& node : scene_resources.sceneNodes)
			{
				ImGui::PushID(&node);
				ImGui::SeparatorText(node->Name().data());
				ImGui::Indent(20.0f);
				Flux_ImGui_Util::StringInputText("Name", &node->Name());
				ImGui::SeparatorText("Transform");
				Model_Transform_Editor_Panel_UI(node->GetTransformComponent()->transform);
				if (node->HasRenderableComponent())
					RenderableComponentPanel(*node->GetRenderableComponent());

				ImGui::Unindent(20.0f);
				ImGui::PopID();
				ImGui::Spacing();
				ImGui::Spacing();
			}



			//ImGui::Spacing();
			//bool window_cursor_lock = Platform::IsCursorLocked(mWindow);
			//ImGui::Checkbox("Mouse Cursor", &window_cursor_lock);
			//ImGui::Spacing();

			ImGui::SeparatorText("Camera Properties");
			if (scene_resources.camera)
			{
				auto cam_pos = (*scene_resources.camera).position;
				ImGui::Text("Position x: %f, y: %f, z: %f", cam_pos.x, cam_pos.y, cam_pos.z);
				auto cam_forward = scene_resources.camera->forward;
				ImGui::Text("Forward x: %f, y: %f, z: %f", cam_forward.x, cam_forward.y, cam_forward.z);
				ImGui::SliderFloat("Move Speed", &scene_resources.camera->moveSpeed, 0.01f, 10.0f);
				ImGui::SliderFloat("Rot Speed", &scene_resources.camera->rotSpeed, 0.01f, 1.0f);
				ImGui::SliderFloat("FOV", &scene_resources.camera->fov, 0.0f, 179.0f, "%.1f");
				ImGui::DragFloat("Near", &scene_resources.camera->nearPlane, 0.1f, 0.01f, 50.0f, "%.1f");
				ImGui::DragFloat("Far", &scene_resources.camera->farPlane, 0.1f, 0.0f, 500.0f, "%.1f");
			}
			else
				ImGui::Text("No vaild Camera !!!!!!");




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

		}



		static void SceneRuntimeUIWindow(SceneResources& scene_resources)
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
				if (ImGui::BeginTabItem("Scene Editor"))
				{
					ImGui::Text("Scene controls and modifications.");
					//static std::string scene_name;
					//ImGui::InputText("Scene", )
					//if (ImGui::InputText("Scene", (char*)scene_resources.instantSceneName->c_str(), scene_resources.instantSceneName->capacity() + 1))
					//{
					//	printf("Text size: %d\n", scene_resources.instantSceneName->size());
					//	printf("Text capacity: %d\n", scene_resources.instantSceneName->capacity());
					//}

					if (Flux_ImGui_Util::StringInputText("Scene Name", scene_resources.instantSceneName))
					{
						//printf("Text size: %d\n", (int)scene_resources.instantSceneName->size());
						//printf("Text capacity: %d\n", (int)scene_resources.instantSceneName->capacity());
					}
					ImGui::Text("Data: %p\nSize: %d\nCapacity: %d", (void*)scene_resources.instantSceneName->data()[0], scene_resources.instantSceneName->size(), scene_resources.instantSceneName->capacity());
				
					MainSceneUI(scene_resources);
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Renderer"))
				{
					ImGui::Text("Renderer settings go here.");
					ImGui::SeparatorText("Renderer Properties");
					glm::vec3 col;
					ImGui::ColorEdit3("clear colour", &col[0]);
					ImGui::EndTabItem();
				}

				
				if (ImGui::BeginTabItem("Experimental"))
				{
					

					// For this demo we are using ImVector as a string container.
					// Note that because we need to store a terminating zero character, our size/capacity are 1 more
					// than usually reported by a typical string class.
					//static ImVector<char> my_str;
					static std::string my_str;
					//if (my_str.empty())
					//	my_str.push_back(0);
					//Funcs::MyInputTextMultiline("##MyStr", &my_str, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16));
					Flux_ImGui_Util::StringInputText("##MyStr", &my_str);
					ImGui::Text("Data: %p\nSize: %d\nCapacity: %d", (void*)my_str.data()[0], my_str.size(), my_str.capacity());
					ImGui::EndTabItem();
				}



				ImGui::EndTabBar();
			}

			

			ImGui::End();
		}

	} //# Editor

} //# Fluxon

