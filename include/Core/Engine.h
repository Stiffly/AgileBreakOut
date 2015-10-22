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
#include "Rendering/OBJ.h"
#include "Rendering/Model.h"
#include "Rendering/Texture.h"
#include "EventBroker.h"
#include "Rendering/RenderQueue.h"
#include "Rendering/Renderer.h"
#include "InputManager.h"
//TODO: Remove includes that are only here for the temporary draw solution.
#include "World.h"
#include "CTransform.h"
#include "CTemplate.h"
#include "Core/ConfigFile.h"
#include "Core/EventBroker.h"
#include "Rendering/CModel.h"
#include "Rendering/CSprite.h"
#include "Rendering/CPointLight.h"
#include "Transform/TransformSystem.h"
#include "Sound/SoundSystem.h"
#include "Sound/CCollisionSound.h"

#include "Game/LevelSystem.h"
#include "Game/PadSystem.h"
#include "Game/CBall.h"
#include "Game/CPad.h"
#include "Game/CLifebuoy.h"
#include "Game/CProjectile.h"
#include "Game/CBrick.h"
#include "Game/CBrickPart.h"
#include "Game/CWall.h"
#include "Game/CKraken.h"
#include "Game/CBackground.h"
#include "Game/CTravels.h"
#include "Game/CStickyAim.h"
#include "Game/BallSystem.h"
#include "Game/LifeSystem.h"
#include "Game/HitLagSystem.h"
#include "Game/TravellingSystem.h"
#include "Game/LifebuoySystem.h"
#include "Game/ProjectileSystem.h"
#include "Game/KrakenSystem.h"
#include "Game/WaterSystem.h"
#include "Game/Bricks/CPowerUpBrick.h"

#include "Game/BrickComponents.h"

#include "Game/EScreenShake.h"

#include "Physics/PhysicsSystem.h"
#include "Physics/CPhysics.h"
#include "Physics/CRectangleShape.h"
#include "Physics/ESetImpulse.h"
#include "Physics/CWaterVolume.h"
#include "Physics/CParticle.h"
#include "Physics/CParticleEmitter.h"

#include "Rendering/AnimationSystem.h"

#include "Game/EGameStart.h"
#include "Sound/EPlaySound.h"

#include "GUI/Frame.h"
#include "GUI/Button.h"
#include "Game/GUI/MainMenu.h"
#include "Game/GUI/HUD.h"

namespace dd
{

class Engine
{

public:
	Engine(int argc, char* argv[]) {
		auto config = ResourceManager::Load<ConfigFile>("Config.ini");

		LOG_LEVEL = static_cast<_LOG_LEVEL>(config->GetValue<int>("Debug.LogLevel", 1));

		m_EventBroker = std::make_shared<EventBroker>();

		m_Renderer = std::make_shared<Renderer>();
		m_Renderer->SetFullscreen(config->GetValue<bool>("Video.Fullscreen", false));
		m_Renderer->SetResolution(Rectangle(
			0, 
			0, 
			config->GetValue<int>("Video.Width", 675), 
			config->GetValue<int>("Video.Height", 1080)
		));
		m_Renderer->Initialize();

		m_FrameStack = new GUI::Frame(m_EventBroker.get());
		m_FrameStack->Width = 675;
		m_FrameStack->Height = 1080;

		if (config->GetValue<bool>("Debug.SkipStory", false)) {
			Events::GameStart e;
			m_EventBroker->Publish(e);
			auto hud = new GUI::HUD(m_FrameStack, "HUD");
		}
		else {
			auto menu = new GUI::MainMenu(m_FrameStack, "MainMenu");
		}

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
		m_World->ComponentFactory.Register<Components::BrickPart>();
		m_World->ComponentFactory.Register<Components::Pad>();
		m_World->ComponentFactory.Register<Components::Wall>();
		m_World->ComponentFactory.Register<Components::Background>();
		m_World->ComponentFactory.Register<Components::Life>();
		m_World->ComponentFactory.Register<Components::Lifebuoy>();
		m_World->ComponentFactory.Register<Components::Projectile>();
		m_World->ComponentFactory.Register<Components::Kraken>();
		m_World->ComponentFactory.Register<Components::StickyAim>();
		m_World->ComponentFactory.Register<Components::Travels>();

		m_World->ComponentFactory.Register<Components::PowerUp>();
		m_World->ComponentFactory.Register<Components::PowerUpBrick>();

		m_World->ComponentFactory.Register<Components::StandardBrick>();
		m_World->ComponentFactory.Register<Components::MultiBallBrick>();
		m_World->ComponentFactory.Register<Components::LifebuoyBrick>();
		m_World->ComponentFactory.Register<Components::StickyBrick>();
		m_World->ComponentFactory.Register<Components::InkBlasterBrick>();
		m_World->ComponentFactory.Register<Components::KrakenAttackBrick>();

		m_World->SystemFactory.Register<Systems::LevelSystem>(
			[this]() { return new Systems::LevelSystem(m_World.get(), m_EventBroker); });
		m_World->AddSystem<Systems::LevelSystem>();
		m_World->SystemFactory.Register<Systems::PadSystem>(
				[this]() { return new Systems::PadSystem(m_World.get(), m_EventBroker); });
		m_World->AddSystem<Systems::PadSystem>();
		m_World->SystemFactory.Register<Systems::BallSystem>(
				[this]() { return new Systems::BallSystem(m_World.get(), m_EventBroker); });
		m_World->AddSystem<Systems::BallSystem>();
		m_World->SystemFactory.Register<Systems::HitLagSystem>(
				[this]() { return new Systems::HitLagSystem(m_World.get(), m_EventBroker); });
		m_World->AddSystem<Systems::HitLagSystem>();
		m_World->SystemFactory.Register<Systems::LifebuoySystem>(
			[this]() { return new Systems::LifebuoySystem(m_World.get(), m_EventBroker); });
		m_World->AddSystem<Systems::LifebuoySystem>();
		m_World->SystemFactory.Register<Systems::ProjectileSystem>(
			[this]() { return new Systems::ProjectileSystem(m_World.get(), m_EventBroker); });
		m_World->AddSystem<Systems::ProjectileSystem>();
		m_World->SystemFactory.Register<Systems::PhysicsSystem>(
				[this]() { return new Systems::PhysicsSystem(m_World.get(), m_EventBroker); });
		m_World->AddSystem<Systems::PhysicsSystem>();
		m_World->SystemFactory.Register<Systems::AnimationSystem>(
				[this]() { return new Systems::AnimationSystem(m_World.get(), m_EventBroker); });
		m_World->AddSystem<Systems::AnimationSystem>();
		m_World->SystemFactory.Register<Systems::TravellingSystem>(
			[this]() { return new Systems::TravellingSystem(m_World.get(), m_EventBroker); });
		m_World->AddSystem<Systems::TravellingSystem>();
		m_World->SystemFactory.Register<Systems::KrakenSystem>(
			[this]() { return new Systems::KrakenSystem(m_World.get(), m_EventBroker); });
		m_World->AddSystem<Systems::KrakenSystem>();
		m_World->SystemFactory.Register<Systems::WaterSystem>(
			[this]() { return new Systems::WaterSystem(m_World.get(), m_EventBroker); });
		m_World->AddSystem<Systems::WaterSystem>();
		m_World->SystemFactory.Register<Systems::LifeSystem>(
			[this]() { return new Systems::LifeSystem(m_World.get(), m_EventBroker); });
		m_World->AddSystem<Systems::LifeSystem>();

		m_World->ComponentFactory.Register<Components::Model>();
		m_World->ComponentFactory.Register<Components::Template>();
		m_World->ComponentFactory.Register<Components::PointLight>();
		m_World->ComponentFactory.Register<Components::WaterVolume>();
		m_World->ComponentFactory.Register<Components::Particle>();
		m_World->ComponentFactory.Register<Components::ParticleEmitter>();
		m_World->Initialize();

		//auto particleEmitter= m_World->AddComponent<Components::Emitter>(Pe);


		
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

		//ResourceManager::Update();
		if (m_GameIsRunning) {
			m_World->Update(dt);
		} else {
			m_EventBroker->Process<Systems::SoundSystem>();
		}
		m_EventBroker->Process<GUI::Frame>();
		m_FrameStack->UpdateLayered(dt);

		m_RendererQueue.Clear();
		m_FrameStack->DrawLayered(m_RendererQueue);
		//TODO Fill up the renderQueue with models (Temp fix)
		if (m_GameIsRunning) {
			TEMPAddToRenderQueue();
		}

		std::unique_ptr<dd::Camera> defaultCamera = nullptr;

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
					Components::Animation* animationComponent = m_World->GetComponent<Components::Animation>(entity);
					EnqueueModel(modelAsset, modelMatrix, modelComponent, animationComponent);
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
	void EnqueueModel(Model* model, glm::mat4 modelMatrix, const Components::Model* modelComponent, const Components::Animation* animationComponent)
	{
		for (auto texGroup : model->TextureGroups)
		{
			ModelJob job;
			job.TextureID = (texGroup.Texture) ? texGroup.Texture->ResourceID : 0;
			job.DiffuseTexture = texGroup.Texture.get();
			job.NormalTexture = texGroup.NormalMap.get();
			job.SpecularTexture = texGroup.SpecularMap.get();
			job.Model = model;
			job.StartIndex = texGroup.StartIndex;
			job.EndIndex = texGroup.EndIndex;
			job.ModelMatrix = modelMatrix * model->m_Matrix;
			job.Color = modelComponent->Color;
			
			if (model->m_Skeleton != nullptr && animationComponent != nullptr) {
				job.Skeleton = model->m_Skeleton;
				job.AnimationName = animationComponent->Name;
				job.AnimationTime = animationComponent->Time;
				job.NoRootMotion = animationComponent->NoRootMotion;
			}

			m_RendererQueue.Deferred.Add(job);
		}
	}

	// TODO: Get this out of engine.h
	void EnqueueSprite(Texture* texture, Texture* normalTexture, Texture* specularTexture, glm::mat4 modelMatrix, glm::vec4 color, float depth)
	{
		SpriteJob job;
		job.TextureID = texture->ResourceID;
		job.DiffuseTexture = texture;
		job.NormalTexture = normalTexture;
		job.SpecularTexture = specularTexture;
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
	glm::vec3 m_CameraPosition = glm::vec3(0.f, 0.f, 0.f);

	//TODO: Redo
	bool m_GameIsRunning = false;
	dd::EventRelay<Engine, dd::Events::GameStart> m_EGameStart;
	bool OnGameStart(const dd::Events::GameStart &event)
	{
		m_GameIsRunning = true;
		//Todo: Move this
		{
// 			dd::Events::PlaySound e;
// 			e.FilePath = "Sounds/BGM/under-the-sea-instrumental.wav";
// 			e.IsAmbient = true;
// 			m_EventBroker->Publish(e);
		}
		{
// 			dd::Events::PlaySound e;
// 			e.FilePath = "Sounds/BGM/water-flowing.wav";
// 			e.Gain = 0.3f;
// 			e.IsAmbient = true;
// 			m_EventBroker->Publish(e);
		}

		return true;
	};
	double m_LastTime;
};

}
