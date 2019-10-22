#include "CExplosion.h"
#include "CPlayer.h"
#include <iostream>

CExplosion::CExplosion()
{
}


CExplosion::~CExplosion()
{
}

void CExplosion::initialize(CPlayer* parent)
{
	p_cParentPlayer = parent;
}

CExplosion& CExplosion::operator=(const CExplosion& other)
{
	if (this != &other)
	{
		v_tComponents.clear();
		this->v_tComponents = other.v_tComponents;
		p_cParentPlayer = other.p_cParentPlayer;
		timer = other.timer;
		maxTime = other.maxTime;
		strength = other.strength;
		alive = other.alive;
	}
	return *this;
}