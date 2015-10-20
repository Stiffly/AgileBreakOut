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
#include "Rendering/Renderer.h"

void dd::Renderer::Initialize()
{
	// Initialize GLFW
	if (!glfwInit()) {
		LOG_ERROR("GLFW: Initialization failed");
		exit(EXIT_FAILURE);
	}

	// Create a window
	GLFWmonitor* monitor = nullptr;
	if (m_Fullscreen) {
		monitor = glfwGetPrimaryMonitor();
	}
	glfwWindowHint(GLFW_SAMPLES, 8);
	m_Window = glfwCreateWindow(m_Resolution.Width, m_Resolution.Height, "daydream", monitor, nullptr);
	if (!m_Window) {
		LOG_ERROR("GLFW: Failed to create window");
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(m_Window);

	// GL version info
	glGetIntegerv(GL_MAJOR_VERSION, &m_GLVersion[0]);
	glGetIntegerv(GL_MINOR_VERSION, &m_GLVersion[1]);
	m_GLVendor = (GLchar*)glGetString(GL_VENDOR);
	std::stringstream ss;
	ss << m_GLVendor << " OpenGL " << m_GLVersion[0] << "." << m_GLVersion[1];
#ifdef DEBUG
	ss << " DEBUG";
#endif
	LOG_INFO(ss.str().c_str());
	glfwSetWindowTitle(m_Window, ss.str().c_str());

	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		LOG_ERROR("GLEW: Initialization failed");
		exit(EXIT_FAILURE);
	}

	// Create default camera
	m_DefaultCamera = std::unique_ptr<dd::Camera>(new dd::Camera((float)m_Resolution.Width / m_Resolution.Height, 45.f, 0.01f, 5000.f));
	m_DefaultCamera->SetPosition(glm::vec3(0, 0, 0));
	if (m_Camera == nullptr) {
		m_Camera = m_DefaultCamera.get();
	}

	glfwSwapInterval(m_VSYNC);

	LoadShaders();
	CreateBuffers();

	m_CurrentScreenBuffer = m_TFinal;
}

void dd::Renderer::LoadShaders()
{
	/*
		Deferred rendering
	*/

	// Pass #1: Fill G-buffers
	m_SpDeferred1 = ResourceManager::Load<ShaderProgram>("Shaders/OpenGL/Deferred/1/");
	m_SpDeferred1->BindFragDataLocation(0, "GDiffuse");
	m_SpDeferred1->BindFragDataLocation(1, "GPosition");
	m_SpDeferred1->BindFragDataLocation(2, "GNormal");
	m_SpDeferred1->BindFragDataLocation(3, "GSpecular");
	m_SpDeferred1->Link();

	// Pass #2: Lighting
	m_SpDeferred2 = ResourceManager::Load<ShaderProgram>("Shaders/OpenGL/Deferred/2/");
	//glBindFragDataLocation(m_SPDeferred2, 0, "FragmentLighting");
	m_SpDeferred2->Link();

	//Water Pass
	m_SpWater = ResourceManager::Load<ShaderProgram>("Shaders/OpenGL/Deferred/water/");
	m_SpWater->Link();
	m_SpWater2 = ResourceManager::Load<ShaderProgram>("Shaders/OpenGL/Deferred/water2/");
	m_SpWater2->Link();

	// Pass #3: Combining into final image
	m_SpDeferred3 = ResourceManager::Load<ShaderProgram>("Shaders/OpenGL/Deferred/3/");
	m_SpDeferred3->Link();

	/*
		Forward rendering
	*/
	m_SpForward = ResourceManager::Load<ShaderProgram>("Shaders/OpenGL/Forward/");
	m_SpForward->Link();

	/*
		Screen draw
	*/
	m_SpScreen = ResourceManager::Load<ShaderProgram>("Shaders/OpenGL/Screen/");
	m_SpScreen->Link();
}

void dd::Renderer::CreateBuffers()
{
	//TODO: Make the most common cases of texture create and FBO create into a function so it's not so cluttered in here.
	m_ScreenQuad = CreateQuad();
	m_UnitQuad = ResourceManager::Load<Model>("Models/Core/UnitQuad.obj");
	m_UnitSphere = ResourceManager::Load<Model>("Models/Core/UnitSphere.obj");
	m_ErrorTexture = ResourceManager::Load<Texture>("Textures/Core/ErrorTexture.png");
	m_StandardNormal = ResourceManager::Load<Texture>("Textures/Core/NeutralNormalMap.png");
	m_StandardSpecular = ResourceManager::Load<Texture>("Textures/Core/NeutralSpecularMap.png");
	m_WhiteSphereTexture = ResourceManager::Load<Texture>("Textures/Test/Water.png");

	glGenRenderbuffers(1, &m_RbDepthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_RbDepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_Resolution.Width, m_Resolution.Height);

	// Generate G-buffer textures
	glGenTextures(1, &m_GDiffuse);
	glBindTexture(GL_TEXTURE_2D, m_GDiffuse);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Resolution.Width, m_Resolution.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glGenTextures(1, &m_GPosition);
	glBindTexture(GL_TEXTURE_2D, m_GPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_Resolution.Width, m_Resolution.Height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glGenTextures(1, &m_GNormal);
	glBindTexture(GL_TEXTURE_2D, m_GNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_Resolution.Width, m_Resolution.Height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glGenTextures(1, &m_GSpecular);
	glBindTexture(GL_TEXTURE_2D, m_GSpecular);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_Resolution.Width, m_Resolution.Height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glGenTextures(1, &m_Gwater);
	glBindTexture(GL_TEXTURE_2D, m_Gwater);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_Resolution.Width, m_Resolution.Height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Create first pass framebuffer
	glGenFramebuffers(1, &m_FbDeferred1);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FbDeferred1);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RbDepthBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_GDiffuse, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_GPosition, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_GNormal, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_GSpecular, 0);
	GLenum firstPassDrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, firstPassDrawBuffers);
	if (GLenum fbStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		LOG_ERROR("m_FbDeferred1 incomplete: 0x%x\n", fbStatus);
		exit(EXIT_FAILURE);
	}

	// Generate lighting texture
	glGenTextures(1, &m_TLighting);
	glBindTexture(GL_TEXTURE_2D, m_TLighting);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Resolution.Width, m_Resolution.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Create second pass framebuffer
	glGenFramebuffers(1, &m_FbDeferred2);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FbDeferred2);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TLighting, 0);
	GLenum secondPassDrawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, secondPassDrawBuffers);
	if (GLenum fbStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		LOG_ERROR("m_FbDeferred2 incomplete: 0x%x\n", fbStatus);
		exit(EXIT_FAILURE);
	}

	//Fill Water Texture
	glGenTextures(1, &m_Gwater);
	glBindTexture(GL_TEXTURE_2D, m_Gwater);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Resolution.Width, m_Resolution.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Fill water pass
	glGenFramebuffers(1, &m_FbWater);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FbWater);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Gwater, 0);
	GLenum waterPassDrawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, waterPassDrawBuffers);
	if (GLenum fbStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		LOG_ERROR("m_FbDeferred2 incomplete: 0x%x\n", fbStatus);
		exit(EXIT_FAILURE);
	}

	//water Blur texture
	glGenTextures(1, &m_BWater);
	glBindTexture(GL_TEXTURE_2D, m_BWater);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Resolution.Width, m_Resolution.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//Fill waterBlur pass
	glGenFramebuffers(1, &m_FbWaterBlur);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FbWaterBlur);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_BWater, 0);
	GLenum waterBlurDrawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, waterBlurDrawBuffers);
	if (GLenum fbStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		LOG_ERROR("m_FbDeferred2 incomplete: 0x%x\n", fbStatus);
		exit(EXIT_FAILURE);
	}

	//water Blur texture2
	glGenTextures(1, &m_BWater2);
	glBindTexture(GL_TEXTURE_2D, m_BWater2);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RbDepthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Resolution.Width, m_Resolution.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//Fill waterBlur pass2
	glGenFramebuffers(1, &m_FbWaterBlur2);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FbWaterBlur2);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_BWater2, 0);
	GLenum waterBlur2DrawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, waterBlur2DrawBuffers);
	if (GLenum fbStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		LOG_ERROR("m_FbDeferred2 incomplete: 0x%x\n", fbStatus);
		exit(EXIT_FAILURE);
	}

	// Generate final deferred texture
	glGenTextures(1, &m_TFinal);
	glBindTexture(GL_TEXTURE_2D, m_TFinal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Resolution.Width, m_Resolution.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Create third pass framebuffer
	glGenFramebuffers(1, &m_FbDeferred3);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FbDeferred3);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RbDepthBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TFinal, 0);
	GLenum thirdPassDrawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, thirdPassDrawBuffers);
	if (GLenum fbStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		LOG_ERROR("m_FbDeferred3 incomplete: 0x%x\n", fbStatus);
		exit(EXIT_FAILURE);
	}
}

void dd::Renderer::Draw(RenderQueueCollection& rq)
{
	rq.Forward.Jobs.sort(dd::Renderer::DepthSort);
	DrawDeferred(rq.Deferred, rq.Lights);
	DrawForward(rq.Forward, rq.Lights);
	DrawGUI(rq.GUI);

	// Finally: Draw the deferred+forward combined texture to the screen
	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(1, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	m_SpScreen->Bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_CurrentScreenBuffer);
	glBindVertexArray(m_ScreenQuad);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glfwSwapBuffers(m_Window);

	DebugKeys();
}

void dd::Renderer::DrawDeferred(RenderQueue &objects, RenderQueue &lights)
{
	// Pass #1: Fill G-buffers
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FbDeferred1);
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_SpDeferred1->Bind();
	DrawScene(objects, *m_SpDeferred1);

	// Pass #2: Lighting
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);
	glDepthMask(GL_FALSE);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FbDeferred2);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	m_SpDeferred2->Bind();
	DrawLightSpheres(lights);

	// Pass #3: Combine into final deferred image
	glCullFace(GL_BACK);
	glDisable(GL_BLEND);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FbDeferred3);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	m_SpDeferred3->Bind();
	glUniform3fv(glGetUniformLocation(*m_SpDeferred3, "La"), 1, glm::value_ptr(glm::vec3(0.5f)));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_GDiffuse);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_TLighting);
	glBindVertexArray(m_ScreenQuad);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void dd::Renderer::DrawForward(RenderQueue &objects, RenderQueue &lights)
{
	// Forward-render semi-transparent objects on top of the current framebuffer
	glDisable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindFramebuffer(GL_FRAMEBUFFER, m_FbDeferred3);
//	glClearColor(1, 0.9, 0.8f, 1);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_SpForward->Bind();
	DrawScene(objects, *m_SpForward);

	//WaterPass
	glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FbWater);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	m_SpWater->Bind();
	DrawWater(objects);
}

void dd::Renderer::PlaceCamera(glm::vec3 position)
{
	m_DefaultCamera->SetPosition(position);
}

void dd::Renderer::DrawScene(RenderQueue &objects, ShaderProgram &program)
{
	GLuint shaderProgramHandle = program;

	glm::mat4 viewMatrix = m_Camera->ViewMatrix();
	glm::mat4 PV = m_Camera->ProjectionMatrix() * viewMatrix;
	glm::mat4 MVP;

	for (auto &job : objects) {
		auto modelJob = std::dynamic_pointer_cast<ModelJob>(job);
		if (modelJob) {
			glm::mat4 modelMatrix = modelJob->ModelMatrix;
			MVP = PV * modelMatrix;
			glUniform4fv(glGetUniformLocation(shaderProgramHandle, "Color"), 1, glm::value_ptr(modelJob->Color));
			glUniformMatrix4fv(glGetUniformLocation(shaderProgramHandle, "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
			glUniformMatrix4fv(glGetUniformLocation(shaderProgramHandle, "M"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
			glUniformMatrix4fv(glGetUniformLocation(shaderProgramHandle, "V"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniform3fv(glGetUniformLocation(shaderProgramHandle, "LightPosition"), 1, glm::value_ptr(glm::vec3(0.f,0.f,-9.f)));
			glUniform3fv(glGetUniformLocation(shaderProgramHandle, "LightSpecular"), 1, glm::value_ptr(glm::vec3(1.f)));
			glUniform3fv(glGetUniformLocation(shaderProgramHandle, "LightDiffuse"), 1, glm::value_ptr(glm::vec3(1.f)));
			glUniform1f(glGetUniformLocation(shaderProgramHandle, "LightRadius"), 40.0f);
			glUniform1f(glGetUniformLocation(shaderProgramHandle, "MaterialShininess"), modelJob->Shininess);

			if (modelJob->DiffuseTexture != nullptr) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, modelJob->DiffuseTexture->m_Texture);
			} else {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, m_ErrorTexture->m_Texture);
			}

			if (modelJob->NormalTexture != nullptr) {
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, modelJob->NormalTexture->m_Texture);
			} else {
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, m_StandardNormal->m_Texture);
			}

			if (modelJob->SpecularTexture != nullptr) {
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, modelJob->SpecularTexture->m_Texture);
			} else {
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, m_StandardSpecular->m_Texture);
			}

			if (modelJob->Skeleton != nullptr) {
				auto animation = modelJob->Skeleton->GetAnimation(modelJob->AnimationName);
				if (animation != nullptr) {
					std::vector<glm::mat4> frameBones = modelJob->Skeleton->GetFrameBones(
						*animation,
						modelJob->AnimationTime,
						modelJob->NoRootMotion
					);
					glUniformMatrix4fv(glGetUniformLocation(shaderProgramHandle, "Bones"), frameBones.size(), GL_FALSE, glm::value_ptr(frameBones[0]));
				} else {
					LOG_WARNING("Tried to play unknown animation \"%s\"", modelJob->AnimationName.c_str());
				}
			}

			glBindVertexArray(modelJob->Model->VAO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelJob->Model->ElementBuffer);
			glDrawElementsBaseVertex(GL_TRIANGLES, modelJob->EndIndex - modelJob->StartIndex + 1, GL_UNSIGNED_INT, 0, modelJob->StartIndex);

			continue;
		}

		auto spriteJob = std::dynamic_pointer_cast<SpriteJob>(job);
		if (spriteJob)
		{
			glm::mat4 modelMatrix = spriteJob->ModelMatrix;
			MVP = PV * modelMatrix;
			glUniformMatrix4fv(glGetUniformLocation(shaderProgramHandle, "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
			glUniformMatrix4fv(glGetUniformLocation(shaderProgramHandle, "M"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
			glUniformMatrix4fv(glGetUniformLocation(shaderProgramHandle, "V"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniform4fv(glGetUniformLocation(shaderProgramHandle, "Color"), 1, glm::value_ptr(spriteJob->Color));

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, spriteJob->DiffuseTexture->m_Texture);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, spriteJob->NormalTexture->m_Texture);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, spriteJob->SpecularTexture->m_Texture);

			glBindVertexArray(m_UnitQuad->VAO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_UnitQuad->ElementBuffer);
			glDrawElementsBaseVertex(GL_TRIANGLES, m_UnitQuad->m_Indices.size(), GL_UNSIGNED_INT, 0, 0);

			continue;
		}
	}
}

void dd::Renderer::DrawLightSpheres(RenderQueue &lights)
{
	GLuint shaderProgramHandle = *m_SpDeferred2;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_GPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_GNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_GSpecular);

	glm::mat4 projectionMatrix = m_Camera->ProjectionMatrix();
	glm::mat4 viewMatrix = m_Camera->ViewMatrix();
	glm::mat4 PV = projectionMatrix * viewMatrix;
	glm::mat4 MVP;

	for (auto &job : lights) {
		auto pointLightJob = std::dynamic_pointer_cast<PointLightJob>(job);
		if (pointLightJob) {
			glm::mat4 modelMatrix = glm::translate(pointLightJob->Position) * glm::scale(glm::vec3(pointLightJob->Radius * 2.f));
			MVP = PV * modelMatrix;
			glUniform2fv(glGetUniformLocation(shaderProgramHandle, "ViewportSize"), 1, glm::value_ptr(glm::vec2(m_Resolution.Width, m_Resolution.Height)));
			glUniformMatrix4fv(glGetUniformLocation(shaderProgramHandle, "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
			glUniformMatrix4fv(glGetUniformLocation(shaderProgramHandle, "M"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
			glUniformMatrix4fv(glGetUniformLocation(shaderProgramHandle, "V"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(glGetUniformLocation(shaderProgramHandle, "P"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
			glUniform3fv(glGetUniformLocation(shaderProgramHandle, "LightPosition"), 1, glm::value_ptr(pointLightJob->Position));
			glUniform1f(glGetUniformLocation(shaderProgramHandle, "LightRadius"), pointLightJob->Radius);
			glUniform3fv(glGetUniformLocation(shaderProgramHandle, "LightDiffuse"), 1, glm::value_ptr(pointLightJob->DiffuseColor));
			glUniform3fv(glGetUniformLocation(shaderProgramHandle, "LightSpecular"), 1, glm::value_ptr(pointLightJob->SpecularColor));

			glBindVertexArray(m_UnitSphere->VAO);
			glDrawArrays(GL_TRIANGLES, 0, m_UnitSphere->m_Vertices.size());
		}
	}
}

void dd::Renderer::DrawWater(RenderQueue &rq)
{
	GLuint shaderProgramHandle = *m_SpWater;

	glm::mat4 projectionMatrix = m_Camera->ProjectionMatrix();
	glm::mat4 viewMatrix = m_Camera->ViewMatrix();
	glm::mat4 PV = projectionMatrix * viewMatrix;
	glm::mat4 MVP;
	for ( auto &job : rq ) {
		auto waterJob = std::dynamic_pointer_cast<WaterParticleJob>(job);
		if (waterJob) {
			glm::mat4 modelMatrix = waterJob->ModelMatrix;
			MVP = PV * modelMatrix;
			glUniformMatrix4fv(glGetUniformLocation(shaderProgramHandle, "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
			glUniformMatrix4fv(glGetUniformLocation(shaderProgramHandle, "M"), 1, GL_FALSE,
							   glm::value_ptr(modelMatrix));
			glUniformMatrix4fv(glGetUniformLocation(shaderProgramHandle, "V"), 1, GL_FALSE,
							   glm::value_ptr(viewMatrix));

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_WhiteSphereTexture->m_Texture);

			glBindVertexArray(m_UnitQuad->VAO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_UnitQuad->ElementBuffer);
			glDrawElementsBaseVertex(GL_TRIANGLES, m_UnitQuad->m_Indices.size(), GL_UNSIGNED_INT, 0, 0);
		}
	}



	//blur1
	shaderProgramHandle = *m_SpWater2;
	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FbWaterBlur);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	m_SpWater2->Bind();
	//TODO: Add this in water particle component. Blurradius
	float radius = 3.f;

	glUniform2fv(glGetUniformLocation(shaderProgramHandle, "dir"), 1, glm::value_ptr(glm::vec2(1.0f, 0.0f)));
	glUniform1f(glGetUniformLocation(shaderProgramHandle, "res"), m_Resolution.Width);
	glUniform1f(glGetUniformLocation(shaderProgramHandle, "radius"), radius);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_Gwater);
	glBindVertexArray(m_ScreenQuad);
	glDrawArrays(GL_TRIANGLES, 0, 6);


	//blur2
	shaderProgramHandle = *m_SpWater2;
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FbDeferred3);
	m_SpWater2->Bind();

	glUniform2fv(glGetUniformLocation(shaderProgramHandle, "dir"), 1, glm::value_ptr(glm::vec2(0.0f, 1.0f)));
	glUniform1f(glGetUniformLocation(shaderProgramHandle, "res"), m_Resolution.Height);
	glUniform1f(glGetUniformLocation(shaderProgramHandle, "radius"), radius);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_BWater);
	glBindVertexArray(m_ScreenQuad);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

GLuint dd::Renderer::CreateQuad()
{
	float quadVertices[] =
	{
		-1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,

		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
	};
	float quadTexCoords[] =
	{
		0.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
	};
	GLuint vbo[2], vao;
	glGenBuffers(2, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, 3 * 6 * sizeof(float), quadVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, 2 * 6 * sizeof(float), quadTexCoords, GL_STATIC_DRAW);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(4);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return vao;
}

void dd::Renderer::DebugKeys()
{
	if (glfwGetKey(m_Window, GLFW_KEY_F1)) {
		m_CurrentScreenBuffer = m_TFinal;
	}
	if (glfwGetKey(m_Window, GLFW_KEY_F2)) {
		m_CurrentScreenBuffer = m_GDiffuse;
	}
	if (glfwGetKey(m_Window, GLFW_KEY_F3)) {
		m_CurrentScreenBuffer = m_GPosition;
	}
	if (glfwGetKey(m_Window, GLFW_KEY_F4)) {
		m_CurrentScreenBuffer = m_GNormal;
	}
	if (glfwGetKey(m_Window, GLFW_KEY_F5)) {
		m_CurrentScreenBuffer = m_GSpecular;
	}
	if (glfwGetKey(m_Window, GLFW_KEY_F6)) {
		m_CurrentScreenBuffer = m_TLighting;
	}
	if (glfwGetKey(m_Window, GLFW_KEY_F7)) {
		m_CurrentScreenBuffer  = m_Gwater;
	}
	if (glfwGetKey(m_Window, GLFW_KEY_F8)) {
		m_CurrentScreenBuffer  = m_BWater;
	}
}

void dd::Renderer::DrawGUI(dd::RenderQueue& rq)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FbDeferred3);
	glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	//glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);

	m_SpScreen->Bind();

	glm::mat4 MVP;

	for (auto &job : rq) {
		auto frameJob = std::dynamic_pointer_cast<FrameJob>(job);
		if (frameJob) {
			FrameJob jobCopy = *(frameJob.get());
			glm::mat4 viewMatrix = glm::mat4(1); //frameJob->ViewMatrix;
			glm::mat4 PV = glm::mat4(1); //frameJob->ProjectionMatrix * viewMatrix;
			glm::mat4 modelMatrix = glm::mat4(1); //frameJob->ModelMatrix;
			MVP = PV * modelMatrix;
			glUniformMatrix4fv(glGetUniformLocation(*m_SpScreen, "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
			glUniformMatrix4fv(glGetUniformLocation(*m_SpScreen, "M"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
			glUniformMatrix4fv(glGetUniformLocation(*m_SpScreen, "V"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniform4fv(glGetUniformLocation(*m_SpScreen, "Color"), 1, glm::value_ptr(frameJob->Color));

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, frameJob->DiffuseTexture->m_Texture);
			//glActiveTexture(GL_TEXTURE1);
			//glBindTexture(GL_TEXTURE_2D, frameJob->NormalTexture);
			//glActiveTexture(GL_TEXTURE2);
			//glBindTexture(GL_TEXTURE_2D, frameJob->SpecularTexture);

			Rectangle& vp = frameJob->Viewport;
			glViewport(vp.X, m_Resolution.Height - vp.Y - vp.Height, vp.Width, vp.Height);
			Rectangle& sc = frameJob->Scissor;
			if (sc == Rectangle()) {
				glDisable(GL_SCISSOR_TEST);
			} else {
				glEnable(GL_SCISSOR_TEST);
				glScissor(sc.X, m_Resolution.Height - sc.Y - sc.Height, sc.Width, sc.Height);
			}

			glBindVertexArray(m_ScreenQuad);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			continue;
		}
	}

	glDisable(GL_SCISSOR_TEST);
	glViewport(0, 0, m_Resolution.Width, m_Resolution.Height);
}
