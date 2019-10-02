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

		while (anim->_time > anim->currentAnimation->frames[anim->currentFrameIndex + 1].time)
		{
			anim->currentFrameIndex++;

			if (anim->currentFrameIndex == (anim->currentAnimation->frames.size() - 1))
			{
				if (anim->currentAnimation->loops)
				{
					anim->_time -= anim->currentAnimation->frames[anim->currentFrameIndex].time;
					anim->currentFrameIndex = 0;
				}
				else
				{
					anim->currentFrameIndex--;
					anim->_time = anim->currentAnimation->frames[anim->currentFrameIndex].time;
					return;
				}
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
			if (toChangeTo == anim->animations[i].AnimationName)
			{
				anim->currentAnimation = &anim->animations[i];
				return 1;
			}
		}
	}
	return -1;
}

void CPlayer::ResetAnimation()
{

	TComponent* c;
	if (this->GetComponent(COMPONENT_TYPE::ANIM, c))
	{
		TAnimComponent* anim = (TAnimComponent*)c;

		anim->currentFrameIndex = 0;
		anim->_time = 0.0;
	}
}

void CPlayer::Initialize()
{
	controller = CharacterController();
	if (!controller.IsControllerConnected())
	{

	}
}

