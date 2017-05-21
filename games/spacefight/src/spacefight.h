#pragma once
#include "game.h"
#include "model.h"
#include "input.h"
#include "textures.h"

class Spacefight : public Game {

public:
	Spacefight();
	~Spacefight();
	virtual void Init(Renderer& renderer); 
	virtual void DoFrame(Renderer& renderer, FrameInput* input, long long frameTime) override;
	virtual void ShutDown();

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
	ID3D11Texture2D* _shipTexture = nullptr;
	ID3D11Texture2D* _startButtonTex = nullptr;
	ID3D11Texture2D* _anjaniTex = nullptr;
	ID3D11Texture2D* _metalTex = nullptr;
	Texture* _hexTex = nullptr;
	

	float _menuShipRot = 0;
	float _sineUpDown = 0;
	float _camMovZ = 0;
};
