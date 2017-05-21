#pragma once
#include <DirectXMath.h>
#include <string>
#include <vector>
#include <resource_management.h>

using namespace DirectX;

struct Model {


	~Model() {
		safeRelease(&posBuffer);
		safeRelease(&uvBuffer);
		safeRelease(&normalBuffer);
		safeRelease(&indexBuffer);
	}
	std::vector<XMFLOAT3> positions;
	std::vector<XMFLOAT3> normals;
	std::vector<XMFLOAT2> uvs;
	std::vector<unsigned int> indices;

	ID3D11Buffer* posBuffer;
	ID3D11Buffer* uvBuffer;
	ID3D11Buffer* normalBuffer;
	ID3D11Buffer* indexBuffer;

};
