#include <windows.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tchar.h>
#include "renderer.h"
#include "math_types.h"
#include "view.h"
#include <chrono>
#include <vector>
#include <bitset>
#include <windowsx.h>


using namespace std;
using namespace end;

static TCHAR szWindowClass[] = _T("DesktopApp");

static TCHAR szTitle[] = _T("Windows Desktop Guided Tour Application");

HINSTANCE hInst;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

float2 mouseDelta = { 0.0f, 0.0f };
int windowMiddle[2] = { 0,0 };
float sensitivity = 0.1f;

struct Key
{
	bool currState;
	bool prevState;
	int keyIdent;

	bool held() { return currState && prevState; }
	bool pressed() { return currState && !prevState; }
	bool released() { return !currState && prevState; }
	bool idle() { return !currState && !prevState; }
};

enum Keys
{
	KEY_A = 0,
	KEY_W,
	KEY_S,
	KEY_D,
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_O,
	KEY_R,
	KEY_M,
	KEY_SPACE,
	KEY_ENTER
};

vector<int> keyCodes =
{
	'A',
	'W',
	'S',
	'D',
	VK_UP,
	VK_DOWN,
	VK_LEFT,
	VK_RIGHT,
	'O',
	'R',
	'M',
	VK_SPACE,
	VK_RETURN
};

vector<Key> inputs(keyCodes.size());


int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{

	WNDCLASSEX wcex;



	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL, _T("Call to RegisterClassEx failed!"), _T("Windows Desktop Guided Tour"), NULL);
		return 1;
	}

	hInst = hInstance;



#ifndef NDEBUG

	AllocConsole();
	FILE* new_std_in_out;
	freopen_s(&new_std_in_out, "CONOUT$", "w", stdout);
	freopen_s(&new_std_in_out, "CONIN$", "r", stdin);
	cout << "Hello World!\n";

#endif // !NDEBUG
	HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, NULL, NULL, hInstance, NULL);


	for (int i = 0; i < 8; i++)
	{
		inputs[i].currState = inputs[i].prevState = false;
		inputs[i].keyIdent = i;
	}
	if (!hWnd)
	{
		MessageBox(NULL, _T("Call to CreateWindow failed!"), _T("Windows Desktop Guided Tour"), NULL);
		return 1;
	}


	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	renderer* _theRenderer = new renderer(hWnd);
	ShowCursor(false);
	//sorted_pool_t<particle, 1024> particles;

	RECT winRect;
	GetWindowRect(hWnd, &winRect);
	windowMiddle[0] = (winRect.left + winRect.right) / 2;
	windowMiddle[1] = (winRect.top + winRect.bottom) / 2;
	SetCursorPos(windowMiddle[0], windowMiddle[1]);
	HCURSOR cursor;
	cursor = GetCursor();
	POINT cursorPos = {};


	float timePassed = 0;
	float frameTime = 0;
	long long prevFrame = 0;
	long long currFrame = 0;

	float2 movingTargetStats;
	float2 cameraMovement;

	MSG msg;
	while (true)
	{
		//Process all messages, stop on WM_QUIT
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			//WM_QUIT does not need to be translated or dispatched
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			for (int i = 0; i < inputs.size(); ++i)
			{
				inputs[i].prevState = inputs[i].currState;
				inputs[i].currState = GetAsyncKeyState(keyCodes[i]);
			}
#ifndef NDEBUG
			system("cls");
#endif // !NDEBUG
			prevFrame = currFrame;
			currFrame = clock();
			GetCursorPos(&cursorPos);
			if (GetFocus() == hWnd)
				mouseDelta = { (float)(cursorPos.x - windowMiddle[0]), (float)(cursorPos.y - windowMiddle[1]) };
			frameTime = ((float)currFrame - (float)prevFrame) / 1000.0f;

			/*
			if (inputs[KEY_O].pressed())
			{
				_theRenderer->default_view._viewMatrix = LookAt(float3(0.0f, 15.0f, 0.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 0.0f, 1.0f));
				_theRenderer->default_view._projetionMatrix = DirectX::XMMatrixOrthographicLH(32, 18, 0.1f, 1000.0f);
			}
			*/
			if (inputs[KEY_SPACE].pressed())
				_theRenderer->nextKeyFrame();

			if (inputs[KEY_R].pressed())
			{
				_theRenderer->default_view._viewMatrix = LookAt(float3(0.0f, 15.0f, -15.0f), float3(0.0f, 0.0f, 0.0f), float3(0.0f, 1.0f, 0.0f));
				_theRenderer->default_view._projetionMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(45), 1280.0f / 720.0f, 0.1f, 1000.0f);
			}
			if (inputs[KEY_M].pressed())
				_theRenderer->toggleRenderMode = true;
			if (inputs[KEY_ENTER].pressed())
				_theRenderer->toggleManualInput();

			//	movingTargetStats = float2((5.0f * frameTime * (inputs[KEY_UP].held() & 1)) + (-5.0f * frameTime * (inputs[KEY_DOWN].held() & 1)), ((-frameTime * (inputs[KEY_LEFT].held() & 1)) + (frameTime * (inputs[KEY_RIGHT].held() & 1))));

			cameraMovement = float2((5.0f * frameTime * (inputs[KEY_D].held() & 1)) + (-5.0f * frameTime * (inputs[KEY_A].held() & 1)), ((5.0f * frameTime * (inputs[KEY_W].held() & 1)) + (-5.0f * frameTime * (inputs[KEY_S].held() & 1))));


			_theRenderer->update(frameTime* 0.06f, { mouseDelta.y * sensitivity * frameTime, mouseDelta.x * sensitivity * frameTime }, movingTargetStats, cameraMovement);
			_theRenderer->draw();
			mouseDelta = { 0.0f, 0.0f };
			if (GetFocus() == hWnd)
				SetCursorPos(windowMiddle[0], windowMiddle[1]);
		}
	}

#ifndef NDEBUG
	FreeConsole();
#endif // !NDEBUG

	delete(_theRenderer);

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR greeting[] = _T("Hello, Windows desktop!");
	//mouseMovements = { 0.0f, 0.0f };
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
	{
		if (wParam == VK_ESCAPE)
			SendMessage(hWnd, WM_CLOSE, 0, 0);
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}