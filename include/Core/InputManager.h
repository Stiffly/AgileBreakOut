/*
	This file is part of Daydream Engine.
	Copyright 2014 Adam Byléhn, Tobias Dahl, Simon Holmberg, Viktor Ljung
	
	Daydream Engine is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	
	Daydream Engine is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.
	
	You should have received a copy of the GNU Lesser General Public License
	along with Daydream Engine.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef InputManager_h__
#define InputManager_h__

#include <array>

#include "Core/EventBroker.h"
#include "EKeyDown.h"
#include "EKeyUp.h"
#include "EMousePress.h"
#include "EMouseRelease.h"
#include "EMouseMove.h"
#include "ELockMouse.h"
#include "EGamepadAxis.h"
#include "EGamepadButton.h"

namespace dd
{

class InputManager
{
public:
	InputManager(GLFWwindow* window, std::shared_ptr<dd::EventBroker> eventBroker)
	: m_GLFWWindow(window)
	, EventBroker(eventBroker)
	, m_CurrentKeyState()
	, m_LastKeyState()
	, m_CurrentMouseState()
	, m_LastMouseState()
	, m_CurrentMouseX(0), m_CurrentMouseY(0)
	, m_LastMouseX(0), m_LastMouseY(0)
	, m_CurrentMouseDeltaX(0), m_CurrentMouseDeltaY(0)
	, m_MouseLocked(false)
	{ Initialize(); }

	void Initialize();

	static const short MAX_GAMEPADS = 4;

	void Update(double dt);

private:
	GLFWwindow* m_GLFWWindow;
	std::shared_ptr<dd::EventBroker> EventBroker;

	EventRelay<InputManager, Events::LockMouse> m_ELockMouse;
	bool OnLockMouse(const Events::LockMouse &event);
	EventRelay<InputManager, Events::UnlockMouse> m_EUnlockMouse;
	bool OnUnlockMouse(const Events::UnlockMouse &event);

	std::array<int, GLFW_KEY_LAST+1> m_CurrentKeyState;
	std::array<int, GLFW_KEY_LAST+1> m_LastKeyState;
	std::array<int, GLFW_MOUSE_BUTTON_LAST+1> m_CurrentMouseState;
	std::array<int, GLFW_MOUSE_BUTTON_LAST+1> m_LastMouseState;
	typedef std::array<float, static_cast<int>(Gamepad::Axis::LAST) + 1> GamepadAxisState;
	std::array<GamepadAxisState, MAX_GAMEPADS> m_CurrentGamepadAxisState;
	std::array<GamepadAxisState, MAX_GAMEPADS> m_LastGamepadAxisState;
	typedef std::array<bool, static_cast<int>(Gamepad::Button::LAST) + 1> GamepadButtonState;
	std::array<GamepadButtonState, MAX_GAMEPADS> m_CurrentGamepadButtonState;
	std::array<GamepadButtonState, MAX_GAMEPADS> m_LastGamepadButtonState;

	double m_CurrentMouseX, m_CurrentMouseY;
	double m_LastMouseX, m_LastMouseY;
	double m_CurrentMouseDeltaX, m_CurrentMouseDeltaY;
	bool m_MouseLocked;

	void PublishGamepadAxisIfChanged(int gamepadID, Gamepad::Axis axis);
	void PublishGamepadButtonIfChanged(int gamepadID, Gamepad::Button button);
};

}

#endif // InputManager_h__
