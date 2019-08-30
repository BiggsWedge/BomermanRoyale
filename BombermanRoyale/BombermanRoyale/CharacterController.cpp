#include "CharacterController.h"

std::vector<int> P1DefaultControls
{
	'E',
	'Q',
	'A',
	'D',
	'W',
	'S',
	'C'
};

std::vector<int> P2DefaultControls
{
	'O',
	'U',
	'J',
	'L',
	'I',
	'K',
	'M'
};


void Button::Update(int controllerIndex, bool inputMode)
{
	prevState = currState;
	if (inputMode)
	{
		float currStateF = 0.0f;
		g_pControllerInput->GetState(controllerIndex, buttonID, currStateF);
		currState = (currStateF == 0.0f) ? false : true;
	}
	else
	{
		currState = GetAsyncKeyState(buttonID) & 0x8000;
	}
}

void CharacterController::Update()
{

	LeftXAxis = 0.0f;
	LeftYAxis = 0.0f;
	g_pControllerInput->IsConnected(controllerIndex, connected);

	if (connected)
	{
		for (Button& button : controllerButtons)
			button.Update(controllerIndex, connected);
		g_pControllerInput->GetState(controllerIndex, G_LX_AXIS, LeftXAxis);
		g_pControllerInput->GetState(controllerIndex, G_LY_AXIS, LeftYAxis);
		//g_pControllerInput->GetState(controllerIndex, G_LY_AXIS, LeftYAxis);
	}
	else
	{
		for (Button& button : keyboardButtons)
			button.Update(controllerIndex, connected);
		//if (keyboardButtons[])
	}

}


void CharacterController::SetButtonCodes(int actionButton, int pauseButton, int helpButton, int crouchButton)
{
	controllerButtons.resize(DEFAULT_BUTTONS::PAUSE + 2);
	controllerButtons[DEFAULT_BUTTONS::ACTION].SetButtonID(actionButton);
	controllerButtons[DEFAULT_BUTTONS::ACTION].Reset(false);
	controllerButtons[DEFAULT_BUTTONS::PAUSE].SetButtonID(pauseButton);
	controllerButtons[DEFAULT_BUTTONS::PAUSE].Reset(false);
	//controllerButtons[DEFAULT_BUTTONS::CROUCH].SetButtonID(crouchButton);
	//controllerButtons[DEFAULT_BUTTONS::CROUCH].Reset(false);

	keyboardButtons.resize(DEFAULT_BUTTONS::COUNT);
	for (Button& button : keyboardButtons)
		button.Reset(false);
	keyboardButtons[DEFAULT_BUTTONS::ACTION].SetButtonID((controllerIndex == 0) ? P1DefaultControls[DEFAULT_BUTTONS::ACTION] : P2DefaultControls[DEFAULT_BUTTONS::ACTION]);
	keyboardButtons[DEFAULT_BUTTONS::LEFT].SetButtonID((controllerIndex == 0) ? P1DefaultControls[DEFAULT_BUTTONS::LEFT] : P2DefaultControls[DEFAULT_BUTTONS::LEFT]);
	keyboardButtons[DEFAULT_BUTTONS::RIGHT].SetButtonID((controllerIndex == 0) ? P1DefaultControls[DEFAULT_BUTTONS::RIGHT] : P2DefaultControls[DEFAULT_BUTTONS::RIGHT]);
	keyboardButtons[DEFAULT_BUTTONS::UP].SetButtonID((controllerIndex == 0) ? P1DefaultControls[DEFAULT_BUTTONS::UP] : P2DefaultControls[DEFAULT_BUTTONS::UP]);
	keyboardButtons[DEFAULT_BUTTONS::DOWN].SetButtonID((controllerIndex == 0) ? P1DefaultControls[DEFAULT_BUTTONS::DOWN] : P2DefaultControls[DEFAULT_BUTTONS::DOWN]);
	keyboardButtons[DEFAULT_BUTTONS::PAUSE].SetButtonID((controllerIndex == 0) ? P1DefaultControls[DEFAULT_BUTTONS::PAUSE] : P2DefaultControls[DEFAULT_BUTTONS::PAUSE]);
	keyboardButtons[DEFAULT_BUTTONS::CROUCH].SetButtonID((controllerIndex == 0) ? P1DefaultControls[DEFAULT_BUTTONS::CROUCH] : P2DefaultControls[DEFAULT_BUTTONS::CROUCH]);
}



