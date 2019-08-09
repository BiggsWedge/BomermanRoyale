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



int main(void)
{
	InitializeGlobals();

	LoadTextures();
	LoadModels();
	v_tMeshTemplates[1].loadModel(".\\Assets\\BattleMage.mesh", ".\\Assets\\BattleMage.mat", ".\\Assets\\BattleMage.anim");
	v_tMeshTemplates[1].initialize(g_d3dData->d3dDevice);
	LoadMenuScreen(15, 11, 0, ".//Assets//Menu.mat");
	LoadMenuScreen(19, 11, 1, ".//Assets//Menu.mat");



	CGame* p_cGame = new CGame();
	bool result = p_cGame->Initialize();
	if (!result)
	{
		g_pLogger->LogCatergorized("FAILURE", "Failed to initialize the game");
		return 1;
	}
	g_pLogger->LogCatergorized("SUCCESS", "Successfully initialized the game");
	p_cGame->LoadObject();
	p_cGame->Run();

	delete p_cGame;
	g_d3dData->Cleanup();

	v_tMeshTemplates.clear();



	g_pWindow->DecrementCount();
	g_pLogger->DecrementCount();

	return 0;
}





