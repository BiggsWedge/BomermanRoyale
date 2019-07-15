/****************************************************
*	Filename:			CustomUtil.cpp
*	Date:				07/11/2019
*	Mod. Date:			07/12/2019;
*	Mod. Initials:		D.S.
*	Author:				Dominic Sondgeroth
*	Purpose:			This is the impelmentation
*						file that is the companion
*						to the customUtil.h file;
****************************************************/
#include "customUtil.h"

GW::SYSTEM::GLog* g_pcLogger = nullptr;

std::string GetCurrentDateAndTime()
{
	time_t now = time(0);
	tm lmt;
	localtime_s(&lmt, &now);


	std::string currDateTime;
	currDateTime.clear();
	currDateTime.append(std::to_string(1 + lmt.tm_mon));
	currDateTime.append(".");
	currDateTime.append(std::to_string(lmt.tm_mday));
	currDateTime.append(".");
	currDateTime.append(std::to_string(1900 + lmt.tm_year));
	currDateTime.append("_");
	currDateTime.append(std::to_string(lmt.tm_hour));
	currDateTime.append(".");
	currDateTime.append(std::to_string(lmt.tm_min));
	currDateTime.append(".");
	currDateTime.append(std::to_string(lmt.tm_sec));

	return currDateTime;
}

bool InitializeLogger()
{
	std::string cOutFileName;
	cOutFileName.append("ErrorLog");
	cOutFileName.append(GetCurrentDateAndTime());
	cOutFileName.append(".log");

	if (G_SUCCESS(CreateGLog(cOutFileName.c_str(), &g_pcLogger)))
	{
		g_pcLogger->EnableConsoleLogging(true);
		g_pcLogger->EnableVerboseLogging(true);
		g_pcLogger->Log("Error Logging system successfully initialized.");
		return true;
	}
	else
		return false;
}