/****************************************************
*	Filename:			CGame.h
*	Date:				07/11/2019
*	Mod. Date:			07/15/2019
*	Mod. Initials:		D.S.
*	Author:				Dominic Sondgeroth
*	Purpose:			This is the header file for
*						the Game class and holds
*						all of its declerations and
*						members
****************************************************/
#pragma once

#include "CRenderer.h"

class CGame
{
private:

	GW::SYSTEM::GWindow*			p_cWindow = nullptr;
	CRenderer*						p_cRenderer;

public:

	/*****************************************************
	*	CGame()
	*							Default Constructor
	*	Ins:
	*							None
	*	Outs:
	*							None
	*	Returns:
	*							None
	*
	*	Mod. Date:				07/15/2019
	*	Mod. Initials:			D.S.
	*****************************************************/
	CGame();

	/*****************************************************
	*	Initialize()
	*							Initializes the Game,
	*							returns true if the Game
	*							was initialized properly,
	*							false if not
	*	Ins:
	*							None
	*	Outs:
	*							None
	*	Returns:
	*							bool
	*
	*	Mod. Date:				07/15/2019
	*	Mod. Initials:			D.S.
	*****************************************************/
	bool Initialize();

	/****************************************************
	*	Run()
	*							Runs the Game.
	*	Ins:
	*							None
	*	Outs:
	*							None
	*	Returns:
	*							void
	*
	*	Mod. Date:				07/15/2019
	*	Mod. Intials:			D.S.
	****************************************************/
	void Run();

	/***********************************************************
	*	~CGame()
	*							Destructor
	*	Ins:
	*							None
	*	Outs:
	*							None
	*	Returns:
	*							None
	*
	*	Mod. Date:				07/15/2019
	*	Mod. Initials:			D.S.
	***********************************************************/
	~CGame();
};


