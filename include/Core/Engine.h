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

#include <string>
#include <sstream>

#include "ResourceManager.h"
#include "OBJ.h"
#include "Model.h"
#include "Texture.h"
#include "EventBroker.h"
#include "RenderQueue.h"
#include "Renderer.h"
#include "InputManager.h"
//TODO: Remove includes that are only here for the temporary draw solution.
#include "World.h"
#include "CTransform.h"
#include "Rendering/CModel.h"
#include "CTemplate.h"
#include "Transform/TransformSystem.h"

namespace dd
{

class Engine
{
public:
	Engine(int argc, char* argv[])
	{
		m_EventBroker = std::make_shared<EventBroker>();

		m_Renderer = std::make_shared<Renderer>();
		m_Renderer->SetFullscreen(false);
		m_Renderer->SetResolution(Rectangle(0, 0, 1920, 1080));
		m_Renderer->Initialize();

		m_InputManager = std::make_shared<InputManager>(m_Renderer->Window(), m_EventBroker);

		m_World = std::make_shared<World>(m_EventBroker);

		//TODO: Move this out of engine.h
		m_World->ComponentFactory.Register<Components::Transform>();
		m_World->SystemFactory.Register<Systems::TransformSystem>([this]() { return new Systems::TransformSystem(m_World.get(), m_EventBroker); });
		m_World->AddSystem<Systems::TransformSystem>();
		m_World->ComponentFactory.Register<Components::Model>();
		m_World->ComponentFactory.Register<Components::Template>();
		m_World->Initialize();

		auto ent = m_World->CreateEntity();
		std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(ent);
		transform->Position = glm::vec3(0.f, 0.f, -10.f);
		std::shared_ptr<Components::Model> model = m_World->AddComponent<Components::Model>(ent);
		model->ModelFile = "Models/Core/UnitSphere.obj";


		m_LastTime = glfwGetTime();
	}

	bool Running() const { return !glfwWindowShouldClose(m_Renderer->Window()); }

	void Tick()
	{
		double currentTime = glfwGetTime();
		double dt = currentTime - m_LastTime;
		m_LastTime = currentTime;

		ResourceManager::Update();

		// Update input
		m_InputManager->Update(dt);

		m_World->Update(dt);

		if (glfwGetKey(m_Renderer->Window(), GLFW_KEY_R)) {
			ResourceManager::Reload("Shaders/Deferred/3/Fragment.glsl");
		}

		//TODO Fill up the renderQueue with models (Temp fix)
		TEMPAddToRenderQueue();

		// Render scene
		//TODO send renderqueue to draw.
		m_Renderer->Draw(m_RendererQueue);

		// Swap event queues
		m_EventBroker->Clear();

		glfwPollEvents();
	}

	std::shared_ptr<Systems::TransformSystem> m_TransformSystem;

	//TODO: Get this out of engine.h
	void TEMPAddToRenderQueue()
	{

		if (!m_TransformSystem)
			m_TransformSystem = m_World->GetSystem<Systems::TransformSystem>();

		m_RendererQueue.Clear();

		for (auto &pair : *m_World->GetEntities())
		{
			EntityID entity = pair.first;

			auto templateComponent = m_World->GetComponent<Components::Template>(entity);
			if (templateComponent)
				continue;

			auto transform = m_World->GetComponent<Components::Transform>(entity);
			if (!transform)
				continue;

			auto modelComponent = m_World->GetComponent<Components::Model>(entity);
			if (modelComponent)
			{
				Model* modelAsset = nullptr;
				modelAsset = ResourceManager::Load<Model>(modelComponent->ModelFile);

				if (modelAsset)
				{
					Components::Transform absoluteTransform = m_TransformSystem->AbsoluteTransform(entity);
					glm::mat4 modelMatrix = glm::translate(glm::mat4(), absoluteTransform.Position)
						* glm::toMat4(absoluteTransform.Orientation)
						* glm::scale(absoluteTransform.Scale);
					EnqueueModel(modelAsset, modelMatrix, modelComponent->Transparent, modelComponent->Color);
				}
			}
		}

		m_RendererQueue.Sort();
	}

	//TODO: Get this out of engine.h
	void EnqueueModel(Model* model, glm::mat4 modelMatrix, float transparent, glm::vec4 color)
	{
		for (auto texGroup : model->TextureGroups)
		{
			ModelJob job;
			job.TextureID = (texGroup.Texture) ? texGroup.Texture->ResourceID : 0;
			job.DiffuseTexture = (texGroup.Texture) ? *texGroup.Texture : 0;
			job.NormalTexture = (texGroup.NormalMap) ? *texGroup.NormalMap : 0;
			job.SpecularTexture = (texGroup.SpecularMap) ? *texGroup.SpecularMap : 0;
			job.VAO = model->VAO;
			job.ElementBuffer = model->ElementBuffer;
			job.StartIndex = texGroup.StartIndex;
			job.EndIndex = texGroup.EndIndex;
			job.ModelMatrix = modelMatrix;
			job.Color = color;

			m_RendererQueue.Deferred.Add(job);
		}
	}

private:
	//std::shared_ptr<ResourceManager> m_ResourceManager;
	std::shared_ptr<EventBroker> m_EventBroker;
	std::shared_ptr<Renderer> m_Renderer;
	RenderQueueCollection m_RendererQueue;
	std::shared_ptr<InputManager> m_InputManager;
	std::shared_ptr<World> m_World;

	double m_LastTime;
};

}
