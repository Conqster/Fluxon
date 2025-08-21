#include "SceneSerialisation.h"
#include "Resources/MaterialResource.h"

namespace Fluxon {

	void SceneSerialiser::SerialiseScene(const std::string& file_path, Scene& scene)
	{
		std::string dir;
		if (ValidateFileDirectory(file_path, dir))
			printf("Failed to validate file path..........\n");


		YAML::Emitter out;
		out << YAML::BeginMap;

		out << YAML::Key << "mName" << YAML::Value << scene.mName;

		bool has_camera = (scene.mSceneResources.camera);
		out << YAML::Key << "Has camera" << YAML::Value << has_camera;
		if (has_camera)
		{
			out << YAML::Key << "Main Camera";
			out << YAML::convert<EditorCamera>::encode(*scene.mSceneResources.camera);
		}

		bool has_dir_light = (scene.mSceneResources.directionalLight);
		out << YAML::Key << "Has direction light" << YAML::Value << has_dir_light;
		if (has_dir_light)
		{
			out << YAML::Key << "Directional light";
			out << YAML::convert<GPUStructures::DirectionalLight>::encode(*scene.mSceneResources.directionalLight);
		}

		SerialiseSceneNodes(out, scene.GetRootNode(), scene.GetAllocatedSceneNodes());
		SerialiseRenderableComponents(out, scene.GetAllocatedRenderableComponent());
		SerialiseMeshResources(out, scene.GetAllocatedMeshResources());
		SerialiseMaterialResources(out, scene.GetAllocatedMaterialResources());

		out << YAML::Key << "Experimental Matrix";
		out << glm::mat4(1.0f);

		out << YAML::EndMap;

		std::ofstream fout(file_path);
		fout << out.c_str();
	}





	void SceneSerialiser::SerialiseSceneNodes(YAML::Emitter& out, const SceneNode* root, const std::vector<SceneNode*>& flat_nodes)
	{
		out << YAML::Key << "SceneNodes";
		out << YAML::BeginMap;
		out << YAML::Key << "count" << static_cast<uint32_t>(flat_nodes.size());
		for (auto& node : flat_nodes)
		{
			out << YAML::Key << node->GetIdx();
			out << YAML::convert<SceneNode>::encode(*node);
		}
		out << YAML::EndMap;
	}


	void SceneSerialiser::SerialiseRenderableComponents(YAML::Emitter& out, const std::vector<RenderableComponent*>& renderable_pool)
	{
		out << YAML::Key << "Renderable Components";
		//out << YAML::BeginMap;
		//out << YAML::Key << "count" << static_cast<uint32_t>(renderable_pool.size());
		//for (auto& node : renderable_pool)
		//	SerialiseRenderableComponent(out, *node);
		//out << YAML::EndMap;

		YAML::Node node;
		for (auto& renderable : renderable_pool)
			node.push_back(*renderable);

		out << node;
	}


	void SceneSerialiser::SerialiseMeshResources(YAML::Emitter& out, const std::vector<MeshResource*>& mesh_reso_pool)
	{
		out << YAML::Key << "Mesh Resources";


		//out << YAML::BeginMap;					//<----- Old
		//for (auto& mesh_reso : mesh_reso_pool)
		//	SerialiseMeshResource(out, mesh_reso->descriptor);
		//out << YAML::EndMap;


		YAML::Node node;
		for (auto& mesh_reso : mesh_reso_pool)
			node.push_back(mesh_reso->descriptor);

		out << node;
	}

	void SceneSerialiser::SerialiseMaterialResources(YAML::Emitter& out, const std::vector<MaterialResource*>& mat_resource_pool)
	{
		out << YAML::Key << "Material Resources";


		YAML::Node node;
		for (auto& mat_reso : mat_resource_pool)
			node.push_back(mat_reso->materialDesc);

		out << node;
	}



	void SceneSerialiser::LoadScene(Vulkan::VulkanContext* vk_ctx, VkCommandPool cmd_pool, const std::string& file_path, Scene& scene)
	{
		YAML::Node root_node = YAML::LoadFile(file_path);

		if (root_node["mName"])
			scene.mName = root_node["mName"].as<std::string>();

		//bug as thw pointe would be overwritten 
		if (root_node["Has camera"])
		{
			if (root_node["Has camera"].as<bool>())
			{
				auto& cam = scene.mSceneResources.camera;
				//YAML::convert<Camera>::decode(root_node["Main Camera"], *cam);
				*cam = root_node["Main Camera"].as<EditorCamera>();
			}
		}

		if (root_node["Has direction light"])
		{
			if (root_node["Has direction light"].as<bool>())
			{
				auto& light = scene.mSceneResources.directionalLight;
				YAML::convert<GPUStructures::DirectionalLight>::decode(root_node["Directional light"], *light);
				//*light = root_node["Directional light"].as<GPUStructures::DirectionalLight>();
			}
		}


		if (root_node["Mesh Resources"])
			DeserialiseMeshResources(vk_ctx, cmd_pool, root_node["Mesh Resources"], scene);

		if(root_node["Material Resources"])
			DeserialiseMaterialResources(vk_ctx, cmd_pool, root_node["Material Resources"], scene);


		if (root_node["SceneNodes"])
			DeserialiseSceneNodes(root_node["SceneNodes"], scene);
		//renderable component depends on mesh & material 
		// and scene node {for transformation}
		//for ref pointer
		if (root_node["Renderable Components"])
			DeserialiseRenderableComponents(root_node["Renderable Components"], scene);


		return;
	}



	void SceneSerialiser::DeserialiseSceneNodes(const YAML::Node& yaml_scene_node, Scene& scene)
	{
		int count = yaml_scene_node["count"].as<int>();

		for (int i = 0; i < count; i++)
		{
			SceneNode temp_node = yaml_scene_node[i].as<SceneNode>();
			Scene::CreateNewNode(scene, temp_node.GetParentIdx(), temp_node);
		}

		SceneNode try_root_node = yaml_scene_node[0].as<SceneNode>();
		if (try_root_node.GetParentIdx() == -1 && try_root_node.Name() == "Root-Node")
			scene.SetRootNode(&scene.GetSceneNodeByIdx(try_root_node.GetIdx()));
	}

	void SceneSerialiser::DeserialiseRenderableComponents(const YAML::Node& yaml_renderable_node, Scene& scene)
	{
		std::vector<RenderableComponent> components = yaml_renderable_node.as<std::vector<RenderableComponent>>();

		for (auto& render_comp : components)
			Scene::CreateNewRenderableComponent(scene, render_comp);
	}

	void SceneSerialiser::DeserialiseMeshResources(Vulkan::VulkanContext* vk_ctx, VkCommandPool cmd_pool, const YAML::Node& yaml_mesh_reso_node, Scene& scene)
	{
		std::vector<MeshDescriptor> mesh_resources = yaml_mesh_reso_node.as<std::vector<MeshDescriptor>>();


		for (auto& mesh_reso : mesh_resources)
			Scene::CreateMesh(scene, vk_ctx, cmd_pool, mesh_reso.primitiveType, mesh_reso.modelPath);

	}

	void SceneSerialiser::DeserialiseMaterialResources(Vulkan::VulkanContext* vk_ctx, VkCommandPool cmd_pool, const YAML::Node& yaml_mesh_reso_node, Scene& scene)
	{
		std::vector<MaterialDescriptor> mat_resources = yaml_mesh_reso_node.as<std::vector<MaterialDescriptor>>();


		for (auto& mat_reso : mat_resources)
		{
			uint32_t idx = Scene::CreateNewMaterial(scene, nullptr, vk_ctx, cmd_pool, mat_reso.pbrPaths, mat_reso.format, mat_reso.anisotropicSampling);
			//MaterialResource me = MaterialResource::CreatePBRMaterialResource(vk_ctx, cmd_pool, scene.GetMaterialByIdx(idx));
		}
	}

} //namespace Fluxon