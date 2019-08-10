#include "CPlayer.h"


CPlayer::CPlayer()
{
}

CPlayer::~CPlayer()
{
}

void CPlayer::resetStats()
{
	bombs.clear();
	alive = true;
	numBombs = 1;
}

void CPlayer::updatePlayer()
{

}

void CPlayer::Initialize()
{
	bombs = std::vector<int>(5);
}
