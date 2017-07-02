#include <stdafx.h>
#include "spacefight.h"
#include "model_import.h"
#include "textures.h"
#include "shaders.h"
#include <d3dcompiler.h>
#include <resource_management.h>
#include <dinput.h>
#include "model.h"
#include <primitives.h>
#include <chrono>
#include <py_embed.h>
#include <consoleprint.h>

// Python stuff

PyObject* spacefight_getHealth(PyObject* self, PyObject* args) {
	OutputDebugString("emb_getHealth called from python!\n");
	cwprintf("spacefight_getHealth called from python!\n");
	int playerNr = 0;
	int playerHealth = 100;
	if (!PyArg_ParseTuple(args, "i", &playerNr)) {
		return nullptr;
	}

	// TODO make fake impl real
	// e.g. lookup health for player somewhere
	return PyLong_FromLong(playerHealth);
}

PyMethodDef SpacefightMethods[] = {
	{ "get_player_health", spacefight_getHealth, METH_VARARGS,
	"Return the current health value of the player given by its number." },
	{ NULL, NULL, 0, NULL }
};

PyModuleDef SpacefightModule = {
	PyModuleDef_HEAD_INIT, "spacefight", NULL, -1, SpacefightMethods,
	//NULL, NULL, NULL, NULL
};

PyObject* PyInit_Spacefight(void) {
	try {
		// BEWARE: I received an exception when linking to release 
		// Python lib & dll. 
		// When in debug mode, it seems to be important to link to 
		// debug python lib & dll. 
		PyObject* m = PyModule_Create(&SpacefightModule);
		if (m) {
			OutputDebugString("loaded python module\n");
			return m;
		}
	}
	catch (...) {
		OutputDebugString("caught exception while calling PyModuleCreate\n");
	}
}



// end python stuff

static Spacefight spacefight;

Game* GetGame() {
	return &spacefight;
}

std::string Spacefight::GetIntroImageName() {
	return "games/spacefight/textures/spacefight_intro.png";
}

void Spacefight::DoPythonFrame() {
	// TODO
}

bool Spacefight::InitPythonEnv() {
	// We add our module to the system modules.
	// Call this before Py_Initialize().
	int res = PyImport_AppendInittab("spacefight", &PyInit_Spacefight);

	Py_Initialize();
	// Where to look for our scripts
	PySys_SetPath(L"./games/spacefight/py_scripts");
	// This is the name of our python script.
	pName = PyUnicode_DecodeFSDefault("gamelogic");

	pModule = PyImport_Import(pName);
	pFunc = PyObject_GetAttrString(pModule, "doFrame");
	
	
	pArgs = PyTuple_New(1);
	pValue = PyLong_FromLong(1);
	PyTuple_SetItem(pArgs, 0, pValue);
	
	PyObject* retval = PyObject_CallObject(pFunc, pArgs);
	long val = PyLong_AsLong(retval);

	// do something in between...
	// and call again...
	// Problem is, I can not hold the state of the python script!
	// In the frame, if I reference the member variables, it crashes.
	retval = PyObject_CallObject(pFunc, pArgs);
	val = PyLong_AsLong(retval);
	
	return true;
}


void Spacefight::DoFrame(Renderer& renderer, FrameInput* input, long long frameTime) {
	// Python call
	auto start = std::chrono::high_resolution_clock::now();
	/*Py_Initialize();
	// Where to look for our scripts
	PySys_SetPath(L"./games/spacefight/py_scripts");
	// This is the name of our python script.
	pName = PyUnicode_DecodeFSDefault("gamelogic");

	pModule = PyImport_Import(pName);
	pFunc = PyObject_GetAttrString(pModule, "doFrame");
	auto diff = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start);
	
	cwprintf("time for python init (micros): %d\n", diff);
	start = std::chrono::high_resolution_clock::now();
	
	pArgs = PyTuple_New(1);
	pValue = PyLong_FromLong(1);
	PyTuple_SetItem(pArgs, 0, pValue);*/
	PyObject* retval = PyObject_CallObject(pFunc, pArgs);
	long val = PyLong_AsLong(retval);
	
	//diff = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start);
	//cwprintf("time for python call (micros): %d\n", diff);
	
	
	// End python frame stuff
	
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
	XMMATRIX scaleMat = DirectX::XMMatrixScaling(0.7f, 0.7f, 0.7f);
	// Move the button to the lower left corner
	XMMATRIX transMat = DirectX::XMMatrixTranslation(2.2f, -1.95f, 0.0f);	
	modelMat = XMMatrixTranspose(XMMatrixMultiply(transMat, scaleMat));
	XMFLOAT3 eyePosS = XMFLOAT3(0, 0, -1);
	XMFLOAT3 eyeDirS = XMFLOAT3(0, 0, 1);
	XMFLOAT3 upDirS = XMFLOAT3(0, 1, 0);
	XMMATRIX viewMatS = XMMatrixTranspose(XMMatrixLookToLH(XMLoadFloat3(&eyePosS), XMLoadFloat3(&eyeDirS), XMLoadFloat3(&upDirS)));
	XMMATRIX projMatSplash = XMMatrixTranspose(XMMatrixOrthographicLH(5.0f, 5.0f, 0.1f, 100.0f));

	float clearColors[] = { 0.0f, 0.0f, 0.0f, 1.0f };
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
	XMFLOAT3 eyeDir = XMFLOAT3(0, -1, 0);
	upDir = XMFLOAT3(0, 0, 1);
	XMMATRIX mapView = XMMatrixTranspose(XMMatrixLookToLH(XMLoadFloat3(&eyePos), XMLoadFloat3(&eyeDir), XMLoadFloat3(&upDir)));
	renderer.setTextureRenderTarget(&radarMapRTV);
	float clearColorsTex[] = { 0.0f, 0.0f, 0.0f, 1.0f };
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

Spacefight::Spacefight() : clearColors{ 0.0f, 0.0f, 0.0f, 1.0f } {

}

Spacefight::~Spacefight() {
	ShutDown();
}

void Spacefight::Init(Renderer& renderer) {
	InitPythonEnv();
	
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

	//loadTextureFromFile("textures/SF_Corvette-F3_diffuse.jpg", &_shipTexture, &renderer);
	loadTextureFromFile("games/spacefight/models/ajani_diff.png", &_anjaniTex, &renderer);
	loadTextureFromFile("games/spacefight/textures/btn_start.png", &_startButtonTex, &renderer);
	loadTextureFromFile("games/spacefight/textures/StainedMetal.jpg", &_metalTex, &renderer);
	
	_hexTex = new Texture("games/spacefight/textures/HexTexture.png", renderer);
	_serverTex = new Texture("games/spacefight/textures/sdiff.png", renderer);
	_shipTex = new Texture("games/spacefight/textures/SF_Corvette-F3_diffuse.jpg", renderer);

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

	// Python cleanup
	Py_FinalizeEx();

}