#include <stdafx.h>
#include "spacefight.h"
#include "model_import.h"
#include "textures.h"
#include "shaders.h"
#include <d3dcompiler.h>
#include <resource_management.h>
#include <dinput.h>
#include "model.h"
#include "primitives.h"
#include <chrono>

static Spacefight spacefight;

Game* GetGame() {
	return &spacefight;
}

std::string GetIntroImageName() {
	return "games/spacefight/textures/spacefight_intro.png";
}



void Spacefight::DoFrame(Renderer& renderer, FrameInput* input, long long frameTime) {

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
	
	
	_menuShipRot += 0.0007f;
	_modelMat = XMMatrixIdentity();
	
	_sineUpDown = sin(_menuShipRot)* 0.5f;
	
	XMFLOAT3 zAxis = XMFLOAT3(1, 0, 1);
	XMFLOAT3 yAxis = XMFLOAT3(0, 1, 0);
	XMFLOAT3 xAxis = XMFLOAT3(1, 0, 0);
	XMMATRIX rotMatZ = DirectX::XMMatrixRotationAxis(XMLoadFloat3(&zAxis), _menuShipRot*4.0f);
	XMMATRIX rotMatY = DirectX::XMMatrixRotationAxis(XMLoadFloat3(&yAxis), _menuShipRot*0.0f);
	XMMATRIX transZ = DirectX::XMMatrixTranslation(0, 0, _sineUpDown - 10.0f);
	//_modelMat = XMMatrixMultiply(_modelMat, rotMatZ);
	_modelMat = XMMatrixMultiply(_modelMat, rotMatY);
	_modelMat = XMMatrixTranspose(XMMatrixMultiply(_modelMat, transZ));

	renderer.clearBackbuffer(clearColors);
	renderer.setViewport(0, 0, GAMEWIDTH, GAMEHEIGHT);
	//renderer.renderMesh(_shipModel->positions, _shipModel->uvs, _shipModel->indices, _modelMat, _viewMat, _projMat, _vs, _ps, _inputLayout, _shipTexture);
	//renderer.renderMesh(_cardModel->positions, _cardModel->uvs, _cardModel->normals, _cardModel->indices, _modelMat, _viewMat, _projMat, _vs, _ps, _inputLayout, _anjaniTex);

	// play table
	XMMATRIX tableModelMat = XMMatrixRotationAxis(XMLoadFloat3(&xAxis), 1.2f);
	tableModelMat = XMMatrixTranspose(XMMatrixMultiply(tableModelMat, XMMatrixScaling(2.0f, 1.0f, 2.0f)));
	tableModelMat = XMMatrixTranspose(XMMatrixMultiply(tableModelMat, XMMatrixTranslation(0, 0, -5)));
	//renderer.renderMesh(_playTable->positions, _playTable->uvs, _playTable->normals, _playTable->indices, tableModelMat, _viewMat, _projMat, _vs, _ps, _inputLayout, _metalTex);

	// Draw the hex field
	XMMATRIX hexModelMat = XMMatrixRotationAxis(XMLoadFloat3(&xAxis), -0.8f);
	
	if (input->keyState[DIK_W]) {
		_camMovZ += 0.02f * frameTime;
	}

	if (input->keyState[DIK_S]) {
		_camMovZ -= 0.02f * frameTime;
	}

	XMFLOAT3 eyePos = XMFLOAT3(0, 40, -35 + (_camMovZ));
	XMFLOAT3 eyeDir = XMFLOAT3(0, -0.75, 1);
	XMFLOAT3 upDir = XMFLOAT3(0, 1, 0);
	_viewMat = DirectX::XMMatrixTranspose( XMMatrixLookToLH(XMLoadFloat3(&eyePos), XMLoadFloat3(&eyeDir), XMLoadFloat3(&upDir)));
	
	auto start = std::chrono::high_resolution_clock::now();
	float xOffset = 0;
	for (int x = 0; x < 10; ++x) {
		for (int y = 0; y < 10; ++y) {
			if (y % 2 == 1) {
				xOffset = 1.0f;
			}
			else {
				xOffset = 0;
			}
			hexModelMat = XMMatrixIdentity();
			hexModelMat = XMMatrixTranspose(XMMatrixMultiply(hexModelMat, 
				XMMatrixTranslation((-9) + x * 1.9 + 1.4 + xOffset, 0, y*2 + 2)));
			renderer.renderModel(*_basicHex, hexModelMat, _viewMat, _projMat, 
									_vs, _ps, _inputLayout, *_hexTex);
		}
	}
	auto diff = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start);
	//char buf[200];
	//sprintf_s(buf, 200, "hexfield rendertime: %d\n", diff);
	//OutputDebugString(buf);

	// Draw a server
	XMMATRIX serverModelMat = XMMatrixIdentity();
	serverModelMat = XMMatrixTranspose(XMMatrixMultiply(serverModelMat, XMMatrixScaling(0.4f, 0.4f, 0.4f)));
	serverModelMat = XMMatrixTranspose(XMMatrixMultiply(serverModelMat,
		XMMatrixTranslation((1.75f), 0.35f, 6.1f)));

	renderer.renderModel(*_server, serverModelMat, _viewMat, _projMat, _vs, _ps, _inputLayout, *_serverTex);
	
	// Draw a start button
	XMMATRIX modelMat = DirectX::XMMatrixIdentity();
	XMMATRIX scaleMat = DirectX::XMMatrixScaling(0.7f, 0.7f, 0.7f);
	// Move the button to the lower left corner
	XMMATRIX transMat = DirectX::XMMatrixTranslation(2.2f, -1.95f, 0.0f);	
	modelMat = XMMatrixTranspose(XMMatrixMultiply(transMat, scaleMat));
	XMFLOAT3 eyePosS = XMFLOAT3(0, 0, -1);
	XMFLOAT3 eyeDirS = XMFLOAT3(0, 0, 1);
	XMFLOAT3 upDirS = XMFLOAT3(0, 1, 0);
	XMMATRIX viewMatS = XMMatrixTranspose(XMMatrixLookToLH(XMLoadFloat3(&eyePosS), XMLoadFloat3(&eyeDirS), XMLoadFloat3(&upDirS)));
	XMMATRIX projMatSplash = XMMatrixTranspose(XMMatrixOrthographicLH(5.0f, 5.0f, 0.1f, 100.0f));

	float clearColors[] = { 0.01f, 0.02f, 0.02f, 1.0f };
	std::vector<XMFLOAT3> mesh;
	std::vector<XMFLOAT3> normals;
	std::vector<XMFLOAT2> uvs;
	std::vector<UINT> indices;
	fillQuadVertexData(mesh, uvs, normals, indices);

	//renderer.renderMesh(mesh, uvs, normals, indices, modelMat, viewMatS, projMatSplash, _vs, _ps, _inputLayout, _startButtonTex);

	// RENDER TO TEXTURE
	// Render the whole image to a texture from above and display it
	ID3D11Texture2D* radarMapTex;
	ID3D11RenderTargetView* radarMapRTV;
	ID3D11ShaderResourceView* radarMapSRV;
	renderer.createRenderTargetTexture(400, 300, &radarMapTex);
	renderer.createRenderTargetViewForTexture(radarMapTex, &radarMapRTV);
	renderer.createRenderTargetShaderResourceViewForTexture(radarMapTex, &radarMapSRV);
	
	eyePos = XMFLOAT3(0, 75, 0);
	eyeDir = XMFLOAT3(0, -1, 0);
	upDir = XMFLOAT3(0, 0, 1);
	XMMATRIX mapView = XMMatrixTranspose(XMMatrixLookToLH(XMLoadFloat3(&eyePos), XMLoadFloat3(&eyeDir), XMLoadFloat3(&upDir)));
	renderer.setTextureRenderTarget(&radarMapRTV);
	float clearColorsTex[] = { 0.01f, 0.02f, 0.8f, 1.0f };
	renderer.clearTexture(clearColorsTex, radarMapRTV);
	renderer.setViewport(0, 0, 400, 300);
	//renderer.renderMesh(_shipModel->positions, _shipModel->uvs, _shipModel->normals, _shipModel->indices, _modelMat, mapView, _projMat, _vs, _ps, _inputLayout, _shipTexture);
	

	// After rendering the scene from birds eye view to our texture, 
	// render the texture onto a quad.
	// Move our map quad to the left of the screen.
	renderer.setBackBufferRenderTarget();
	transMat = DirectX::XMMatrixTranslation(-2.0f, -1.0f, 0.0f);
	scaleMat = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
	modelMat = XMMatrixTranspose(XMMatrixMultiply(transMat, scaleMat));
	renderer.setViewport(0, 0, GAMEWIDTH, GAMEHEIGHT);
	//renderer.renderMesh(mesh, uvs, normals, indices, modelMat, viewMatS, projMatSplash, _vs, _ps, _inputLayout, radarMapTex);

	safeRelease(&radarMapSRV);
	safeRelease(&radarMapRTV);
	safeRelease(&radarMapTex);
	// END RENDER TO TEXTURE

	renderer.presentBackBuffer();

}

Spacefight::Spacefight() : clearColors{ 0.01f, 0.1f, 0.1f, 1.0f } {

}

Spacefight::~Spacefight() {
	ShutDown();
}

void Spacefight::Init(Renderer& renderer) {
	
	
	// Import assets
	_shipModel = new Model();
	_simpleShipModel = new Model();
	_cardModel = new Model();
	_playTable = new Model();
	_basicHex = new Model();
	_server = new Model();
	importModel("models/corvette1.obj", _shipModel, renderer);
	importModel("games/spacefight/models/simple_ship1.obj", _simpleShipModel, renderer);
	importModel("games/spacefight/models/card.obj", _cardModel, renderer);
	importModel("games/spacefight/models/play_table.obj", _playTable, renderer);
	importModel("games/spacefight/models/basic_hex.obj", _basicHex, renderer);
	importModel("games/spacefight/models/server.obj", _server, renderer);

	loadTextureFromFile("textures/SF_Corvette-F3_diffuse.jpg", &_shipTexture, &renderer);
	loadTextureFromFile("games/spacefight/models/ajani_diff.png", &_anjaniTex, &renderer);
	loadTextureFromFile("games/spacefight/textures/btn_start.png", &_startButtonTex, &renderer);
	loadTextureFromFile("games/spacefight/textures/StainedMetal.jpg", &_metalTex, &renderer);
	
	_hexTex = new Texture("games/spacefight/textures/HexTexture.png", renderer);
	_serverTex = new Texture("games/spacefight/textures/sdiff.png", renderer);

	// Setup basic world, view and projection matrices
	XMFLOAT3 eyePos = XMFLOAT3(0, 10, -25);
	XMFLOAT3 eyeDir = XMFLOAT3(0, -0.25, 1);
	XMFLOAT3 upDir = XMFLOAT3(0, 1, 0);
	_modelMat = DirectX::XMMatrixScaling(5.1f, 5.1f, 5.1f);
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
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },		// other slot (buffer), starting at 0

	};
	res = const_cast<ID3D11Device*>(renderer.getDevice())->CreateInputLayout(ied, 3, vs->GetBufferPointer(), vs->GetBufferSize(), &_inputLayout);
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
	safeRelease(&_anjaniTex);
	safeRelease(&_metalTex);
	

	if (_hexTex) {
		delete(_hexTex); _hexTex = nullptr;
	}

	if (_cardModel) {
		delete(_cardModel); _cardModel = nullptr;
	}
	if (_shipModel) {
		delete(_shipModel); _shipModel = nullptr;
	}
	if (_basicHex) {
		delete(_basicHex); _basicHex = nullptr;
	}
	if (_playTable) {
		delete(_playTable); _playTable = nullptr;
	}
	if (_simpleShipModel) {
		delete(_simpleShipModel); _simpleShipModel = nullptr;
	}
	if (_server) {
		delete(_server); _server = nullptr;
	}

	if (_serverTex) {
		delete(_serverTex); _serverTex = nullptr;
	}
}