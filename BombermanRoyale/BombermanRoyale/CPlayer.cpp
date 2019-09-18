#include "CPlayer.h"


CPlayer::CPlayer()
{
}

CPlayer::~CPlayer()
{
	CObject::~CObject();
}

void CPlayer::resetStats()
{
	bombs.clear();
	alive = true;
	numBombs = 1;
}

void CPlayer::updatePlayer(double frameTime)
{
	TComponent* c;
	if (this->GetComponent(COMPONENT_TYPE::ANIM, c))
	{
		TAnimComponent* anim = (TAnimComponent*)c;

		anim->_time += frameTime;
		while (anim->currentAnimation->frames[anim->currentFrameIndex + 1].time)
		{
			anim->currentFrameIndex++;
			if (currFrameIndex == (anim->currentAnimation->frames.size() - 1))
			{
				anim->_time -= anim->currentAnimation->frames[currFrameIndex].time;
				currFrameIndex = 0;
			}
		}
	}
}

int CPlayer::SetCurrentAnimaion(std::string toChangeTo)
{
	TComponent* c;
	if (this->GetComponent(COMPONENT_TYPE::ANIM, c))
	{
		TAnimComponent* anim = (TAnimComponent*)c;

		if (anim->currentAnimation->AnimationName == toChangeTo)
			return 0;

		for (int i = 0; i < anim->animations.size(); ++i)
		{
			if (toChangeTo == anim->animations[i]->AnimationName)
			{
				return 1;
			}
		}
	}
	return -1;
}

void CPlayer::Initialize()
{
	controller = CharacterController();
	if (!controller.IsControllerConnected())
	{

	}
}

