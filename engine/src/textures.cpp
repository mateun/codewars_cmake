#include "stdafx.h"
#include "textures.h"
#include "resource_management.h"
#include "renderer.h"

HRESULT loadTextureFromFile(const std::string & fileName, ID3D11Texture2D ** textureTarget, Renderer * renderer) {
	FREE_IMAGE_FORMAT format = FreeImage_GetFileType(fileName.c_str(), 0);
	FIBITMAP* image = FreeImage_Load(format, fileName.c_str());
	UINT bpp = FreeImage_GetBPP(image);
	if (bpp != 32) image = FreeImage_ConvertTo32Bits(image);
	int w = FreeImage_GetWidth(image);
	int h = FreeImage_GetHeight(image);
	bpp = FreeImage_GetBPP(image);
	int size = sizeof(BYTE) * w * h * 4;
	//FreeImage_FlipHorizontal(image);
	BYTE* bits = FreeImage_GetBits(image);
	BYTE* data = (BYTE*)malloc(size);
	memcpy(data, bits, size);


	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = w;
	desc.Height = h;
	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	HRESULT res = res = const_cast<ID3D11Device*>(renderer->getDevice())->CreateTexture2D(&desc, NULL, textureTarget);
	if (FAILED(res)) {
		OutputDebugStringW(L"texture creation failed\n");
		return res;
	}

	D3D11_MAPPED_SUBRESOURCE mapped;
	renderer->getContext()->Map(*textureTarget, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	BYTE* mappedData = reinterpret_cast<BYTE*>(mapped.pData);
	for (UINT i = 0; i < h; ++i) {
		memcpy(mappedData, data, w * 4);
		mappedData += mapped.RowPitch;
		data += (w * 4);
	}
	// Unused code for poking 
	// bgra values directly into the image - 
	// for procedural texture generation.
	/*for (UINT x = 0; x < 64; x += 4) {
	for (UINT i = 0; i < 64; ++i)
	{
	BYTE b = 0xaf;
	BYTE g = 0xbb;
	BYTE r = 0x10;
	BYTE a = 0xff;
	mappedData[x + (i * 64)] = b;
	mappedData[x + (i * 64) + 1] = g;
	mappedData[x + (i * 64) + 2] = r;
	mappedData[x + (i * 64) + 3] = a;
	}
	}*/
	renderer->getContext()->Unmap(*textureTarget, 0);
}

//Texture::Texture(


Texture::Texture(const std::string& fileName, Renderer& renderer) {
	loadTextureFromFile(fileName, &_tex, &renderer);

	renderer.getDevice()->CreateShaderResourceView(_tex, NULL, &_srv);
	renderer.getContext()->PSSetShaderResources(0, 1, &_srv);

	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.MinLOD = 0;
	sd.MaxLOD = D3D11_FLOAT32_MAX;

	renderer.getDevice()->CreateSamplerState(&sd, &_ss);
	renderer.getContext()->PSSetSamplers(0, 1, &_ss);

}

Texture::~Texture() {
	safeRelease(&_ss);
	safeRelease(&_srv);
	safeRelease(&_tex);
}

Texture::Texture(ID3D11Texture2D** tex, Renderer& renderer) {
    _tex = *tex;
    renderer.getDevice()->CreateShaderResourceView(_tex, NULL, &_srv);
    renderer.getContext()->PSSetShaderResources(0, 1, &_srv);

    D3D11_SAMPLER_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sd.MinLOD = 0;
    sd.MaxLOD = D3D11_FLOAT32_MAX;

    renderer.getDevice()->CreateSamplerState(&sd, &_ss);
    renderer.getContext()->PSSetSamplers(0, 1, &_ss);

}
