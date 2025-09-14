//#pragma once
//
//#include <optional>
//
//#include <imgui/imgui.h>
//#include "Core/Camera.h"
//
//#include <unordered_map>
//
//
//namespace Fluxon {
//	namespace Editor {
//
//		//Optional hooks 
//		template<typename T>
//		struct UITrait
//		{
//			//atomic widgets
//			static void DrawFields(T&) {}
//			//grouped ui block
//			static void DrawPanel(T&) {}
//			//full ui window block
//			static void DrawWindow(T&, std::optional<const char*> win_name = std::nullopt, bool* p_win_open = nullptr) {}
//		};
//
//		//enum class UIFieldBits : u8
//		//{
//		//	None,
//		//	Visible,
//		//	DebugOnly,
//		//	ReadOnly,
//		//};
//
//		//
//
//		////later inhert for Slider, Draw, Colour etcx
//		//struct UIFieldBase
//		//{
//		//	const char* label;
//		//	bool draw = false;
//		//	std::function<void()> imguiDrawFunc;
//		//};
//
//		//
//		//struct UIFieldBase2
//		//{
//		//	const char* label;
//		//	Flag<UIFieldBits> uiFieldFilter;
//		//	std::function<void()> imguiDrawFunc;
//		//};
//
//		//
//		
//
//		///Specialisations 
//		template<>
//		struct UITrait<Camera>
//		{
//
//
//
//			//static void CustomDrawFields(Camera& camera)
//			//{
//			//	static std::array<UIFieldBase, 4> fields =
//			//	{
//			//		UIFieldBase{"Near Z", true,[&camera]() {ImGui::SliderFloat("Near Z", &camera.mNearZ, 0.0f, 100.0f); }},
//			//	};
//
//
//			//	for (auto& field : fields)
//			//		if (field.draw)
//			//			field.imguiDrawFunc();
//
//			//}
//
//			//static std::unordered_map<std::string, UIFieldBase2> fields;
//
//
//			//static void CustomDrawFields(Camera& camera, std::unordered_map<std::string, Flag<UIFieldBits>> context)
//			//{
//			//	//static std::array<UIFieldBase, 4> fields =
//			//	//{
//			//	//	UIFieldBase{"Near Z", true,[&camera]() {ImGui::SliderFloat("Near Z", &camera.mNearZ, 0.0f, 100.0f); }},
//			//	//};
//
//			//	std::vector<UIFieldBase2> fields;
//			//	for (auto& ctx : context)
//			//	{
//			//		if (ctx.second.Contains(UIFieldBits::Visible))
//			//		{
//			//			auto it = cam_fields.find(ctx.first);
//			//			if (it != cam_fields.end())
//			//				fields.push_back(it.second);
//			//		}
//			//	}
//
//
//			//	for (auto& field : fields)
//			//		if (field.draw)
//			//			field.imguiDrawFunc();
//
//			//}
//
//
//
//			//UIField<float> nearZ_fields =
//			//{
//			//	"Near Z",
//			//	&camera.mNearZ,
//			//	true,
//			//	[](float* val) {ImGui::DragFloat("Nearz", val, 0.1, 100.0f); }
//			//};
//
//			//static bool bAllowFullEditParameter = true;
//			static void DrawFields(Camera& camera)
//			{
//				ImGui::DragFloat3("Position", &camera.mPosition[0], 0.1f);
//				ImGui::DragFloat3("Forward", &camera.mForward[0], 0.1f);
//				ImGui::SliderFloat("Near Z", &camera.mNearZ, 0.0f, 100.0f);
//				ImGui::SliderFloat("Far Z", &camera.mFarZ, 1.0f, 500.0f);
//				if (camera.IsPerspective())
//					ImGui::SliderFloat("FOV", &camera.mFovY, 0.0f, 179.0f, "%.1f");
//				else
//					ImGui::SliderFloat("Scale", &camera.mScale, 0.0f, 179.0f, "%.1f");
//			}
//
//			static void DrawPanel(Camera& camera)
//			{
//				ImGui::PushID(&camera);
//				ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Camera");
//				DrawFields(camera);
//				ImGui::PopID();
//			}
//
//
//			static void DrawWindow(Camera& camera, std::optional<const char*> win_name = std::nullopt, bool* p_win_open = nullptr)
//			{
//
//					//check if user provide a flag handle (i.e p_win_open not null)
//					//to toggle window 
//					//if not just keep window open
//					if (p_win_open == nullptr || *p_win_open)
//					{
//						const char* _name = (win_name.has_value()) ? win_name.value() : "Camera Window";
//
//						if (ImGui::Begin(_name, p_win_open))
//							DrawPanel(camera);
//
//						ImGui::End();
//					}
//			}
//		};
//
//		template<>
//		struct UITrait<DynamicCamera>
//		{
//			static void DrawFields(DynamicCamera& camera)
//			{
//				UITrait<Camera>::DrawFields(camera);
//				ImGui::DragFloat3("Up", &camera.mForward[0], 0.1f);
//				ImGui::DragFloat3("Right", &camera.mForward[0], 0.1f);
//			}
//
//			static void DrawPanel(DynamicCamera& camera)
//			{
//				ImGui::PushID(&camera);
//				ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Dynamic Camera");
//				DrawFields(camera);
//				ImGui::PopID();
//			}
//
//			static void DrawWindow(DynamicCamera& camera, std::optional<const char*> win_name, bool* p_win_open = nullptr)
//			{
//				//check if user provide a flag handle (i.e p_win_open not null)
//				//to toggle window 
//				//if not just keep window open
//				if (p_win_open == nullptr || *p_win_open)
//				{
//					const char* _name = (win_name.has_value()) ? win_name.value() : "Dynamic Camera Window";
//
//					if (ImGui::Begin(_name, p_win_open))
//						DrawPanel(camera);
//
//					ImGui::End();
//				}
//			}
//
//		};
//
//		template<>
//		struct UITrait<EditorCamera>
//		{
//			static void DrawFields(EditorCamera& camera)
//			{
//				UITrait<DynamicCamera>::DrawFields(camera);
//					ImGui::DragFloat("Move speed", &camera.mMoveSpeed, 0.1f);
//					ImGui::DragFloat("Rot speed", &camera.mRotSpeed, 0.1f);
//			}
//
//			static void DrawPanel(EditorCamera& camera)
//			{
//				ImGui::PushID(&camera);
//				ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Editor Camera");
//				DrawFields(camera);
//				ImGui::PopID();
//			}
//
//			static void DrawWindow(EditorCamera& camera, std::optional<const char*> win_name, bool* p_win_open = nullptr)
//			{
//				//check if user provide a flag handle (i.e p_win_open not null)
//				//to toggle window 
//				//if not just keep window open
//				if (p_win_open == nullptr || *p_win_open)
//				{
//					const char* _name = (win_name.has_value()) ? win_name.value() : "Editor Camera Window";
//
//					if (ImGui::Begin(_name, p_win_open))
//						DrawPanel(camera);
//
//					ImGui::End();
//				}
//			}
//
//		};
//
//	}//namespace Editor
//}//namespace Fluxon
//
//
