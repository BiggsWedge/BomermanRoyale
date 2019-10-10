#pragma once
#include "CPlayer.h"
#include "Skybox.h"

class CGame;

class CRendererManager
{
private:
	std::vector<CObject*> rendereableObjects;
	std::vector<CObject*> renderableParticles;
	Skybox* theSkybox;
public:
	bool HasComponent(CObject _objToCheck, int _componentToCheck);
	void RenderObject(CObject* _objToDraw);
	void RenderParticle(CObject* _particleToDraw);
	void RenderObjectCollider(CObject* _objToDraw);
	bool Draw(double timepassed, int gamestate, CGame* parentGame);
	CRendererManager();
	~CRendererManager();
};

