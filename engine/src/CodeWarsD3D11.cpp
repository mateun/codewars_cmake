// CodeWarsD3D11.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "CodeWarsD3D11.h"
#include "renderer.h"
#include <d3dcompiler.h>
#include "shaders.h"
#include <FreeImage.h>
#include "textures.h"
#include "game.h"
#include "model_import.h"
#include "resource_management.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
Renderer* renderer;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_CODEWARSD3D11, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CODEWARSD3D11));

#pragma region splash_and_loading_screen


	XMMATRIX modelMat = DirectX::XMMatrixScaling(2.5, 2.5, 2.5);

	XMFLOAT3 eyePos = XMFLOAT3(0, 0, -55);
	XMFLOAT3 eyeDir = XMFLOAT3(0, 0, 1);
	XMFLOAT3 upDir = XMFLOAT3(0, 1, 0);
	XMMATRIX viewMat = DirectX::XMMatrixLookToLH(XMLoadFloat3(&eyePos), XMLoadFloat3(&eyeDir), XMLoadFloat3(&upDir));
	XMMATRIX projMat = DirectX::XMMatrixPerspectiveFovLH(0.45f, 4.0f / 3.0f, 0.1f, 100.0f);
	modelMat = XMMatrixTranspose(modelMat);
	viewMat = XMMatrixTranspose(viewMat);
	projMat = XMMatrixTranspose(projMat);

	/// SHADER SETUP
	ID3DBlob* vs = nullptr;
	ID3DBlob* errBlob = nullptr;
	HRESULT res = D3DCompileFromFile(L"shaders/basic.hlsl", NULL, NULL, "VShader", "vs_5_0", 0, 0, &vs, &errBlob);
	if (FAILED(res)) {
		OutputDebugStringW(L"shader load failed\n");
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
	res = D3DCompileFromFile(L"shaders/basic.hlsl", NULL, NULL, "PShader", "ps_5_0", 0, 0, &ps, &errBlob);
	if (FAILED(res)) {
		OutputDebugString("shader load failed\n");
		if (errBlob)
		{
			OutputDebugStringA((char*)errBlob->GetBufferPointer());
			safeRelease(&errBlob);
		}

		if (ps)
			safeRelease(&ps);

		exit(1);
	}
	ID3D11VertexShader* vshader;
	CreateVertexShader(renderer->getDevice(), vs, &vshader);
	ID3D11PixelShader* pShader;
	CreatePixelShader(renderer->getDevice(), ps, &pShader);
	/// END SHADER SETUP


	/// INPUT LAYOUT SETUP
	D3D11_INPUT_ELEMENT_DESC ied[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		//{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },	// same slot, but 12 bytes after the pos
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },		// other slot (buffer), starting at 0

	};
	ID3D11InputLayout* inputLayout;
	const_cast<ID3D11Device*>(renderer->getDevice())->CreateInputLayout(ied, 2, vs->GetBufferPointer(), vs->GetBufferSize(), &inputLayout);
	/// END INPUT LAYOUT SETUP

	// RenderSplash
	ID3D11Texture2D* tex;
	loadTextureFromFile("textures/engine_splash.png", &tex, renderer);
	//loadTextureFromFile(GetIntroImageName(), &tex, renderer);

	modelMat = DirectX::XMMatrixScaling(2, 2, 2);
	//modelMat = XMMatrixMultiply(modelMat, XMMatrixTranslation(-0.0, 0.0, 0));

	XMMATRIX projMatSplash = DirectX::XMMatrixOrthographicLH(2.0f, 2.0f, 0.1f, 100.0f);
	// This IS IMPORTANT!!! Without this transposition, the quad is totally screwed :) !
	modelMat = XMMatrixTranspose(modelMat);
	projMatSplash = XMMatrixTranspose(projMatSplash);

	
	float clearColors[] = { 0.01f, 0.02f, 0.02f, 1.0f };
	std::vector<XMFLOAT3> mesh;
	mesh.push_back({ -.5, 0.5, 0 });
	mesh.push_back({ .5, -.5, 0 });
	mesh.push_back({ -.5, -.5, 0 });
	mesh.push_back({ .5, .5, 0 });
	std::vector<XMFLOAT2> uvs;
	uvs.push_back({ 0, 1 });
	uvs.push_back({ 1, 0 });
	uvs.push_back({ 0, 0 });
	uvs.push_back({ 1, 1 });
	std::vector<UINT> indices;
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(0);
	indices.push_back(3);
	indices.push_back(1);

	renderer->setViewport(0, 0, 800, 600);
	renderer->clearBackbuffer(clearColors);
	renderer->renderMesh(mesh, uvs, indices, modelMat, viewMat, projMatSplash, vshader, pShader, inputLayout, tex);
	renderer->presentBackBuffer();
	
	Sleep(2000);

	// render loading screen
	safeRelease(&tex);
	loadTextureFromFile(GetIntroImageName(), &tex, renderer);
	renderer->clearBackbuffer(clearColors);
	modelMat = DirectX::XMMatrixScaling(1.8f, 1.8f, 1.8f);
	projMatSplash = DirectX::XMMatrixOrthographicLH(2.0f, 2.0f, 0.1f, 100.0f);
	renderer->renderMesh(mesh, uvs, indices, modelMat, viewMat, projMatSplash, vshader, pShader, inputLayout, tex);
	renderer->presentBackBuffer();
	Sleep(3000);

	// Init the game
	Game* game = GetGame();
	if (!game) {
		OutputDebugString("No game was provided via GetGame function\n");
		exit(1);
	}

	game->Init(*renderer);

	
	float rotZ = 0.0f;
	
	#pragma endregion

	// Main message loop when done with the static resources
	MSG msg;
	while (true) { 
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT) break;

#pragma region test_engine_frame
		

		//renderer->clearBackbuffer(clearColors);
		//renderer->setViewport(0, 0, 800, 600);
		//renderer->renderMesh(imp_pos, imp_uvs, imp_indices, modelMat, viewMat, projMat, vshader, pShader, inputLayout, nullptr);
		//renderer->presentBackBuffer();
#pragma endregion

		game->DoFrame(*renderer);

		

	}

	safeRelease(&tex);
	safeRelease(&inputLayout);
	safeRelease(&pShader);
	safeRelease(&vshader);
	safeRelease(&ps);
	safeRelease(&vs);
	safeRelease(&errBlob);
	game->ShutDown();
	if (renderer) delete(renderer);
	
    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASS wcex;

    //wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CODEWARSD3D11));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = "menu"; //MAKEINTRESOURCEW(IDC_CODEWARSD3D11);
    wcex.lpszClassName  = "winclass";
    //wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClass(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindow("winclass", 
					"codewars", 
					WS_OVERLAPPEDWINDOW,
					CW_USEDEFAULT, 0, 
					800, 600, 
					nullptr, nullptr, 
					hInstance, 
					nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   renderer = new Renderer(800, 600, hWnd);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_KEYDOWN: 
		{
			if (wParam == VK_ESCAPE) DestroyWindow(hWnd);
		}
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
