#pragma once
#include "CObject.h"
class CPlayer;

class CBomb : public CObject
{
private:
	CPlayer* p_cParentPlayer;
	double timer;
	double maxTime;
	int strength;
public:
	CBomb();
	~CBomb();
	void Explode();
};

