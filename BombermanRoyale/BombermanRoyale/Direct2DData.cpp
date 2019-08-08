#include "Direct2DData.h"



HRESULT CreateDeviceIndependentResources()
{
	g_pWindow->GetWidth(width);
	g_pWindow->GetWidth(height);
	
	g_pWindow->GetWindowHandle(sizeof(HWND), );
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory_);
	if (SUCCEEDED(hr))
	{
		hr = DWriteCreateFactory
		(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown**>(&pDWriteFactory_)
		);
	}

	wszText_ = L"Hello World using  DirectWrite!";
	cTextLength_ = (UINT32)wcslen(wszText_);

	if (SUCCEEDED(hr))
	{
		hr = pDWriteFactory_->CreateTextFormat(
			L"Gabriola",                // Font family name.
			NULL,                       // Font collection (NULL sets it to use the system font collection).
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			72.0f,
			L"en-us",
			&pTextFormat_
		);
	}

	if (SUCCEEDED(hr))
	{
		hr = pTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	}

	if (SUCCEEDED(hr))
	{
		hr = pTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}
	return hr;
}


HRESULT CreateDeviceDependentResources()
{
	D2D1_SIZE_U size = D2D1::SizeU(width, height);

	if (!pRT_)
	{
		 Create a Direct2D render target.
		hr = pD2DFactory_->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(_hwnd, size), &pRT_);

		 Create a black brush.
		if (SUCCEEDED(hr))
		{
			hr = pRT_->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Black),
				&pBlackBrush_
			);
		}
	}
	return hr;
}


HRESULT DrawUI()
{
	hr = CreateDeviceIndependentResources();
	hr = CreateDeviceDependentResources();

	if (SUCCEEDED(hr))
	{
		pRT_->BeginDraw();

		pRT_->SetTransform(D2D1::IdentityMatrix());

		pRT_->Clear(D2D1::ColorF(D2D1::ColorF::White));

		//Actual DRAW
		D2D1_RECT_F layoutRect = D2D1::RectF(static_cast<FLOAT>);


		pRT_->DrawTextW(
			wszText_,        // The string to render.
			cTextLength_,    // The string's length.
			pTextFormat_,    // The text format.
			layoutRect,       // The region of the window where the text will be rendered.
			pBlackBrush_     // The brush used to draw the text.
		);


		if (SUCCEEDED(hr))
		{
			hr = pRT_->EndDraw(
			);
		}
	}

	return hr;
}