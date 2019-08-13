#pragma once

#include "CBomb.h"
#include "CharacterController.h"


class CPlayer : public CObject
{
private:
	std::vector<int> bombs;
	int numBombs = 1;
	bool alive;
	int controllerIndex;
	CharacterController controller;

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
	inline void GetInput() { controller.Update(); }
	inline CharacterController* GetCharacterController() { return &controller; }



	//MUTATORS
	inline void setControllerIndex(int index) { controller.SetControllerIndex(index); }
	inline void AddBombIndex(int index) { bombs.push_back(index); }
	inline void incNumBombs() { numBombs++; }
	inline void setAlive(bool _alive) { alive = _alive; }
	inline void deleteBomb(int index) { bombs.erase(bombs.begin() + index); }

};


