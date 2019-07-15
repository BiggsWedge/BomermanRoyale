/****************************************************
*	Filename:			CRenderer.h
*	Date:				07/11/2019
*	Mod. Date:			07/15/2019
*	Mod. Initials:		D.S.
*	Author:				Dominic Sondgeroth
*	Purpose:			This is the header file for
*						the Renderer class and holds
*						all of its declerations and
*						members
****************************************************/
#pragma once

#include "customUtil.h"

class CRenderer
{
private:

	struct TImpl;

	TImpl* p_tImpl;

public:

	/***********************************************************
	*	CRenderer()
	*							Default Constructor
	*	Ins:
	*							None
	*	Outs:
	*							None
	*	Returns:
	*							None
	*
	*	Mod. Date:				7/11/2019
	*	Mod. Initials:			D.S.
	***********************************************************/
	CRenderer();

	/***********************************************************
	*	Initialize()
	*							Initializes the Renderer. Returns
	*							true if initialized successfully
	*							and false if not.
	*	Ins:
	*							GWindow* window_handle
	*	Outs:
	*							None
	*	Returns:
	*							bool
	*
	*	Mod. Date:				07/15/2019
	*	Mod. Initials:			D.S.
	***********************************************************/
	bool Initialize(GW::SYSTEM::GWindow* window_handle);

	/**********************************************************
	*	Update()
	*							Updates all relevent information
	*							the renderer needs to draw the
	*							objects
	*	Ins:
	*							None
	*	Outs:
	*							None
	*	Returns:
	*							void
	*
	*	Mod. Date:				07/15/2019
	*	Mod. Initials:			D.S.
	***********************************************************/
	void Update();

	/***************************************************************
	*	Draw()
	*							Draws to the screen
	*	Ins:
	*							None
	*	Outs:
	*							None
	*	Returns:
	*							void
	*
	*	Mod. Date:				07/15/2019
	*	Mod. Initials:			D.S.
	***************************************************************/
	void Draw();

	/**************************************************
	*	~CRenderer()
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
	***************************************************/
	~CRenderer();

	/********************************************************
	*	Cleanup()
	*							Cleans up the Renderer. Called
								before delete
	*	Ins:
	*							None
	*	Outs:
	*							None
	*	Returns:
	*							void
	*
	*	Mod. Date:				07/15/2019
	*	Mod. Initials:			D.S.
	**********************************************************/
	void Cleanup();
};

struct RENDER_TARGET_VIEW
{
	enum { DEFAULT = 0, COUNT };
};

struct VIEWPORT
{
	enum { DEFAULT = 0, COUNT };
};

struct CONSTANT_BUFFER
{
	enum { DEFAULT = 0, COUNT };
};

struct INDEX_BUFFER
{
	enum { DEFAULT = 0, COUNT };
};

struct VERTEX_BUFFER
{
	enum { DEFAULT = 0, COUNT };
};

struct VERTEX_SHADER
{
	enum { DEFAULT = 0, COUNT };
};

struct PIXEL_SHADER
{
	enum { DEFAULT = 0, COUNT };
};

struct GEOMETRY_SHADER
{
	enum { DEFAULT = 0, COUNT };
};

struct INPUT_LAYOUT
{
	enum { DEFAULT = 0, COUNT };
};

struct RASTERIZER_STATE
{
	enum { DEFAULT = 0, COUNT };
};

struct DEPTH_STENCIL_VIEW
{
	enum { DEFAULT = 0, COUNT };
};

struct DEPTH_STENCIL_STATE
{
	enum { DEFAULT = 0, COUNT };
};