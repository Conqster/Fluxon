#pragma once

#include <string>
#include "Core/Scene/Scene.h"

#include <yaml-cpp/yaml.h>

#include <fstream>

#include <filesystem>
#include "Core/Utilities.h"

#include "YAMLConverters.h"

//utilties
//namespace YAML {
//
//	template<>
//	struct convert<glm::vec3>
//	{
//		static Node encode(const glm::vec3& rhs)
//		{
//			Node node;
//			node.push_back(rhs.x);
//			node.push_back(rhs.y);
//			node.push_back(rhs.z);
//			return node;
//		}
//
//		static bool decode(const Node& node, glm::vec3& rhs)
//		{
//			if (!node.IsSequence() || node.size() != 3)
//				return false;
//
//			rhs.x = node[0].as<double>();
//			rhs.y = node[1].as<double>();
//			rhs.z = node[2].as<double>();
//			return true;
//		}
//	};
//} //#YAML

inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
{
	out << YAML::Flow;
	//out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
	out << YAML::BeginMap;
		out << YAML::Key << "x" << v.x;
		out << YAML::Key << "y" << v.y;
		out << YAML::Key << "z" << v.z ;
	out << YAML::EndMap;
	return out;
}


inline YAML::Emitter& operator<<(YAML::Emitter& out, const glm::mat4& m) {

	out << YAML::BeginMap;
	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++)
			out << YAML::Key << "m"+ std::to_string(i) + std::to_string(j) << m[i][j];
	out << YAML::EndMap;

	return out;
}

inline YAML::Emitter& operator<<(YAML::Emitter& emitter, const Fluxon::TransformComponent transform_comp) {
	emitter << YAML::Key << "mTransformComponent";
	glm::vec3 translate, rotation, scale;
	Fluxon::Utils::DecomposeTransform(transform_comp.transform, translate, rotation, scale);
	emitter << YAML::BeginMap;
		emitter << YAML::Key << "mPosition" << translate;
		emitter << YAML::Key << "mRotation" << rotation;
		emitter << YAML::Key << "mScale" << scale;
	emitter << YAML::EndMap;
	return emitter;
}


namespace Fluxon {







	////have a base class that is Seraialiable that object would overrider to serial datas
	//static void SerialiseCameraData(YAML::Emitter& out, const Camera& camera)
	//{
	//	//YAML::Node node = YAML::convert<Camera>::encode(camera);
	//	out << YAML::Key << "Main Camera";
	//	out << YAML::convert<Camera>::encode(camera);;
	//	//out << YAML::BeginMap;
	//	//out << YAML::Key << "mPosition" << camera.mPosition;
	//	//out << YAML::Key << "forward" << camera.forward;
	//	//out << YAML::Key << "up" << camera.up;
	//	//out << YAML::Key << "mRight" << camera.mRight;
	//	//out << YAML::Key << "mFovY" << camera.mFovY;
	//	//out << YAML::Key << "mNearZ" << camera.mNearZ;
	//	//out << YAML::Key << "mFarZ" << camera.mFarZ;
	//	//out << YAML::Key << "moveSpeed" << camera.moveSpeed;
	//	//out << YAML::Key << "rotSpeed" << camera.rotSpeed;
	//	//out << YAML::EndMap;
	//}

	//static void SerialiseLightData(YAML::Emitter& out, const GPUStructures::DirectionalLight& light_data)
	//{
	//	//direction light 
	//	out << YAML::Key << "Directional light";
	//	out << YAML::convert<GPUStructures::DirectionalLight>::encode(light_data);
	//	//out << YAML::BeginMap;
	//	//out << YAML::Key << "direction" << light_data.direction;
	//	//out << YAML::Key << "enable" << light_data.enable;
	//	////out << YAML::Key << "enable" << YAML::Flow << light_data.enable;
	//	//out << YAML::Key << "colour" << light_data.colour;
	//	//out << YAML::Key << "shinnes" << light_data.shinness;
	//	//out << YAML::EndMap;

	//}



	//static void SerialiseSceneNode(YAML::Emitter& out, const SceneNode& node)
	//{
	//	//std::string map_key_name = node.Name() + std::to_string(node.GetIdx());
	//	out << YAML::Key << node.GetIdx();
	//	out << YAML::convert<SceneNode>::encode(node);
	//	//out << YAML::BeginMap;
	//	//	out << YAML::Key << "mName" << node.Name();
	//	//	out << YAML::Key << "idx" << node.GetIdx();
	//	//	out << YAML::Key << "mParent" << node.GetParentIdx();
	//	//	out << YAML::Key << "Components";
	//	//	out << YAML::BeginMap;
	//	//		out << YAML::Key << "mRenderableComponentIdx" << node.GetRenderableComponentIdx();
	//	//		out << *node.GetTransformComponent();
	//	//	out << YAML::EndMap;
	//	//	out << YAML::Key << "Has Children" << (node.GetChildren().size() > 0);
	//	//	out << YAML::Key << "mChildren";
	//	//	out << YAML::Flow;
	//	//	//out << YAML::BeginSeq;
	//	//	//for (auto& c : node.GetChildren())
	//	//	//	out << c;
	//	//	//out << YAML::EndSeq;
	//	//	out << node.GetChildren();
	//	//out << YAML::EndMap;
	//}

	//static void SerialiseSceneNodes(YAML::Emitter& out, const SceneNode* root, const std::vector<SceneNode*>& flat_nodes)
	//{
	//	out << YAML::Key << "SceneNodes";
	//	out << YAML::BeginMap;
	//		out << YAML::Key << "count" << static_cast<uint32_t>(flat_nodes.size());
	//		for (auto& node : flat_nodes)
	//		{
	//			//out << SerialiseSceneNode(node)
	//			SerialiseSceneNode(out, *node);
	//		}
	//	out << YAML::EndMap;
	//}


	//static void SerialiseRenderableComponent(YAML::Emitter& out, const RenderableComponent& renderable_comp)
	//{
	//	out << YAML::Key << renderable_comp.id;
	//	out << YAML::convert<RenderableComponent>::encode(renderable_comp);
	//	//out << YAML::BeginMap;
	//	//	out << YAML::Key << "id" << renderable_comp.id;
	//	//	out << YAML::Key << "mesh" << (renderable_comp.meshIdx.has_value() ? (int)renderable_comp.meshIdx.value() : -1);
	//	//	out << YAML::Key << "material" << (renderable_comp.materialIdx.has_value() ? (int)renderable_comp.materialIdx.value() : -1);
	//	//	out << YAML::Key << "visible" << renderable_comp.visible;
	//	//	out << YAML::Key << "pipelineGlobalBucketIdx" << static_cast<int>(renderable_comp.pipelineGlobalBucketIdx);
	//	//	out << YAML::Key << "descSetSpecificationGlobalBucketIdx" << static_cast<int>(renderable_comp.descSetSpecificationGlobalBucketIdx);
	//	//out << YAML::EndMap;
	//}

	//static void SerialiseRenderableComponents(YAML::Emitter& out, const std::vector<RenderableComponent*>& renderable_pool)
	//{
	//	out << YAML::Key << "Renderable Components";
	//	//out << YAML::BeginMap;
	//	//out << YAML::Key << "count" << static_cast<uint32_t>(renderable_pool.size());
	//	//for (auto& node : renderable_pool)
	//	//	SerialiseRenderableComponent(out, *node);
	//	//out << YAML::EndMap;

	//	YAML::Node node;
	//	for (auto& renderable : renderable_pool)
	//		node.push_back(*renderable);

	//	out << node;
	//}

	//static void SerialiseMeshResource(YAML::Emitter& out, const MeshDescriptor& mesh_reso_descriptor)
	//{
	//	out << YAML::Key << mesh_reso_descriptor.id;
	//	out << YAML::convert<Fluxon::MeshDescriptor>::encode(mesh_reso_descriptor);
	//	//out << YAML::BeginMap;
	//	//out << YAML::Key << "id" << mesh_reso_descriptor.id;
	//	//out << YAML::Key << "type" << static_cast<uint32_t>(mesh_reso_descriptor.type);
	//	////out << YAML::Key << "primitiveType" << static_cast<uint32_t>(mesh_reso_descriptor.primitiveType);
	//	//out << YAML::Key << "primitiveType" << YAML::convert<Fluxon::EPrimitiveMeshType>::encode(mesh_reso_descriptor.primitiveType);
	//	//if (mesh_reso_descriptor.primitiveType == EPrimitiveMeshType::EPMT_ModelMesh)
	//	//	out << YAML::Key << "model path" << mesh_reso_descriptor.modelPath;
	//	//out << YAML::EndMap;
	//}

	//static void SerialiseMeshResources(YAML::Emitter& out, const std::vector<MeshResource*>& mesh_reso_pool)
	//{
	//	out << YAML::Key << "Mesh Resources";
	//	
	//	
	//	//out << YAML::BeginMap;					//<----- Old
	//	//for (auto& mesh_reso : mesh_reso_pool)
	//	//	SerialiseMeshResource(out, mesh_reso->descriptor);
	//	//out << YAML::EndMap;


	//	YAML::Node node;
	//	for (auto& mesh_reso : mesh_reso_pool)
	//		node.push_back(mesh_reso->descriptor);

	//	out << node;


	//}

	//static void SerialiseScene(const std::string& file, Scene& scene)
	//{
	//	std::string dir = file.substr(0, file.find_last_of('/'));

	//	if (!std::filesystem::exists(dir.c_str()))
	//		if (std::filesystem::create_directories(dir.c_str()))
	//			printf("Created file directory\n");

	//	YAML::Emitter out;
	//	out << YAML::BeginMap;

	//	out << YAML::Key << "mName" << YAML::Value << scene.mName;

	//	bool has_camera = (scene.mSceneResources.camera);
	//	out << YAML::Key << "Has camera" << YAML::Value << has_camera;
	//	if (has_camera)
	//		SerialiseCameraData(out, *scene.mSceneResources.camera);

	//	bool has_dir_light = (scene.mSceneResources.directionalLight);
	//	out << YAML::Key << "Has direction light" << YAML::Value << has_dir_light;
	//	if (has_dir_light)
	//		SerialiseLightData(out, *scene.mSceneResources.directionalLight);

	//	SerialiseSceneNodes(out, scene.GetRootNode(), scene.GetAllocatedSceneNodes());
	//	SerialiseRenderableComponents(out, scene.GetAllocatedRenderableComponent());
	//	SerialiseMeshResources(out, scene.GetAllocatedMeshResources());

	//	out << YAML::Key << "Experimental Matrix";
	//	out << glm::mat4(1.0f);

	//	out << YAML::EndMap;

	//	std::ofstream fout(file);
	//	fout << out.c_str();
	//}



	static bool ValidateFileDirectory(const std::string& file, std::string& out_dir)
	{
		out_dir = file.substr(0, file.find_last_of('/'));

		if (!std::filesystem::exists(out_dir.c_str()))
			if (std::filesystem::create_directories(out_dir.c_str()))
				return true;

		return false;
	}


	struct MaterialResource;
	struct SceneSerialiser
	{
		void SerialiseScene(const std::string& file_path, Scene& scene);

		void LoadScene(Vulkan::VulkanContext* vk_ctx, VkCommandPool cmd_pool, const std::string& file_path, Scene& scene);

	private:
		//have a base class that is Seraialiable that object would overrider to serial datas
		//void SerialiseCameraData(YAML::Emitter& out, const Camera& camera);
		//void SerialiseLightData(YAML::Emitter& out, const GPUStructures::DirectionalLight& light_data);
		//void SerialiseSceneNode(YAML::Emitter& out, const SceneNode& node);
		void SerialiseSceneNodes(YAML::Emitter& out, const SceneNode* root, const std::vector<SceneNode*>& flat_nodes);
		//void SerialiseRenderableComponent(YAML::Emitter& out, const RenderableComponent& renderable_comp);
		void SerialiseRenderableComponents(YAML::Emitter& out, const std::vector<RenderableComponent*>& renderable_pool);
		//void SerialiseMeshResource(YAML::Emitter& out, const MeshDescriptor& mesh_reso_descriptor);
		void SerialiseMeshResources(YAML::Emitter& out, const std::vector<MeshResource*>& mesh_reso_pool);
		void SerialiseMaterialResources(YAML::Emitter& out, const std::vector<MaterialResource*>& mat_resource_pool);


		void DeserialiseSceneNodes(const YAML::Node& yaml_scene_node, Scene& scene);
		void DeserialiseRenderableComponents(const YAML::Node& yaml_renderable_node, Scene& scene);
		void DeserialiseMeshResources(Vulkan::VulkanContext* vk_ctx, VkCommandPool cmd_pool, const YAML::Node& yaml_mesh_reso_node, Scene& scene);
		void DeserialiseMaterialResources(Vulkan::VulkanContext* vk_ctx, VkCommandPool cmd_pool, const YAML::Node& yaml_mesh_reso_node, Scene& scene);

	};

} //namespace Fluxon