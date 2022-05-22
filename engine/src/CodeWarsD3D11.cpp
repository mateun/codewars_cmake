// CodeWarsD3D11.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "CodeWarsD3D11.h"
#include "renderer.h"
#include <d3dcompiler.h>
#include "shaders.h"
#include <FreeImage.h>
#include <textures.h>
#include "game.h"
#include "model_import.h"
#include "resource_management.h"
#include "input.h"
#include <dinput.h>
#include <chrono>
#include <logging.h>
#include "engine.h"

#include <consoleprint.h>


#define MAX_LOADSTRING 100


// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

constexpr UINT WIDTH = 1920;
constexpr UINT HEIGHT = 1080;

// DINPUT
LPDIRECTINPUT8 di = nullptr;
LPDIRECTINPUTDEVICE8 diKeyboard = nullptr;
LPDIRECTINPUTDEVICE8 diMouse = nullptr;
DIPROPDWORD diProps;
bool keyState[256];
DIMOUSESTATE mouseState;
int mouseMovX, mouseMovY;
// end DINPUT

static bool run = true;
static EngineData* engineData = nullptr;

void initDirectInput(HINSTANCE hinstance, HWND hwnd) {
	HRESULT res = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&di, nullptr);
	if (FAILED(res)) {
		OutputDebugString("failed to create directinput8\n");
		exit(1);
	}

	// keyboard
	res = di->CreateDevice(GUID_SysKeyboard, &diKeyboard, nullptr);
	if (FAILED(res)) { OutputDebugString("failed to create keyboard device\n"); exit(1); }
	res = diKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(res)) { OutputDebugString("failed to set default data format for keyboard\n"); exit(1); }
	res = diKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(res)) { OutputDebugString("failed to set cooperative level\n"); exit(1); }


	// mouse
	res = di->CreateDevice(GUID_SysMouse, &diMouse, nullptr);
	if (FAILED(res)) { OutputDebugString("failed to create mouse device\n"); exit(1); }
	res = diMouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(res)) { OutputDebugString("failed to set default data for mouse device\n"); exit(1); }
	res = diMouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(res)) { OutputDebugString("failed to set cooperative level for mouse device\n"); exit(1); }

	ZeroMemory(&diProps, sizeof(diProps));
	diProps.diph.dwSize = sizeof(DIPROPDWORD);
	diProps.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	diProps.diph.dwObj = 0;
	diProps.diph.dwHow = DIPH_DEVICE;
	// buffer size
	diProps.dwData = 16;
	diMouse->SetProperty(DIPROP_BUFFERSIZE, &diProps.diph);


}

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


void initSplash() {

    auto game = GetGame();

    XMMATRIX modelMat = DirectX::XMMatrixScaling(2.5, 2.5, 2.5);
    XMFLOAT3 eyePos = XMFLOAT3(0, 0, -5);
    XMFLOAT3 eyeDir = XMFLOAT3(0, 0, 1);
    XMFLOAT3 upDir = XMFLOAT3(0, 1, 0);
    XMMATRIX viewMat =XMMatrixTranspose( XMMatrixLookToLH(XMLoadFloat3(&eyePos), XMLoadFloat3(&eyeDir), XMLoadFloat3(&upDir)));

    if (!game) {
        OutputDebugString("No game was provided via GetGame function\n");
        cwprintf("no game was provided via GetGame function\n");
        exit(1);
    }

    float clearColors[] = { 0.0f, 0.02f, 0.02f, 1.0f };
    XMMATRIX projMatSplash = XMMatrixTranspose(XMMatrixOrthographicOffCenterLH(0, game->getScreenWidth(),0, game->getScreenHeight(), 0.1f, 100.0f));


    Model planeModel;
    createPlaneModel(&planeModel, *engineData->renderer);
    engineData->renderer->setViewport(0, 0, game->getScreenWidth(), game->getScreenHeight());

    // RenderSplash
    {
        const std::string splashFile = "../engine/textures/engine_splash.png";
        Texture tex(splashFile, *engineData->renderer);

        auto matScale = XMMatrixScaling(game->getScreenWidth(), game->getScreenHeight(), 1);
        modelMat = XMMatrixTranspose(XMMatrixMultiply(matScale, XMMatrixTranslation(game->getScreenWidth()/2, game->getScreenHeight()/3, 0)));
        engineData->renderer->enableAlphaBlending(true);

        engineData->renderer->clearBackbuffer(clearColors);
        engineData->renderer->renderModel(planeModel, modelMat, viewMat, projMatSplash, tex);
        engineData->renderer->presentBackBuffer();
        Sleep(3000);
        float factor = 0.001;
        for (int i = 0; i < 5000; i++) {
            float col = 1.0f - (factor * (float)i);
            if (col < 0) col = 0;
            engineData->renderer->setAmbientColor({col, col, col, 1});
            engineData->renderer->clearBackbuffer(clearColors);
            engineData->renderer->renderModel(planeModel, modelMat, viewMat, projMatSplash, tex);
            engineData->renderer->presentBackBuffer();
        }

    }

    // Render loading screen
    {
        Texture tex(game->GetIntroImageName(), *engineData->renderer);

        auto mtrans = XMMatrixTranslation(game->getScreenWidth()/2, game->getScreenHeight()/2, 0);
        auto mscale = DirectX::XMMatrixScaling(game->getScreenWidth(), game->getScreenHeight(), 1);
        modelMat = XMMatrixTranspose(XMMatrixMultiply(mscale, mtrans));
        engineData->renderer->clearBackbuffer(clearColors);
        engineData->renderer->setAmbientColor({1, 1, 1, 1});
        engineData->renderer->renderModel(planeModel, modelMat, viewMat, projMatSplash, tex);
        engineData->renderer->presentBackBuffer();
        Sleep(4000);
    }

}

WPARAM mainLoop() {
    auto game = GetGame();
    ////////////////////////////////////////////////
    // Main message loop wrapping the game loop
    ////////////////////////////////////////////////
    MSG msg;
    UINT frameTime = 0;
    auto start = std::chrono::high_resolution_clock::now();
    while (run) {
        start = std::chrono::high_resolution_clock::now();

        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (msg.message == WM_QUIT) break;

        // gather the keyboard input
        FrameInput frameInput;
        HRESULT res = diKeyboard->GetDeviceState(sizeof(keyState), (void**)&keyState);
        if (FAILED(res)) {
            diKeyboard->Acquire();
        }
        frameInput.keyState = keyState;

        // gather mouse data - bit more complicated...
        DWORD numElements = 1;
        DIDEVICEOBJECTDATA data;
        ZeroMemory(&data, sizeof(data));

        res = diMouse->GetDeviceState(sizeof(mouseState), (void**)&mouseState);
        if (FAILED(res)) {
            diMouse->Acquire();
        }
        frameInput.relMouseMovX = mouseState.lX;
        frameInput.relMouseMovY = mouseState.lY;

        res = diMouse->GetDeviceData(sizeof(data), &data, &numElements, 0);
        if (FAILED(res)) {
            diMouse->Acquire();
        }
        switch (data.dwOfs) {
            case DIMOFS_BUTTON0:
                if (data.dwData) {
                    frameInput.mouse1Down = true; break;
                }
                else {
                    frameInput.mouse1Up = true; break;
                }
            case DIMOFS_BUTTON1:
                if (data.dwData) {
                    frameInput.mouse2Down = true; break;
                }
                else {
                    frameInput.mouse2Up = true; break;
                }
        }



        // end input gathering

        game->DoFrame(*getEngineData()->renderer, &frameInput, frameTime);
        frameTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();

#ifdef GAME_DEBUG
        char buf[2000];
		sprintf_s(buf, 2000, "============================frametime: %d\n", frameTime);
		OutputDebugString(buf);
#endif


        // TODO: this should come from the system refresh rate,
        // e.g. 60 HZ should give ~16
        UINT desiredFrameTime = 16;
        int timeToWait = desiredFrameTime - frameTime;
        if (timeToWait < 0) {
#ifdef GAME_DEBUG
            OutputDebugString("waittime longer than 16ms>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
#endif
            timeToWait = 0;
        }

        if (timeToWait > 0) {
#ifdef GAME_DEBUG
            sprintf_s(buf, 2000, "waitTime: %d\n", timeToWait);
			OutputDebugString(buf);
#endif
            // Sleep is not needed for vsync, but we need it to give a bit of breathing room for the machine,
            // otherwise it runs right away with > 40%.
            Sleep(timeToWait);
        }

    }

    return msg.wParam;
}





int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	initConsole();
	cwprintf("console initialized: %d\n", 1);
	
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

    initSplash();

    auto wparam = mainLoop();

	diKeyboard->Unacquire();
	safeRelease(&diKeyboard);
	safeRelease(&di);

    shutdownEngine(engineData);
	
    return (int) wparam;
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
	wcex.lpszMenuName = NULL;
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
    cwprintf("In init instance.\n");
   
    hInst = hInstance; // Store instance handle in our global variable

    auto game = GetGame();
    RECT corrRect = {0, 0, game->getScreenWidth(), game->getScreenHeight()};
    AdjustWindowRect(&corrRect, WS_OVERLAPPEDWINDOW, false);

    HWND hWnd = CreateWindow("winclass",
					"codewars", 
					WS_OVERLAPPEDWINDOW,
					CW_USEDEFAULT, 0,
                    corrRect.right - corrRect.left, corrRect.bottom - corrRect.top,
					nullptr, nullptr, 
					hInstance, 
					nullptr);

    if (!hWnd)
    {
       return FALSE;
    }
    cwprintf("Window created.\n");
    flog("Window created.\n");


    cwprintf("game created.\n");
    engineData = initEngine(game->getScreenWidth(), game->getScreenHeight(), hWnd);
    cwprintf("engine initialized.\n");
    game->Init(*getEngineData()->renderer);
    cwprintf("game initialized.\n");



    cwprintf("DI initialized.\n");
    flog("DI initialized.\n");

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    initDirectInput(hInst, hWnd);

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
