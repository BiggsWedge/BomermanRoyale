/****************************************************
*	Filename:			main.cpp
*	Date:				07/11/2019
*	Mod. Date:			07/16/2019;
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
	LoadModel(".//Assets//Cube.mesh", ".//Assets//Cube.mat");
	LoadModel(".//Assets//BattleMage.mesh", ".//Assets//BattleMage.mat");
	LoadMenuScreen(19, 11, 0, ".//Assets//Menu.mat");



	CGame* p_cGame = new CGame();
	p_cGame->Initialize();
	p_cGame->LoadObject();
	p_cGame->Run();

	delete p_cGame;
	g_d3dData->Cleanup();
	g_pWindow->DecrementCount();
	g_pLogger->DecrementCount();

	return 0;
}






