#pragma once
#include "game.h"
#include "model.h"
#include "input.h"

class Spacefight : public Game {

public:
	Spacefight();
	~Spacefight();
	virtual void Init(Renderer& renderer); 
	virtual void DoFrame(Renderer& renderer, FrameInput* input) override;
	virtual void ShutDown();

private:
	float clearColors[4];
	XMMATRIX _modelMat;
	XMMATRIX _viewMat;
	XMMATRIX _projMat;
	ID3D11VertexShader* _vs;
	ID3D11PixelShader* _ps;
	ID3D11InputLayout* _inputLayout;
	Model* _shipModel;
	Model* _simpleShipModel;
	Model* _cardModel;
	Model* _playTable;
	ID3D11Texture2D* _shipTexture;
	ID3D11Texture2D* _startButtonTex;
	ID3D11Texture2D* _anjaniTex;
	ID3D11Texture2D* _metalTex;


	float _menuShipRot = 0;
	float _sineUpDown = 0;
};
