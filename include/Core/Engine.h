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
#include <Sound/CCollisionSound.h>

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
#include "Sound/SoundSystem.h"
#include "Sound/CCollisionSound.h"
#include "Game/LevelSystem.h"
#include "Game/PadSystem.h"
#include "Game/CBall.h"
#include "Game/CPad.h"
#include "Game/CBrick.h"
#include "Game/BallSystem.h"
#include "Game/Bricks/CPowerUpBrick.h"

#include "Physics/PhysicsSystem.h"
#include "Physics/CPhysics.h"
#include "Physics/CRectangleShape.h"
#include "Physics/ESetImpulse.h"
#include "Physics/CWaterVolume.h"

#include "Game/EGameStart.h"
#include "Sound/EPlaySound.h"

#include "GUI/Frame.h"
#include "GUI/Button.h"
#include "Game/MainMenu.h"
#include "Game/HUD.h"

namespace dd
{

class Engine
{

public:
	Engine(int argc, char* argv[]) {
		m_EventBroker = std::make_shared<EventBroker>();

		m_Renderer = std::make_shared<Renderer>();
		m_Renderer->SetFullscreen(false);
		//m_Renderer->SetResolution(Rectangle(0, 0, 1920, 1080));
		m_Renderer->SetResolution(Rectangle(0, 0, 675, 1080));
		m_Renderer->Initialize();

		m_FrameStack = new GUI::Frame(m_EventBroker.get());
		m_FrameStack->Width = 675;
		m_FrameStack->Height = 1080;
		auto hud = new GUI::HUD(m_FrameStack, "HUD");
		auto menu = new GUI::MainMenu(m_FrameStack, "MainMenu");

		m_InputManager = std::make_shared<InputManager>(m_Renderer->Window(), m_EventBroker);

		m_World = std::make_shared<World>(m_EventBroker);

		//TODO: Move this out of engine.h
		m_World->ComponentFactory.Register<Components::Transform>();
		m_World->SystemFactory.Register<Systems::TransformSystem>(
				[this]() { return new Systems::TransformSystem(m_World.get(), m_EventBroker); });
		m_World->AddSystem<Systems::TransformSystem>();
		m_World->ComponentFactory.Register<Components::Model>();
		m_World->ComponentFactory.Register<Components::Template>();

		m_World->ComponentFactory.Register<Components::CollisionSound>();
		m_World->SystemFactory.Register<Systems::SoundSystem>(
				[this]() { return new Systems::SoundSystem(m_World.get(), m_EventBroker); });
		m_World->AddSystem<Systems::SoundSystem>();

		m_World->ComponentFactory.Register<Components::Sprite>();

		m_World->ComponentFactory.Register<Components::RectangleShape>();
		m_World->ComponentFactory.Register<Components::Physics>();
		m_World->ComponentFactory.Register<Components::Ball>();
		m_World->ComponentFactory.Register<Components::Brick>();
		m_World->ComponentFactory.Register<Components::Pad>();
		m_World->ComponentFactory.Register<Components::Life>();

		m_World->ComponentFactory.Register<Components::PowerUp>();
		m_World->ComponentFactory.Register<Components::PowerUpBrick>();

		m_World->SystemFactory.Register<Systems::PhysicsSystem>(
				[this]() { return new Systems::PhysicsSystem(m_World.get(), m_EventBroker); });
		m_World->AddSystem<Systems::PhysicsSystem>();

		m_World->SystemFactory.Register<Systems::LevelSystem>(
				[this]() { return new Systems::LevelSystem(m_World.get(), m_EventBroker); });
		m_World->AddSystem<Systems::LevelSystem>();
		m_World->SystemFactory.Register<Systems::PadSystem>(
				[this]() { return new Systems::PadSystem(m_World.get(), m_EventBroker); });
		m_World->AddSystem<Systems::PadSystem>();
		m_World->SystemFactory.Register<Systems::BallSystem>(
				[this]() { return new Systems::BallSystem(m_World.get(), m_EventBroker); });
		m_World->AddSystem<Systems::BallSystem>();

		m_World->ComponentFactory.Register<Components::Model>();
		m_World->ComponentFactory.Register<Components::Template>();
		m_World->ComponentFactory.Register<Components::PointLight>();
		m_World->ComponentFactory.Register<Components::WaterVolume>();
		m_World->Initialize();


		//OctoBall
		{
          	auto ent = m_World->CreateEntity();
        	std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(ent);
			transform->Position = glm::vec3(-0.f, 0.26f, -9.f);
			transform->Scale = glm::vec3(0.5f, 0.5f, 0.5f);
			transform->Velocity = glm::vec3(0.0f, -10.f, 0.f);
          	auto model = m_World->AddComponent<Components::Model>(ent);
			model->ModelFile = "Models/Test/Ball/Ballopus.obj";
          	std::shared_ptr<Components::CircleShape> circleShape = m_World->AddComponent<Components::CircleShape>(ent);
			std::shared_ptr<Components::Ball> ball = m_World->AddComponent<Components::Ball>(ent);
			ball->Speed = 5.f;
			std::shared_ptr<Components::Physics> physics = m_World->AddComponent<Components::Physics>(ent);
			physics->Static = false;
			physics->Category = CollisionLayer::Type::Ball;
			physics->Mask = CollisionLayer::Type::Pad | CollisionLayer::Type::Brick | CollisionLayer::Type::Wall;
			physics->Calculate = true;

			auto plight = m_World->AddComponent<Components::PointLight>(ent);
			plight->Radius = 2.f;

			m_World->CommitEntity(ent);
		}

		//PointLightTest
		{
			auto t_Light = m_World->CreateEntity();
			auto transform = m_World->AddComponent<Components::Transform>(t_Light);
			transform->Position = glm::vec3(-3.f, 6.f, -5.f);
			auto pl = m_World->AddComponent<Components::PointLight>(t_Light);
			pl->Radius = 20.f;
			pl->Diffuse = glm::vec3(0.8f, 0.7f, 0.05f);
			auto model = m_World->AddComponent<Components::Model>(t_Light);
			model->ModelFile = "Core/UnitSphere.obj";
			m_World->CommitEntity(t_Light);
		}
		{
			auto t_Light = m_World->CreateEntity();
			auto transform = m_World->AddComponent<Components::Transform>(t_Light);
			transform->Position = glm::vec3(3.f, -5.f, -5.f);
			auto pl = m_World->AddComponent<Components::PointLight>(t_Light);
			pl->Radius = 15.f;
			pl->Diffuse = glm::vec3(0.1f, 0.5f, 0.8f);
			auto model = m_World->AddComponent<Components::Model>(t_Light);
			model->ModelFile = "Core/UnitSphere.obj";
			m_World->CommitEntity(t_Light);
		}

		//Halfpipe background test model.
		{
			auto t_halfPipe = m_World->CreateEntity();
			auto transform = m_World->AddComponent<Components::Transform>(t_halfPipe);
			transform->Position = glm::vec3(0.f, 0.f, -15.f);
			transform->Scale = glm::vec3(6.f, 6.f, 10.f);
			auto model = m_World->AddComponent<Components::Model>(t_halfPipe);
			model->ModelFile = "Models/Test/halfpipe/Halfpipe.obj";
			model->Color = glm::vec4(1.f, 1.f, 1.f, 0.3f);
			m_World->CommitEntity(t_halfPipe);
		}

		//Background
		{
			auto background = m_World->CreateEntity();
			auto transform = m_World->AddComponent<Components::Transform>(background);
			transform->Position = glm::vec3(0.f, 0.f, -30.f);
			transform->Scale = glm::vec3(2681.f / 50.f, 1080.f / 50.f, 1.f);
			auto sprite = m_World->AddComponent<Components::Sprite>(background);
			sprite->SpriteFile = "Textures/Background.png";
			m_World->CommitEntity(background);
		}

		//Water test
		{
			auto t_waterBody = m_World->CreateEntity();
			auto transform = m_World->AddComponent<Components::Transform>(t_waterBody);
			transform->Position = glm::vec3(0.f, -4.5f, -10.f);
			transform->Scale = glm::vec3(7.f, 1.5f, 1.f);
			auto water = m_World->AddComponent<Components::WaterVolume>(t_waterBody);
			auto body = m_World->AddComponent<Components::RectangleShape>(t_waterBody);
			m_World->CommitEntity(t_waterBody);
		}
		//TODO: Why does the ball not collide with these bricks?
		//BottomBox
		{
			auto topWall = m_World->CreateEntity();
			auto transform = m_World->AddComponent<Components::Transform>(topWall);
			transform->Position = glm::vec3(0.f, -6.f, -9.9f);
			transform->Scale = glm::vec3(10.f, 0.5f, 1.f);
			std::shared_ptr<Components::Sprite> sprite = m_World->AddComponent<Components::Sprite>(topWall);
			sprite->SpriteFile = "Textures/Core/ErrorTexture.png";
			std::shared_ptr<Components::RectangleShape> boxShape = m_World->AddComponent<Components::RectangleShape>(
					topWall);
			std::shared_ptr<Components::Physics> physics = m_World->AddComponent<Components::Physics>(topWall);
			physics->Static = true;
			m_World->CommitEntity(topWall);
		}
		//SideBox
//		{
//			auto topWall = m_World->CreateEntity();
//			std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(topWall);
//			transform->Position = glm::vec3(3.f, -3.0f, -9.9f);
//			transform->Scale = glm::vec3(0.5f, 3.f, 1.f);
//			std::shared_ptr<Components::Sprite> sprite = m_World->AddComponent<Components::Sprite>(topWall);
//			sprite->SpriteFile = "Textures/Core/ErrorTexture.png";
//			std::shared_ptr<Components::RectangleShape> boxShape = m_World->AddComponent<Components::RectangleShape>(
//					topWall);
//			std::shared_ptr<Components::Physics> physics = m_World->AddComponent<Components::Physics>(topWall);
//			physics->Static = true;
//			m_World->CommitEntity(topWall);
//		}
//		//OtherSideBox
//		{
//			auto topWall = m_World->CreateEntity();
//			std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(topWall);
//			transform->Position = glm::vec3(-4.f, -3.0f, -9.9f);
//			transform->Scale = glm::vec3(0.5f, 3.0f, 1.f);
//			std::shared_ptr<Components::Sprite> sprite = m_World->AddComponent<Components::Sprite>(topWall);
//			sprite->SpriteFile = "Textures/Core/ErrorTexture.png";
//			std::shared_ptr<Components::RectangleShape> boxShape = m_World->AddComponent<Components::RectangleShape>(
//					topWall);
//			std::shared_ptr<Components::Physics> physics = m_World->AddComponent<Components::Physics>(topWall);
//			physics->Static = true;
//			m_World->CommitEntity(topWall);
//		}

		{
			auto topWall = m_World->CreateEntity();
			std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(
					topWall);
			transform->Position = glm::vec3(0.f, 6.f, -10.f);
			transform->Scale = glm::vec3(20.f, 0.5f, 1.f);

			std::shared_ptr<Components::Sprite> sprite = m_World->AddComponent<Components::Sprite>(topWall);
			sprite->SpriteFile = "Textures/Core/ErrorTexture.png";

			std::shared_ptr<Components::RectangleShape> boxShape = m_World->AddComponent<Components::RectangleShape>(
					topWall);

			std::shared_ptr<Components::Physics> physics = m_World->AddComponent<Components::Physics>(topWall);
			physics->Static = true;
			physics->Category = CollisionLayer::Type::Wall;
			physics->Mask = CollisionLayer::Type::Ball | CollisionLayer::Type::Brick;

			m_World->CommitEntity(topWall);
		}

		{
			auto leftWall = m_World->CreateEntity();
			std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(
					leftWall);
			transform->Position = glm::vec3(-4.f, 1.f, -10.f);
			transform->Scale = glm::vec3(0.5f, 20.f, 1.f);

			std::shared_ptr<Components::Sprite> sprite = m_World->AddComponent<Components::Sprite>(leftWall);
			sprite->SpriteFile = "Textures/Core/ErrorTexture.png";

			std::shared_ptr<Components::RectangleShape> boxShape = m_World->AddComponent<Components::RectangleShape>(
					leftWall);

			std::shared_ptr<Components::Physics> physics = m_World->AddComponent<Components::Physics>(leftWall);
			physics->Static = true;
			physics->Category = CollisionLayer::Type::Wall;
			physics->Mask = CollisionLayer::Type::Ball | CollisionLayer::Type::Brick;

			m_World->CommitEntity(leftWall);
		}

		{
			auto rightWall = m_World->CreateEntity();
			std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(
					rightWall);
			transform->Position = glm::vec3(4.f, 1.f, -10.f);
			transform->Scale = glm::vec3(0.5f, 20.f, 1.f);

			std::shared_ptr<Components::Sprite> sprite = m_World->AddComponent<Components::Sprite>(rightWall);
			sprite->SpriteFile = "Textures/Core/ErrorTexture.png";

			std::shared_ptr<Components::RectangleShape> boxShape = m_World->AddComponent<Components::RectangleShape>(
					rightWall);

			std::shared_ptr<Components::Physics> physics = m_World->AddComponent<Components::Physics>(rightWall);
			physics->Static = true;
			physics->Category = CollisionLayer::Type::Wall;
			physics->Mask = CollisionLayer::Type::Ball | CollisionLayer::Type::Brick;

			m_World->CommitEntity(rightWall);
		}

		{
			auto ent = m_World->CreateEntity();
			m_World->SetProperty(ent, "Name", "Pad");
			auto ctransform = m_World->AddComponent<Components::Transform>(ent);
			ctransform->Position = glm::vec3(0.f, -3.5f, -10.f);
			ctransform->Scale = glm::vec3(1.0f, 1.0f, 1.f);
			auto rectangle = m_World->AddComponent<Components::RectangleShape>(ent);
			auto physics = m_World->AddComponent<Components::Physics>(ent);
			physics->Static = false;
			physics->Category = CollisionLayer::Type::Pad;
			physics->Mask = CollisionLayer::Type::Ball | CollisionLayer::Type::PowerUp;
			physics->Calculate = true;
			auto cModel = m_World->AddComponent<Components::Model>(ent);
			cModel->ModelFile = "Models/Submarine2.obj";

			auto pad = m_World->AddComponent<Components::Pad>(ent);
			m_World->CommitEntity(ent);
		}


		//EVENT_SUBSCRIBE_MEMBER(m_EGameStart, &Engine::OnGameStart);
		m_EGameStart = decltype(m_EGameStart)(std::bind(&Engine::OnGameStart, this, std::placeholders::_1));
		m_EventBroker->Subscribe(m_EGameStart);

			m_LastTime = glfwGetTime();
	}

	bool Running() const { return !glfwWindowShouldClose(m_Renderer->Window()); }

	void Tick()
	{
		double currentTime = glfwGetTime();
		double dt = currentTime - m_LastTime;
		m_LastTime = currentTime;

		// Update input
		m_InputManager->Update(dt);
		// Swap event queues to get fresh input data in the read queue
		//m_EventBroker->Swap();

		ResourceManager::Update();
		if (m_GameIsRunning) {
			m_World->Update(dt);
		}
		m_EventBroker->Process<GUI::Frame>();
		m_FrameStack->UpdateLayered(dt);

		m_RendererQueue.Clear();
		m_FrameStack->DrawLayered(m_RendererQueue);
		//TODO Fill up the renderQueue with models (Temp fix)
		if (m_GameIsRunning) {
			TEMPAddToRenderQueue();
		}

		// Render scene
		//TODO send renderqueue to draw.
		m_Renderer->Draw(m_RendererQueue);

		m_EventBroker->Process<Engine>();
		// Swap event queues
		m_EventBroker->Swap();

		glfwPollEvents();
	}

	std::shared_ptr<Systems::TransformSystem> m_TransformSystem;
	std::shared_ptr<Systems::LevelSystem> m_LevelSystem;

	//TODO: Get this out of engine.h
	void TEMPAddToRenderQueue()
	{
		if (!m_TransformSystem)
			m_TransformSystem = m_World->GetSystem<Systems::TransformSystem>();

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
					EnqueueModel(modelAsset, modelMatrix, modelComponent->Transparent, modelComponent->Color, modelComponent->ModelFile);
				}
			}

			auto pointLightComponent = m_World->GetComponent<Components::PointLight>(entity);
			if (pointLightComponent)
			{
				Components::Transform absoluteTransform = m_TransformSystem->AbsoluteTransform(entity);
				EnqueuePointLight(absoluteTransform.Position,
								  pointLightComponent->Diffuse,
								  pointLightComponent->Specular,
								  pointLightComponent->Radius);
			}

			auto parent = m_World->GetEntityParent(entity);
			if(parent != 0) {
				auto waterParticleComponent = m_World->GetComponent<Components::WaterVolume>(parent);
				if (waterParticleComponent) {
					//TODO: Remove hardcoded color.
					//TODO: Do i even need modelMatrix?
					Components::Transform absoluteTransform = m_TransformSystem->AbsoluteTransform(entity);
					glm::mat4 modelMatrix = glm::translate(absoluteTransform.Position)
											* glm::scale(absoluteTransform.Scale);
					EnqueueWaterParticles(absoluteTransform.Position, glm::vec4(1.f, 1.f, 1.f, 1.f), modelMatrix, absoluteTransform.Position.z);
				}
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

	}

	//TODO: Get this out of engine.h
	void EnqueueModel(Model* model, glm::mat4 modelMatrix, float transparent, glm::vec4 color, std::string fileName)
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

	void EnqueueWaterParticles(glm::vec3 position, glm::vec4 color, glm::mat4 modelMatrix, float depth)
	{
		WaterParticleJob job;
		job.Position = position;
		job.Color = color;
		job.ModelMatrix = modelMatrix;
		job.Depth = depth;

		m_RendererQueue.Forward.Add(job);
	}

private:
	std::shared_ptr<EventBroker> m_EventBroker;
	GUI::Frame* m_FrameStack = nullptr;
	std::shared_ptr<Renderer> m_Renderer;
	RenderQueueCollection m_RendererQueue;
	std::shared_ptr<InputManager> m_InputManager;
	std::shared_ptr<World> m_World;

	//TODO: Redo
	bool m_GameIsRunning = false;
	dd::EventRelay<Engine, dd::Events::GameStart> m_EGameStart;
	bool OnGameStart(const dd::Events::GameStart &event)
	{
		m_GameIsRunning = true;
		//Todo: Move this
		{
			dd::Events::PlaySound e;
			e.path = "Sounds/BGM/under-the-sea-instrumental.wav";
			e.isAmbient = true;
			m_EventBroker->Publish(e);
		}
		{
			dd::Events::PlaySound e;
			e.path = "Sounds/BGM/water-flowing.wav";
			e.volume = 0.3f;
			e.isAmbient = true;
			m_EventBroker->Publish(e);
		}
	};
	double m_LastTime;
};

}
