/****************************************************
*	Filename:			main.cpp
*	Mod. Date:			07/27/2019;
*	Mod. Initials:		D.S.
*	Author:				Dominic Sondgeroth
*	Purpose:			This file is the main
*						entrypoint for the game
****************************************************/
#include "CGame.h"
#include <DirectXCollision.h>
#define _CRTDBG_MAP_ALLOC


int main(void)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetBreakAlloc(-1); //Important!


#ifndef _DEBUG
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
	InitializeGlobals();
	LoadTextures();
	LoadModels();
	v_tMeshTemplates[MODELS::BATTLEMAGE].loadModel(".\\Assets\\BattleMage.mesh", ".\\Assets\\BattleMage.mat", ".\\Assets\\BattleMage.anim", 0.5f);
	v_tMeshTemplates[MODELS::BATTLEMAGE].initialize(g_d3dData->d3dDevice);
	LoadMenuScreen(15, 11, 0, ".//Assets//Menu.mat");
	LoadMenuScreen(19, 11, 0, ".//Assets//Menu.mat");

	CGame* p_cGame = new CGame();
	bool result = p_cGame->Initialize();
	if (!result)
	{
		g_pLogger->LogCatergorized("FAILURE", "Failed to initialize the game");
		return 1;
	}
	g_pLogger->LogCatergorized("SUCCESS", "Successfully initialized the game");

	p_cGame->loadTempMenus();

	p_cGame->Run();

	p_cGame->Cleanup();
	delete p_cGame;
	p_cGame = nullptr;

	CleanGlobals();
	return 0;
}

