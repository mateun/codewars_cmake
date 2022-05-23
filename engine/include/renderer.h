#pragma once
#include <d3d11_4.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <vector>
#include "model.h"
#include "textures.h"

using namespace DirectX;

struct MY_VERTEX {
	XMFLOAT3 pos;
	XMFLOAT2 uv;
};

struct MatrixBufferType {
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX proj;
};

struct LightBufferType {
	XMFLOAT4 ambientcol;
};


class Renderer {

public:
	Renderer(int w, int h, HWND hWnd);
	~Renderer();
	void render();
	void clearBackbuffer(float *colors);
	void presentBackBuffer();
	void Renderer::setViewport(int x, int y, int w, int h);
	void renderModel(const Model& model, const XMMATRIX &modelMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projMatrix,
		ID3D11VertexShader* vs, ID3D11PixelShader* ps, ID3D11InputLayout* inputLayout, Texture& texture);
	void renderModel(const Model& model, const XMMATRIX &modelMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projMatrix,
					 Texture& texture);

	void createRenderTargetTexture(UINT w, UINT h, ID3D11Texture2D** tex);
	void createRenderTargetViewForTexture(ID3D11Texture2D* tex, ID3D11RenderTargetView** rtv);
	void createRenderTargetShaderResourceViewForTexture(ID3D11Texture2D* tex, ID3D11ShaderResourceView** srv);
	void setTextureRenderTarget(ID3D11RenderTargetView** rtv);
	void setBackBufferRenderTarget();
	void clearTexture(float *color, ID3D11RenderTargetView* rtv);
	ID3D11Device* getDevice() { return _device; }
	ID3D11DeviceContext* getContext() { return _ctx; }
	ID3D11RenderTargetView* getBackbufferTexture(){ return _rtv; }

	void setAmbientColor(XMFLOAT4 col) { _ambientColor = col;}


	void enableAlphaBlending(bool value);

    void setShader(ID3D11VertexShader *vShader, ID3D11PixelShader *pShader);

private:
	void init(int w, int h, HWND hWnd);
	ID3D11Device *_device;
	ID3D11DeviceContext *_ctx;
	IDXGISwapChain *_swapChain;
	ID3D11Debug* _debugger;
	ID3D11Texture2D *_backBuffer;
	ID3D11RenderTargetView* _rtv;
	ID3D11Texture2D* _depthStencilBuffer;
	ID3D11DepthStencilView *_depthStencilView;
	ID3D11BlendState* _blendStateTransparency = nullptr;
	ID3D11BlendState* _blendStateNoTransparency = nullptr;
	XMFLOAT4 _ambientColor = {1, 1, 1, 1};
    ID3D11VertexShader* _vShader = nullptr;
    ID3D11PixelShader* _pShader = nullptr;
};
