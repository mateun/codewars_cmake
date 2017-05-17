#include <stdafx.h>
#include "spacefight.h"
#include "model_import.h"
#include "textures.h"
#include "shaders.h"
#include <d3dcompiler.h>
#include "model.h"
#include <resource_management.h>
#include <dinput.h>

static Spacefight spacefight;

Game* GetGame() {
	return &spacefight;
}

std::string GetIntroImageName() {
	return "games/spacefight/textures/spacefight_intro.png";
}

void Spacefight::DoFrame(Renderer& renderer, FrameInput* input) {

	if (input->keyState[DIK_A]) {
		OutputDebugString("A pressed!\n");
	}

	if (input->mouse1Down) OutputDebugString("mousebutton1 pressed!\n");
	if (input->mouse1Up) OutputDebugString("mousebutton1 up\n");

	if (input->mouse2Down) OutputDebugString("mousebutton2 pressed!\n");
	if (input->mouse2Up) OutputDebugString("mousebutton2 up\n");

	
		// track the movement
	if (input->relMouseMovX != 0) {
		char buf[500];
		sprintf_s(buf, 500, "mouse X__: %d\n", input->relMouseMovX);
		OutputDebugString(buf);
	}
	
	
	_menuShipRot = 0.00002f;
	XMFLOAT3 zAxis = XMFLOAT3(1, 0, 1);
	XMFLOAT3 yAxis = XMFLOAT3(0, 1, 0);
	XMMATRIX rotMatZ = DirectX::XMMatrixRotationAxis(XMLoadFloat3(&zAxis), _menuShipRot*4.0f);
	XMMATRIX rotMatY = DirectX::XMMatrixRotationAxis(XMLoadFloat3(&yAxis), _menuShipRot*2.0f);
	_modelMat = XMMatrixMultiply(_modelMat, rotMatZ);
	_modelMat = XMMatrixMultiply(_modelMat, rotMatY);

	renderer.clearBackbuffer(clearColors);
	renderer.setViewport(0, 0, 800, 600);
	renderer.renderMesh(_shipModel->positions, _shipModel->uvs, _shipModel->indices, _modelMat, _viewMat, _projMat, _vs, _ps, _inputLayout, _shipTexture);
	
	// Draw a start button
	XMMATRIX modelMat = DirectX::XMMatrixIdentity();
	XMMATRIX scaleMat = DirectX::XMMatrixScaling(0.7f, 0.7f, 0.7f);
	// Move the button to the lower left corner
	XMMATRIX transMat = DirectX::XMMatrixTranslation(2.2f, -1.95f, 0.0f);	
	modelMat = XMMatrixTranspose(XMMatrixMultiply(transMat, scaleMat));
	XMFLOAT3 eyePosS = XMFLOAT3(0, 0, -2);
	XMFLOAT3 eyeDirS = XMFLOAT3(0, 0, 1);
	XMFLOAT3 upDirS = XMFLOAT3(0, 1, 0);
	XMMATRIX viewMatS = XMMatrixTranspose(XMMatrixLookToLH(XMLoadFloat3(&eyePosS), XMLoadFloat3(&eyeDirS), XMLoadFloat3(&upDirS)));
	
	XMMATRIX projMatSplash = XMMatrixTranspose(XMMatrixOrthographicLH(5.0f, 5.0f, 0.1f, 100.0f));

	float clearColors[] = { 0.01f, 0.02f, 0.02f, 1.0f };
	std::vector<XMFLOAT3> mesh;
	mesh.push_back({ -0.5f, 0.5f, 0 });
	mesh.push_back({ .5f, -.5f, 0 });
	mesh.push_back({ -.5f, -.5f, 0 });
	mesh.push_back({ .5f, .5f, 0 });
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

	renderer.renderMesh(mesh, uvs, indices, modelMat, viewMatS, projMatSplash, _vs, _ps, _inputLayout, _startButtonTex);
	renderer.presentBackBuffer();

}

Spacefight::Spacefight() : clearColors{ 0.01f, 0.02f, 0.02f, 1.0f } {

}

Spacefight::~Spacefight() {
	ShutDown();
}

void Spacefight::Init(Renderer& renderer) {
	
	// load ship model from filesystem
	std::vector<XMFLOAT3> imp_pos;
	std::vector<XMFLOAT2> imp_uvs;
	std::vector<UINT> imp_indices;
	_shipModel = new Model();
	importModel("models/corvette1.obj", _shipModel);
	loadTextureFromFile("textures/SF_Corvette-F3_diffuse.jpg", &_shipTexture, &renderer);
	loadTextureFromFile("games/spacefight/textures/btn_start.png", &_startButtonTex, &renderer);

	XMFLOAT3 eyePos = XMFLOAT3(0, 0, -55);
	XMFLOAT3 eyeDir = XMFLOAT3(0, 0, 1);
	XMFLOAT3 upDir = XMFLOAT3(0, 1, 0);
	_modelMat = DirectX::XMMatrixScaling(1.5, 1.5, 1.5);
	_viewMat = DirectX::XMMatrixLookToLH(XMLoadFloat3(&eyePos), XMLoadFloat3(&eyeDir), XMLoadFloat3(&upDir));
	_projMat = DirectX::XMMatrixPerspectiveFovLH(0.45f, 4.0f / 3.0f, 0.1f, 100.0f);
	_modelMat = XMMatrixTranspose(_modelMat);
	_viewMat = XMMatrixTranspose(_viewMat);
	_projMat = XMMatrixTranspose(_projMat);


	/// SHADER SETUP
	ID3DBlob* vs = nullptr;
	ID3DBlob* errBlob = nullptr;
	HRESULT res = D3DCompileFromFile(L"shaders/basic.hlsl", NULL, NULL, "VShader", "vs_5_0", 0, 0, &vs, &errBlob);
	if (FAILED(res)) {
		OutputDebugStringW(L"shader load failed\n");
		if (errBlob)
		{
			OutputDebugStringA((char*)errBlob->GetBufferPointer());
			errBlob->Release();
		}

		if (vs)
			vs->Release();

		exit(1);
	}
	ID3DBlob* ps = nullptr;
	res = D3DCompileFromFile(L"shaders/basic.hlsl", NULL, NULL, "PShader", "ps_5_0", 0, 0, &ps, &errBlob);
	if (FAILED(res)) {
		OutputDebugStringW(L"shader load failed\n");
		if (errBlob)
		{
			OutputDebugStringA((char*)errBlob->GetBufferPointer());
			errBlob->Release();
		}

		if (ps)
			ps->Release();

		exit(1);
	}
	
	CreateVertexShader(renderer.getDevice(), vs, &_vs);
	CreatePixelShader(renderer.getDevice(), ps, &_ps);
	
	/// END SHADER SETUP

	/// INPUT LAYOUT SETUP
	D3D11_INPUT_ELEMENT_DESC ied[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },	// same slot, but 12 bytes after the pos
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },		// other slot (buffer), starting at 0

	};
	res = const_cast<ID3D11Device*>(renderer.getDevice())->CreateInputLayout(ied, 2, vs->GetBufferPointer(), vs->GetBufferSize(), &_inputLayout);
	if (FAILED(res)) {
		OutputDebugStringW(L"Failed to create input layout\n");
		exit(1);
	}

	ps->Release();
	vs->Release();
	/// END INPUT LAYOUT SETUP

	
}

void Spacefight::ShutDown() {
	safeRelease(&_vs);
	safeRelease(&_ps);
	safeRelease(&_shipTexture);
	safeRelease(&_inputLayout);
	safeRelease(&_startButtonTex);
	if (_shipModel) {
		delete(_shipModel); _shipModel = nullptr;
	}
}