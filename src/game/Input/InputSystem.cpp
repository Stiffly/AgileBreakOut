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

#include "PrecompiledHeader.h"
#include "Input/InputSystem.h"
#include "Core/World.h"

void dd::Systems::InputSystem::RegisterComponents(ComponentFactory* cf)
{
	
}

void dd::Systems::InputSystem::Initialize()
{
	// Subscribe to events
	EVENT_SUBSCRIBE_MEMBER(m_EKeyDown, &Systems::InputSystem::OnKeyDown);
	EVENT_SUBSCRIBE_MEMBER(m_EKeyUp, &Systems::InputSystem::OnKeyUp);
	EVENT_SUBSCRIBE_MEMBER(m_EMousePress, &Systems::InputSystem::OnMousePress);
	EVENT_SUBSCRIBE_MEMBER(m_EMouseRelease, &Systems::InputSystem::OnMouseRelease);
	EVENT_SUBSCRIBE_MEMBER(m_EGamepadAxis, &Systems::InputSystem::OnGamepadAxis);
	EVENT_SUBSCRIBE_MEMBER(m_EGamepadButtonDown, &Systems::InputSystem::OnGamepadButtonDown);
	EVENT_SUBSCRIBE_MEMBER(m_EGamepadButtonUp, &Systems::InputSystem::OnGamepadButtonUp);
	EVENT_SUBSCRIBE_MEMBER(m_EBindKey, &Systems::InputSystem::OnBindKey);
	EVENT_SUBSCRIBE_MEMBER(m_EBindMouseButton, &Systems::InputSystem::OnBindMouseButton);
	EVENT_SUBSCRIBE_MEMBER(m_EBindGamepadAxis, &Systems::InputSystem::OnBindGamepadAxis);
	EVENT_SUBSCRIBE_MEMBER(m_EBindGamepadButton, &Systems::InputSystem::OnBindGamepadButton);
}

void dd::Systems::InputSystem::Update(double dt)
{
// #ifdef DEBUG
// 	// Wireframe
// 	if (m_CurrentKeyState[GLFW_KEY_F1] && !m_LastKeyState[GLFW_KEY_F1])
// 	{
// 		m_Renderer->DrawWireframe(!m_Renderer->DrawWireframe());
// 	}
// 	// Normals
// 	if (m_CurrentKeyState[GLFW_KEY_F2] && !m_LastKeyState[GLFW_KEY_F2])
// 	{
// 		m_Renderer->DrawNormals(!m_Renderer->DrawNormals());
// 	}
// 	// Bounds
// 	if (m_CurrentKeyState[GLFW_KEY_F3] && !m_LastKeyState[GLFW_KEY_F3])
// 	{
// 		m_Renderer->DrawBounds(!m_Renderer->DrawBounds());
// 	}
// #endif
}

bool dd::Systems::InputSystem::OnKeyDown(const Events::KeyDown &event)
{
	auto range = m_KeyBindings.equal_range(event.KeyCode);
	for (auto bindingIt = range.first; bindingIt != range.second; bindingIt++)
	{
		std::string command;
		float value;
		std::tie(command, value) = bindingIt->second;
		m_CommandKeyboardValues[command][event.KeyCode] = value;
		PublishCommand(1, command, GetCommandTotalValue(command));
	}

	return true;
}

bool dd::Systems::InputSystem::OnKeyUp(const Events::KeyUp &event)
{
	auto range = m_KeyBindings.equal_range(event.KeyCode);
	for (auto bindingIt = range.first; bindingIt != range.second; bindingIt++)
	{
		std::string command;
		float value;
		std::tie(command, value) = bindingIt->second;
		m_CommandKeyboardValues[command][event.KeyCode] = 0;
		PublishCommand(1, command, GetCommandTotalValue(command));;
	}

	return true;
}

bool dd::Systems::InputSystem::OnMousePress(const Events::MousePress &event)
{
	auto range = m_MouseButtonBindings.equal_range(event.Button);
	for (auto bindingIt = range.first; bindingIt != range.second; bindingIt++)
	{
		std::string command;
		float value;
		std::tie(command, value) = bindingIt->second;
		m_CommandMouseButtonValues[command][event.Button] = value;
		PublishCommand(1, command, GetCommandTotalValue(command));
	}

	return true;
}

bool dd::Systems::InputSystem::OnMouseRelease(const Events::MouseRelease &event)
{
	auto range = m_MouseButtonBindings.equal_range(event.Button);
	for (auto bindingIt = range.first; bindingIt != range.second; bindingIt++)
	{
		std::string command;
		float value;
		std::tie(command, value) = bindingIt->second;
		m_CommandMouseButtonValues[command][event.Button] = 0;
		PublishCommand(1, command, GetCommandTotalValue(command));
	}

	return true;
}

bool dd::Systems::InputSystem::OnGamepadAxis(const Events::GamepadAxis &event)
{
	auto range = m_GamepadAxisBindings.equal_range(event.Axis);
	for (auto bindingIt = range.first; bindingIt != range.second; bindingIt++)
	{
		std::string command;
		float value;
		std::tie(command, value) = bindingIt->second;
		m_CommandGamepadAxisValues[command][event.Axis] = event.Value * value;
		PublishCommand(event.GamepadID + 1, command, GetCommandTotalValue(command));
	}

	return true;
}

bool dd::Systems::InputSystem::OnGamepadButtonDown(const Events::GamepadButtonDown &event)
{
	auto range = m_GamepadButtonBindings.equal_range(event.Button);
	for (auto bindingIt = range.first; bindingIt != range.second; bindingIt++)
	{
		std::string command;
		float value;
		std::tie(command, value) = bindingIt->second;
		m_CommandGamepadButtonValues[command][event.Button] = value;
		PublishCommand(event.GamepadID + 1, command, GetCommandTotalValue(command));
	}

	return true;
}

bool dd::Systems::InputSystem::OnGamepadButtonUp(const Events::GamepadButtonUp &event)
{
	auto range = m_GamepadButtonBindings.equal_range(event.Button);
	for (auto bindingIt = range.first; bindingIt != range.second; bindingIt++)
	{
		std::string command;
		float value;
		std::tie(command, value) = bindingIt->second;
		m_CommandGamepadButtonValues[command][event.Button] = 0;
		PublishCommand(event.GamepadID + 1, command, GetCommandTotalValue(command));
	}

	return true;
}

bool dd::Systems::InputSystem::OnBindKey(const Events::BindKey &event)
{
	if (event.Command.empty())
		return false;

	m_KeyBindings.insert(std::make_pair(event.KeyCode, std::make_tuple(event.Command, event.Value)));
	LOG_DEBUG("Input: Bound key %i to %s", event.KeyCode, event.Command.c_str());

	return true;
}

bool dd::Systems::InputSystem::OnBindMouseButton(const Events::BindMouseButton &event)
{
	if (event.Command.empty())
		return false;

	m_MouseButtonBindings.insert(std::make_pair(event.Button, std::make_tuple(event.Command, event.Value)));
	LOG_DEBUG("Input: Bound mouse button %i to %s", event.Button, event.Command.c_str());

	return true;
}

bool dd::Systems::InputSystem::OnBindGamepadAxis(const Events::BindGamepadAxis &event)
{
	if (event.Command.empty())
		return false;

	m_GamepadAxisBindings.insert(std::make_pair(event.Axis, std::make_tuple(event.Command, event.Value)));
	LOG_DEBUG("Input: Bound gamepad axis %i to %s", event.Axis, event.Command.c_str());

	return true;
}

bool dd::Systems::InputSystem::OnBindGamepadButton(const Events::BindGamepadButton &event)
{
	if (event.Command.empty())
		return false;

	m_GamepadButtonBindings.insert(std::make_pair(event.Button, std::make_tuple(event.Command, event.Value)));
	LOG_DEBUG("Input: Bound gamepad axis %i to %s", event.Button, event.Command.c_str());

	return true;
}

float dd::Systems::InputSystem::GetCommandTotalValue(std::string command)
{
	float value = 0.f;

	auto keyboardIt = m_CommandKeyboardValues.find(command);
	if (keyboardIt != m_CommandKeyboardValues.end())
	{
		for (auto &key : keyboardIt->second)
		{
			value += key.second;
		}
	}

	auto mouseButtonIt = m_CommandMouseButtonValues.find(command);
	if (mouseButtonIt != m_CommandMouseButtonValues.end())
	{
		for (auto &button : mouseButtonIt->second)
		{
			value += button.second;
		}
	}

	auto gamepadAxisIt = m_CommandGamepadAxisValues.find(command);
	if (gamepadAxisIt != m_CommandGamepadAxisValues.end())
	{
		for (auto &axis : gamepadAxisIt->second)
		{
			value += axis.second;
		}
	}

	auto gamepadButtonIt = m_CommandGamepadButtonValues.find(command);
	if (gamepadButtonIt != m_CommandGamepadButtonValues.end())
	{
		for (auto &button : gamepadButtonIt->second)
		{
			value += button.second;
		}
	}

	return std::max(-1.f, std::min(value, 1.f));
}

void dd::Systems::InputSystem::PublishCommand(int playerID, std::string command, float value)
{
	Events::InputCommand e;
	e.PlayerID = playerID;
	e.Command = command;
	e.Value = value;
	EventBroker->Publish(e);

	LOG_DEBUG("Input: Published command %s=%f for player %i", e.Command.c_str(), e.Value, playerID);
}
