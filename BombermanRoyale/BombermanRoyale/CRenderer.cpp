/****************************************************
*	Filename:			CRenderer.cpp
*	Date:				07/11/2019
*	Mod. Date:			07/12/2019;
*	Mod. Initials:		D.S.
*	Author:				Dominic Sondgeroth
*	Purpose:			This is the impelmentation
*						file that is the companion
*						to the CRenderer.h file;
****************************************************/

#include "TImpl.h"


CRenderer::CRenderer()
{
}


bool CRenderer::Initialize(GW::SYSTEM::GWindow* window_handle)
{
	p_tImpl = new TImpl();
	return p_tImpl->Intialize(window_handle);
}

CRenderer::~CRenderer()
{
}

void CRenderer::Cleanup()
{
	p_tImpl->Cleanup();
}

void CRenderer::Update()
{

}

void CRenderer::Draw()
{
	p_tImpl->Render();
}