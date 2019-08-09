#pragma once

#ifndef CONST_EXPRESSIONS
#define CONST_EXPRESSIONS
#include <vector>

static const std::vector<const wchar_t*> diffuseTextures =
{
	L".//Assets//Cube.fbm//Crate.jpg",
	L".//Assets//BattleMage.fbm//PPG_3D_Player_D.png",
	L".//Assets//Menus.fbm//Menu.png",
	L".//Assets//Menus.fbm//In-Game Barnyard Blast.png",
	L".//Assets//Menus.fbm//Playe Status.png",
	L".//Assets//Menus.fbm//Win_Screen.png",
	L".//Assets//Menus.fbm//Controls.png",
	L".//Assets//Menus.fbm//ArcadeButton.png",
	L".//Assets//Menus.fbm//BattleButton.png",
	L".//Assets//Menus.fbm//OptionsButton.png",
	L".//Assets//Menus.fbm//ExitButton.png",
	L".//Assets//Cube.fbm//red_texture.jpg",
	L".//Assets//Cube.fbm//blue_texture.jpg",
	L".//Assets//Cube.fbm//black_texture.jpg",
	L".//Assets//Cube.fbm//fire.jpg",
	L".//Assets//Cube.fbm//Hay.jpg",
	L".//Assets//bomb.fbm//Bomb.png",
	L".//Assets//Chicken.fbm//ChickenP1.png",
	L".//Assets//Chicken.fbm//ChickenP2.png"
};

struct TMeshLoadInfo
{
	std::string name;
	const char* meshFile;
};

static const std::vector<TMeshLoadInfo> modelLoadInfos =
{
	{"Cube", ".//Assets//Cube.mesh"},
	{"BattleMage", ".//Assets//BattleMage.mesh"},
	{"Bomb", ".//Assets//Bomb.mesh"},
	{"Chicken", ".//Assets//Chicken.mesh"}
};

struct MODELS
{
	enum { CUBE = 0, BATTLEMAGE, BOMB, CHICKEN, MENU1, MENU2, COUNT };
};

#endif // !CONST_EXPRESSIONS