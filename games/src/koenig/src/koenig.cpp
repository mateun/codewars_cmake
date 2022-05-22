#include "stdafx.h"
#include "koenig.h"
#include "model_import.h"
#include "textures.h"
#include "shaders.h"
#include <d3dcompiler.h>
#include "resource_management.h"
#include <dinput.h>
#include "model.h"
#include "primitives.h"
#include <chrono>
#include "consoleprint.h"

//////////////////////////////////
// The game object itself
static Koenig koenig;

Game* GetGame() {
	return &koenig;
}
////////////////////////////////

std::string Koenig::GetIntroImageName() {
	return "../games/assets/koenig/textures/koenig_splash.png";
}




void Koenig::DoFrame(Renderer& renderer, FrameInput* input, long long frameTime) {

	_menuShipRot += 0.007f;
	_modelMat = XMMatrixIdentity();
	
	_sineUpDown = sin(_menuShipRot)* 0.5f;
	
	XMFLOAT3 zAxis = XMFLOAT3(1, 0, 1);
	XMFLOAT3 yAxis = XMFLOAT3(0, 1, 0);
	XMFLOAT3 xAxis = XMFLOAT3(1, 0, 0);
	XMMATRIX rotMatZ = DirectX::XMMatrixRotationAxis(XMLoadFloat3(&zAxis), _menuShipRot*4.0f);
	XMMATRIX rotMatY = DirectX::XMMatrixRotationAxis(XMLoadFloat3(&yAxis), _menuShipRot*0.5f);
	XMMATRIX transZ = DirectX::XMMatrixTranslation(0, 0, _sineUpDown - 10.0f);
	//_modelMat = XMMatrixMultiply(_modelMat, rotMatZ);
	_modelMat = XMMatrixMultiply(_modelMat, rotMatY);
	//_modelMat = XMMatrixTranspose(XMMatrixMultiply(_modelMat, transZ));

	renderer.clearBackbuffer(clearColors);
	renderer.setViewport(0, 0, GAMEWIDTH, GAMEHEIGHT);
    renderer.renderModel(*_shipModel, _modelMat, _viewMat, _projMat, _vs, _ps, _inputLayout, *_shipTex);
    renderer.renderModel(*_cardModel, _modelMat, _viewMat, _projMat, _vs, _ps, _inputLayout, *_anjaniTexture);
	//renderer.renderMesh(_shipModel->positions, _shipModel->uvs, _shipModel->indices, _modelMat, _viewMat, _projMat, _vs, _ps, _inputLayout, _shipTexture);
	//renderer.renderMesh(_cardModel->positions, _cardModel->uvs, _cardModel->normals, _cardModel->indices, _modelMat, _viewMat, _projMat, _vs, _ps, _inputLayout, _anjaniTex);

	
	XMFLOAT3 eyePos = XMFLOAT3(0, 40, -25);
	XMFLOAT3 focusPos = XMFLOAT3(0, 0, 0);
	XMVECTOR eyeDirection = XMVectorSubtract(XMLoadFloat3(&focusPos), XMLoadFloat3(&eyePos));
	XMFLOAT3 upDir = XMFLOAT3(0, 1, 0);
	
	// recalc the view matrix
	// r always points to the right, regardless of our pitch.
	XMFLOAT3 r = XMFLOAT3(1, 0, 0);
	XMFLOAT3 u0 = XMFLOAT3(0, 1, 0);
	XMFLOAT3 f0 = XMFLOAT3(0, 0, 1);

	XMVECTOR ry = XMQuaternionRotationAxis(XMLoadFloat3(&u0), -0.0);
	
	XMVECTOR eyedir_rotated = XMVector3Rotate(XMLoadFloat3(&f0), ry);
	XMVECTOR u = XMVector3Cross(eyedir_rotated, XMLoadFloat3(&r));

	_viewMat = DirectX::XMMatrixTranspose(XMMatrixLookAtLH(XMLoadFloat3(&eyePos), XMLoadFloat3(&focusPos), u));
	
	// Draw a server
	XMMATRIX serverModelMat = XMMatrixIdentity();
	serverModelMat = XMMatrixTranspose(XMMatrixMultiply(serverModelMat, XMMatrixScaling(0.4f, 0.4f, 0.4f)));
	serverModelMat = XMMatrixTranspose(XMMatrixMultiply(serverModelMat,
		XMMatrixTranslation((1.75f), 0.35f, 6.1f)));

	//renderer.renderModel(*_server, serverModelMat, _viewMat, _projMat, _vs, _ps, _inputLayout, *_serverTex);

	renderer.renderModel(*_shipModel, _modelMat, _viewMat, _projMat, _vs, _ps, _inputLayout, *_shipTex);
	
	// Draw a start button
	XMMATRIX modelMat = DirectX::XMMatrixIdentity();
	XMMATRIX scaleMat = DirectX::XMMatrixScaling(128, 128, 0.7f);
	// Move the button to the lower left corner
	XMMATRIX transMat = DirectX::XMMatrixTranslation(364, 500, 0.0f);
	modelMat = XMMatrixMultiply(scaleMat, transMat);
    modelMat = XMMatrixTranspose(modelMat);
	XMFLOAT3 eyePosS = XMFLOAT3(0, 0, -1);
	XMFLOAT3 eyeDirS = XMFLOAT3(0, 0, 1);
	XMFLOAT3 upDirS = XMFLOAT3(0, 1, 0);
	XMMATRIX viewMatS = XMMatrixTranspose(XMMatrixLookToLH(XMLoadFloat3(&eyePosS), XMLoadFloat3(&eyeDirS), XMLoadFloat3(&upDirS)));
	XMMATRIX projMatSplash = XMMatrixTranspose(XMMatrixOrthographicOffCenterLH(0, GAMEWIDTH, 0, GAMEHEIGHT, 0.1f, 100.0f));

	renderer.renderModel(*_planeModel, modelMat, viewMatS, projMatSplash, _vs, _ps, _inputLayout, *_anjaniTexture);

	// RENDER TO TEXTURE
	// Render the whole image to a texture from above and display it
    float clearColors[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	eyePos = XMFLOAT3(0, 75, 0);
	XMFLOAT3 eyeDir = XMFLOAT3(0, -1, 0);
	upDir = XMFLOAT3(0, 0, 1);
	XMMATRIX mapView = XMMatrixTranspose(XMMatrixLookToLH(XMLoadFloat3(&eyePos), XMLoadFloat3(&eyeDir), XMLoadFloat3(&upDir)));
	renderer.setTextureRenderTarget(&_radarMapRTV);
	float clearColorsTex[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	renderer.clearTexture(clearColorsTex, _radarMapRTV);
	renderer.setViewport(0, 0, 400, 300);
	renderer.renderModel(*_shipModel, _modelMat, mapView, _projMat, _vs, _ps, _inputLayout, *_shipTex);
	

	// After rendering the scene from birds eye view to our texture, 
	// render the texture onto a quad.
	// Move our map quad to the left of the screen.
	renderer.setBackBufferRenderTarget();
	transMat = DirectX::XMMatrixTranslation(128.0f, 128, 0.0f);
	scaleMat = DirectX::XMMatrixScaling(256.0f, 256.0f, 1.0f);
	modelMat = XMMatrixTranspose(XMMatrixMultiply(scaleMat, transMat));
	renderer.setViewport(0, 0, GAMEWIDTH, GAMEHEIGHT);
    renderer.renderModel(*_planeModel, modelMat, viewMatS, projMatSplash, _vs, _ps, _inputLayout, *_radarMapTexture);
//	renderer.renderMesh(mesh, uvs, normals, indices, modelMat, viewMatS, projMatSplash, _vs, _ps, _inputLayout, radarMapTex);


	// END RENDER TO TEXTURE

	renderer.presentBackBuffer();

}

Koenig::Koenig() : clearColors{ 0.0f, 0.0f, 0.0f, 1.0f } {

}

Koenig::~Koenig() {
	ShutDown();
}

void Koenig::Init(Renderer& renderer) {

	// Import assets
	_shipModel = new Model();
	_simpleShipModel = new Model();
	_cardModel = new Model();
	_playTable = new Model();
	_basicHex = new Model();
	_server = new Model();

    std::vector<XMFLOAT3> mesh;
    std::vector<XMFLOAT3> normals;
    std::vector<XMFLOAT2> uvs;
    std::vector<UINT> indices;
    fillQuadVertexData(mesh, uvs, normals, indices);

    _planeModel = createFromRawData(mesh, uvs, normals, indices, renderer);
    //importObjModel("../games/assets/spacefight/models/plane.obj", _shipModel, renderer);
    importObjModel("../games/assets/spacefight/models/corvette1.obj", _shipModel, renderer);
//	importObjModel("../games/assets/spacefight/models/simple_ship1.obj", _simpleShipModel, renderer);
//	importObjModel("../games/assets/spacefight/models/card.obj", _cardModel, renderer);
//	importObjModel("../games/assets/spacefight/models/play_table.obj", _playTable, renderer);
//	importObjModel("../games/assets/spacefight/models/basic_hex.obj", _basicHex, renderer);
//	importObjModel("../games/assets/spacefight/models/server.obj", _server, renderer);

	//loadTextureFromFile("textures/SF_Corvette-F3_diffuse.jpg", &_shipTexture, &renderer);
	loadTextureFromFile("../games/assets/spacefight/models/ajani_diff.png", &_anjaniTex, &renderer);
	loadTextureFromFile("../games/assets/spacefight/textures/btn_start.png", &_startButtonTex, &renderer);
	loadTextureFromFile("../games/assets/spacefight/textures/StainedMetal.jpg", &_metalTex, &renderer);
	
	_hexTex = new Texture("../games/assets/spacefight/textures/HexTexture.png", renderer);
	_serverTex = new Texture("../games/assets/spacefight/textures/sdiff.png", renderer);
	_shipTex = new Texture("../games/assets/spacefight/textures/SF_Corvette-F3_diffuse.jpg", renderer);
    _anjaniTexture = new Texture(&_anjaniTex, renderer);

    ID3D11Texture2D* radarMapTex;
    renderer.createRenderTargetTexture(400, 300, &radarMapTex);
    renderer.createRenderTargetViewForTexture(radarMapTex, &_radarMapRTV);
    renderer.createRenderTargetShaderResourceViewForTexture(radarMapTex, &_radarMapSRV);
    _radarMapTexture = new Texture(&radarMapTex, renderer);


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
	HRESULT res = D3DCompileFromFile(L"../games/assets/spacefight/shaders/basic.hlsl", NULL, NULL, "VShader", "vs_5_0", 0, 0, &vs, &errBlob);
	if (FAILED(res)) {
		OutputDebugStringW(L"vshader load failed\n");
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
	res = D3DCompileFromFile(L"../games/assets/spacefight/shaders/basic.hlsl", NULL, NULL, "PShader", "ps_5_0", 0, 0, &ps, &errBlob);
	if (FAILED(res)) {
		OutputDebugStringW(L"pshader load failed\n");
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

void Koenig::ShutDown() {
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

	if (_shipTex) {
		delete(_shipTex); _shipTex = nullptr;
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

    safeRelease(&_radarMapSRV);
    safeRelease(&_radarMapRTV);
//    safeRelease(&radarMapTex);


}