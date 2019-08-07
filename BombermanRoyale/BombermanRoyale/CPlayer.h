#pragma once

#include "CBomb.h"


class CPlayer : public CObject
{
private:
	std::vector<int> bombs;
	int numBombs = 1;
	bool alive;
public:
	CPlayer();
	~CPlayer();
	inline bool isAlive() { return alive; }
	inline void setAlive(bool _alive) { alive = _alive; }
	inline void incNumBombs() { numBombs++; }
	inline bool hasAvailableBombSlot() { return (bombs.size() < numBombs ? true : false); }
	inline void deleteBomb(int index) { bombs.erase(bombs.begin() + index); }
	inline std::vector<int> getBombIndices() { return bombs; };

};

