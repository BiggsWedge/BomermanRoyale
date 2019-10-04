#pragma once
#include "CObject.h"
class CPlayer;

class CExplosion : public CObject
{



private:
	CPlayer* p_cParentPlayer;
	double timer;
	double maxTime;
	int strength;
	bool alive;
public:
	CExplosion();
	~CExplosion();
	inline double getTimer() { return timer; }
	inline void updateBomb(double timePassed) { timer += timePassed; }
	inline CPlayer* getParent() { return p_cParentPlayer; }
	void initialize(CPlayer* parent);
	inline bool isAlive() { return alive; }
	CExplosion& operator=(const CExplosion& other);
	inline int GetStrength() { return strength; }
	inline void SetAlive(bool check) { alive = check; }
};
