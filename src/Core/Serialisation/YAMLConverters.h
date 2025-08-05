#pragma once

#include <yaml-cpp/yaml.h>
#include "Core/Camera.h"
#include "Core/Scene/Scene.h"

#include "Resources/MeshResource.h"

namespace YAML {

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node["x"] = rhs.x;
			node["y"] = rhs.y;
			node["z"] = rhs.z;
			node.SetStyle(YAML::EmitterStyle::Flow);
			return node; 
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			rhs.x = node["x"].as<double>();
			rhs.y = node["y"].as<double>();
			rhs.z = node["z"].as<double>();
			return true;
		}
	};

	template<>
	struct convert<Fluxon::TransformComponent>
	{
		static Node encode(const Fluxon::TransformComponent& transform_comp)
		{
			Node node;
			glm::vec3 translate, rotation, scale;
			Fluxon::Utils::DecomposeTransform(transform_comp.transform, translate, rotation, scale);
			node["mPosition"] = translate;
			node["mRotation"] = rotation;
			node["mScale"] = scale;
			return node;
		}


		static bool decode(const Node& node, Fluxon::TransformComponent& transform_comp)
		{
			glm::vec3 translate, rotation, scale;
			translate = node["mPosition"].as<glm::vec3>();
			rotation = node["mRotation"].as<glm::vec3>();
			scale = node["mScale"].as<glm::vec3>();
			//transform_comp.transform = glm::translate(glm::mat4(1.0f), translate) *
			//	//glm::rotate()
			//	glm::scale(glm::mat4(1.0f), scale);
			transform_comp.transform = Fluxon::Utils::ComposeTransform(translate, glm::quat(glm::radians(rotation)), scale);
			return true;
		}
	};

	template<>
	struct convert<Fluxon::Camera> {
		static Node encode(const Fluxon::Camera& camera) {
			Node node;
			node["position"] = camera.position;
			node["forward"] = camera.forward;
			node["up"] = camera.up;
			node["right"] = camera.right;
			node["fov"] = camera.fov;
			node["nearPlane"] = camera.nearPlane;
			node["farPlane"] = camera.farPlane;
			node["moveSpeed"] = camera.moveSpeed;
			node["rotSpeed"] = camera.rotSpeed;
			return node;
		}

		static bool decode(const Node& node, Fluxon::Camera& camera)
		{
			camera.position = node["position"].as<glm::vec3>();
			camera.forward = node["forward"].as<glm::vec3>();
			camera.up = node["up"].as<glm::vec3>();
			camera.right = node["right"].as<glm::vec3>();
			camera.fov = node["fov"].as<float>();
			camera.nearPlane = node["nearPlane"].as<float>();
			camera.farPlane = node["farPlane"].as<float>();
			camera.moveSpeed = node["moveSpeed"].as<float>();
			camera.rotSpeed = node["rotSpeed"].as<float>();


			return true;
		}
	};

	template<>
	struct convert<Fluxon::EPrimitiveMeshType> {
		static Node encode(const Fluxon::EPrimitiveMeshType& primitive_type)
		{
			Node node;

			switch (primitive_type)
			{
			case Fluxon::EPrimitiveMeshType::EPMT_None:
				node = "None";
				break;
			case Fluxon::EPrimitiveMeshType::EPMT_Cube:
				node = "Cube";
				break;
			case Fluxon::EPrimitiveMeshType::EPMT_ModelMesh:
				node = "ModelMesh";
				break;
			case Fluxon::EPrimitiveMeshType::EPMT_Sphere:
				node = "Sphere";
				break;
			case Fluxon::EPrimitiveMeshType::EPMT_Plane:
				node = "Plane";
				break;
			default:
				node = "Unknown";
				break;
			}

			return node;
		}


		static bool decode(const Node& node, Fluxon::EPrimitiveMeshType& primitive_type)
		{
			std::string& value = node.as<std::string>();
			
			if (value == "None")
				primitive_type = Fluxon::EPrimitiveMeshType::EPMT_None;
			else if (value == "Cube")
				primitive_type = Fluxon::EPrimitiveMeshType::EPMT_Cube;
			else if (value == "ModelMesh")
				primitive_type = Fluxon::EPrimitiveMeshType::EPMT_ModelMesh;
			else if (value == "Sphere")
				primitive_type = Fluxon::EPrimitiveMeshType::EPMT_Sphere;
			else if (value == "Plane")
				primitive_type = Fluxon::EPrimitiveMeshType::EPMT_Plane;
			else
				return false;


			return true;
		}
	};


	template<>
	struct convert<Fluxon::EMeshType> {
		static Node encode(const Fluxon::EMeshType& mesh_type)
		{
			Node node;

			switch (mesh_type)
			{
			case Fluxon::EMeshType::EMT_Static:
				node = "Static";
				break;
			default:
				node = "Unknown";
				break;
			}

			return node;
		}


		static bool decode(const Node& node, Fluxon::EMeshType& mesh_type)
		{
			std::string& value = node.as<std::string>();

			if (value == "Static")
				mesh_type = Fluxon::EMeshType::EMT_Static;
			else
				return false;


			return true;
		}
	};


	template<>
	struct convert<Fluxon::MeshDescriptor> {
		static Node encode(const Fluxon::MeshDescriptor& mesh_description)
		{
			Node node;
			node["id"] = mesh_description.id;
			node["type"] = mesh_description.type;
			node["primitiveType"] = mesh_description.primitiveType;
			if (mesh_description.primitiveType == Fluxon::EPrimitiveMeshType::EPMT_ModelMesh)
				node["model path"] = mesh_description.modelPath;
			return node;
		}

		static bool decode(const Node& node, Fluxon::MeshDescriptor& mesh_description)
		{
			mesh_description.id = node["id"].as<int32_t>();
			mesh_description.type = node["type"].as<Fluxon::EMeshType>();
			mesh_description.primitiveType = node["primitiveType"].as<Fluxon::EPrimitiveMeshType>();
			if (mesh_description.primitiveType == Fluxon::EPrimitiveMeshType::EPMT_ModelMesh)
				mesh_description.modelPath = node["model path"].as<std::string>();

			return true;
		}
	};


	template<>
	struct convert<Fluxon::RenderableComponent> {
		static Node encode(const Fluxon::RenderableComponent& render_comp)
		{
			Node node;
			node["id"] = render_comp.id;
			node["mesh"] = (render_comp.meshIdx.has_value() ? (int)render_comp.meshIdx.value() : -1);
			node["material"] = (render_comp.materialIdx.has_value() ? (int)render_comp.materialIdx.value() : -1);
			node["visible"] = render_comp.visible;
			node["pipelineGlobalBucketIdx"] = static_cast<int>(render_comp.pipelineGlobalBucketIdx);
			node["descSetSpecificationGlobalBucketIdx"] = static_cast<int>(render_comp.descSetSpecificationGlobalBucketIdx);
			return node;
		}

		static bool decode(const Node& node, Fluxon::RenderableComponent& render_comp)
		{
			render_comp.id = node["id"].as<uint32_t>();
			if(node["mesh"].as<int>() != -1)
				render_comp.meshIdx = node["mesh"].as<uint32_t>();
			if(node["material"].as<int>() != -1)
			render_comp.materialIdx = node["material"].as<uint32_t>();
			render_comp.visible = node["visible"].as<bool>();
			render_comp.pipelineGlobalBucketIdx = node["pipelineGlobalBucketIdx"].as<int>();
			render_comp.descSetSpecificationGlobalBucketIdx = node["descSetSpecificationGlobalBucketIdx"].as<int>();

			return true;
		}
	};

	//template<>
	//struct convert<Fluxon::Vulkan::PBRMaterial> {
	//	static Node encode(const Fluxon::Vulkan::PBRMaterial& mat)
	//	{
	//		Node node;

	//		return node;
	//	}

	//};

	template<>
	struct convert<VkFormat> {
		static Node encode(const VkFormat& vk_format)
		{
			Node node;

			switch (vk_format)
			{
			case VK_FORMAT_R8G8B8A8_UINT:
				node = "VK_FORMAT_R8G8B8A8_UINT";
				break;
			case VK_FORMAT_R8G8B8A8_SINT:
				node = "VK_FORMAT_R8G8B8A8_SINT";
				break;
			case VK_FORMAT_R8G8B8A8_SRGB:
				node = "VK_FORMAT_R8G8B8A8_SRGB";
				break;
			case VK_FORMAT_R8G8B8_UINT:
				node = "VK_FORMAT_R8G8B8_UINT";
				break;
			case VK_FORMAT_R8G8B8_SINT:
				node = "VK_FORMAT_R8G8B8_SINT";
				break;
			case VK_FORMAT_R8G8B8_SRGB:
				node = "VK_FORMAT_R8G8B8_SRGB";
				break;
			case VK_FORMAT_R8G8_UINT:
				node = "VK_FORMAT_R8G8_UINT";
				break;
			case VK_FORMAT_R8G8_SINT:
				node = "VK_FORMAT_R8G8_SINT";
				break;
			case VK_FORMAT_R8G8_SRGB:
				node = "VK_FORMAT_R8G8_SRGB";
				break;
			case VK_FORMAT_R8_UINT:
				node = "VK_FORMAT_R8_UINT";
				break;
			case VK_FORMAT_R8_SINT:
				node = "VK_FORMAT_R8_SINT";
				break;
			case VK_FORMAT_R8_SRGB:
				node = "VK_FORMAT_R8_SRGB";
				break;
				//case Fluxon::EPrimitiveMeshType::EPMT_Sphere:
				//	node = "Sphere";
				//	break;
				//case Fluxon::EPrimitiveMeshType::EPMT_Plane:
				//	node = "Plane";
				//	break;
			default:
				node = "Unknown";
				break;
			}

			return node;
		}


		static bool decode(const Node& node, VkFormat& vk_format)
		{
			std::string& value = node.as<std::string>();

			if (value == "VK_FORMAT_R8G8B8A8_UINT")
				vk_format = VK_FORMAT_R8G8B8A8_UINT;
			else if (value == "VK_FORMAT_R8G8B8A8_SINT")
				vk_format = VK_FORMAT_R8G8B8A8_SINT;
			else if (value == "VK_FORMAT_R8G8B8A8_SRGB")
				vk_format = VK_FORMAT_R8G8B8A8_SRGB;
			else if (value == "VK_FORMAT_R8G8B8_UINT")
				vk_format = VK_FORMAT_R8G8B8_UINT;
			else if (value == "VK_FORMAT_R8G8B8_SINT")
				vk_format = VK_FORMAT_R8G8B8_SINT;
			else if (value == "VK_FORMAT_R8G8B8_SRGB")
				vk_format = VK_FORMAT_R8G8B8_SRGB;
			else if (value == "VK_FORMAT_R8G8_UINT")
				vk_format = VK_FORMAT_R8G8_UINT;
			else if (value == "VK_FORMAT_R8G8_SINT")
				vk_format = VK_FORMAT_R8G8_SINT;
			else if (value == "VK_FORMAT_R8G8_SRGB")
				vk_format = VK_FORMAT_R8G8_SRGB;
			else if (value == "VK_FORMAT_R8_UINT")
				vk_format = VK_FORMAT_R8_UINT;
			else if (value == "VK_FORMAT_R8_SINT")
				vk_format = VK_FORMAT_R8_SINT;
			else if (value == "VK_FORMAT_R8_SRGB")
				vk_format = VK_FORMAT_R8_SRGB;
			else
				return false;

			return true;
		}
	};


	template<>
	struct convert<Fluxon::MaterialDescriptor>
	{
		static Node encode(const Fluxon::MaterialDescriptor& material_reso)
		{
			Node node;
			node["id"] = material_reso.id;
			node["format"] = material_reso.format;
			node["anisotropicSampling"] = material_reso.anisotropicSampling;

			//file paths 
			Node file_path_map;
			for (uint32_t i = 0; i < (uint32_t)Fluxon::MaterialDescriptor::ETextureMap::TMap_Count; i++)
			{
				const auto& map = static_cast<Fluxon::MaterialDescriptor::ETextureMap>(i);
				switch (map)
				{
				case Fluxon::MaterialDescriptor::ETextureMap::TMap_Albedo:
					file_path_map["Albedo"] = material_reso.pbrPaths[i];
					break;
				case Fluxon::MaterialDescriptor::ETextureMap::TMap_Normal:
					file_path_map["Normal"] = material_reso.pbrPaths[i];
					break;
				case Fluxon::MaterialDescriptor::ETextureMap::TMap_aoRM:
					file_path_map["aoRM"] = material_reso.pbrPaths[i];
					break;
				default:
					file_path_map["Unknown"] = material_reso.pbrPaths[i];
					break;

				}
			}
			//for (auto& path : material_reso.pbrPaths)
			//	file_path_map.push_back(path);
			node["Texture Maps"] = file_path_map;

			return node;
		}


		static bool decode(const Node& node, Fluxon::MaterialDescriptor& material_reso)
		{
			material_reso.id = node["id"].as<int32_t>();
			material_reso.format = node["format"].as<VkFormat>();
			material_reso.anisotropicSampling = node["anisotropicSampling"].as<bool>();


			if (node["Texture Maps"] && node["Texture Maps"].size() == 3)
			{
				Node tex_map_node = node["Texture Maps"];

				for(uint32_t i = 0; i < 3; i++)
				{
					const auto& map = static_cast<Fluxon::MaterialDescriptor::ETextureMap>(i);
					std::string out_string;
					switch (map)
					{
					case Fluxon::MaterialDescriptor::ETextureMap::TMap_Albedo:
						material_reso.pbrPaths[i] =  std::move(tex_map_node["Albedo"].as<std::string>());
						break;
					case Fluxon::MaterialDescriptor::ETextureMap::TMap_Normal:
						material_reso.pbrPaths[i] = tex_map_node["Normal"].as<std::string>();
						break;
					case Fluxon::MaterialDescriptor::ETextureMap::TMap_aoRM:
						material_reso.pbrPaths[i] = tex_map_node["aoRM"].as<std::string>();
						break;
					default:
						break;

					}
				}
			}
			return true;
		}
	};




	template<>
	struct convert<Fluxon::SceneNode> {
		static Node encode(const Fluxon::SceneNode& scene_node)
		{
			Node node;
			node["mName"] = scene_node.Name();
			node["idx"] = scene_node.GetIdx();
			node["mParent"] = scene_node.GetParentIdx();

			Node children_node = Node(NodeType::Sequence);
			children_node = scene_node.GetChildren();
			children_node.SetStyle(EmitterStyle::Flow);

			node["Has Children"] = (scene_node.GetChildren().size() > 0);
			node["mChildren"] = children_node;

			Node component_node;
			component_node["mRenderableComponentIdx"] = scene_node.GetRenderableComponentIdx();
			component_node["mTransformComponent"] = *scene_node.GetTransformComponent();
			
			node["Components"] = component_node;

			return node;
		}

		static bool decode(const Node& node, Fluxon::SceneNode& scene_node)
		{

			Fluxon::TransformComponent transform_comp; 
			
			std::optional<uint32_t> renderable_comp_idx;
			const Node yaml_component_node = node["Components"];
			if (yaml_component_node)
			{
				if (yaml_component_node["mRenderableComponentIdx"] &&
					yaml_component_node["mRenderableComponentIdx"].as<int>() != -1)
					renderable_comp_idx = (uint32_t)yaml_component_node["mRenderableComponentIdx"].as<int>();


				if (yaml_component_node["mTransformComponent"])
					transform_comp = yaml_component_node["mTransformComponent"].as<Fluxon::TransformComponent>();
			}
			scene_node = Fluxon::SceneNode(node["mName"].as<std::string>(),
										transform_comp.transform, 
											node["idx"].as<int>());
			scene_node.SetParentIdx((uint32_t)node["mParent"].as<int>());
			if(renderable_comp_idx.has_value())
				scene_node.AttachRenderable(renderable_comp_idx.value());

			if (node["Has Children"])
			{
				std::vector<uint32_t> children = node["mChildren"].as<std::vector<uint32_t>>();
				for (uint32_t c : children)
					scene_node.AddChild(c);
			}

			return true;
		}
	};


	template<>
	struct convert<Fluxon::GPUStructures::DirectionalLight>
	{
		static Node encode(const Fluxon::GPUStructures::DirectionalLight& light_data)
		{
			Node node;
			node["direction"] =  light_data.direction;
			//node["enable"] =  light_data.enable;
			node["enable"] =  static_cast<bool>(light_data.enable);
			node["colour"] =  light_data.colour;
			node["shinness"] =  light_data.shinness;
			return node;
		}

		static bool decode(const Node& node, Fluxon::GPUStructures::DirectionalLight& light_data)
		{
			if (node.size() != 4)
				return false;
			light_data.direction = node["direction"].as<glm::vec3>();
			light_data.enable = static_cast<int>(node["enable"].as<bool>()); 
			light_data.colour = node["colour"].as<glm::vec3>();
			light_data.shinness = node["shinness"].as<float>();
			return true;
		}
	};

	//template<>
	////do not do scene but per scene nodes is find
	//do not do scene but per scene nodes is find
	//class convert<Fluxon::Scene> {
	//	static bool decode(const Node& node, Fluxon::Scene& scene) {

	//		node["mName"].as<std::string>();

	//		//int scene_node_count = node["SceneNodes"].as<Fluxon::SceneNodes>()
	//		scene_nodes = node["SceneNodes"].as<Fluxon::SceneNodes>();

	//		int counts = scene_nodes["count"].as<int>()
	//	}
	//};
} //namespace YAML
namespace Fluxon {
	namespace YAMLConverter {

	} //YAMLConverter
} //namespcae Fluxon