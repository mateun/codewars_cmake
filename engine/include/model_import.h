#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include <string>
#include <vector>
#include "model.h"
#include "renderer.h"

using namespace DirectX;

struct ModelImportData {
    std::vector<XMFLOAT3>& positions;
    std::vector<XMFLOAT2>& uvs;
    std::vector<XMFLOAT3>& normals;
    std::vector<XMFLOAT2>& uvsFinal;
    std::vector<XMFLOAT3>& normalsFinal;
    std::vector<UINT>& indices;
};

bool importObjModel(const std::string& file, std::vector<XMFLOAT3>& positions,
	std::vector<XMFLOAT2>& uvs, std::vector<XMFLOAT3>& normals,
	std::vector<UINT>& indices);
bool importObjModel(const std::string& file, Model* model, Renderer& renderer);
char* readTextFile(const std::string& fileName);

	