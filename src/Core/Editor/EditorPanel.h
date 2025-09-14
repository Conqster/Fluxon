#pragma once

#include <imgui/imgui.h>

#include "../Scene/SceneData.h"
#include "../Scene/ISceneComponent.h"
#include "Core/Scene/SceneNode.h"

//#include "Core/Scene/Scene.h"

#include "Core/Utilities.h"

#include "ImGuiHelpers.h"

namespace Fluxon {
	namespace UI {



		//quick utility
		static bool StringInputTextRow(const char* label, std::string& value, bool with_table = true, ImGuiTableFlags imgui_flags = ImGuiTableFlags_BordersInnerV)
		{
			bool modified = false;
			Internals::BeginTableScope table_scope("Slider table row", 2, with_table, imgui_flags);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			
			ImGui::TextUnformatted(label);

			ImGui::TableSetColumnIndex(1);


			char value_buff[256] = {};
			//strncpy(value_buff, value.c_str(), sizeof(value_buff));
			snprintf(value_buff, sizeof(value_buff), "%s", value.c_str());

			char id[32] = "##";
			strncat(id, label, sizeof(id) - strlen(id) - 1);

			if (ImGui::InputText(id, value_buff, 256))
			{
				modified = true;
				value = value_buff;
			}
			return modified;
		}


		static bool StringInputTextRow(std::string& value, bool with_table = true, ImGuiTableFlags imgui_flags = ImGuiTableFlags_BordersInnerV)
		{
			bool modified = false;
			Internals::BeginTableScope table_scope("Slider table row", 1, with_table, imgui_flags, ImVec2(0.0f, 0.0f), 1000.0f);

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			//ImGui::TextUnformatted(label);

			//ImGui::TableSetColumnIndex(1);


			char value_buff[256] = {};
			//strncpy(value_buff, value.c_str(), sizeof(value_buff));
			snprintf(value_buff, sizeof(value_buff), "%s", value.c_str());

			char id[32] = "##";
			strncat(id, "label", sizeof(id) - strlen(id) - 1);


			if (ImGui::InputText(id, value_buff, 256))
			{
				modified = true;
				value = value_buff;
			}
			return modified;
		}




		static inline bool DrawMat4Transform(glm::mat4& transform, bool with_table = true, const char* format = "%.2f", ImGuiTableFlags imgui_flags = ImGuiTableFlags_BordersInnerV)
		{
			bool modified = false;
			static ImGuiTreeNodeFlags base_flag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
				ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen;
			ImGui::PushID(&transform);
			if (ImGui::TreeNodeEx("TRANSFORM", base_flag))
			{
				{
				
					imgui_flags |= ImGuiTableFlags_BordersOuterV | ImGuiTableFlags_RowBg;
					Internals::BeginTableScope table_scope("Draw table row", 2, with_table, imgui_flags);
					glm::quat quat;
					glm::vec3 scale;
					glm::vec3 translate;
					Utils::DecomposeTransform(transform, translate, quat, scale);

					modified |= Internals::DrawDragVec3Row("Translation", translate, 0.01f, 0.0f, 0.0f, glm::vec3(.0f), false);

					//ImGui::TableNextRow();
					//ImGui::TableSetColumnIndex(0);
					//ImGui::Separator();
					//ImGui::TableSetColumnIndex(1);

					glm::vec3 euler = glm::degrees(glm::eulerAngles(quat));
					if (Internals::DrawDragVec3Row("Euler", euler, 0.05f, 0.0f, 0.0f, glm::vec3(.0f), false))
					{
						Internals::ApplyEuler(quat, euler);
						modified = true;
					}

					//ImGui::TableNextRow();
					//ImGui::TableSetColumnIndex(0);
					//ImGui::Separator();
					//ImGui::TableSetColumnIndex(1);

					modified |= Internals::DrawDragVec3Row("Scale", scale, 0.01f, 0.0f, 0.0f, glm::vec3(1.0f), false);
					if (modified)
						transform = Utils::ComposeTransform(translate, quat, scale);

				}
				ImGui::TreePop();
			}


			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::PopID();
			return modified;
		}

		static inline bool DrawRenderableComponent(RenderableComponent& render_comp, bool with_table = true, const char* format = "%.2f", ImGuiTableFlags imgui_flags = ImGuiTableFlags_BordersInnerV)
		{
			bool modified = false;
			ImGui::PushID(&render_comp);

			const ImGuiStyle* style = &ImGui::GetStyle();
			static ImGuiTreeNodeFlags base_flag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
				ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen;
			ImVec2 draw_min = ImGui::GetCursorScreenPos();
			if (ImGui::TreeNodeEx("RENDERABLE", base_flag))
			{
				draw_min = ImVec2(ImGui::GetItemRectMin().x, //<--- get x of left hand side 
					ImGui::GetItemRectMax().y); //<-- gets right bottm corner

				{
					imgui_flags |= ImGuiTableFlags_BordersOuterV;
					Internals::BeginTableScope table_scope("Draw table row", 2, with_table, imgui_flags);

					ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, Colour32(0.02f, 0.02f, 0.02f, 0.53f));


					modified |= Internals::DrawCheckboxV("Visible", &render_comp.visible, false);

				}
				static std::vector<const char*> names = RenderMaskBitsLayerNamesVector();
				Internals::DrawFlagTree("Render Mask Layer", render_comp.renderPrimitiveData.renderMask, names, true);
				//Flux_ImGui_Util::DrawFlagSelector("Render Mask Layer", render_comp.renderPrimitiveData.renderMask, names);

				// ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, Colour32(0.02f, 0.02f, 0.02f, 0.53f));


				{
					imgui_flags |= ImGuiTableFlags_BordersOuterV | ImGuiTableFlags_RowBg;
					Internals::BeginTableScope table_scope("Draw table row2", 2, with_table, imgui_flags);
					//modified |= Flux_ImGui_Util::DrawCheckboxV("Visible2", &render_comp.visible, false);

					//modified |= Flux_ImGui_Util::DrawCheckboxV("Visible1", &render_comp.visible, false);
					//modified |= Flux_ImGui_Util::DrawCheckboxV("Visible3", &render_comp.visible, false);
				}

				//ImGui::SameLine();
				//ImVec2 draw_max = ImGui::GetCursorScreenPos();
				//ImVec2 draw_max = ImGui::GetCursorScreenPos();
				//adjust
				draw_min.x += style->FrameRounding * 0.5f;
				ImVec2 draw_max = ImGui::GetItemRectMax();

				ImDrawList* draw_list = ImGui::GetWindowDrawList();
				ImVec4 c = style->Colors[ImGuiCol_TableBorderStrong];
				Colour32 col = Colour32(c.x, c.y, c.z, c.w);
				//draw_list->AddRect(draw_min, draw_max, col, 0.5f, 0, 2.0f);
				draw_list->AddLine(draw_min, ImVec2(draw_min.x, draw_max.y), col, 2.0f);
				draw_list->AddLine(ImVec2(draw_min.x, draw_max.y), draw_max, col, 2.0f);
				draw_list->AddLine(draw_max, ImVec2(draw_max.x, draw_min.y), col, 2.0f);
				ImGui::TreePop();
			}




			ImGui::Spacing();
			ImGui::Spacing();


			ImGui::PopID();
			return modified;
		}

		static inline bool DrawAnimationComponent(SceneNode& node, bool with_table = true, const char* format = "%.2f", ImGuiTableFlags imgui_flags = ImGuiTableFlags_BordersInnerV)
		{
			bool modified = false;
			ImGui::PushID(&node);

			const ImGuiStyle* style = &ImGui::GetStyle();
			static ImGuiTreeNodeFlags base_flag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
				ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen;
			ImVec2 draw_min = ImGui::GetCursorScreenPos();
			if (ImGui::TreeNodeEx("ANIMATION", base_flag))
			{
				Internals::DrawCheckboxV("Animate Obj", &node.mAnimateObj);
				ImGui::TreePop();
			}

			ImGui::Spacing();


			ImGui::PopID();
			return modified;
		}


		struct EditorPanelContext
		{
			int selectedNodeIdx = -1;
			float viewportWidth;
			float viewportHeight;

			bool bViewportResize = false;
		};

		class IEditorPanel
		{
			//virtual ~IEditorPanel(){}
			virtual const char* GetName() const = 0;
			virtual void Draw(SceneResources& scene_resources, EditorPanelContext& ctx) = 0;
		};


		class HierarchyPanel : public IEditorPanel
		{
			
		public:
			const char* GetName() const override { return "Hierarchy"; }
			
			void Draw(SceneResources& scene_resources, EditorPanelContext& ctx) override
			{

				auto& scene_nodes = scene_resources.sceneNodes;
				if(ImGui::Begin("Hierarchy"))
				{
					ImVec2 size = ImGui::GetWindowSize();
					ImGui::Text("Scene Object Count: %d", (int)scene_nodes.size());
					ImGuiTableFlags imgui_flags = ImGuiTableFlags_BordersOuterV | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInner | ImGuiTableFlags_Resizable;
					ImGui::BeginTable("##Hierarchy", 2, imgui_flags, ImVec2(0.0f, 0.0f), size.x);// , 200);
					///ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 200);
					//ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch);


					ImGui::TableSetupColumn("Label", 0);// , 200);
					ImGui::TableSetupColumn("Type", 0);
					

					ImGui::TableHeadersRow();
					
					if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
						ctx.selectedNodeIdx = -1;


					for (int i = 0; i < scene_nodes.size(); i++)
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						std::string node_name = scene_nodes[i]->Name();
						bool selected = ctx.selectedNodeIdx == i;
	/*					ImGui::PushID((node_name + std::to_string(i)).c_str());
						if (ImGui::Selectable(node_name.c_str(), selected))
							ctx.selectedNodeIdx = i;
						ImGui::PopID();*/


						bool has_children = false;
						ImGuiTreeNodeFlags tree_flag = ImGuiTreeNodeFlags_SpanAllColumns;
						if (selected)
						{
							//header colout is used for active item itn table
							ImGuiStyle* style = &ImGui::GetStyle();
							ImVec4 col = style->Colors[ImGuiCol_HeaderActive];
							col.w = 0.4f;
							ImGui::PushStyleColor(ImGuiCol_Header, col);

							tree_flag |= ImGuiTreeNodeFlags_Selected;
						}
						if(!has_children)
							tree_flag |= ImGuiTreeNodeFlags_Leaf;

						bool drop_down_tree = ImGui::TreeNodeEx(node_name.c_str(), tree_flag);
						if(ImGui::IsItemClicked())
							ctx.selectedNodeIdx = i;
						if (drop_down_tree)
						{
							ImGui::TreePop();
						}

						if(selected)
							ImGui::PopStyleColor(1);

	
					}

					ImGui::EndTable();
				}
				ImGui::End();
			}
		};


		class InspectorPanel : public IEditorPanel
		{

		public:
			const char* GetName() const override { return "Hierarchy"; }
			void Draw(SceneResources& scene_resources, EditorPanelContext& ctx)
			{

				auto& scene_nodes = scene_resources.sceneNodes;
				auto& selected_idx = ctx.selectedNodeIdx;
				if (ImGui::Begin("Inspector"))
				{
					if (selected_idx >= 0 && selected_idx < (int)scene_nodes.size())
					{
						SceneNode& obj = *scene_nodes[selected_idx];
					
						//StringInputTextRow("Name", obj.Name());
						StringInputTextRow(obj.Name());


						TransformComponent* trans_comp = obj.GetTransformComponent();
						if (trans_comp != nullptr)
						{
							if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
							{

								ImGuiTableFlags imgui_flags = ImGuiTableFlags_BordersOuterV | ImGuiTableFlags_RowBg;
								{
									Internals::BeginTableScope table_scope("Draw table row", 2, true, imgui_flags);
									glm::quat quat;
									glm::vec3 scale;
									glm::vec3 translate;
									Utils::DecomposeTransform(trans_comp->transform, translate, quat, scale);

									bool modified = Internals::DrawDragVec3Row("Translation", translate, 0.01f, 0.0f, 0.0f, glm::vec3(.0f), false);

									glm::vec3 euler = glm::degrees(glm::eulerAngles(quat));
									if (Internals::DrawDragVec3Row("Euler", euler, 0.05f, 0.0f, 0.0f, glm::vec3(.0f), false))
									{
										Internals::ApplyEuler(quat, euler);
										modified = true;
									}


									modified |= Internals::DrawDragVec3Row("Scale", scale, 0.01f, 0.0f, 0.0f, glm::vec3(1.0f), false);
									if (modified)
										trans_comp->transform = Utils::ComposeTransform(translate, quat, scale);
								}
							}

						}

						if (obj.HasRenderableComponent())
						{
							DrawRenderableComponent(*obj.GetRenderableComponent());
						}

						DrawAnimationComponent(obj);
					}
					else
						ImGui::Text("No Scene Object Selected");
				}
				ImGui::End();
			}
		};



		class EngineSettingPanel : public IEditorPanel
		{
		public:
			const char* GetName() const override { return "Hierarchy"; }
			void Draw(SceneResources& scene_resources, EditorPanelContext& ctx)
			{
				if (ImGui::Begin("Engine Setting"))
				{
					{
						ImGuiTableFlags imgui_flags = /*ImGuiTableFlags_BordersOuterV |*/ ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_RowBg;
						ImGui::BeginTable("##Engine setting table", 2, imgui_flags);
						//Internals::BeginTableScope("Engine setting table", 2, true, imgui_flags);

						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						if (ImGui::CollapsingHeader("GENERAL", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAllColumns))
						{
							Internals::DrawTextUnformattedVProperty("ITEM", "MainCamera");

							if(scene_resources.camera != nullptr)
							{
								EditorCamera& cam = *scene_resources.camera;
								glm::vec3 temp_v = cam.GetPosition();

								Internals::DrawTextVProperty("Position", "x: %f, y: %f, z: %f", temp_v.x, temp_v.y, temp_v.z);
								temp_v = cam.GetForward();
								Internals::DrawTextVProperty("Forward", "x: %f, y: %f, z: %f", temp_v.x, temp_v.y, temp_v.z);

								float v = cam.GetMoveSpeed();
								if (Internals::DrawSliderFloatV("Move Speed", &v, 0.01f, 10.0f, false))
									cam.SetMoveSpeed(v);

								v = cam.GetRotSpeed();
								if (Internals::DrawSliderFloatV("Rot Speed", &v, 0.01f, 1.0f, false))
									cam.SetRotSpeed(v);

								v = cam.GetNearZ();
								if (Internals::DrawDragFloatV("Near", &v, 0.1f, 0.01f, 50.0f, false))
									cam.SetNearZ(v);

								v = cam.GetFarZ();
								if (Internals::DrawDragFloatV("Far", &v, 0.1f, 0.0f, 500.0f, false))
									cam.SetFarZ(v);
								if (cam.IsPerspective())
								{
									v = cam.GetFovY();
									if (Internals::DrawSliderFloatV("FOV", &v, 0.0f, 179.0f, false))
										cam.SetFovY(v);
								}
								else
								{
									v = cam.GetScale();
									if (Internals::DrawSliderFloatV("Scale", &v, 0.0f, 179.0f, false))
										cam.SetScale(v);
								}

								static ECameraType& cam_type = cam.GetType();
								const char* cam_names[] = { "Perspective", "Orthographic" };
								if (Internals::DrawDropDown("Camera Type", cam_type, cam_names, false))
									cam.SetType(cam_type);
							}
							else
							{
								ImGui::TableSetColumnIndex(1);
								ImGui::Text("NO VALID CAMERA !!!!!");
							}

						}

						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						if (ImGui::CollapsingHeader("RENDERER", ImGuiTreeNodeFlags_SpanAllColumns))
						{

							Internals::DrawTextUnformattedVProperty("ITEM", "Directional Light");


							if (scene_resources.directionalLight != nullptr)
							{
								auto& dir_light = *scene_resources.directionalLight;
								bool enable = static_cast<bool>(dir_light.enable);
								if (Internals::DrawCheckboxV("Enable", &enable, false))
									dir_light.enable = enable;
								Internals::DrawDragVec3Row("light direction", dir_light.direction, 0.1f, -2.0f, 2.0f, { 0.0f, 0.9f, 0.3f }, false);
								Internals::DrawColourVec3Edit("light colour", dir_light.colour, false);
								Internals::DrawSliderIntV("model shinness", &dir_light.shinness, 1, 128, false);
								ImGui::DragFloat4("light experiment move", &scene_resources.experimentLightMove[0]);
							}
							else
							{
								ImGui::TableSetColumnIndex(1);
								ImGui::Text("NO VALID DIRECTONAL LIGHT !!!!!");
							}


							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(0);

							ImGui::Spacing();

							Internals::DrawTextUnformattedVProperty("ITEM", "Skybox");
							if (scene_resources.skybox)
							{
								auto& skybox = *scene_resources.skybox;

								Internals::DrawCheckboxV("Enable", (bool*)&skybox.enable, false);
								Internals::DrawSliderFloatV("skybox influence", &skybox.skyboxInfluencity, 0.0f, 1.0f, false);
								Internals::DrawSliderFloatV("skybox reflect", &skybox.objectReflectivity, 0.0f, 1.0f, false);
								Internals::DrawSliderIntV("hack _padding for switch material map", &skybox._padding, 0, 10, false);

							}
							else
							{
								ImGui::TableSetColumnIndex(1);
								ImGui::Text("NO VALID SKYBOX !!!!!");
							}

						}

						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						if (ImGui::CollapsingHeader("PARTICLE", ImGuiTreeNodeFlags_SpanAllColumns))
						{
							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(0);
							ImGui::Text("NO DATA/INFO YET !!!!!");
						}

						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						if (ImGui::CollapsingHeader("EXPERIMENTAL", ImGuiTreeNodeFlags_SpanAllColumns))
						{
							ImGui::TableNextRow();
							ImGui::TableSetColumnIndex(0);
							ImGui::Text("NO DATA/INFO YET !!!!!");
						}


						ImGui::EndTable();
					}
				}
				ImGui::End();
			}
		};




		class TextureViewPanel : public IEditorPanel
		{

		public:
			const char* GetName() const override { return "Hierarchy"; }
			void Draw(SceneResources& scene_resources, EditorPanelContext& ctx)
			{

				if(ImGui::Begin("Texture View Panel"));
				{
					if (scene_resources.experiment_texture != nullptr)
					{
						ImGui::Text("pointer = %p", scene_resources.experiment_texture->desc);
						ImGui::Text("size = %d x %d", scene_resources.experiment_texture->width, scene_resources.experiment_texture->height);
						ImGui::Image((ImTextureID)scene_resources.experiment_texture->desc, ImVec2(scene_resources.experiment_texture->width, scene_resources.experiment_texture->height));
					}

					if (scene_resources.shadowPassDepthRT != nullptr && scene_resources.shadowPassDepthRT->imguiDescSet != nullptr)
					{
						auto& shadowRT = scene_resources.shadowPassDepthRT;
						static float scale = 0.1f;
						ImGui::DragFloat("Scale", &scale, 0.01f, 0.01f, 1.0f, "%.2f");
						ImGui::Image((ImTextureID)shadowRT->imguiDescSet, ImVec2(shadowRT->extent.width * scale, shadowRT->extent.height * scale));
					}
					if (scene_resources.voxelDepthRT != nullptr && scene_resources.voxelDepthRT->imguiDescSet != nullptr)
					{
						auto& voxelRT = scene_resources.voxelDepthRT;
						static float scale2 = 1.51f;
						ImGui::DragFloat("Scale2", &scale2, 0.01f, 0.01f, 1.0f, "%.2f");
						ImGui::Image((ImTextureID)voxelRT->imguiDescSet, ImVec2(voxelRT->extent.width * scale2, voxelRT->extent.height * scale2));
					}
				}
				ImGui::End();
			}
		};



		class ViewportPanel : IEditorPanel
		{
		public: 
			ImTextureID mRenderTexture = 0;
			ImVec2 mSize = ImVec2(1800, 600);

			const char* GetName() const override { return "Viewport"; }

			void Draw(SceneResources& scene_resources, EditorPanelContext& ctx)
			{
				//ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });



				//ImGui::Begin(("Viewport {" + std::to_string(mSize.x) + ", " + std::to_string(mSize.y) + "}").c_str());
				ImGui::Begin("Viewport");
				{
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip("Viewport {%.1f, %.1f}", mSize.x, mSize.y);
					{
						ImGuiTableFlags imgui_flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
						ImGui::BeginTable("##viewport text", 2, imgui_flags);
						//Internals::BeginTableScope("##viewport text", 2, true, imgui_flags);
						//Internals::DrawTextVProperty("Viewport Size", "{%.1f, %.1f}", mSize.x, mSize.y);
						//Internals::DrawTextVProperty("Viewport Size", "{%.1f, %.1f}", scene_resources.viewportWidth, scene_resources.viewportHeight);
						//ImGui::Text("Viewport Need Resize: &s", (ctx.bViewportResize) ? "True" : "False");
						ImGui::EndTable();
					}
					ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
					mSize = { viewport_panel_size.x, viewport_panel_size.y };

					if (ctx.viewportWidth != mSize.x || ctx.viewportHeight != mSize.y)
					{
						ctx.bViewportResize = true;
						ctx.viewportWidth = mSize.x;
						ctx.viewportHeight = mSize.y;

					}
					//auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
					//auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
					//auto viewportOffset = ImGui::GetWindowPos();
					//m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
					//m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

					//m_ViewportFocused = ImGui::IsWindowFocused();
					//m_ViewportHovered = ImGui::IsWindowHovered();

					//Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportHovered);

					mRenderTexture = (ImTextureID)scene_resources.experiment_texture->desc;

					if (scene_resources.sceneRenderOutput != nullptr && scene_resources.sceneRenderOutput->imguiDescSet != nullptr)
					{
						mRenderTexture = (ImTextureID)scene_resources.sceneRenderOutput->imguiDescSet;
					}

					if (mRenderTexture != 0)
						ImGui::Image(reinterpret_cast<void*>(mRenderTexture), ImVec2{ mSize.x, mSize.y });//, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
					else
					{
						ImGui::Dummy(mSize);
						ImGui::Text("Viewport (no render target bound)");
					}

				}

				ImGui::End();
				
				

				scene_resources.viewportWidth = mSize.x;
				scene_resources.viewportHeight = mSize.y;
			}
		};


		class EditorUI //or SceneEditorContext
		{
			EditorPanelContext panelsContext;

			HierarchyPanel mHierarchyPanel;
			InspectorPanel mInspectorPanel;
			EngineSettingPanel mEngineSettingPanel;
			TextureViewPanel mTextureViewPanel;

			ViewportPanel mViewportPanel;

			std::string mMainScene = "experiment.fluxon";
			std::string mSceneDirectory = "assets/scenes";

			std::vector<std::function<void(uint32 width, uint32 height)>> mOnViewportResizeRegistery;
		public:

			void RegisterOnViewportResize(const std::function<void(uint32 width, uint32 height)>& on_resize) {
				mOnViewportResizeRegistery.emplace_back(on_resize);
			}

			void RenderUI(SceneResources& scene_reso)
			{
				DrawDockspace(scene_reso);
				mViewportPanel.Draw(scene_reso, panelsContext);
				mHierarchyPanel.Draw(scene_reso, panelsContext);
				mInspectorPanel.Draw(scene_reso, panelsContext);
				mEngineSettingPanel.Draw(scene_reso, panelsContext);
				mTextureViewPanel.Draw(scene_reso, panelsContext);

				ImGui::Begin("Debug Editor UI 2");
				ImGui::Text("Context Selected Node: %d", panelsContext.selectedNodeIdx);
				ImGui::Text("Viewport Size{%.1f, %.1f}", panelsContext.viewportWidth, panelsContext.viewportHeight);
				ImGui::Text("Viewport Size2{%.1f, %.1f}", scene_reso.viewportWidth, scene_reso.viewportHeight);
				ImGui::Text("Viewport Need Resize: %s", (panelsContext.bViewportResize) ? "True" : "False");


				//RenderTarget info 
				ImGui::SeparatorText("Viewport Render Target");
				VkExtent2D extent = scene_reso.sceneRenderOutput->extent;
				ImGui::Text("Viewport {%.1f, %.1f}", (float)extent.width, (float)extent.height);
				ImGui::End();

				//ImGui::End();
			}


			
			void CheckViewportResize()
			{

				if (panelsContext.bViewportResize)
				{
					for(auto& func : mOnViewportResizeRegistery)
						func((uint32)panelsContext.viewportWidth, (uint32)panelsContext.viewportHeight);
				}

				panelsContext.bViewportResize = false;
			}

			void DrawDockspace(SceneResources& scene_reso)
			{
				//ImGui::ShowDemoWindow
				// Note: Switch this to true to enable dockspace
				static bool dockspaceOpen = true;
				static bool opt_fullscreen_persistant = true;
				bool opt_fullscreen = opt_fullscreen_persistant;
				static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

				// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
				// because it would be confusing to have two docking targets within each others.
				ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
				if (opt_fullscreen)
				{
					ImGuiViewport* viewport = ImGui::GetMainViewport();
					ImGui::SetNextWindowPos(viewport->Pos);
					ImGui::SetNextWindowSize(viewport->Size);
					ImGui::SetNextWindowViewport(viewport->ID);
					ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
					ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
					window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
					window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
				}

				// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
				if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
					window_flags |= ImGuiWindowFlags_NoBackground;

				// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
				// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
				// all active windows docked into it will lose their parent and become undocked.
				// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
				// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
				if(ImGui::Begin("Testing Dockspace", &dockspaceOpen, window_flags))
				{
					ImGui::PopStyleVar();

					if (opt_fullscreen)
						ImGui::PopStyleVar(2);

					// DockSpace
					ImGuiIO& io = ImGui::GetIO();
					ImGuiStyle& style = ImGui::GetStyle();
					float minWinSizeX = style.WindowMinSize.x;
					style.WindowMinSize.x = 370.0f;
					if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
					{
						ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
						ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
					}

					style.WindowMinSize.x = minWinSizeX;

					if (ImGui::BeginMenuBar())
					{
						if (ImGui::BeginMenu("File"))
						{
							if (ImGui::MenuItem("Open Project...", "Ctrl+O"))
							{
								//OpenProject();
							}

							ImGui::Separator();

							if (ImGui::MenuItem("New Scene", "Ctrl+N"))
							{
								//NewScene();
							}

							if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
							{
								//SaveScene();
			/*					if(p_scene != nullptr)
								{
									SceneSerialiser scene_serialiser;
									scene_serialiser.SerialiseScene(mSceneDirectory + "/" + mMainScene, *p_scene);
								}*/
							}

							if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
							{
								//SaveSceneAs();
							}

							ImGui::Separator();

							if (ImGui::MenuItem("Exit"))
							{
								//Application::Get().Close();
							}

							ImGui::EndMenu();
						}

						if (ImGui::BeginMenu("Script"))
						{
							if (ImGui::MenuItem("Reload assembly", "Ctrl+R"))
							{
								//ScriptEngine::ReloadAssembly();
							}

							ImGui::EndMenu();
						}

						if (ImGui::BeginMenu("Docking Options"))
						{
							// Disabling fullscreen would allow the window to be moved to the front of other windows,
							// which we can't undo at the moment without finer window depth/z control.
							ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
							ImGui::Separator();

							if (ImGui::MenuItem("Flag: NoDockingOverCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingOverCentralNode) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingOverCentralNode; }
							if (ImGui::MenuItem("Flag: NoDockingSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingSplit) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingSplit; }
							if (ImGui::MenuItem("Flag: NoUndocking", "", (dockspace_flags & ImGuiDockNodeFlags_NoUndocking) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoUndocking; }
							if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
							if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
							if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
							ImGui::Separator();


							ImGui::EndMenu();
						}

						ImGui::EndMenuBar();
					}
				}
				ImGui::End();
			}
		};



	} //UI
} //namespace Fluxon