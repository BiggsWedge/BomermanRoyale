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
	L".//Assets//Menus.fbm//Chicken_Pic7.jpg",
	L".//Assets//Menus.fbm//Chicken_Pic8.jpg",
	L".//Assets//Menus.fbm//Chicken_Pic9.jpg",
	L".//Assets//Menus.fbm//Chicken_Pic10.jpg",
	L".//Assets//Menus.fbm//In-Game Barnyard Blast.png",
	L".//Assets//Menus.fbm//Screen_TopBar.png",
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
	L".//Assets//Menus.fbm//RUSHED_Development.png",
	L".//Assets//Menus.fbm//PauseScreenBackground.png",
	L".//Assets//Menus.fbm//Controls2.png",
	L".//Assets//Menus.fbm//Arcade_Menu4.0.png",
	L".//Assets//Menus.fbm//Arcade_Menu5.0.png",
	L".//Assets//Menus.fbm//Arcade_Menu6.0.png",
	L".//Assets//Menus.fbm//Arcade_Menu7.0.png",
	L".//Assets//Menus.fbm//Arcade_Menu8.0.png",
	L".//Assets//Menus.fbm//Arcade_Menu9.0.png",
	L".//Assets//Menus.fbm//Arcade_Menu10.0.png",
	L".//Assets//Menus.fbm//Arcade_Menu11.0.png",
	L".//Assets//Menus.fbm//Arcade_Menu12.0.png",
	/*L".//Assets//Menus.fbm//Customization_Screen1.png",*/
	L".//Assets//Menus.fbm//cs1.jpg",
	L".//Assets//Menus.fbm//cs2.jpg",
	L".//Assets//Menus.fbm//cs3.jpg",
	L".//Assets//Menus.fbm//cs4.jpg",
	L".//Assets//Menus.fbm//cs5.jpg",
	L".//Assets//Menus.fbm//cs6.jpg",
	L".//Assets//Menus.fbm//cs7.jpg",
	L".//Assets//Menus.fbm//cs8.jpg",
	L".//Assets//Menus.fbm//ArcadeButton.png",
	L".//Assets//Menus.fbm//BattleButton.png",
	L".//Assets//Menus.fbm//OptionsButton.png",
	L".//Assets//Menus.fbm//ExitButton.png",
	L".//Assets//Cube.fbm//black_texture.jpg",
	L".//Assets//Cube.fbm//grass2.png",
	L".//Assets//Cube.fbm//dirt4.png",
	L".//Assets//Cube.fbm//fire.jpg",
	L".//Assets//Cube.fbm//lava0003.png",
	L".//Assets//Cube.fbm//Hay.jpg",
	L".//Assets//bomb.fbm//bomb.png",
	L".//Assets//bomb.fbm//tbomb.png",
	L".//Assets//bomb.fbm//xbomb.png",
	L".//Assets//bomb.fbm//+bomb.png",
	L".//Assets//bomb.fbm//ibomb.png",
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
	L".//Assets//Fire//expl1.png",
	L".//Assets//Fire//expl2.png",
	L".//Assets//Fire//expl3.png",
	L".//Assets//Fire//expl4.png",
	L".//Assets//Fire//expl5.png",
	L".//Assets//Fire//expl6.png",
	L".//Assets//Fire//expl7.png",
	L".//Assets//Fire//expl8.png",
	L".//Assets//Fire//expl9.png",
	L".//Assets//Fire//expl10.png",
	L".//Assets//Fire//expl11.png",
	L".//Assets//Fire//expl12.png",
	L".//Assets//Fire//expl13.png",
	L".//Assets//Fire//expl14.png",
	//L".//Assets//Fire//Bomberman_Enemies.png",
	L".//Assets//Goat.fbm//Goat.png",
	L".//Assets//Boar.fbm//Boar.png",
	L".//Assets//Boar.fbm//Boar2.png",
	L".//Assets//Chicken.fbm//ChickenNormal.png",
	L".//Assets//Goat.fbm//GoatNormal.png",
	L".//Assets//Boar.fbm//BoarNormal.png",
	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0001.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0002.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0003.jpg",
	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0004.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0005.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0006.jpg",
	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0007.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0008.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0009.jpg",
	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0010.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0011.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0012.jpg",
	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0013.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0014.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0015.jpg",
	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0016.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0017.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0018.jpg",
	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0019.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0020.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0021.jpg",
	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0022.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0023.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0024.jpg",
	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0025.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0026.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0027.jpg",
	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0028.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0029.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0030.jpg",
	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0031.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0032.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0033.jpg",
	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0034.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0035.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0036.jpg",
	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0037.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0038.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0039.jpg",
	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0040.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0041.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0042.jpg",
	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0043.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0044.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0045.jpg",
	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0046.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0047.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0048.jpg",
	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0049.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0050.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0051.jpg",
	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0052.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0053.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0054.jpg",
	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0055.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0056.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0057.jpg",
	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0058.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0059.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0060.jpg",
	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0061.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0062.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0063.jpg",
	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0064.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0065.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0066.jpg",
	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0067.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0068.jpg",	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0069.jpg",
	L".//Assets//Menus.fbm//Menu1//RECOVER_GameScreen0070.jpg",
	L".//Assets//Menus.fbm//Menu2//SettingScreenV020001.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020002.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020003.jpg",
	L".//Assets//Menus.fbm//Menu2//SettingScreenV020004.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020005.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020006.jpg",
	L".//Assets//Menus.fbm//Menu2//SettingScreenV020007.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020008.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020009.jpg",
	L".//Assets//Menus.fbm//Menu2//SettingScreenV020010.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020011.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020012.jpg",
	L".//Assets//Menus.fbm//Menu2//SettingScreenV020013.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020014.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020015.jpg",
	L".//Assets//Menus.fbm//Menu2//SettingScreenV020016.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020017.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020018.jpg",
	L".//Assets//Menus.fbm//Menu2//SettingScreenV020019.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020020.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020021.jpg",
	L".//Assets//Menus.fbm//Menu2//SettingScreenV020022.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020023.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020024.jpg",
	L".//Assets//Menus.fbm//Menu2//SettingScreenV020025.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020026.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020027.jpg",
	L".//Assets//Menus.fbm//Menu2//SettingScreenV020028.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020029.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020030.jpg",
	L".//Assets//Menus.fbm//Menu2//SettingScreenV020031.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020032.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020033.jpg",
	L".//Assets//Menus.fbm//Menu2//SettingScreenV020034.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020035.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020036.jpg",
	L".//Assets//Menus.fbm//Menu2//SettingScreenV020037.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020038.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020039.jpg",
	L".//Assets//Menus.fbm//Menu2//SettingScreenV020040.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020041.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020042.jpg",
	L".//Assets//Menus.fbm//Menu2//SettingScreenV020043.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020044.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020045.jpg",
	L".//Assets//Menus.fbm//Menu2//SettingScreenV020046.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020047.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020048.jpg",
	L".//Assets//Menus.fbm//Menu2//SettingScreenV020049.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020050.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020051.jpg",
	L".//Assets//Menus.fbm//Menu2//SettingScreenV020052.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020053.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020054.jpg",
	L".//Assets//Menus.fbm//Menu2//SettingScreenV020055.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020056.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020057.jpg",
	L".//Assets//Menus.fbm//Menu2//SettingScreenV020058.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020059.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020060.jpg",
	L".//Assets//Menus.fbm//Menu2//SettingScreenV020061.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020062.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020063.jpg",
	L".//Assets//Menus.fbm//Menu2//SettingScreenV020064.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020065.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020066.jpg",
	L".//Assets//Menus.fbm//Menu2//SettingScreenV020067.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020068.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020069.jpg",
	L".//Assets//Menus.fbm//Menu2//SettingScreenV020070.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020071.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020072.jpg",
	L".//Assets//Menus.fbm//Menu2//SettingScreenV020073.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020074.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020075.jpg",
	L".//Assets//Menus.fbm//Menu2//SettingScreenV020076.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020077.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020078.jpg",
	L".//Assets//Menus.fbm//Menu2//SettingScreenV020079.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020080.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020081.jpg",
	L".//Assets//Menus.fbm//Menu2//SettingScreenV020082.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020083.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020084.jpg",
	L".//Assets//Menus.fbm//Menu2//SettingScreenV020085.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020086.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020087.jpg",
	L".//Assets//Menus.fbm//Menu2//SettingScreenV020088.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020089.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020090.jpg",
	L".//Assets//Menus.fbm//Menu2//SettingScreenV020091.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020092.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020093.jpg",
	L".//Assets//Menus.fbm//Menu2//SettingScreenV020094.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020095.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020096.jpg",
	L".//Assets//Menus.fbm//Menu2//SettingScreenV020097.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020098.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020099.jpg",
	L".//Assets//Menus.fbm//Menu2//SettingScreenV020100.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020101.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020102.jpg",
	L".//Assets//Menus.fbm//Menu2//SettingScreenV020103.jpg", L".//Assets//Menus.fbm//Menu2//SettingScreenV020104.jpg",
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
