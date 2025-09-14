//#pragma once
////Fluxon Sim & Rendering Sandbox 
////This UITrait serves as an utility for UI 
////to make draw engine part have specialistaion as 
//// a ui trait and the trait would be a friend to the part 
////to access private memebers 
//
//#include <optional>
//
//
//namespace Fluxon {
//	namespace Editor {
//		template<typename T>
//		struct UITrait;
//
//		/// <summary>
//		/// dispatch atomic widgets
//		/// </summary>
//		/// <typeparam name="T"></typeparam>
//		/// <param name="value"></param>
//		template<typename T>
//		void DispatchDrawFields(T& value) {
//			UITrait<T>::DrawFields(value);
//		}
//
//		/// <summary>
//		/// dispatch grouped ui block
//		/// </summary>
//		/// <typeparam name="T"></typeparam>
//		/// <param name="value"></param>
//		template<typename T>
//		void DispatchDrawPanel(T& value) {
//			UITrait<T>::DrawPanel(value);
//		}
//
//		/// <summary>
//		/// dispatch full ui window block
//		/// </summary>
//		/// <typeparam name="T"></typeparam>
//		/// <param name="value"></param>
//		/// <param name="win_name"></param>
//		/// <param name="p_win_open"></param>
//		template<typename T>
//		void DispatchDrawWindow(T& value, std::optional<const char*> win_name = std::nullopt, bool* p_win_open = nullptr) {
//			UITrait<T>::DrawWindow(value, win_name, p_win_open);
//		}
//
//
//	}//namespace Editor
//}//namespace Fluxon
//
////Friend injection
//#define MAKE_PROPERTY_UI_TRAIT(Type) friend struct Fluxon::Editor::UITrait<Type>
//
//
////
////
////template<typename T>
////void DispatchDrawPanel(T& value) {
////	UITrait<T>::EditorDrawPanel(value);
////}
////
////template<typename T>
////void DispatchDrawWindow(T& value) {
////	UITrait<T>::EditorDrawWindow(value);
////}
//
