//
// Created by mgrus on 22.05.2022.
//

#ifndef CODEWARS_ENGINE_H
#define CODEWARS_ENGINE_H
#include <Windows.h>


class Renderer;
struct ID3D11InputLayout;
struct ID3D11VertexShader;
struct ID3D11PixelShader;

struct EngineData {
    Renderer* renderer = nullptr;
    ID3D11InputLayout* inputLayout = nullptr;
    ID3D11VertexShader* vShader = nullptr;
    ID3D11PixelShader* pShader = nullptr;
};

EngineData* getEngineData();
EngineData* initEngine(int width, int height, HWND);
void shutdownEngine(EngineData*);

#endif //CODEWARS_ENGINE_H
