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
	bool alive;
public:
	CBomb();
	~CBomb();
	void Explode();
	inline bool shouldExplode() { return timer >= maxTime; }
	inline double getTimer() { return timer; }
	inline void updateBomb(double timePassed) { timer += timePassed; }
	inline CPlayer* getParent() { return p_cParentPlayer; }
	inline void setParent(CPlayer* _player) { p_cParentPlayer = _player; }
	void initialize(CPlayer* parent);
	void Draw(double timepassed);
	inline bool isAlive() { return alive; }
	CBomb& operator=(const CBomb& other);
	inline int GetStrength() { return strength; }
	inline void SetAlive(bool check) { alive = check; }
	inline void SetToExplode() { timer = maxTime - 0.01f; }
};

