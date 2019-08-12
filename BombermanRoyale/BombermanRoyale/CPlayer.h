#pragma once

#include "CBomb.h"


class CPlayer : public CObject
{
private:
	std::vector<int> bombs;
	int numBombs = 2;
	bool alive;
	int controllerIndex;

public:
	CPlayer();
	~CPlayer();
	void resetStats();
	void updatePlayer();
	void Initialize();



	//ACCESSORS
	inline bool isAlive() { return alive; }
	inline int GetNumBombs() { return numBombs; }
	inline bool hasAvailableBombSlot() { return (bombs.size() < numBombs ? true : false); }
	inline int GetControllerIndex() { return controllerIndex; }
	inline std::vector<int> getBombIndices() { return bombs; };


	//MUTATORS
	inline void setControllerIndex(int index) { controllerIndex = index; }
	inline void AddBombIndex(int index) { bombs.push_back(index); }
	inline void incNumBombs() { numBombs++; }
	inline void setAlive(bool _alive) { alive = _alive; }
	inline void deleteBomb(int index) { bombs.erase(bombs.begin() + index); }
};


