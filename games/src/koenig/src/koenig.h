#pragma once
#include "game.h"
#include "model.h"
#include "input.h"
#include "textures.h"

#include <vector>
#include <string>

constexpr unsigned int GAMEWIDTH = 1280;
constexpr unsigned int GAMEHEIGHT = 720;

class Koenig : public Game {

public:
	Koenig();
	~Koenig();
	void Init(Renderer& renderer) override;
	void DoFrame(Renderer& renderer, FrameInput* input, long long frameTime) override;
	virtual void ShutDown();

	std::string GetIntroImageName() override;
    int getScreenWidth() override { return GAMEWIDTH; }
    int getScreenHeight() override { return GAMEHEIGHT;}

	void RegisterModule(const std::string& module);

private:
	float clearColors[4];
	XMMATRIX _modelMat;
	XMMATRIX _viewMat;
	XMMATRIX _projMat;
	ID3D11VertexShader* _vs = nullptr;
	ID3D11PixelShader* _ps = nullptr;
	ID3D11InputLayout* _inputLayout = nullptr;
	Model* _shipModel = nullptr;
	Model* _simpleShipModel = nullptr;
	Model* _cardModel = nullptr;
	Model* _playTable = nullptr;
	Model* _basicHex = nullptr;
	Model* _server = nullptr;
    Model* _planeModel = nullptr;
	ID3D11Texture2D* _shipTexture = nullptr;
	ID3D11Texture2D* _startButtonTex = nullptr;
	ID3D11Texture2D* _anjaniTex = nullptr;
	ID3D11Texture2D* _metalTex = nullptr;

    ID3D11RenderTargetView* _radarMapRTV;
    ID3D11ShaderResourceView* _radarMapSRV;

	Texture* _hexTex = nullptr;
	Texture* _serverTex = nullptr;
	Texture* _shipTex = nullptr;
    Texture* _anjaniTexture = nullptr;
    Texture* _startButtonTexture = nullptr;
    Texture* _radarMapTexture = nullptr;
	

	float _menuShipRot = 0;
	float _sineUpDown = 0;
	float _camMovZ = 0;

};
