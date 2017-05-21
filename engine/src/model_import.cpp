#include "stdafx.h"
#include "model_import.h"

using namespace DirectX;

bool importModel(const std::string& file,
	std::vector<XMFLOAT3>& positions,
	std::vector<XMFLOAT2>& uvs,
	std::vector<XMFLOAT3>& normals,
	std::vector<UINT>& indices) {
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(file, aiProcess_Triangulate /*| aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder /*aiProcess_FlipUVs */ | aiProcess_JoinIdenticalVertices);
	if (!scene) {
		OutputDebugString("model import failed!\n");
		exit(1);
	}

	unsigned int numMeshes = scene->mNumMeshes;
	for (int i = 0; i < numMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[i];
		for (int v = 0; v < mesh->mNumVertices; ++v) {
			aiVector3D vertex = mesh->mVertices[v];
			aiVector3D normal = mesh->mNormals[v];
			aiVector3D texcoord = mesh->mTextureCoords[0][v];

			positions.push_back({ vertex.x, vertex.y, vertex.z });
			normals.push_back({ normal.x, normal.y, normal.z });
			uvs.push_back({ texcoord.x, texcoord.y });

		}

		for (int f = 0; f < mesh->mNumFaces; ++f) {
			aiFace face = mesh->mFaces[f];
			indices.push_back({ face.mIndices[0] });
			indices.push_back({ face.mIndices[1] });
			indices.push_back({ face.mIndices[2] });
		}
	}
}


bool importModel(const std::string& file, Model* model) {
	std::vector<XMFLOAT3> pos;
	std::vector<XMFLOAT2> uvs;
	std::vector<XMFLOAT3> normals;
	std::vector<UINT> ind;
	importModel(file, pos, uvs, normals, ind);
	model->positions = pos;
	model->normals = normals;
	model->uvs = uvs;
	model->indices = ind;

	return true;
}