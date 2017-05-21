#include "stdafx.h"
#include "model_import.h"
#include <d3d11.h>
#include <renderer.h>

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


bool importModel(const std::string& file, Model* model, Renderer& renderer) {
	std::vector<XMFLOAT3> pos;
	std::vector<XMFLOAT2> uvs;
	std::vector<XMFLOAT3> normals;
	std::vector<UINT> ind;
	importModel(file, pos, uvs, normals, ind);
	model->positions = pos;
	model->normals = normals;
	model->uvs = uvs;
	model->indices = ind;

	// pos buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(XMFLOAT3) * pos.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	renderer.getDevice()->CreateBuffer(&bd, NULL, &model->posBuffer);
	D3D11_MAPPED_SUBRESOURCE ms;
	renderer.getContext()->Map(model->posBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	memcpy(ms.pData, pos.data(), sizeof(XMFLOAT3) * pos.size());
	renderer.getContext()->Unmap(model->posBuffer, NULL);

	// uv buffer
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(XMFLOAT2) * uvs.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA uvData;
	uvData.pSysMem = uvs.data();
	uvData.SysMemPitch = 0;
	uvData.SysMemSlicePitch = 0;
	renderer.getDevice()->CreateBuffer(&bd, &uvData, &model->uvBuffer);

	// normal buffer
	
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(XMFLOAT3) * normals.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA normalsData;
	normalsData.pSysMem = normals.data();
	normalsData.SysMemPitch = 0;
	normalsData.SysMemSlicePitch = 0;
	renderer.getDevice()->CreateBuffer(&bd, &normalsData, &model->normalBuffer);

	// index buffer
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(unsigned int) * ind.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = ind.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	ID3D11Buffer* indexBuffer;
	renderer.getDevice()->CreateBuffer(&ibd, &indexData, &model->indexBuffer);

	return true;
}