#include "CPlayer.h"


CPlayer::CPlayer()
{
}

CPlayer::~CPlayer()
{
	CObject::~CObject();
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
	controller = CharacterController();
}

