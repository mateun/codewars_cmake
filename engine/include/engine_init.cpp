
#include "engine.h"
#include <d3dcompiler.h>
#include "logging.h"
#include "consoleprint.h"
#include "resource_management.h"
#include "renderer.h"
#include "shaders.h"

static EngineData* engineData = nullptr;
EngineData *getEngineData() {
    return engineData;
}

EngineData* initEngine(int width, int height, HWND hwnd) {
    if (!engineData) {
        engineData = new EngineData();
        engineData->renderer = new Renderer(width, height, hwnd);
    } else {
        cwprintf("Engine reinit! Exiting\n");
        exit(1);
    }

    /// SHADER SETUP
    ID3DBlob* vs = nullptr;
    ID3DBlob* errBlob = nullptr;
    HRESULT res = D3DCompileFromFile(L"../games/assets/spacefight/shaders/basic.hlsl", NULL, NULL, "VShader", "vs_5_0", 0, 0, &vs, &errBlob);
    if (FAILED(res)) {
        OutputDebugStringW(L"shader load failed\n");
        cwprintf("vshader load failed\n");
        if (errBlob)
        {
            OutputDebugStringA((char*)errBlob->GetBufferPointer());
            safeRelease(&errBlob);
        }

        if (vs)
            safeRelease(&vs);

        exit(1);
    }
    ID3DBlob* ps = nullptr;
    res = D3DCompileFromFile(L"../games/assets/spacefight/shaders/basic.hlsl", NULL, NULL, "PShader", "ps_5_0", 0, 0, &ps, &errBlob);
    if (FAILED(res)) {
        OutputDebugString("shader load failed\n");
        cwprintf("pshader load failed\n");
        if (errBlob)
        {
            OutputDebugStringA((char*)errBlob->GetBufferPointer());
            safeRelease(&errBlob);
        }

        if (ps)
            safeRelease(&ps);

        exit(1);
    }

    CreateVertexShader(engineData->renderer->getDevice(), vs, &engineData->vShader);
    CreatePixelShader(engineData->renderer->getDevice(), ps, &engineData->pShader);

    /// END SHADER SETUP

    D3D11_INPUT_ELEMENT_DESC ied[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            //{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },	// same slot, but 12 bytes after the pos
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },		// other slot (buffer), starting at 0
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }		// other slot (buffer), starting at 0
    };
    res = const_cast<ID3D11Device*>(engineData->renderer->getDevice())->CreateInputLayout(ied, 3, vs->GetBufferPointer(), vs->GetBufferSize(), &engineData->inputLayout);
    if (FAILED(res)) {
        OutputDebugString("input layout in main failed\n");
        cwprintf("input layout failed\n");
        exit(1);
    }

//    safeRelease(&ps);
//    safeRelease(&vs);
//    safeRelease(&errBlob);

    return engineData;
}

void shutdownEngine(EngineData* engineData) {
    if (engineData->renderer) delete(engineData->renderer);
    engineData->renderer = nullptr;

    // Do we even care to release this here as the program ends?
	safeRelease(&engineData->inputLayout);
	safeRelease(&engineData->pShader);
	safeRelease(&engineData->vShader);


}


