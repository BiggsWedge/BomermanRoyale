/****************************************************
*	Filename:			CustomUtil.h
*	Date:				07/11/2019
*	Mod. Date:			07/12/2019;
*	Mod. Initials:		D.S.
*	Author:				Dominic Sondgeroth
*	Purpose:			This file contails some
*						functions and varaible that
*						are not otherwise specific
*						to a particular class or file
****************************************************/
#pragma once

#include <ctime>
#include <string>
#include <d3d11.h>
#include "..\Gateware\Interface\G_System\GLog.h"
#include "..\Gateware\Interface\G_System\GWindow.h"
#include "..\Gateware\Interface\G_Graphics\GDirectX11Surface.h"

#define SAFE_RELEASE(ptr) {if(ptr) {ptr->Release(); ptr = nullptr;}}

extern GW::SYSTEM::GLog* g_pcLogger;

/***********************************************************************
*	GetCurrentDateAndTime():	Returns, in a string format, the
*								current date and time.
*	Ins:						none
*
*	Outs:						none
*
*	Returns:					string
*
*	Mod. Date:					07/11/2019
*	Mod. Initials:				D.S.
***********************************************************************/
std::string GetCurrentDateAndTime();

/***********************************************************************
*	InitializeLogger():			Initializes the errorlogger.
								Returns true if successfully initialized
								or false if not.
*	Ins:						none
*
*	Outs:						none
*
*	Returns:					bool
*
*	Mod. Date:					07/12/2019
*	Mod. Initials:				D.S.
***********************************************************************/
bool InitializeLogger();
