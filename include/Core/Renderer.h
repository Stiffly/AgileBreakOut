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

#ifndef DD_RENDERER_H__
#define DD_RENDERER_H__

#include "Util/Rectangle.h"
#include "Core/System.h"
#include "Core/CTransform.h"
#include "Core/CTemplate.h"
#include "Core/EventBroker.h"
#include "Core/World.h"
#include "Game/EScreenShake.h"
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
	void PlaceCamera(glm::vec3 position);

private:
	Rectangle m_Resolution = Rectangle(1280, 720);
	bool m_Fullscreen = false;
	bool m_VSYNC = false;
	std::unique_ptr<dd::Camera> m_DefaultCamera = nullptr;
	const dd::Camera* m_Camera = nullptr;

	int m_GLVersion[2];
	std::string m_GLVendor;
	GLFWwindow* m_Window = nullptr;

	ShaderProgram* m_SpDeferred1;
	ShaderProgram* m_SpDeferred2;
	ShaderProgram* m_SpDeferred3;
	ShaderProgram* m_SpForward;
	ShaderProgram* m_SpScreen;
	ShaderProgram* m_SpWater;
	ShaderProgram* m_SpWater2;

	GLuint m_ScreenQuad = 0;
	Model* m_UnitSphere = nullptr;
	Model* m_UnitQuad = nullptr;
	Texture* m_StandardNormal;
	Texture* m_StandardSpecular;
	Texture* m_WhiteSphereTexture;

	GLuint m_RbDepthBuffer = 0;	//DepthBuffer Texture
	GLuint m_FbDeferred1 = 0;	//Framebuffer for first pass
	GLuint m_GDiffuse = 0;		//Texture for diffuseColors
	GLuint m_GPosition = 0;		//Texture for positions
	GLuint m_GNormal = 0;		//Texture for normals
	GLuint m_GSpecular = 0;		//Texture for specular
	GLuint m_FbDeferred2 = 0;	//Framebuffer that handles the lighting pass
	GLuint m_TLighting = 0;		//Texture for the lighting pass
	GLuint m_FbDeferred3 = 0;	//Frambuffer for handling the last pass.
	GLuint m_TFinal = 0;		//Final Texture to be printed to screen
	GLuint m_Gwater = 0;		//Water Color Texture
	GLuint m_BWater;			//Water blur texture
	GLuint m_BWater2;			//Water blur texture
	GLuint m_FbWater;			//Waterpass Framebuffer
	GLuint m_FbWaterBlur; 		//Waterpass Framebuffer
	GLuint m_FbWaterBlur2;		//Waterpass Framebuffer

	GLuint m_CurrentScreenBuffer = 0;
	void LoadShaders();
	void CreateBuffers();
	GLuint CreateQuad();
	GLuint CreateWaterParticleVAO(RenderQueue &particles);

	static bool DepthSort(const std::shared_ptr<RenderJob> &i, const std::shared_ptr<RenderJob> &j) { return (i->Depth < j->Depth); }

	void DrawDeferred(RenderQueue &objects, RenderQueue &lights);
	void DrawForward(RenderQueue &objects, RenderQueue &lights);
	void DrawScene(RenderQueue &objects, ShaderProgram &program);
	void DrawLightSpheres(RenderQueue &lights);
	void DrawWater(RenderQueue &objects);
	void DrawGUI(RenderQueue& rq);
	void DebugKeys();

	dd::EventRelay<Renderer, dd::Events::ScreenShake> m_EScreenShake;
	bool OnScreenShake(dd::Events::ScreenShake &event);
	bool m_ScreenShake = false;
	float m_ShakeIntensity = 0;
	float m_ShakeTimer = 0;
};

}

#endif // Renderer_h__
