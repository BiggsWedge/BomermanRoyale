#pragma once

#include "CBomb.h"
#include "CExplosion.h"
#include "CharacterController.h"


class CPlayer : public CObject
{
private:
	std::vector<int> bombs;
	int numBombs = 1;
	bool alive;
	float respawntimer = 0.0f;
	int controllerIndex;
	int bombType = 0;
	int placedBombs = 0;
	CharacterController controller;
	bool crouched = false;
	bool renderable = true;
	int score = 0;

public:
	CPlayer();
	~CPlayer();
	void resetStats();
	void updatePlayer(double frameTime);
	int SetCurrentAnimaion(std::string toChangeTo);
	void ResetAnimation();
	void Initialize();

	//ACCESSORS
	inline bool isAlive() { return alive; }
	inline int GetNumBombs() { return numBombs; }
	inline bool hasAvailableBombSlot() { return (placedBombs < numBombs ? true : false); }
	inline bool GetRenderable() { return renderable; }
	inline int GetControllerIndex() { return controllerIndex; }
	inline std::vector<int> getBombIndices() { return bombs; };
	inline int GetBombType() { return bombType; }
	inline void GetInput() { controller.Update(); }
	inline bool GetCrouchStatus() { return crouched; }
	inline float GetRespawnTimer() { return respawntimer; }
	inline void SetRespawnTimer(float timepassed) { respawntimer += timepassed; }
	inline int getScore() { return score; }
	inline void setScore(int scorechange) { score += scorechange; }
	inline CharacterController* GetCharacterController() { return &controller; }

	//MUTATORS
	inline void setControllerIndex(int index) { controller.SetControllerIndex(index); }
	inline void AddBombIndex(int index) { bombs.push_back(index); }
	inline void incNumBombs() { numBombs++; }
	inline void SetRenderable(bool _toSet) { renderable = _toSet; }
	inline void ToggleRenderable() { renderable = !renderable; }
	inline void ChangeCrouchStatus() { crouched = !crouched; }
	inline void setAlive(bool _alive) { alive = _alive; }
	inline void deleteBomb(int index) { bombs.erase(bombs.begin() + index); }
	inline void SetBombType(int _bombType) { bombType = _bombType; }
	inline void IncPlacedBombs() { placedBombs++; }
	inline void DecPlacedBombs() { placedBombs--; if (placedBombs < 0)placedBombs = 0; }
};


