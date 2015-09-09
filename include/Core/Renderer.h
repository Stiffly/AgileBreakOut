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

#ifndef Renderer_h__
#define Renderer_h__

#include "Util/Rectangle.h"
#include "Camera.h"
#include "RenderQueue.h"
#include "ShaderProgram.h"

namespace dd
{

class Renderer
{
public:
	GLFWwindow* Window() const { return m_Window; }
	Rectangle Resolution() const { return m_Resolution; }
	void SetResolution(const Rectangle& resolution) { m_Resolution = resolution; }
	bool Fullscreen() { return m_Fullscreen; }
	void SetFullscreen(bool fullscreen) { m_Fullscreen = fullscreen; }
	bool VSYNC() const { return m_VSYNC; }
	void SetVSYNC(bool vsync) { m_VSYNC = vsync; }
	//void SetViewport(const Rectangle& viewport) { m_Viewport = viewport; }
	//void SetScissor(const Rectangle& scissor) { m_Scissor = scissor; }
	const dd::Camera* Camera() const { return m_Camera; }
	void SetCamera(const dd::Camera* camera)
	{
		if (camera == nullptr) {
			m_Camera = m_DefaultCamera.get();
		} else {
			m_Camera = camera;
		}
	}

	void Initialize();
	void Draw(RenderQueueCollection& rq);

private:
	Rectangle m_Resolution = Rectangle(1280, 720);
	bool m_Fullscreen = false;
	bool m_VSYNC = false;
	//Rectangle m_Viewport;
	//Rectangle m_Scissor;
	std::unique_ptr<dd::Camera> m_DefaultCamera = nullptr;
	const dd::Camera* m_Camera = nullptr;

	int m_GLVersion[2];
	std::string m_GLVendor;
	GLFWwindow* m_Window = nullptr;

	ShaderProgram* m_spDeferred1;
	ShaderProgram* m_spDeferred2;
	ShaderProgram* m_spDeferred3;
	ShaderProgram* m_spForward;
	ShaderProgram* m_spScreen;

	GLuint m_UnitQuad = 0;
	Model* m_UnitSphere = nullptr;

	GLuint m_rbDepthBuffer = 0;
	GLuint m_fbDeferred1 = 0;
	GLuint m_GDiffuse = 0;
	GLuint m_GPosition = 0;
	GLuint m_GNormal = 0;
	GLuint m_GSpecular = 0;
	GLuint m_fbDeferred2 = 0;
	GLuint m_tLighting = 0;
	GLuint m_fbDeferred3 = 0;
	GLuint m_tFinal = 0;

	GLuint m_CurrentScreenBuffer = 0;
	void LoadShaders();
	void CreateBuffers();
	GLuint CreateQuad();

	void DrawDeferred(RenderQueue &objects, RenderQueue &lights);
	void DrawForward(RenderQueue &objects, RenderQueue &lights);
	void DrawScene(RenderQueue &objects, ShaderProgram &program);
	void DrawLightSpheres(RenderQueue &lights);
	void DebugKeys();
};

}

#endif // Renderer_h__
