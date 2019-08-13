#pragma once
#include "Utilities.h"

#include "../Gateware/Interface/G_System/GController.h"
#include "../Gateware/Interface/G_System/GControllerInputDefines.h"
#include "../Gateware/Interface/G_System/GInput.h"

struct Button
{
private:
	bool currState, prevState;
	int buttonID;
public:
	inline void SetButtonID(int ID) { buttonID = ID; }
	inline bool Pressed() { return !prevState && currState; }
	inline bool Held() { return prevState && currState; }
	inline bool Idle() { return !prevState && !currState; }
	inline bool Released() { return prevState && !currState; }
	inline void Reset(bool toSetTo) { currState = prevState = toSetTo; }
	void Update(int controllerIndex, bool inputMode)
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
};

struct DEFAULT_CONTROLLER_BUTTONS
{
	enum { ACTION = 0, PAUSE, HELP, COUNT };
};



class CharacterController
{
private:
	int controllerIndex;
	std::vector<Button> buttons;
	float LeftXAxis, LeftYAxis;
	bool connected;

public:

	inline bool ButtonPressed(int button) { return buttons[button].Pressed(); }
	inline bool ButtonHeld(int button) { return buttons[button].Held(); }
	inline bool ButtonIdle(int button) { return buttons[button].Idle(); }
	inline bool ButtonReleased(int button) { return buttons[button].Released(); }
	inline bool IsControllerConnected() { return connected; }
	void Update()
	{
		LeftXAxis = 0.0f;
		LeftYAxis = 0.0f;
		g_pControllerInput->IsConnected(controllerIndex, connected);

		if (connected)
		{
			for (Button& button : buttons)
				button.Update(controllerIndex, connected);
			g_pControllerInput->GetState(controllerIndex, G_LX_AXIS, LeftXAxis);
			g_pControllerInput->GetState(controllerIndex, G_LY_AXIS, LeftYAxis);
		}

	}
	inline void SetControllerIndex(int index) { controllerIndex = index; }
	void SetButtonCodes(int actionButton, int pauseButton, int helpButton)
	{
		buttons.resize(DEFAULT_CONTROLLER_BUTTONS::COUNT);
		buttons[DEFAULT_CONTROLLER_BUTTONS::ACTION].SetButtonID(actionButton);
		buttons[DEFAULT_CONTROLLER_BUTTONS::ACTION].Reset(false);
		buttons[DEFAULT_CONTROLLER_BUTTONS::PAUSE].SetButtonID(pauseButton);
		buttons[DEFAULT_CONTROLLER_BUTTONS::PAUSE].Reset(false);
		buttons[DEFAULT_CONTROLLER_BUTTONS::HELP].SetButtonID(helpButton);
		buttons[DEFAULT_CONTROLLER_BUTTONS::HELP].Reset(false);

	}
	inline float GetLeftRight() { return LeftXAxis; };
	inline float GetUpDown() { return LeftYAxis; }
};
