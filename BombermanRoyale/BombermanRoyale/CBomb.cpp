#include "CBomb.h"
#include "CPlayer.h"
#include <iostream>

CBomb::CBomb()
{
}


CBomb::~CBomb()
{
}

void CBomb::Explode()
{
	timer = maxTime - 0.0001f;
}

void CBomb::initialize(CPlayer* parent)
{
	alive = true;
	timer = 0.0;
	maxTime = 3.0;
	p_cParentPlayer = parent;
}

CBomb& CBomb::operator=(const CBomb& other)
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
