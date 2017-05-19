#pragma once
#include <DirectXMath.h>
#include <vector>
#include <Windows.h>


void fillQuadVertexData(std::vector<XMFLOAT3>& positions, std::vector<XMFLOAT2>& uvs, std::vector<UINT>& indices) {
	
	positions.push_back({ -0.5f, 0.5f, 0 });
	positions.push_back({ .5f, -.5f, 0 });
	positions.push_back({ -.5f, -.5f, 0 });
	positions.push_back({ .5f, .5f, 0 });
	
	uvs.push_back({ 0, 1 });
	uvs.push_back({ 1, 0 });
	uvs.push_back({ 0, 0 });
	uvs.push_back({ 1, 1 });
	
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(0);
	indices.push_back(3);
	indices.push_back(1);

}
