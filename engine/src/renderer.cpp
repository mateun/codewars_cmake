#include "stdafx.h"
#include "renderer.h"
#include <dxgi.h>
#include <chrono>
#include <string>

Renderer::Renderer(int w, int h, HWND hWnd) {
	init(w, h, hWnd);
}

Renderer::~Renderer() {
	
	OutputDebugStringW(L"Before renderer cleanup\n");
	_debugger->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	_ctx->ClearState();
	_depthStencilView->Release();
	_depthStencilBuffer->Release();
	_rtv->Release();
	_backBuffer->Release();
	
	_swapChain->Release();
	
	_ctx->Release();
	_device->Release();
	OutputDebugStringW(L"---------------------------------------------------------------------------------------------------------------------------------------\n");
	OutputDebugStringW(L"After renderer cleanup\n");
	if (_debugger) _debugger->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

	_debugger->Release();

}

void Renderer::clearTexture(float *color, ID3D11RenderTargetView* rtv) {
	ID3D11RenderTargetView* rtvs[1] = { rtv };
	_ctx->OMSetRenderTargets(1, rtvs, nullptr);
	_ctx->ClearRenderTargetView(rtv, color);
}

void Renderer::clearBackbuffer(float *clearColors) {
	ID3D11RenderTargetView* rtvs[1] = { _rtv };
	_ctx->OMSetRenderTargets(1, rtvs, _depthStencilView);
	_ctx->ClearRenderTargetView(_rtv, clearColors);

	// clear our depth target as well
	_ctx->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH, 1, 0);
}

void Renderer::presentBackBuffer() {
	_swapChain->Present(0, 0);
}

/**
	Sets the current viewport, expects
	a valid D3D11 context.

*/
void Renderer::setViewport(int x, int y, int w, int h) {
	D3D11_VIEWPORT vp;
	ZeroMemory(&vp, sizeof(vp));
	vp.TopLeftX = x;
	vp.TopLeftY = y;
	vp.Width = w;
	vp.Height = h;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	_ctx->RSSetViewports(1, &vp);
}

void Renderer::render() {

	
}

void printTime(const std::string& text, const std::chrono::microseconds& diff) {
	auto start = std::chrono::high_resolution_clock::now();
	char buf[800];
	sprintf_s(buf, 800, " >>>>>>>>>>>> time diff: %s %d\n", text.c_str(), diff);
	OutputDebugString(buf);
	auto diff2 = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start);
	sprintf_s(buf, 800, " >> timeprint duration: %d\n", diff2);
	OutputDebugString(buf);

}

void Renderer::renderModel(const Model& model, const XMMATRIX &modelMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projMatrix,
	ID3D11VertexShader* vs, ID3D11PixelShader* ps, ID3D11InputLayout* inputLayout, Texture& texture) {

	auto startOverall = std::chrono::high_resolution_clock::now();

	// Bind buffers, shaders and input layout
	auto start = std::chrono::high_resolution_clock::now();
	_ctx->IASetIndexBuffer(model.indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	_ctx->VSSetShader(vs, 0, 0);
	_ctx->PSSetShader(ps, 0, 0);
	_ctx->IASetInputLayout(inputLayout);
	UINT stride = sizeof(XMFLOAT3);
	UINT offset = 0;
	_ctx->IASetVertexBuffers(0, 1, &model.posBuffer, &stride, &offset);
	UINT uvstride = sizeof(XMFLOAT2);
	_ctx->IASetVertexBuffers(1, 1, &model.uvBuffer, &uvstride, &offset);
	_ctx->IASetVertexBuffers(2, 1, &model.normalBuffer, &stride, &offset);
	auto diff = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start);
	//printTime("bufferSetting in micros: ", diff);

	// Handle mvp matrices
	start = std::chrono::high_resolution_clock::now();
	D3D11_BUFFER_DESC mbd;
	ZeroMemory(&mbd, sizeof(mbd));
	mbd.Usage = D3D11_USAGE_DYNAMIC;
	mbd.ByteWidth = sizeof(MatrixBufferType);
	mbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	mbd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	ID3D11Buffer* matrixBuffer;
	HRESULT res = _device->CreateBuffer(&mbd, nullptr, &matrixBuffer);
	if (FAILED(res)) {
		OutputDebugStringW(L"matrix constant buffer creation failed\n");
		exit(1);
	}
	D3D11_MAPPED_SUBRESOURCE bufSR;
	MatrixBufferType* dataPtr;
	res = _ctx->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufSR);
	dataPtr = (MatrixBufferType*)bufSR.pData;
	dataPtr->world = modelMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->proj = projMatrix;
	_ctx->Unmap(matrixBuffer, 0);
	_ctx->VSSetConstantBuffers(0, 1, &matrixBuffer);
	diff = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start);
	//printTime("matrixBuffer in micros: ", diff);

	// Handle lights
	start = std::chrono::high_resolution_clock::now();
	D3D11_BUFFER_DESC lbd;
	ZeroMemory(&lbd, sizeof(lbd));
	lbd.Usage = D3D11_USAGE_DYNAMIC;
	lbd.ByteWidth = sizeof(LightBufferType);
	lbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lbd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	ID3D11Buffer* lightBuffer;
	res = _device->CreateBuffer(&lbd, nullptr, &lightBuffer);
	if (FAILED(res)) {
		OutputDebugStringW(L"light constant buffer creation failed\n");
		exit(1);
	}

	LightBufferType* dataPtrLight;
	res = _ctx->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufSR);
	dataPtrLight = (LightBufferType*)bufSR.pData;
	dataPtrLight->ambientcol = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	_ctx->Unmap(lightBuffer, 0);
	_ctx->PSSetConstantBuffers(0, 1, &lightBuffer);
	diff = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start);
	//printTime("lightBuffer in micros: ", diff);

	// Handle textures
	start = std::chrono::high_resolution_clock::now();
	
	_ctx->PSSetShaderResources(0, 1, texture.getSRV());
	_ctx->PSSetSamplers(0, 1, texture.getSamplerState());
	
	diff = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start);
	//printTime("textureSetup in micros: ", diff);

	_ctx->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Setting wireframe RS
	start = std::chrono::high_resolution_clock::now();
	ID3D11RasterizerState* rs;
	D3D11_RASTERIZER_DESC rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.FillMode = D3D11_FILL_SOLID;
	rd.DepthClipEnable = true;
	rd.CullMode = D3D11_CULL_BACK;
	res = _device->CreateRasterizerState(&rd, &rs);
	if (FAILED(res)) {
		OutputDebugStringW(L"wireframe rs failed\n");
		exit(1);
	}
	_ctx->RSSetState(rs);
	diff = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start);
	//printTime("change RS in micros: ", diff);
	// end RS

	start = std::chrono::high_resolution_clock::now();
	_ctx->DrawIndexed(model.indices.size(), 0, 0);
	diff = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start);
	//printTime("drawCall micros: ", diff);

	// cleanup
	safeRelease(&rs);
	safeRelease(&lightBuffer);
	safeRelease(&matrixBuffer);

	//auto diffOverall = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - startOverall);
	//printTime("renderModelOverall in micros: ", diffOverall);
	
}


void Renderer::setTextureRenderTarget(ID3D11RenderTargetView** rtv) {
	_ctx->OMSetRenderTargets(1, rtv, nullptr);
}

void Renderer::setBackBufferRenderTarget() {
	_ctx->OMSetRenderTargets(1, &_rtv, _depthStencilView);
}

void Renderer::createRenderTargetViewForTexture(ID3D11Texture2D* tex, ID3D11RenderTargetView** rtv) {
	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	ZeroMemory(&rtvd, sizeof(rtvd));
	rtvd.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvd.Texture2D.MipSlice = 0;
	_device->CreateRenderTargetView(tex, &rtvd, rtv);
}

void Renderer::createRenderTargetShaderResourceViewForTexture(ID3D11Texture2D* tex, ID3D11ShaderResourceView** srv) {
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MipLevels = 1;
	srvd.Texture2D.MostDetailedMip = 0;
	_device->CreateShaderResourceView(tex, &srvd, srv);
}

void Renderer::createRenderTargetTexture(UINT w, UINT h, ID3D11Texture2D** tex) {
	ID3D11RenderTargetView* radarMapRTV;
	ID3D11ShaderResourceView* radarMapRV;
	
	D3D11_TEXTURE2D_DESC mtd;
	ZeroMemory(&mtd, sizeof(mtd));
	mtd.Width = w;
	mtd.Height = h;
	mtd.MipLevels = 1;
	mtd.ArraySize = 1;
	mtd.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	mtd.SampleDesc.Count = 1;
	mtd.Usage = D3D11_USAGE_DEFAULT;
	mtd.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	mtd.CPUAccessFlags = 0;
	mtd.MiscFlags = 0;
	_device->CreateTexture2D(&mtd, NULL, tex);

	
}


void Renderer::init(int w, int h, HWND hWnd) {
	// Query the adapters
	IDXGIFactory* tmpFac = nullptr;
	HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&tmpFac);
	if (FAILED(result)) {
		OutputDebugString("IDXGI factory creation failed\n");
		exit(2);
	}
	IDXGIAdapter* adapter;
	result = tmpFac->EnumAdapters(0, &adapter);
	if (FAILED(result)) {
		OutputDebugString("Adapter enum failed\n");
		exit(2);
	}

	IDXGIOutput* adapterOutput;
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result)) {
		OutputDebugString("Failed to create adapter output\n");
		exit(2);
	}

	UINT numModes;
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, nullptr);
	if (FAILED(result)) {
		OutputDebugString("Failed to list modes\n");
		exit(2);
	}

	DXGI_MODE_DESC* modeList = new DXGI_MODE_DESC[numModes];
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, modeList);
	if (FAILED(result)) {
		OutputDebugString("Failed to create mode list\n");
		exit(2);
	}

	UINT numerator = 0;
	UINT denumarator = 0;
	for (int i = 0; i < numModes; ++i) {
		if (modeList[i].Width == w) {
			if (modeList[i].Height == h) {
				numerator = modeList[i].RefreshRate.Numerator;
				denumarator = modeList[i].RefreshRate.Denominator;
				OutputDebugString("Found valid refresh rate setting\n");
			}
		}
	}

	if (numerator == 0) {
		OutputDebugString("Could not find valid refresh rate\n");
		exit(2);
	}

	delete [] modeList;
	modeList = nullptr;

	adapterOutput->Release();
	adapterOutput = nullptr;

	adapter->Release();
	adapter = nullptr;

	tmpFac->Release();
	tmpFac = nullptr;

	// end adapter querying


	D3D_FEATURE_LEVEL featureLevels =  D3D_FEATURE_LEVEL_11_1;

	result = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, &featureLevels, 1, D3D11_SDK_VERSION, &_device, NULL, &_ctx);
	if (FAILED(result)) {
		OutputDebugStringW(L"CreateDevice failed\n");
		exit(2);
	}

	UINT ql;
	_device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UINT, 8, &ql);

	DXGI_SWAP_CHAIN_DESC scdesc;
	ZeroMemory(&scdesc, sizeof(scdesc));
	scdesc.BufferCount = 1;
	scdesc.BufferDesc.Height = h;
	scdesc.BufferDesc.Width = w;
	scdesc.Windowed = true;

	scdesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	
	scdesc.BufferDesc.RefreshRate.Numerator = 60;
	scdesc.BufferDesc.RefreshRate.Denominator = 1;
	
	scdesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	scdesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scdesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	
	scdesc.OutputWindow = hWnd;
	scdesc.SampleDesc.Count = 4;	// samples per pixel
	scdesc.SampleDesc.Quality = 0;
	scdesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scdesc.Flags = 0;
	
	IDXGIDevice * pDXGIDevice = nullptr;
	result = _device->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDXGIDevice);
	IDXGIAdapter * pDXGIAdapter = nullptr;
	result = pDXGIDevice->GetAdapter(&pDXGIAdapter);
	IDXGIFactory * pIDXGIFactory = nullptr;
	pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&pIDXGIFactory);
	
	result = pIDXGIFactory->CreateSwapChain(_device, &scdesc, &_swapChain);
	if (FAILED(result)) {
		OutputDebugStringW(L"error creating swapchain\n");
		exit(1);
	}

	pIDXGIFactory->Release();
	pDXGIAdapter->Release();
	pDXGIDevice->Release();


	// Gather the debug interface
	_debugger = 0;
	result = _device->QueryInterface(__uuidof(ID3D11Debug), (void**)&_debugger);
	if (FAILED(result)) {
		OutputDebugStringW(L"debuger creation failed\n");
		exit(1);
	}

	

	// Create a backbuffer
	result = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&_backBuffer);
	if (FAILED(result)) {
		OutputDebugStringW(L"backbuffer creation failed\n");
		exit(1);
	}

	// Bind the backbuffer as our render target
	result = _device->CreateRenderTargetView(_backBuffer, NULL, &_rtv);
	if (FAILED(result)) {
		OutputDebugStringW(L"rtv creation failed\n");
		exit(1);
	}

	// Create a depth/stencil buffer
	D3D11_TEXTURE2D_DESC td;
	td.Width = w;
	td.Height = h;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_D32_FLOAT;
	td.SampleDesc.Count = 4;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;
	
	result = _device->CreateTexture2D(&td, 0, &_depthStencilBuffer);
	if (FAILED(result)) {
		OutputDebugStringW(L"D S buffer creation failed\n");
		exit(1);
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dpd;
	ZeroMemory(&dpd, sizeof(dpd));
	dpd.Flags = 0;
	dpd.Format = DXGI_FORMAT_D32_FLOAT;
	dpd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	
	result = _device->CreateDepthStencilView(_depthStencilBuffer, &dpd, &_depthStencilView);
	if (FAILED(result)) {
		OutputDebugStringW(L"D S view creation failed\n");
		exit(1);
	}

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = FALSE;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Currently we just use the default from D3D11 ...
	//ID3D11DepthStencilState *m_DepthStencilState;
	//result = _device->CreateDepthStencilState(&depthStencilDesc, &m_DepthStencilState);
	/*if (FAILED(result)) {
		OutputDebugString(L"failed to set depth stencil state\n");
		exit(1);
	}*/
	//_ctx->OMSetDepthStencilState(m_DepthStencilState, 0);

}