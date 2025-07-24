#pragma once

#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>

namespace Fluxon {
	namespace GPUStructures {
		struct Vertex;
	}
	//#include "../GPUStrutures.h"


	struct LoaderMeshVertices
	{
		std::vector<GPUStructures::Vertex> vertices;
		std::vector<uint16_t> indices;
	};

	struct MeshData
	{
		std::vector<glm::vec4> vertices;
		std::vector<uint16_t> indices;
	};



	class ModelLoader
	{
	public:
		ModelLoader() = default;

		using MeshVerticesData = std::tuple<std::vector<GPUStructures::Vertex>, std::vector<uint16_t>>;
		MeshVerticesData LoadAsVerticesData(const std::string& path, bool flip_uv = false, bool bake_transform = false);

		//RenderableMesh LoadAsSingleMesh(const std::string& path, bool flip_uv = false);
		//std::vector<LoadedMesh> LoadAsCollectionMeshes(const std::string& path, bool flip_uv = false, bool bake_transform = false);

		//MeshData LoadOnlyVertexPosAndIndices(const std::string& path, bool flip_uv, bool bake_transform);
	private:
		std::string mCurrModelDir = "";

		MeshVerticesData ProcessNodeSingleMesh(aiNode* node, const aiScene* scene, const aiMatrix4x4 parent_tranform, bool bake_transform);
		MeshVerticesData ProcessMesh(aiMesh* mesh, const aiScene* scene, const aiMatrix4x4 transform);

		//Helpers
		std::vector<GPUStructures::Vertex> CalcAverageNormalsWcIndices(std::vector<GPUStructures::Vertex>& vertices, std::vector<uint16_t> indices);

		//glm::mat4 AiMatrixToGLM(const aiMatrix4x4& from) const;
		//std::vector<LoadedMesh> ProcessNode(aiNode* node, const aiScene* scene, const aiMatrix4x4 parent_tranform, bool bake_transform);


		//MeshData ProcessNodePositionAndIndices(aiNode* node, const aiScene* scene, const aiMatrix4x4 parent_tranform, bool bake_transform);
		//MeshData ProcessMeshPositionAndIndices(aiMesh* mesh, const aiScene* scene, const aiMatrix4x4 transform);

	};
}//#Fluxon