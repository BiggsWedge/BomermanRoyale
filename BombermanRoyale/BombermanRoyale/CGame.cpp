/****************************************************
*	Filename:			CGame.cpp
*	Date:				07/11/2019
*	Mod. Date:			07/15/2019
*	Mod. Initials:		D.S.
*	Author:				Dominic Sondgeroth
*	Purpose:			This is the impelmentation
*						file that is the companion
*						to the CGame.h file;
****************************************************/

#include "CGame.h"

using namespace GW;
using namespace SYSTEM;
using namespace CORE;


CGame::CGame()
{

}

bool CGame::Initialize()
{
	if (!G_SUCCESS(CreateGWindow(100, 100, 1024, 720, GWindowStyle::WINDOWEDBORDERED, &p_cWindow)))
	{
		g_pcLogger->LogCatergorized("FAILURE", "Failed to create the window");
		return false;
	}
	else
		g_pcLogger->LogCatergorized("SUCCESS", "Successfully created the window");

	p_cRenderer = new CRenderer();
	if (!p_cRenderer->Initialize(p_cWindow))
	{
		g_pcLogger->LogCatergorized("FAILURE", "Failed to initialize Renderer");
		return false;
	}

	HWND window_handle;

	if (!G_SUCCESS(p_cWindow->GetWindowHandle(sizeof(HWND), (void**)&window_handle)))
	{
		g_pcLogger->LogCatergorized("FAILURE", "Failed to get the window handle");
		return false;
	}

	return true;
}

void CGame::Run()
{
	GWindowInputEvents m_LastEvent;
	while (G_SUCCESS(p_cWindow->GetLastEvent(m_LastEvent)) && m_LastEvent != GW::SYSTEM::GWindowInputEvents::DESTROY)
	{
		if (G_FAIL(p_cWindow->ProcessWindowEvents()))
			break;

		p_cRenderer->Draw();
	}
	return;
}

CGame::~CGame()
{
	p_cRenderer->Cleanup();
	delete p_cRenderer;
	p_cRenderer = nullptr;
	p_cWindow->DecrementCount();
}
