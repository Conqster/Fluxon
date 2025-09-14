#pragma once


#include <imgui/imgui.h>
#include <imgui_internal.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <string>

#include "Core/EnumBitsFlag.h"

namespace Fluxon {
	namespace UI {
		namespace Internals {



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

				bool changed = ImGui::InputText(label, &my_str->data()[0], (size_t)my_str->size(), flags | ImGuiInputTextFlags_CallbackResize, StringResizeCallback, (void*)my_str);

				if (!my_str->empty() && (*my_str)[my_str->size() - 1] == '\0')
					my_str->resize(my_str->size() - 1);
				return changed;
			}



			static void ApplyEuler(glm::quat& curr_quat, glm::vec3 ui_euler)
			{
				glm::vec3 delta_euler = ui_euler - glm::degrees(glm::eulerAngles(curr_quat));

				glm::quat qx = glm::angleAxis(glm::radians(delta_euler.x), glm::vec3(1, 0, 0));
				glm::quat qy = glm::angleAxis(glm::radians(delta_euler.y), glm::vec3(0, 1, 0));
				glm::quat qz = glm::angleAxis(glm::radians(delta_euler.z), glm::vec3(0, 0, 1));

				glm::quat delta_quat = qy * qx * qz;
				curr_quat = glm::normalize(delta_quat * curr_quat);
			}

				//RAII
			struct BeginTableScope
			{
				bool active = false;
				BeginTableScope(const char* str_id, int column, bool use, ImGuiTableFlags imgui_flags = 0,
					const ImVec2& outer_size = ImVec2((0.0f), (0.0f)), float label_width = 200.0f, float inner_width = (0.0f))
					: active(use)
				{
					if(active)
						active = ImGui::BeginTable(str_id, column, imgui_flags, outer_size, inner_width);
					if (active)
					{
						ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, label_width);
						if(column > 1)
							ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
					}
				}

				~BeginTableScope()
				{
					if (active)
						ImGui::EndTable();
				}
			};


			/// <summary>
			///// Draw Text Row vertical
			///// </summary>
			///// <param name="label"></param>
			///// <param name="value"></param>
			///// <param name="v_min"></param>
			///// <param name="v_max"></param>
			///// <param name="with_table"></param>
			///// <param name="format"></param>
			///// <param name="imgui_flags"></param>
			///// <returns></returns>
			static inline void DrawTextVProperty(const char* label, const char* fmt, ...)
			{
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::TextUnformatted(label);

				ImGui::TableSetColumnIndex(1);
				va_list args;
				va_start(args, fmt);
				ImGui::TextV(fmt, args);
				va_end(args);
			}

			static inline void DrawTextUnformattedVProperty(const char* label, const char* text, const char* text_end = nullptr)
			{
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::TextUnformatted(label);

				ImGui::TableSetColumnIndex(1);

				ImGui::TextUnformatted(text, text_end);
			}


			static inline bool DrawSliderFloatV(const char* label, float* value, float v_min, float v_max, bool with_table = true, const char* format = "%.3f", ImGuiTableFlags imgui_flags = ImGuiTableFlags_BordersInnerV)
			{
				BeginTableScope table_scope("Slider table row", 2, with_table, imgui_flags);
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::TextUnformatted(label);

				ImGui::TableSetColumnIndex(1);
				ImGui::PushID(label);
				bool changed = ImGui::SliderFloat("##slider", value, v_min, v_max, format);
				ImGui::PopID();
				return changed;
			}

			static inline bool DrawDragFloatV(const char* label, float* value, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, bool with_table = true, const char* format = "%.3f", ImGuiTableFlags imgui_flags = ImGuiTableFlags_BordersInnerV)
			{
				BeginTableScope table_scope("drag table row", 2, with_table, imgui_flags);
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::TextUnformatted(label);

				ImGui::TableSetColumnIndex(1);
				ImGui::PushID(label);
				bool changed = ImGui::DragFloat("##drag", value, v_speed, v_min, v_max, format);
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

			/// <summary>
			/// Draw boolean checkbox Row vertical
			/// </summary>
			/// <param name="label"></param>
			/// <param name="value"></param>
			/// <param name="with_table"></param>
			/// <param name="imgui_flags"></param>
			/// <returns></returns>
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
				const glm::vec3& reset_value = glm::vec3(0.0f), bool with_table = true, const char* format = "%.2f", ImGuiTableFlags imgui_flags = ImGuiTableFlags_BordersInnerV)
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
					value.x = reset_value.x;

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
					value.y = reset_value.y;
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
					value.z = reset_value.z;
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



			//static bool RenderableComponentPanel(RenderableComponent& render_comp) {
			//	bool modified = false;
			//	ImGui::SeparatorText("Renderable Component");
			//	modified |= Flux_ImGui_Util::DrawCheckboxV("Visible", &render_comp.visible);

			//	RenderMasks& render_mask_layer = render_comp.renderPrimitiveData.renderMask;
			//	if (ImGui::TreeNode("Render Mask Layer"))
			//	{

			//		//static bool selected[3] = { false, false, false };

			//		//std::array<bool, 4> selected;//(static_cast<size_t>(ERenderMask::Count));
			//		//bool selected;//(static_cast<size_t>(ERenderMask::Count));
			//		auto& layers_name = RenderMaskBitsLayerNames();
			//		using u_type = std::underlying_type_t<ERenderBits>;
			//		for (u_type i = 0; i < static_cast<u_type>(ERenderBits::Count); ++i)
			//		{
			//			ERenderBits mask = static_cast<ERenderBits>(1 << i);
			//			//ERenderBits flag = i;
			//			//selected[i] = (mask_layer & flag) == flag;
			//			bool selected = (render_mask_layer & mask) == mask;
			//			if (ImGui::Selectable(layers_name[i], &selected))
			//			{
			//				if (selected) //add flag
			//					render_mask_layer |= mask;
			//				//mask_layer |= flag;
			//				else //remove flag
			//					render_mask_layer &= ~mask;
			//				//mask_layer ^= flag;
			//				modified = true;
			//			}

			//		}

			//		ImGui::TreePop();
			//	}

			//	modified |= Flux_ImGui_Util::DrawCheckboxV("Use new material", (bool*)&render_comp.new_mat_sys_hack.w);
			//	return modified;
			//}

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
					ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed; //ImGuiTreeNodeFlags_SpanFullWidth  ImGuiTreeNodeFlags_CollapsingHeader
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
					modified = true;
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









		} //namespace Internal
	}//namespace UI
} //Fluxon