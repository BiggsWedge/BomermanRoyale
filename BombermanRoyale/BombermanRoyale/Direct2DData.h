#pragma once

#include <dwrite.h>
#include <d2d1.h>
#include "Utilities.h"

HRESULT hr;
HWND _hwnd;

//UI Objects
IDWriteFactory* pDWriteFactory_ = nullptr;
IDWriteTextFormat* pTextFormat_ = nullptr;
ID2D1Factory* pD2DFactory_ = nullptr;
ID2D1HwndRenderTarget* pRT_ = nullptr;
ID2D1SolidColorBrush* pBlackBrush_ = nullptr;
const wchar_t* wszText_;
UINT32 cTextLength_;
UINT32 width;
UINT32 height;

HRESULT CreateDeviceIndependentResources();
HRESULT CreateDeviceDependentResources();
HRESULT DrawUI();
