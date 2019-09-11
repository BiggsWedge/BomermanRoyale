#pragma once
#include "Utilities.h"

#include "../Gateware/Interface/G_System/GController.h"
#include "../Gateware/Interface/G_System/GControllerInputDefines.h"
#include "../Gateware/Interface/G_System/GInput.h"

struct DEFAULT_BUTTONS
{
	enum { ACTION = 0, PAUSE, CROUCH, LEFT, RIGHT, UP, DOWN, COUNT };
};

struct Button
{
private:
	bool currState = true, prevState = true;
	int buttonID;
public:
	inline void SetButtonID(int ID) { buttonID = ID; }
	inline bool Pressed() { return !prevState && currState; }
	inline bool Held() { return prevState && currState; }
	inline bool Idle() { return !prevState && !currState; }
	inline bool Released() { return prevState && !currState; }
	inline void Reset(bool toSetTo) { currState = prevState = toSetTo; }
	void Update(int controllerIndex, bool inputMode);
};

class CharacterController
{
private:
	int controllerIndex;
	std::vector<Button> controllerButtons;
	std::vector<Button> keyboardButtons;
	float LeftXAxis, LeftYAxis;
	bool connected;

public:

	inline bool ButtonPressed(int button) { return (connected) ? controllerButtons[button].Pressed() : keyboardButtons[button].Pressed(); }
	inline bool ButtonHeld(int button) { return (connected) ? controllerButtons[button].Held() : keyboardButtons[button].Held(); }
	inline bool ButtonIdle(int button) { return (connected) ? controllerButtons[button].Idle() : keyboardButtons[button].Idle(); }
	inline bool ButtonReleased(int button) { return (connected) ? controllerButtons[button].Released() : keyboardButtons[button].Released(); }
	inline bool IsControllerConnected() { return connected; }
	void Update();
	inline void SetControllerIndex(int index) { controllerIndex = index; }
	void SetButtonCodes(int actionButton, int pauseButton, int helpButton, int crouchButton);
	inline float GetLeftRight() { return LeftXAxis; };
	inline float GetUpDown() { return LeftYAxis; }
};
