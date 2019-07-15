/****************************************************
*	Filename:			main.cpp
*	Date:				07/11/2019
*	Mod. Date:			07/11/2019;
*	Mod. Initials:		D.S.
*	Author:				Dominic Sondgeroth
*	Purpose:			This file is the main
*						entrypoint for the game
****************************************************/
#include "CGame.h"




int main(void)
{
	InitializeLogger();

	CGame* p_cGame = new CGame();

	bool bGameInitializedCorrectly;

	bGameInitializedCorrectly = p_cGame->Initialize();

	if (!bGameInitializedCorrectly)
		return 1;

	p_cGame->Run();


	
	delete p_cGame;
	g_pcLogger->DecrementCount();
	return 0;
}






