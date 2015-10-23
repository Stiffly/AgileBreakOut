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

#ifndef DD_BASERENDERER_H__
#define DD_BASERENDERER_H__

#include "Core/Util/Rectangle.h"
#include "Core/ResourceManager.h"
#include "Rendering/Camera.h"
#include "Rendering/RenderQueue.h"

namespace dd
{

class BaseRenderer
{
public:
	GLFWwindow* Window() const { return m_Window; }
	Rectangle Resolution() const { return m_Resolution; }
	void SetResolution(const Rectangle& resolution) { m_Resolution = resolution; }
	bool Fullscreen() { return m_Fullscreen; }
	void SetFullscreen(bool fullscreen) { m_Fullscreen = fullscreen; }
	bool VSYNC() const { return m_VSYNC; }
	void SetVSYNC(bool vsync) { m_VSYNC = vsync; }
	dd::Camera* Camera() const { return m_Camera; }
	void SetCamera(dd::Camera* camera)
	{
		if (camera == nullptr) {
			m_Camera = m_DefaultCamera.get();
		} else {
			m_Camera = camera;
		}
	}

	virtual void Initialize() = 0;
	virtual void Draw(RenderQueueCollection& rq) = 0;

protected:
	Rectangle m_Resolution = Rectangle(1280, 720);
	bool m_Fullscreen = false;
	bool m_VSYNC = false;
	std::unique_ptr<dd::Camera> m_DefaultCamera = nullptr;
	dd::Camera* m_Camera = nullptr;
	GLFWwindow* m_Window = nullptr;
};

}

#endif // Renderer_h__
