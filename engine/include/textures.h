#pragma once
#include <d3d11.h>
#include <FreeImage.h>
#include <string>

class Renderer;

HRESULT loadTextureFromFile(const std::string& fileName, ID3D11Texture2D** textureTarget, Renderer* renderer);

class Texture {

public:
	Texture(const std::string& fileName, Renderer& renderer);
	virtual ~Texture();

	ID3D11Texture2D** getTexture() { return &_tex; }
	ID3D11ShaderResourceView** getSRV() { return &_srv; }
	ID3D11SamplerState** getSamplerState() { return &_ss; }

private:
	ID3D11Texture2D* _tex;
	ID3D11ShaderResourceView* _srv;
	ID3D11SamplerState* _ss;
};
