#pragma once
#include <DirectXMath.h>
#include <vector>
#include <Windows.h>


/**
	Fills the position, uv and index data of a unit quad into the given vector references. 
*/
void fillQuadVertexData(std::vector<XMFLOAT3>& positions, std::vector<XMFLOAT2>& uvs, std::vector<XMFLOAT3>& normals, std::vector<UINT>& indices) {
	
	positions.push_back({ -0.5f, 0.5f, 0 });
	positions.push_back({ .5f, -.5f, 0 });
	positions.push_back({ -.5f, -.5f, 0 });
	positions.push_back({ .5f, .5f, 0 });

	normals.push_back({ 0, 0, -1 });
	normals.push_back({ 0, 0, -1 });
	normals.push_back({ 0, 0, -1 });
	normals.push_back({ 0, 0, -1 });
	
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
