#pragma once

#include <imgui/imgui.h>
#include "../Scene/SceneData.h"
#include "../Scene/ISceneComponent.h"

#include "Core/Utilities.h"
#include <glm/glm.hpp>

#include "Core/Scene/SceneNode.h"

#include "Renderer/RenderTarget.h"


#include "UITraitSpecialisation.h"


#include <imgui/imgui_internal.h>

namespace Fluxon {
	namespace Editor {



		static void ApplyEuler(glm::quat& curr_quat, glm::vec3 ui_euler)
		{
			glm::vec3 delta_euler = ui_euler - glm::degrees(glm::eulerAngles(curr_quat));

			glm::quat qx = glm::angleAxis(glm::radians(delta_euler.x), glm::vec3(1, 0, 0));
			glm::quat qy = glm::angleAxis(glm::radians(delta_euler.y), glm::vec3(0, 1, 0));
			glm::quat qz = glm::angleAxis(glm::radians(delta_euler.z), glm::vec3(0, 0, 1));

			glm::quat delta_quat = qy * qx * qz;
			curr_quat = glm::normalize(delta_quat * curr_quat);
		}

		namespace Flux_ImGui_Util {

			//RAII
			struct BeginTableScope
			{
				bool active = false;
				BeginTableScope(const char* str_id, int column, bool use, ImGuiTableFlags imgui_flags = 0, 
					const ImVec2& outer_size = ImVec2((0.0f), (0.0f)), float label_width = 200.0f, float inner_width = (0.0f)) 
					: active(use)
				{
					if (active)
					{
						ImGui::BeginTable(str_id, column, imgui_flags, outer_size, inner_width);
						ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, label_width);
						ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
					}
				}

				~BeginTableScope()
				{
					if(active)
						ImGui::EndTable();
				}
			};

			static inline bool DrawSliderFloatV(const char* label, float* value, float v_min, float v_max, bool with_table = true, const char* format = "%.3f", ImGuiTableFlags imgui_flags = ImGuiTableFlags_BordersInnerV)
			{
				BeginTableScope table_scope ("Slider table row", 2, with_table, imgui_flags);
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::TextUnformatted(label);

				ImGui::TableSetColumnIndex(1);
				ImGui::PushID(label);
				bool changed = ImGui::SliderFloat("##slider", value, v_min, v_max, format);
				ImGui::PopID();
				return changed;
			}

			static inline bool DrawSliderIntV(const char* label, int* value, int v_min, int v_max, bool with_table = true, const char* format = "%d", ImGuiTableFlags imgui_flags = ImGuiTableFlags_BordersInnerV)
			{
				BeginTableScope table_scope("Slider table row", 2, with_table, imgui_flags);
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::TextUnformatted(label);

				ImGui::TableSetColumnIndex(1);
				ImGui::PushID(label);
				bool changed = ImGui::SliderInt("##slider", value, v_min, v_max, format);
				ImGui::PopID();
				return changed;
			}

			static inline bool DrawCheckboxV(const char* label, bool* value, bool with_table = true, ImGuiTableFlags imgui_flags = ImGuiTableFlags_BordersInnerV)
			{
				BeginTableScope table_scope("Slider table row", 2, with_table, imgui_flags);
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::TextUnformatted(label);

				ImGui::TableSetColumnIndex(1);
				
				ImGui::PushID(label);
				bool modified = ImGui::Checkbox(("##" + std::string(label)).c_str(), value);
				ImGui::PopID();
				return modified;
			}

			static inline bool DrawSliderVec3Row(const char* label, glm::vec3& value, float v_min, float v_max, bool with_table = true, const char* format = "%.3f", ImGuiTableFlags imgui_flags = ImGuiTableFlags_BordersInnerV)
			{
				BeginTableScope table_scope("Slider table row", 2, with_table, imgui_flags);
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::TextUnformatted(label);

				ImGui::TableSetColumnIndex(1);
				ImGui::PushID(label);
				bool modified = ImGui::SliderFloat3("##slider", &value[0], v_min, v_max, format);
				ImGui::PopID();
				return modified;
			}

			static inline bool DrawDragVec3Row(const char* label, glm::vec3& value, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f,
				float reset_value = 0.0f, bool with_table = true, const char* format = "%.2f", ImGuiTableFlags imgui_flags = ImGuiTableFlags_BordersInnerV)
			{
				BeginTableScope table_scope("Draw table row", 2, with_table, imgui_flags);

				bool modified = false;
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::TextUnformatted(label);

				ImGui::TableSetColumnIndex(1);
				ImGui::PushID(label);

				ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

				//float line_height = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
				float line_height = GImGui->FontSize + GImGui->Style.FramePadding.y * 2.0f;

				ImVec2 button_size = { line_height + 3.0f, line_height };

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
				if (modified |= ImGui::Button("X", button_size))
					value.x = reset_value;

				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("Reset X to %.1f", reset_value);

				ImGui::PopStyleColor(3);


				ImGui::SameLine();
				modified |= ImGui::DragFloat("##drag_x", &value.x, v_speed, v_min, v_max, format);
				ImGui::PopItemWidth();
				ImGui::SameLine();

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.6f, 0.2f, 1.0f });
				if (modified |= ImGui::Button("Y", button_size))
					value.y = reset_value;
				ImGui::PopStyleColor(3);

				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("Reset Y to %.1f", reset_value);

				ImGui::SameLine();
				modified |= ImGui::DragFloat("##drag_y", &value.y, v_speed, v_min, v_max, format);
				ImGui::PopItemWidth();
				ImGui::SameLine();

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
				if (modified |= ImGui::Button("Z", button_size))
					value.z = reset_value;
				ImGui::PopStyleColor(3);

				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("Reset Z to %.1f", reset_value);

				ImGui::SameLine();
				modified |= ImGui::DragFloat("##drag_z", &value.z, v_speed, v_min, v_max, format);
				ImGui::PopItemWidth();

				ImGui::PopStyleVar();
				
				ImGui::PopID();
				return modified;
			}



			static bool RenderableComponentPanel(RenderableComponent& render_comp) {
				bool modified = false;
				ImGui::SeparatorText("Renderable Component");
				modified |= Flux_ImGui_Util::DrawCheckboxV("Visible", &render_comp.visible);

				RenderMasks& render_mask_layer = render_comp.renderPrimitiveData.renderMask;
				if (ImGui::TreeNode("Render Mask Layer"))
				{

					//static bool selected[3] = { false, false, false };

					//std::array<bool, 4> selected;//(static_cast<size_t>(ERenderMask::Count));
					//bool selected;//(static_cast<size_t>(ERenderMask::Count));
					auto& layers_name = RenderMaskBitsLayerNames();
					using u_type = std::underlying_type_t<ERenderBits>;
					for (u_type i = 0; i < static_cast<u_type>(ERenderBits::Count); ++i)
					{
						ERenderBits mask = static_cast<ERenderBits>(1 << i);
						//ERenderBits flag = i;
						//selected[i] = (mask_layer & flag) == flag;
						bool selected = (render_mask_layer & mask) == mask;
						if (ImGui::Selectable(layers_name[i], &selected))
						{
							if (selected) //add flag
								render_mask_layer |= mask;
							//mask_layer |= flag;
							else //remove flag
								render_mask_layer &= ~mask;
							//mask_layer ^= flag;
							modified = true;
						}

					}

					ImGui::TreePop();
				}

				modified |= Flux_ImGui_Util::DrawCheckboxV("Use new material", (bool*)&render_comp.new_mat_sys_hack.w);
				return modified;
			}

			template<typename EnumBitsType, typename = std::enable_if_t<FlagBitMaskTraits<EnumBitsType>::isMask>>
			static inline bool DrawBitmaskSelector(const char* label, EnumBitsType& bits, const std::vector<const char*>& names)
			{
				bool modified = false;
				using underlying_type = std::underlying_type_t<EnumBitsType>;

				if (ImGui::TreeNode(label))
				{
					for (underlying_type i = 0; i < static_cast<underlying_type>(names.size()); ++i)
					{
						EnumBitsType bit = static_cast<EnumBitsType>(Bit<underlying_type>(i));
						bool selected = ((bits & bit) == bit);

						if (ImGui::Selectable(names[i], &selected))
						{
							bits ^= bit;
							modified = true;
						}

					}
					ImGui::TreePop();
				}

				return modified;
			}

			template<typename T>
			static inline bool DrawFlagSelector(const char* label, Flag<T>& flag, std::vector<const char*>& names)
			{
				bool modified = false;
				using underlying_type = std::underlying_type_t<T>;

				if (ImGui::TreeNode(label))
				{
					for (underlying_type i = 0; i < static_cast<underlying_type>(names.size()); ++i)
					{
						T bit = static_cast<T>(Bit<underlying_type>(i));
						bool selected = flag.Contains(bit);

						if (ImGui::Selectable(names[i], &selected))
						{
							flag.Toggle(bit);
							modified = true;
						}
					}
					ImGui::TreePop();
				}
				return modified;
			}


			/// <summary>
			/// This is similar to imgui internal 
			/// CheckboxFlagsBitsT(const char* label, T* flags, T bit)
			/// replacing T* flags as Flag<T>  flags 
			/// </summary>
			/// <typeparam name="T"></typeparam>
			/// <param name="label"></param>
			/// <param name="flags"></param>
			/// <param name="bit"></param>
			/// <returns></returns>
			template<typename T>
			bool CheckboxFlagsBitsT(const char* label, Flag<T>* flags, T bit, bool with_table = true, ImGuiTableFlags imgui_table_flags = ImGuiTableFlags_BordersInnerV)
			{
				BeginTableScope table_scope("Slider table row", 2, with_table, imgui_table_flags);
				
				bool all_on = (*flags & bit) == bit;
				//bool any_on = (*flags & bit) != 0;
				bool any_on = flags->Contains(bit);
				bool modified = false;

				//the scope manual starts new row 
				ImGui::TableNextColumn();
				ImGui::TableSetColumnIndex(0);
				ImGui::TextUnformatted(label);

				ImGui::TableSetColumnIndex(1);

				ImGui::PushID(label);
				if (!all_on && any_on)
				{
					ImGuiContext& g = *GImGui;
					g.NextItemData.ItemFlags |= ImGuiItemFlags_MixedValue;
					modified = ImGui::Checkbox("##check_box", &all_on);
				}
				else
					modified = ImGui::Checkbox("##check_box", &all_on);

				ImGui::PopID();

				if (modified)
				{
					if (all_on || !any_on)
						flags->Toggle(bit);
					else
						flags->Set(bit);
				}

				return modified;
			}

			template<typename T>
			static inline bool DrawFlagDropdownCheckbox(const char* label, Flag<T>& flag, std::vector<const char*>& names, bool with_table = true, ImGuiTableFlags imgui_table_flags = ImGuiTableFlags_BordersInnerV)
			{
				bool modified = false;
				using underlying_type = std::underlying_type_t<T>;

				if (ImGui::TreeNode(label))
				{
					//if (ImGui::BeginChild("Render Flags Child", ImVec2(0, 150), true))
					//{
						{
							BeginTableScope table_scope("Slider table row", 2, with_table, imgui_table_flags);

							//ImGui::CheckboxFlagsT() internal function solves flag but need to unit engine flag & bit 
							//to harier with the toggling with imgui
							for (underlying_type i = 0; i < static_cast<underlying_type>(names.size()); ++i)
							{
								T bit = static_cast<T>(Bit<underlying_type>(i));
								modified |= CheckboxFlagsBitsT(names[i], &flag, bit, false);
							}
						}
					//	ImGui::EndChild();
					//}
					ImGui::TreePop();
				}

				return modified;
			}


			template<typename T>
			static inline bool DrawFlagTree(const char* label, Flag<T>& flag, std::vector<const char*>& names, bool with_table = true, ImGuiTableFlags imgui_table_flags = ImGuiTableFlags_BordersInnerV,
				const ImVec2& outer_size = ImVec2((0.0f), (0.0f)), float label_width = 200.0f, float inner_width = (0.0f))
			{
				bool modified = false;
				using underlying_type = std::underlying_type_t<T>;

				imgui_table_flags |= ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuterV;//ImGuiTableFlags_HighlightHoveredColumn
			

				BeginTableScope table_scope("Slider table row", 2, with_table, imgui_table_flags, outer_size, label_width, inner_width);
			
				ImGui::TableNextRow();

				const ImGuiStyle* style = &ImGui::GetStyle();
				const ImVec4& d_col = style->Colors[/*ImGuiCol_Header*/ImGuiCol_ScrollbarBg];
				//Under the hood Colour32 is uint32, which is equal to ImU32.
				//ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, Colour32(d_col.x, d_col.y, d_col.z, d_col.w));
				ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, Colour32(0.02f, 0.02f, 0.02f, 0.53f));

				ImGui::TableSetColumnIndex(0);

				static ImGuiTreeNodeFlags base_flag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
					ImGuiTreeNodeFlags_SpanAvailWidth |  ImGuiTreeNodeFlags_Framed; //ImGuiTreeNodeFlags_SpanFullWidth  ImGuiTreeNodeFlags_CollapsingHeader
				//Hack to match the current table row background 
				//hmm actuall make invesible 
				ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.02f, 0.02f, 0.02f, 0.0f));
				bool open_child_tree = ImGui::TreeNodeEx(label, base_flag);
				ImGui::PopStyleColor(1);

				ImGui::TableSetColumnIndex(1);
				//ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, IM_COL32(80, 80, 160, 255));
				//bool all_on = flags.All(); <--- need to add this function flags.All()  &flags.Set All()
				Flag<T> all_flags;
				for (underlying_type i = 0; i < static_cast<underlying_type>(names.size()); ++i)
				{
					T bit = static_cast<T>(Bit<underlying_type>(i));
					all_flags.AddBit(bit);
				}


				bool all_on = (flag & all_flags) == all_flags;
				bool any_on = (flag & all_flags).Value() != 0;

				if (!all_on && any_on)
				{
					ImGuiContext& g = *GImGui;
					g.NextItemData.ItemFlags |= ImGuiItemFlags_MixedValue;
					modified |= ImGui::Checkbox("##all", &all_on);
				}
				else
				{
					modified |= ImGui::Checkbox("##all", &all_on);
				}

				if (modified)
				{
					if (all_on)
						flag |= all_flags;
					else
						flag &= ~all_flags;
				}

				//if (ImGui::Checkbox("##all", &all_on))
				//{
				//	if (all_on)
				//		flag |= all_flags;
				//	else
				//		flag &= ~all_flags;
				//}

				if (open_child_tree)
				{
					for (underlying_type i = 0; i < static_cast<underlying_type>(names.size()); ++i)
					{
						T bit = static_cast<T>(Bit<underlying_type>(i));
						modified |= CheckboxFlagsBitsT(names[i], &flag, bit, false);
					}
					ImGui::TreePop();
				}
				return modified;


			}


			template<typename T, size_t N>
			static inline bool DrawDropDown(const char* label, T& value, const char* const (&element_names)[N], bool with_table = true, ImGuiTableFlags imgui_flags = ImGuiTableFlags_BordersInnerV)
			{
				static_assert(std::is_enum_v<T>, "T needs to be an enum class!!!!");


				BeginTableScope table_scope("Slider table row", 2, with_table, imgui_flags);

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::TextUnformatted(label);

				ImGui::TableSetColumnIndex(1);

				bool modified = false;
				ImGui::PushID(label);
				int curr = static_cast<int>(value);
				if (ImGui::Combo("##combo", &curr, element_names, static_cast<int>(N)))
				{
					value = static_cast<T>(curr);
					modified =  true;
				}

				ImGui::PopID();
				return modified;
			}

			static inline bool DrawColourVec3Edit(const char* label, glm::vec3& value, bool with_table = true, ImGuiTableFlags imgui_flags = ImGuiTableFlags_BordersInnerV)
			{
				BeginTableScope table_scope("Slider table row", 2, with_table, imgui_flags);
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::TextUnformatted(label);

				ImGui::TableSetColumnIndex(1);
				ImGui::PushID(label);
				bool modified = ImGui::ColorEdit3("##colour3edit", &value[0]);
				ImGui::PopID();

				return modified;
			}

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
						BeginTableScope table_scope("Draw table row", 2, with_table, imgui_flags);
						glm::quat quat;
						glm::vec3 scale;
						glm::vec3 translate;
						Utils::DecomposeTransform(transform, translate, quat, scale);

						modified |= Flux_ImGui_Util::DrawDragVec3Row("Translation", translate, 0.01f, 0.0f, 0.0f, 0.0f, false);

						//ImGui::TableNextRow();
						//ImGui::TableSetColumnIndex(0);
						//ImGui::Separator();
						//ImGui::TableSetColumnIndex(1);

						glm::vec3 euler = glm::degrees(glm::eulerAngles(quat));
						if (Flux_ImGui_Util::DrawDragVec3Row("Euler", euler, 0.05f, 0.0f, 0.0f, 0.0f, false))
						{
							ApplyEuler(quat, euler);
							modified = true;
						}

						//ImGui::TableNextRow();
						//ImGui::TableSetColumnIndex(0);
						//ImGui::Separator();
						//ImGui::TableSetColumnIndex(1);

						modified |= Flux_ImGui_Util::DrawDragVec3Row("Scale", scale, 0.01f, 0.0f, 0.0f, 1.0f, false);
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
						imgui_flags |=  ImGuiTableFlags_BordersOuterV;
						BeginTableScope table_scope("Draw table row", 2, with_table, imgui_flags);

						ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, Colour32(0.02f, 0.02f, 0.02f, 0.53f));

						
						modified |= Flux_ImGui_Util::DrawCheckboxV("Visible", &render_comp.visible, false);

					}
					static std::vector<const char*> names = RenderMaskBitsLayerNamesVector();
					DrawFlagTree("Render Mask Layer", render_comp.renderPrimitiveData.renderMask, names, true);
					//Flux_ImGui_Util::DrawFlagSelector("Render Mask Layer", render_comp.renderPrimitiveData.renderMask, names);
					
					// ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, Colour32(0.02f, 0.02f, 0.02f, 0.53f));


					{
						imgui_flags |=   ImGuiTableFlags_BordersOuterV | ImGuiTableFlags_RowBg;
						BeginTableScope table_scope("Draw table row2", 2, with_table, imgui_flags);
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
	
		} //namespace ImGuiUtil



		static bool Model_Transform_Editor_Panel_UI(glm::mat4& transform)
		{

			glm::quat quat;
			glm::vec3 scale;
			glm::vec3 translate;
			Utils::DecomposeTransform(transform, translate, quat, scale);

			//bool modified = ImGui::DragFloat3("Translation", &translate[0], 0.01f, (0.0f), (0.0f), "%.2f");
			bool modified = Flux_ImGui_Util::DrawDragVec3Row("Translation", translate, 0.01f);
			//= ImGui::IsItemDeactivatedAfterEdit();//ImGui::IsItemEdited();
			//if (translate_update)
			//	transform.invMatrix = glm::inverse(transform.matrix);

			//modified = ImGui::DragFloat3("Euler", &euler[0], 0.01f, (0.0f), (0.0f), "%.2f");
			glm::vec3 euler = glm::degrees(glm::eulerAngles(quat));
			//if (ImGui::DragFloat3("Euler", &euler[0], 0.05f, 0.0f, (0.0f), "%.2f"))
			if (Flux_ImGui_Util::DrawDragVec3Row("Euler", euler, 0.05f))
			{
				ApplyEuler(quat, euler);
				modified = true;
			}
			//modified |= ImGui::DragFloat3("Scale", &scale[0], 0.01f, (0.0f), (0.0f), "%.2f");
			modified |= Flux_ImGui_Util::DrawDragVec3Row("Scale", scale, 0.01f);
			if (modified)
				transform = Utils::ComposeTransform(translate, quat, scale);

			return modified;
		}

		static bool RenderableComponentPanel(RenderableComponent& render_comp) {
			bool modified = false;
			ImGui::SeparatorText("Renderable Component");
			modified |= Flux_ImGui_Util::DrawCheckboxV("Visible", &render_comp.visible);

			RenderMasks& render_mask_layer = render_comp.renderPrimitiveData.renderMask;
			if (ImGui::TreeNode("Render Mask Layer"))
			{

				//static bool selected[3] = { false, false, false };

				//std::array<bool, 4> selected;//(static_cast<size_t>(ERenderMask::Count));
				//bool selected;//(static_cast<size_t>(ERenderMask::Count));
				auto& layers_name = RenderMaskBitsLayerNames();
				using u_type = std::underlying_type_t<ERenderBits>;
				for (u_type i = 0; i < static_cast<u_type>(ERenderBits::Count); ++i)
				{
					ERenderBits mask = static_cast<ERenderBits>(1 << i);
					//ERenderBits flag = i;
					//selected[i] = (mask_layer & flag) == flag;
					bool selected = (render_mask_layer & mask) == mask;
					if (ImGui::Selectable(layers_name[i], &selected))
					{
						if (selected) //add flag
							render_mask_layer |= mask;
							//mask_layer |= flag;
						else //remove flag
							render_mask_layer &= ~mask;
							 //mask_layer ^= flag;
						modified = true;
					}

				}
				
				ImGui::TreePop();
			}

			modified |= Flux_ImGui_Util::DrawCheckboxV("Use new material", (bool*)&render_comp.new_mat_sys_hack.w);
			return modified;
		}


		static void MainSceneUI(SceneResources& scene_resources)
		{
			for (auto& node : scene_resources.sceneNodes)
			{
				ImGui::PushID(&node);
				ImGui::SeparatorText(node->Name().data());
				//ImGui::Indent(20.0f);
				Flux_ImGui_Util::StringInputText("Name", &node->Name());
				Flux_ImGui_Util::DrawMat4Transform(node->GetTransformComponent()->transform);
				//ImGui::SeparatorText("Transform");
				//Model_Transform_Editor_Panel_UI(node->GetTransformComponent()->transform);
				if (node->HasRenderableComponent())
					Flux_ImGui_Util::DrawRenderableComponent(*node->GetRenderableComponent());
					//RenderableComponentPanel(*node->GetRenderableComponent());
				Flux_ImGui_Util::DrawCheckboxV("Animate Obj", &node->mAnimateObj);
				//ImGui::Unindent(20.0f);
				ImGui::PopID();
				ImGui::Spacing();
				ImGui::Spacing();
			}



			//ImGui::Spacing();
			//bool window_cursor_lock = Platform::IsCursorLocked(mWindow);
			//ImGui::Checkbox("Mouse Cursor", &window_cursor_lock);
			//ImGui::Spacing();

			//ImGui::SeparatorText("Camera Properties");
			//if (scene_resources.camera)
			//{
			//	auto cam_pos = (*scene_resources.camera).mPosition;
			//	ImGui::Text("Position x: %f, y: %f, z: %f", cam_pos.x, cam_pos.y, cam_pos.z);
			//	auto cam_forward = scene_resources.camera->mForward;
			//	ImGui::Text("Forward x: %f, y: %f, z: %f", cam_forward.x, cam_forward.y, cam_forward.z);
			//	ImGui::SliderFloat("Move Speed", &scene_resources.camera->moveSpeed, 0.01f, 10.0f);
			//	ImGui::SliderFloat("Rot Speed", &scene_resources.camera->rotSpeed, 0.01f, 1.0f);
			//	ImGui::DragFloat("Near", &scene_resources.camera->mNearZ, 0.1f, 0.01f, 50.0f, "%.1f");
			//	ImGui::DragFloat("Far", &scene_resources.camera->mFarZ, 0.1f, 0.0f, 500.0f, "%.1f");
			//	if(scene_resources.camera->mType == ECameraType::Perspective)
			//		ImGui::SliderFloat("FOV", &scene_resources.camera->mFovY, 0.0f, 179.0f, "%.1f");
			//	else
			//		ImGui::SliderFloat("Scale", &scene_resources.camera->mScale, 0.0f, 179.0f, "%.1f");
			//	//ImGui::BeginChild("##mesh_type", ImVec2(preview_panel_size.x * 0.75f, preview_panel_size.y * 0.5f));
			//	ImGui::BeginChild("##mesh_type");
			//	//static auto mesh_type = GetCameraTypeAsChar();
			//	const char* mesh_type[] = { "Perspective", "Orthographic" };
			//	int cam_type = (int)scene_resources.camera->mType;
			//	//if(cam_type)
			//	if(ImGui::Combo("Preview Mesh", &cam_type, mesh_type, 2))
			//		scene_resources.camera->mType = static_cast<ECameraType>(cam_type);

			//	ImGui::EndChild();
			//}
			//else
			//	ImGui::Text("No vaild Camera !!!!!!");


			ImGui::SeparatorText("Camera Properties");
			if (scene_resources.camera)
			{
				auto cam_pos = (*scene_resources.camera).GetPosition();
				ImGui::Text("Position x: %f, y: %f, z: %f", cam_pos.x, cam_pos.y, cam_pos.z);
				auto cam_forward = scene_resources.camera->GetForward();
				ImGui::Text("Forward x: %f, y: %f, z: %f", cam_forward.x, cam_forward.y, cam_forward.z);
				float v = scene_resources.camera->GetMoveSpeed();
				if (ImGui::SliderFloat("Move Speed", &v, 0.01f, 10.0f))
					scene_resources.camera->SetMoveSpeed(v);
				v = scene_resources.camera->GetRotSpeed();
				if (ImGui::SliderFloat("Rot Speed", &v, 0.01f, 1.0f))
					scene_resources.camera->SetRotSpeed(v);
				v = scene_resources.camera->GetNearZ();
				if (ImGui::DragFloat("Near", &v, 0.1f, 0.01f, 50.0f, "%.1f"))
					scene_resources.camera->SetNearZ(v);
				v = scene_resources.camera->GetFarZ();
				if (ImGui::DragFloat("Far", &v, 0.1f, 0.0f, 500.0f, "%.1f"))
					scene_resources.camera->SetFarZ(v);
				if (scene_resources.camera->IsPerspective())
				{
					v = scene_resources.camera->GetFovY();
					if(ImGui::SliderFloat("FOV", &v, 0.0f, 179.0f, "%.1f"))
						scene_resources.camera->SetFovY(v);
				}
				else
				{
					v = scene_resources.camera->GetScale();
					if (ImGui::SliderFloat("Scale", &v, 0.0f, 179.0f, "%.1f"))
						scene_resources.camera->SetScale(v);
				}
				//ImGui::BeginChild("##mesh_type", ImVec2(preview_panel_size.x * 0.75f, preview_panel_size.y * 0.5f));
				ImGui::BeginChild("##cam_type");
				//static auto mesh_type = GetCameraTypeAsChar();
				const char* mesh_type[] = { "Perspective", "Orthographic" };
				int cam_type = (int)scene_resources.camera->GetType();
				//if(cam_type)
				if (ImGui::Combo("Preview Mesh", &cam_type, mesh_type, 2))
					scene_resources.camera->SetType(static_cast<ECameraType>(cam_type));

				ImGui::EndChild();
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

				Colour32 c;

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
			ImGui::Text("Frame Delta time: %f", scene_resources.currentFrameDT);
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

					ImGui::SeparatorText("Light Shadow Data");

					//UITrait<Camera>::DrawPanel(scene_resources.lightShadowData.camera);
					//DispatchDrawPanel(scene_resources.lightShadowData.camera);

					ImGui::Text("Dispatching window for camera without name and close handle");
					//DispatchDrawWindow(scene_resources.lightShadowData.camera);
					//ImGui::Text("Dispatching window for camera with name and without close handle");
					//DispatchDrawWindow(scene_resources.lightShadowData.camera, "New Camera Window");
					static bool check_box = true;
					ImGui::Checkbox("New Camera Window2 enable", &check_box);
					//DispatchDrawWindow(scene_resources.lightShadowData.camera, "New Camera Window2", &check_box);

					//DispatchDrawWindow(*scene_resources.camera, "Main Camera");
					//ImGui::SliderFloat("View size", &scene_resources.lightShadowData.viewSize, 0.0f, 100.0f);
					//ImGui::SliderFloat("View near", &scene_resources.lightShadowData.near, 0.1f, 50.0f);
					//ImGui::SliderFloat("View far", &scene_resources.lightShadowData.far, 1.0f, 500.0f);

					//ImGui::DragFloat3("Sample Origin", &scene_resources.lightShadowData.sampleOrigin[0], 0.1f);
					//ImGui::DragFloat3("Sample World Up", &scene_resources.lightShadowData.sampleWorldUp[0], 0.1f);
					
					ImGui::Text("ERRROR ERROR ERRROE ");
					
					ImGui::SliderFloat("Sample Offste", &scene_resources.lightShadowData.sampleShadowDistance, 1.0f, 500.0f);

					Flux_ImGui_Util::DrawSliderFloatV("depthBiasConstantFactor", &scene_resources.lightShadowData.depthBiasConstantFactor, 1.0f, 10.0f);
					static glm::vec3 test_pos = glm::vec3(0.0f);
					Flux_ImGui_Util::DrawSliderVec3Row("Camera Position", test_pos, -5.0f, 25.0f);
					Flux_ImGui_Util::DrawDragVec3Row("Camera Position", test_pos, 0.1f, -5.0f, 25.0f);

					std::string name = "test_property";
					static bool value = true;
						bool updated = false;
						ImGui::TextUnformatted(name.c_str());
						ImGui::NextColumn();
						ImGui::PushItemWidth(-1);

						if (ImGui::Checkbox(("##"+name).data(), &value))
							updated = true;

						ImGui::PopItemWidth();
						ImGui::NextColumn();


						ImGui::Spacing();
						ImGui::Spacing();
						ImGui::Spacing();
					//my custom
						ImGui::Separator();
					std::string name1 = "my_custom";
					ImGui::Text(name1.c_str());
					ImGui::SameLine();
					//ImGui::SetColumnWidth
					ImGui::NextColumn();
					ImGui::PushItemWidth(20);
					ImGui::Checkbox(("##" + name1).data(), &value);
					ImGui::PopItemWidth();
					ImGui::NextColumn();
					ImGui::Separator();

					//my custom2
					std::string name2 = "my_custom2";
					ImGui::Text(name2.c_str());

					ImGui::SameLine();
					ImVec2 pos = ImGui::GetCursorScreenPos();
					static float offset_by = 2.0f;
					ImVec2 offset = ImVec2(pos.x + offset_by, pos.y);
					ImVec2 curr_win_size = ImGui::GetWindowSize();
					
					offset.x = glm::min(offset.x, curr_win_size.x - ImGui::GetWindowContentRegionMin().x);
					ImGui::SetCursorScreenPos(offset);
					ImGui::Checkbox(("##" + name2).data(), &value);
					ImGui::Separator();
					ImGui::SliderFloat("offset by", &offset_by, 0.0f, 250.0f);

					Flux_ImGui_Util::DrawSliderFloatV("offset by", &offset_by, 0.0f, 250.0f);

					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Spacing();
					//my custom3
					Flux_ImGui_Util::DrawCheckboxV("My custom3", &value);

					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Spacing();

					static Flag<ERenderBits> render_mask_layer = ERenderBits::ShadowCaster;
					//auto& raw_names = RenderMaskBitsLayerNames();
					std::vector<const char*> names = RenderMaskBitsLayerNamesVector();
					//std::copy(raw_names[0], raw_names[0] + raw_names.size(), std::back_inserter(names));
					Flux_ImGui_Util::DrawFlagSelector("Test flag", render_mask_layer, names);
					Flux_ImGui_Util::DrawFlagDropdownCheckbox("Test flag2", render_mask_layer, names);
					Flux_ImGui_Util::DrawFlagTree("Test flag2 tree", render_mask_layer, names);



					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Spacing();
					static ERenderBits render_bits = ERenderBits::Skybox;
					Flux_ImGui_Util::DrawBitmaskSelector("Test bits", render_bits, names);


					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Spacing();

					static ECameraType cam_type = ECameraType::Perspective;
					const char* cam_names[] = { "Perspective", "Orthographic" };
					Flux_ImGui_Util::DrawDropDown("Camera Type", cam_type, cam_names);


					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Spacing();

					glm::vec3 colour = glm::vec3(0.0f, 0.0f, 1.0f);
					Flux_ImGui_Util::DrawColourVec3Edit("Test colour", colour);



					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Spacing();

					static glm::mat4 transform = glm::mat4(1.0f);
					Flux_ImGui_Util::DrawMat4Transform(transform);
					static glm::mat4 transform2 = glm::mat4(1.0f);
					Flux_ImGui_Util::DrawMat4Transform(transform2);



					//Test 
					ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1, 0.1, 0.1, 1.0f));
					ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.15, 0.15, 0.15, 1.0f));
					ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.2, 0.2, 0.2, 1.0f));

					ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
					static float test_value;
					static bool test_enable;
					ImGui::InputFloat("Test Test Hollow", &test_value);
					ImGui::Checkbox("Test enable", &test_enable);


					ImGui::PopStyleVar();
					ImGui::PopStyleColor(3);


					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Experimental Particle"))
				{
					if (scene_resources.particleEmitter)
					{
						ImGui::SeparatorText("Emitter Transform");
						Model_Transform_Editor_Panel_UI(scene_resources.particleEmitter->worldTransform);
						ImGui::SeparatorText("Bounds");
						ImGui::Indent(20.0f);
						ImGui::DragFloat3("Min", &scene_resources.particleEmitter->boundsMin[0], 0.1f);
						ImGui::DragFloat3("Max", &scene_resources.particleEmitter->boundsMax[0], 0.1f);
						ImGui::Unindent(20.0f);
						ImGui::SliderFloat("Bound Collision Impluse", &scene_resources.particleEmitter->boundsCollisionImpluse, -2.0f, 2.0f);
						ImGui::SeparatorText("Emitter");
						ImGui::Indent(20.0f);
						ImGui::Text("Pending Spawn count, %d", scene_resources.particleEmitter->spawnCount);
						static int require_count = scene_resources.particleEmitter->spawnCount;
						ImGui::SliderInt("Req Spawn Count", &require_count, 0, 40);
						ImGui::SeparatorText("Particle Properties");
						ImGui::DragFloat("Particle Lifetime", &scene_resources.particleEmitter->particleLifeTime, 0.1f);
						ImGui::ColorEdit3("Start Colour", &scene_resources.particleEmitter->particleStartColour[0]);
						ImGui::ColorEdit3("End Colour", &scene_resources.particleEmitter->particleEndColour[0]);
						ImGui::SliderFloat("Particle Mass", &scene_resources.particleEmitter->particleMass, 0.0f, 10.0f);
						ImGui::SliderFloat("Particle Spawn Velocity", &scene_resources.particleEmitter->startSpeed, -20.0f, 20.0f);
						if(ImGui::Button("Initiate Spawn Req"))
							scene_resources.particleEmitter->spawnCount = require_count;
						ImGui::Unindent(20.0f);
					}
					ImGui::EndTabItem();
				}



				ImGui::EndTabBar();
			}
			ImGui::End();


			return;
			ImGui::Begin("Vulkan Texture Test");
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
			ImGui::End();
		}

	} //# Editor

} //# Fluxon

