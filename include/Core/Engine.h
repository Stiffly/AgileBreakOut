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
#include "Core/EventBroker.h"
#include "Rendering/CModel.h"
#include "Rendering/CSprite.h"
#include "CTemplate.h"
#include "Rendering/CPointLight.h"
#include "Transform/TransformSystem.h"
#include "Game/LevelSystem.h"
#include "Game/PadSystem.h"
#include "Game/CBall.h"
#include "Game/CBrick.h"
#include "Game/CPad.h"

#include "Physics/PhysicsSystem.h"
#include "Physics/CPhysics.h"
#include "Physics/CBoxShape.h"
#include "Physics/ESetImpulse.h"

namespace dd
{

class Engine
{
public:
	Engine(int argc, char* argv[]) {
        m_EventBroker = std::make_shared<EventBroker>();

        m_Renderer = std::make_shared<Renderer>();
        m_Renderer->SetFullscreen(false);
        m_Renderer->SetResolution(Rectangle(0, 0, 1920, 1080));
        m_Renderer->Initialize();

        m_InputManager = std::make_shared<InputManager>(m_Renderer->Window(), m_EventBroker);

        m_World = std::make_shared<World>(m_EventBroker);

        //TODO: Move this out of engine.h
        m_World->ComponentFactory.Register<Components::Transform>();
        m_World->SystemFactory.Register<Systems::TransformSystem>(
                [this]() { return new Systems::TransformSystem(m_World.get(), m_EventBroker); });
        m_World->AddSystem<Systems::TransformSystem>();
//		m_World->ComponentFactory.Register<Components::Model>();
//		m_World->ComponentFactory.Register<Components::Template>();


        m_World->ComponentFactory.Register<Components::Sprite>();

        m_World->ComponentFactory.Register<Components::RectangleShape>();
        m_World->ComponentFactory.Register<Components::Physics>();
		m_World->ComponentFactory.Register<Components::Ball>();
		m_World->ComponentFactory.Register<Components::Brick>();
		m_World->ComponentFactory.Register<Components::Pad>();
		m_World->ComponentFactory.Register<Components::Life>();
        m_World->SystemFactory.Register<Systems::PhysicsSystem>(
                [this]() { return new Systems::PhysicsSystem(m_World.get(), m_EventBroker); });
        m_World->AddSystem<Systems::PhysicsSystem>();

		m_World->SystemFactory.Register<Systems::LevelSystem>([this]() { return new Systems::LevelSystem(m_World.get(), m_EventBroker); });
		m_World->AddSystem<Systems::LevelSystem>();
		m_World->SystemFactory.Register<Systems::PadSystem>([this]() { return new Systems::PadSystem(m_World.get(), m_EventBroker); });
		m_World->AddSystem<Systems::PadSystem>();

        m_World->ComponentFactory.Register<Components::Model>();
        m_World->ComponentFactory.Register<Components::Template>();
		m_World->ComponentFactory.Register<Components::PointLight>();
        m_World->Initialize();


		//TODO: Remove tobias light-test code.
		/*{
		}*/

        {
            auto ent = m_World->CreateEntity();
            std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(ent);
            transform->Position = glm::vec3(0.5f, 0.f, -9.9f);
			transform->Scale = glm::vec3(1.f, 1.f, 1.f);

            std::shared_ptr<Components::Sprite> sprite = m_World->AddComponent<Components::Sprite>(ent);
            sprite->SpriteFile = "Textures/Ball.png";

            std::shared_ptr<Components::CircleShape> circleShape = m_World->AddComponent<Components::CircleShape>(ent);
			std::shared_ptr<Components::Ball> ball = m_World->AddComponent<Components::Ball>(ent);

            std::shared_ptr<Components::Physics> physics = m_World->AddComponent<Components::Physics>(ent);
            physics->Static = false;

			auto plight = m_World->AddComponent<Components::PointLight>(ent);
			plight->Radius = 2.f;

            m_World->CommitEntity(ent);

			Events::SetImpulse e;
			e.Entity = ent;
			e.Impulse = glm::vec2(0.f, -7.f);
			e.Point = glm::vec2(0.5f, 0.f);
			m_EventBroker->Publish(e);
        }

		{
			auto t_BrickWall = m_World->CreateEntity();
			auto transform = m_World->AddComponent<Components::Transform>(t_BrickWall);
			transform->Position = glm::vec3(0.f, 0.f, -10.f);
			auto sprite = m_World->AddComponent<Components::Sprite>(t_BrickWall);
			//TODO: Rename SpriteFile to DiffuseTexture or similar.
			sprite->SpriteFile = "Textures/Ball.png";
			sprite->NormalTexture = "Textures/Test/Brick_Normal.png";
			sprite->SpecularTexture = "Textures/Test/Brick_Specular.png";
		}

		{
			auto t_BrickWall = m_World->CreateEntity();
			auto transform = m_World->AddComponent<Components::Transform>(t_BrickWall);
			transform->Position = glm::vec3(0.4f, 0.f, -9.5f);
			auto sprite = m_World->AddComponent<Components::Sprite>(t_BrickWall);
			//TODO: Rename SpriteFile to DiffuseTexture or similar.
			sprite->SpriteFile = "Textures/Ball.png";
			sprite->NormalTexture = "Textures/Test/Brick_Normal.png";
			sprite->SpecularTexture = "Textures/Test/Brick_Specular.png";
		}

		{
			auto topWall = m_World->CreateEntity();
			std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(topWall);
			transform->Position = glm::vec3(0.f, 6.f, -10.f);
			transform->Scale = glm::vec3(18.f, 0.5f, 1.f);

			std::shared_ptr<Components::Sprite> sprite = m_World->AddComponent<Components::Sprite>(topWall);
			sprite->SpriteFile = "Textures/Core/ErrorTexture.png";

			std::shared_ptr<Components::RectangleShape> boxShape = m_World->AddComponent<Components::RectangleShape>(topWall);

			std::shared_ptr<Components::Physics> physics = m_World->AddComponent<Components::Physics>(topWall);
			physics->Static = true;

			m_World->CommitEntity(topWall);
		}
		{
			auto leftWall = m_World->CreateEntity();
			std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(leftWall);
			transform->Position = glm::vec3(-9.f, 1.f, -10.f);
			transform->Scale = glm::vec3(0.5f, 10.f, 1.f);

			std::shared_ptr<Components::Sprite> sprite = m_World->AddComponent<Components::Sprite>(leftWall);
			sprite->SpriteFile = "Textures/Core/ErrorTexture.png";

			std::shared_ptr<Components::RectangleShape> boxShape = m_World->AddComponent<Components::RectangleShape>(leftWall);

			std::shared_ptr<Components::Physics> physics = m_World->AddComponent<Components::Physics>(leftWall);
			physics->Static = true;

			m_World->CommitEntity(leftWall);
		}
		{
			auto rightWall = m_World->CreateEntity();
			std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(rightWall);
			transform->Position = glm::vec3(9.f, 1.f, -10.f);
			transform->Scale = glm::vec3(0.5f, 10.f, 1.f);

			std::shared_ptr<Components::Sprite> sprite = m_World->AddComponent<Components::Sprite>(rightWall);
			sprite->SpriteFile = "Textures/Core/ErrorTexture.png";

			std::shared_ptr<Components::RectangleShape> boxShape = m_World->AddComponent<Components::RectangleShape>(rightWall);

			std::shared_ptr<Components::Physics> physics = m_World->AddComponent<Components::Physics>(rightWall);
			physics->Static = true;

			m_World->CommitEntity(rightWall);
		}


        /*{
            auto ent = m_World->CreateEntity();
            std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(ent);
            transform->Position = glm::vec3(0.f, -3.f, -10.f);
            transform->Scale = glm::vec3(8.f, 0.5f, 1.f);
            transform->Orientation = glm::rotate(transform->Orientation, glm::radians(25.f), glm::vec3(0, 0, -1));

            std::shared_ptr<Components::Sprite> sprite = m_World->AddComponent<Components::Sprite>(ent);
            sprite->SpriteFile = "Textures/Core/ErrorTexture.png";

            std::shared_ptr<Components::RectangleShape> boxShape = m_World->AddComponent<Components::RectangleShape>(ent);

            std::shared_ptr<Components::Physics> physics = m_World->AddComponent<Components::Physics>(ent);
            physics->Static = true;

            m_World->CommitEntity(ent);
        }*/

		{
			auto ent = m_World->CreateEntity();
			m_World->SetProperty(ent, "Name", "Pad");
			auto ctransform = m_World->AddComponent<Components::Transform>(ent);
			ctransform->Position = glm::vec3(0.f, -5.f, -10.f);
			ctransform->Scale = glm::vec3(3.2, 0.8, 0.);
			auto rectangle = m_World->AddComponent<Components::RectangleShape>(ent);
			auto physics = m_World->AddComponent<Components::Physics>(ent);
			auto csprite = m_World->AddComponent<Components::Sprite>(ent);
			auto pad = m_World->AddComponent<Components::Pad>(ent);
			csprite->SpriteFile = "Textures/Pad.png";
			m_World->CommitEntity(ent);
		}

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


//
//		if (glfwGetKey(m_Renderer->Window(), GLFW_KEY_R)) {
//			ResourceManager::Reload("Shaders/Deferred/3/Fragment.glsl");
//		}
//
		//TODO Fill up the renderQueue with models (Temp fix)
//		TEMPAddToRenderQueue();

		// Render scene
		//TODO send renderqueue to draw.
//		m_Renderer->Draw(m_RendererQueue);

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
	std::shared_ptr<Systems::LevelSystem> m_LevelSystem;

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

			//TODO: Add LightLoadShit

			auto pointLightComponent = m_World->GetComponent<Components::PointLight>(entity);
			if (pointLightComponent)
			{
				Components::Transform absoluteTransform = m_TransformSystem->AbsoluteTransform(entity);
				EnqueuePointLight(absoluteTransform.Position,
								  pointLightComponent->Diffuse,
								  pointLightComponent->Specular,
								  pointLightComponent->Radius);
			}


			auto spriteComponent = m_World->GetComponent<Components::Sprite>(entity);
			if (spriteComponent)
			{

				std::string normal = spriteComponent->NormalTexture;
				std::string spec = spriteComponent->SpecularTexture;

				if (normal.empty()) {
					normal = "Textures/Core/NeutralNormalMap.png";
				}
				if (spec.empty()) {
					spec = "Textures/Core/NeutralSpecularMap.png";
				}
				auto texturediff = ResourceManager::Load<Texture>(spriteComponent->SpriteFile);
				auto texturenorm = ResourceManager::Load<Texture>(normal);
				auto texturespec = ResourceManager::Load<Texture>(spec);


				Components::Transform absoluteTransform = m_TransformSystem->AbsoluteTransform(entity);
				glm::quat orientation2D = glm::angleAxis(glm::eulerAngles(absoluteTransform.Orientation).z, glm::vec3(0, 0, -1));
				glm::mat4 modelMatrix = glm::translate(absoluteTransform.Position)
					* glm::toMat4(orientation2D)
					* glm::scale(absoluteTransform.Scale);
				EnqueueSprite(texturediff, texturenorm, texturespec, modelMatrix, spriteComponent->Color, absoluteTransform.Position.z);
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
			//TODO: This Depth is probably wrong.
			job.Depth = (glm::vec4(1,1,1,0) * modelMatrix).z;

			m_RendererQueue.Deferred.Add(job);
		}
	}

	// TODO: Get this out of engine.h
	void EnqueueSprite(Texture* texture, Texture* normalTexture, Texture* specularTexture, glm::mat4 modelMatrix, glm::vec4 color, float depth)
	{
		SpriteJob job;
		job.TextureID = texture->ResourceID;
		job.DiffuseTexture = *texture;
		job.NormalTexture = *normalTexture;
		job.SpecularTexture = *specularTexture;
		job.ModelMatrix = modelMatrix;
		job.Color = color;
		job.Depth = depth;


		m_RendererQueue.Forward.Add(job);
	}

	void EnqueuePointLight(glm::vec3 position, glm::vec3 diffuseColor, glm::vec3 specularColor, float radius)
	{
		PointLightJob job;
		job.Position = position;
		job.DiffuseColor = diffuseColor;
		job.SpecularColor = specularColor;
		job.Radius = radius;

		m_RendererQueue.Lights.Add(job);

	}

private:
	std::shared_ptr<ResourceManager> m_ResourceManager;
	std::shared_ptr<EventBroker> m_EventBroker;
	std::shared_ptr<Renderer> m_Renderer;
	RenderQueueCollection m_RendererQueue;
	std::shared_ptr<InputManager> m_InputManager;
	std::shared_ptr<World> m_World;

	double m_LastTime;
};

}
