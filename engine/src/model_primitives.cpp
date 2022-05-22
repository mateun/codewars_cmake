#include "stdafx.h"
#include <model.h>
#include <renderer.h>

void createPlaneModel(Model* model, Renderer& renderer) {

	std::vector<XMFLOAT3> mesh;
	mesh.push_back({ -.5, 0.5, 0 });
	mesh.push_back({ .5, -.5, 0 });
	mesh.push_back({ -.5, -.5, 0 });
	mesh.push_back({ .5, .5, 0 });
	std::vector<XMFLOAT3> normals;
	normals.push_back({ 0, 0, -1 });
	normals.push_back({ 0, 0, -1 });
	normals.push_back({ 0, 0, -1 });
	normals.push_back({ 0, 0, -1 });
	std::vector<XMFLOAT2> uvs;
	uvs.push_back({ 0, 1 });
	uvs.push_back({ 1, 0 });
	uvs.push_back({ 0, 0 });
	uvs.push_back({ 1, 1 });
	std::vector<UINT> indices;
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(0);
	indices.push_back(3);
	indices.push_back(1);


	model->positions = mesh;
	model->normals = normals;
	model->uvs = uvs;
	model->indices = indices;

	// pos buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(XMFLOAT3) * mesh.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	renderer.getDevice()->CreateBuffer(&bd, NULL, &model->posBuffer);
	D3D11_MAPPED_SUBRESOURCE ms;
	renderer.getContext()->Map(model->posBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	memcpy(ms.pData, mesh.data(), sizeof(XMFLOAT3) * mesh.size());
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
	ibd.ByteWidth = sizeof(unsigned int) * indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	ID3D11Buffer* indexBuffer;
	renderer.getDevice()->CreateBuffer(&ibd, &indexData, &model->indexBuffer);
}

Model *createFromRawData(std::vector<XMFLOAT3> positions,  std::vector<XMFLOAT2> uvs, std::vector<XMFLOAT3> normals,
                         std::vector<unsigned int> indices, Renderer& renderer) {

    auto model = new Model();

    model->positions = positions;
    model->normals = normals;
    model->uvs = uvs;
    model->indices = indices;

    // pos buffer
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(XMFLOAT3) * positions.size();
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    renderer.getDevice()->CreateBuffer(&bd, NULL, &model->posBuffer);
    D3D11_MAPPED_SUBRESOURCE ms;
    renderer.getContext()->Map(model->posBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
    memcpy(ms.pData, positions.data(), sizeof(XMFLOAT3) * positions.size());
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
    ibd.ByteWidth = sizeof(unsigned int) * indices.size();
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA indexData;
    indexData.pSysMem = indices.data();
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;
    ID3D11Buffer* indexBuffer;
    renderer.getDevice()->CreateBuffer(&ibd, &indexData, &model->indexBuffer);

    return model;
}
