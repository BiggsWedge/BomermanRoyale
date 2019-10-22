#pragma once

#ifndef CONST_EXPRESSIONS
#define CONST_EXPRESSIONS
#include <vector>

struct AnimFileName
{
	const char* file;
	std::string animationName;
	bool loopable;
};

static const std::vector<const wchar_t*> diffuseTextures =
{
	L".//Assets//Cube.fbm//Crate.jpg",
	L".//Assets//BattleMage.fbm//PPG_3D_Player_D.png",
	L".//Assets//Menus.fbm//Chicken_Pic4.jpg",
	L".//Assets//Menus.fbm//Chicken_Pic5.jpg",
	L".//Assets//Menus.fbm//In-Game Barnyard Blast.png",
	L".//Assets//Menus.fbm//Playe Status.png",
	L".//Assets//Menus.fbm//Player1Win.png",
	L".//Assets//Menus.fbm//Player2Win.png",
	L".//Assets//Menus.fbm//Player3Win.png",
	L".//Assets//Menus.fbm//Player4Win.png",
	L".//Assets//Menus.fbm//DrawScreen.png",
	L".//Assets//Menus.fbm//HowToPlay.png",
	L".//Assets//Menus.fbm//DC1.png",
	L".//Assets//Menus.fbm//DC2.png",
	L".//Assets//Menus.fbm//DC3.png",
	L".//Assets//Menus.fbm//DC4.png",
	L".//Assets//Menus.fbm//GPGlogo_solid.jpg",
	L".//Assets//Menus.fbm//Full_Sail_Logo.jpg",
	L".//Assets//Menus.fbm//Pause_Menu.png",
	L".//Assets//Menus.fbm//Controls2.png",
	L".//Assets//Menus.fbm//Arcade_Menu4.png",
	L".//Assets//Menus.fbm//Arcade_Menu5.png",
	L".//Assets//Menus.fbm//Arcade_Menu6.png",
	L".//Assets//Menus.fbm//Arcade_Menu7.png",
	L".//Assets//Menus.fbm//Arcade_Menu8.png",
	L".//Assets//Menus.fbm//Arcade_Menu9.png",
	L".//Assets//Menus.fbm//Arcade_Menu10.png",
	L".//Assets//Menus.fbm//Arcade_Menu11.png",
	L".//Assets//Menus.fbm//Arcade_Menu12.png",
	L".//Assets//Menus.fbm//Customization_Screen1.png",
	L".//Assets//Menus.fbm//Customization_Screen2.png",
	L".//Assets//Menus.fbm//Customization_Screen3.png",
	L".//Assets//Menus.fbm//Customization_Screen4.png",
	L".//Assets//Menus.fbm//Customization_Screen5.png",
	L".//Assets//Menus.fbm//Customization_Screen6.png",
	L".//Assets//Menus.fbm//Customization_Screen7.png",
	L".//Assets//Menus.fbm//Customization_Screen8.png",
	L".//Assets//Menus.fbm//ArcadeButton.png",
	L".//Assets//Menus.fbm//BattleButton.png",
	L".//Assets//Menus.fbm//OptionsButton.png",
	L".//Assets//Menus.fbm//ExitButton.png",
	L".//Assets//Cube.fbm//red_texture.jpg",
	L".//Assets//Cube.fbm//grass2.png",
	L".//Assets//Cube.fbm//dirt4.png",
	L".//Assets//Cube.fbm//fire.jpg",
	L".//Assets//Cube.fbm//Hay.jpg",
	L".//Assets//bomb.fbm//bomb.png",
	L".//Assets//bomb.fbm//bomb2.png",
	L".//Assets//bomb.fbm//bomb3.png",
	L".//Assets//bomb.fbm//bomb4.png",
	L".//Assets//bomb.fbm//bomb.png",
	L".//Assets//bomb.fbm//bomb2Desc.png",
	L".//Assets//bomb.fbm//bomb3Desc.png",
	L".//Assets//bomb.fbm//bomb4Desc.png",
	L".//Assets//bomb.fbm//bomb5Desc.png",
	L".//Assets//Chicken.fbm//ChickenP1.png",
	L".//Assets//Chicken.fbm//ChickenP2.png",
	L".//Assets//Chicken.fbm//ChickenP3.png",
	L".//Assets//Chicken.fbm//ChickenP4.png",
	L".//Assets//Fire//fire1_ 01.png",
	L".//Assets//Fire//fire1_ 02.png",
	L".//Assets//Fire//fire1_ 03.png",
	L".//Assets//Fire//fire1_ 04.png",
	L".//Assets//Fire//fire1_ 05.png",
	L".//Assets//Fire//fire1_ 06.png",
	L".//Assets//Fire//fire1_ 07.png",
	L".//Assets//Fire//fire1_ 08.png",
	L".//Assets//Fire//fire1_ 09.png",
	L".//Assets//Fire//fire1_ 10.png",
	L".//Assets//Fire//fire1_ 11.png",
	L".//Assets//Fire//fire1_ 12.png",
	L".//Assets//Fire//fire1_ 13.png",
	L".//Assets//Goat.fbm//Goat.png",
	L".//Assets//Boar.fbm//Boar.png",
	L".//Assets//Boar.fbm//Boar2.png",
	L".//Assets//Chicken.fbm//ChickenNormal.png",
	L".//Assets//Goat.fbm//GoatNormal.png",
	L".//Assets//Boar.fbm//BoarNormal.png",
};

struct TMeshLoadInfo
{
	std::string name;
	const char* meshFile;
	const char* matFile;
	const char* animFile;
};

static const std::vector<AnimFileName> ChickenAnimFiles
{
	{".//Assets//ChickenIdle.anim","Idle", true},
	{".//Assets//ChickenEat.anim","Eat", true},
	{".//Assets//ChickenWalk.anim","Walk", true},
	{".//Assets//ChickenRun.anim","Run", true},
	{".//Assets//ChickenDie.anim","Die", false}
};

static const std::vector<AnimFileName> GoatAnimFiles
{
	{".//Assets//GoatIdle.anim", "Idle",true},
	{".//Assets//GoatWalk.anim", "Walk",true},
	{".//Assets//GoatRun.anim", "Run",true},
	{".//Assets//GoatDie.anim", "Die",false}
};

static const std::vector<AnimFileName> BoarAnimFiles
{
	{".//Assets//BoarIdle.anim", "Idle",true},
	{".//Assets//BoarWalk.anim", "Walk",true},
	{".//Assets//BoarRun.anim", "Run", true},
	{".//Assets//BoarDie.anim", "Die", false}
};

static const std::vector<AnimFileName> RabbitAnimFiles
{
	{".//Assets//RabbitIdle.anim", "Idle",true},
	{".//Assets//RabbitWalk.anim", "Walk",true},
	{".//Assets//RabbitRun.anim", "Run", true},
	{".//Assets//RabbitDie.anim", "Die", false}

};

static const std::vector<TMeshLoadInfo> modelLoadInfos
{
	{"Cube", ".//Assets//Cube.mesh", nullptr, nullptr},
	{"BattleMage", ".//Assets//BattleMage.mesh", ".\\Assets\\BattleMage.mat", ".\\Assets\\BattleMage.anim"},
	{"Bomb", ".//Assets//Bomb.mesh", nullptr, nullptr},
	{"Bomb2", ".//Assets//Bomb2.mesh", nullptr, nullptr},
	{"Chicken", ".//Assets//Chicken.mesh", nullptr, nullptr},
	{"Goat", ".//Assets//Goat.mesh", nullptr, nullptr},
	{"Boar", ".//Assets//Boar.mesh", nullptr, nullptr},
	{"Rabbit", ".//Assets//Rabbit.mesh", nullptr, nullptr}
};

struct MODELS
{
	enum { CUBE = 0, BATTLEMAGE, BOMB, BOMB2, CHICKEN, GOAT, BOAR, RABBIT, MENU1, MENU2, COUNT };
};

struct PLAYER_MODELS
{
	enum { CHICKEN, GOAT, BOAR};
};

#endif // !CONST_EXPRESSIONS
