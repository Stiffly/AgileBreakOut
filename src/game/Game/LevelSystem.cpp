//
// Created by Adniklastrator on 2015-09-09.
//


#include "PrecompiledHeader.h"
#include "Game/LevelSystem.h"


void dd::Systems::LevelSystem::Initialize()
{
	std::random_device rd;
	m_RandomGenerator = std::mt19937(rd());
	EVENT_SUBSCRIBE_MEMBER(m_EContact, &LevelSystem::OnContact);
	EVENT_SUBSCRIBE_MEMBER(m_EScoreEvent, &LevelSystem::OnScoreEvent);
	EVENT_SUBSCRIBE_MEMBER(m_EMultiBall, &LevelSystem::OnMultiBall);
	EVENT_SUBSCRIBE_MEMBER(m_ECreatePowerUp, &LevelSystem::OnCreatePowerUp);
	EVENT_SUBSCRIBE_MEMBER(m_EMultiBallLost, &LevelSystem::OnMultiBallLost);
	EVENT_SUBSCRIBE_MEMBER(m_EPowerUpTaken, &LevelSystem::OnPowerUpTaken);
	EVENT_SUBSCRIBE_MEMBER(m_EStageCleared, &LevelSystem::OnStageCleared);
	EVENT_SUBSCRIBE_MEMBER(m_EArrivedAtNewStage, &LevelSystem::OnArrivedAtNewStage);
	EVENT_SUBSCRIBE_MEMBER(m_EPause, &LevelSystem::OnPause);
	EVENT_SUBSCRIBE_MEMBER(m_EResume, &LevelSystem::OnResume);
	EVENT_SUBSCRIBE_MEMBER(m_EHitPad, &LevelSystem::OnHitPad);
	EVENT_SUBSCRIBE_MEMBER(m_EBrickGenerating, &LevelSystem::OnBrickGenerating);
	EVENT_SUBSCRIBE_MEMBER(m_EKrakenDefeated, &LevelSystem::OnKrakenDefeated);

	m_GodMode = ResourceManager::Load<ConfigFile>("Config.ini")->GetValue<bool>("Cheat.GodMode", false);

	//PointLightTest
	{
		auto t_Light = m_World->CreateEntity();
		auto transform = m_World->AddComponent<Components::Transform>(t_Light);
		transform->Position = glm::vec3(-3.f, 6.f, -5.f);
		auto pl = m_World->AddComponent<Components::PointLight>(t_Light);
		pl->Radius = 20.f;
		pl->Diffuse = glm::vec3(0.8f, 0.7f, 0.05f);
		auto model = m_World->AddComponent<Components::Model>(t_Light);
		model->ModelFile = "Models/Core/UnitSphere.obj";
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
		model->ModelFile = "Models/Core/UnitSphere.obj";
		m_World->CommitEntity(t_Light);
	}

	//Halfpipe background test model.
	{
		auto t_halfPipe = m_World->CreateEntity();
		auto transform = m_World->AddComponent<Components::Transform>(t_halfPipe);
		auto background = m_World->AddComponent<Components::Background>(t_halfPipe);
		transform->Position = glm::vec3(0.f, 0.f, -15.f);
		transform->Scale = glm::vec3(6.f, 6.f, 10.f);
		auto travels = m_World->AddComponent<Components::Travels>(t_halfPipe);
		travels->CurrentlyTraveling = false;
		auto model = m_World->AddComponent<Components::Model>(t_halfPipe);
		model->ModelFile = "Models/Test/halfpipe/Halfpipe.obj";
		model->Color = glm::vec4(0.8f, 0.8f, 0.8f, 1.f);
		m_World->CommitEntity(t_halfPipe);

		auto t_halfPipe2 = m_World->CloneEntity(t_halfPipe);
		auto transform2 = m_World->GetComponent<Components::Transform>(t_halfPipe);
		transform2->Position = glm::vec3(0.f, 34.6f, -15.f); //Halfpipe Value
	}

	//Background

	/*{
		auto background = m_World->CreateEntity();
		auto transform = m_World->AddComponent<Components::Transform>(background);
		auto travels = m_World->AddComponent<Components::Travels>(background);
		travels->CurrentlyTraveling = false;
		auto backgroundComponent = m_World->AddComponent<Components::Background>(background);
		transform->Position = glm::vec3(0.f, 0.f, -30.f);
		transform->Scale = glm::vec3(2681.f / 50.f, 1080.f / 50.f, 1.f);
		auto sprite = m_World->AddComponent<Components::Sprite>(background);
		sprite->SpriteFile = "Textures/Background.png";
		m_World->CommitEntity(background);
	}*/

	//			auto particleEmitter= m_World->AddComponent<Components::Emitter>(Pe);
	

	//ParticleTest


	/*{
	auto Pe = m_World->CreateEntity();
	auto transform = m_World->AddComponent<Components::Transform>(Pe);
	auto particleEmitter= m_World->AddComponent<Components::ParticleEmitter>(Pe);
	auto emitteSprite = m_World->AddComponent<Components::Sprite>(Pe);

	transform->Position = glm::vec3(0.f, 0.f, -10.f);
	emitteSprite->SpriteFile = "Textures/Test/Brick_Normal.png";
	particleEmitter->GravityScale = 0.0f;
	particleEmitter->SpawnRate = 1.f;
	particleEmitter->NumberOfTicks = 2;
	particleEmitter->Speed = 1.f;
	particleEmitter->ParticlesPerTick = 5;
	particleEmitter->Spread = glm::pi<float>()*2;
	particleEmitter->EmittingAngle = glm::pi<float>();
	particleEmitter->LifeTime = 50;
	
	{
	auto Pt = m_World->CreateEntity(Pe);
	auto PtTransform = m_World->AddComponent<Components::Transform>(Pt);
	auto PtSprite = m_World->AddComponent<Components::Sprite>(Pt);
	auto PtParticle = m_World->AddComponent<Components::Particle>(Pt);
	auto PtTemplate = m_World->AddComponent<Components::Template>(Pt);

	//PtTransform->Velocity = glm::vec3(1.0f, 0.f, 0.f);
	PtTransform->Position = transform->Position;
	PtSprite->SpriteFile = "Textures/Background.png";
	PtParticle->LifeTime = 3.f;
	PtParticle->Flags = static_cast<ParticleFlags::Type>(ParticleFlags::Type::Powder | ParticleFlags::Type::ParticleContactFilter | ParticleFlags::Type::FixtureContactFilter);
	}
	m_World->CommitEntity(Pe);
	}*/

	//TODO: Why does the ball not collide with these bricks?
	//BottomBox
	{
		EntityID BottomWall = m_World->CreateEntity();
		auto transform = m_World->AddComponent<Components::Transform>(BottomWall);
		auto wall = m_World->AddComponent<Components::Wall>(BottomWall);
		transform->Position = glm::vec3(0.f, -6.f, -10.f);
		transform->Scale = glm::vec3(20.f, 0.5f, 1.f);
		std::shared_ptr<Components::Sprite> sprite = m_World->AddComponent<Components::Sprite>(BottomWall);
		sprite->SpriteFile = "Textures/Core/ErrorTexture.png";
		std::shared_ptr<Components::RectangleShape> rectangleShape = m_World->AddComponent<Components::RectangleShape>(BottomWall);
		rectangleShape->Dimensions = glm::vec2(20.f, 0.5f);
		std::shared_ptr<Components::Physics> physics = m_World->AddComponent<Components::Physics>(BottomWall);
		physics->CollisionType = CollisionType::Type::Static;
		physics->Category = CollisionLayer::Wall;
		if (m_GodMode) {
			physics->Mask = static_cast<CollisionLayer::Type>(CollisionLayer::LifeBuoy | CollisionLayer::Ball);
		} else {
			physics->Mask = static_cast<CollisionLayer::Type>(CollisionLayer::LifeBuoy);
		}
		m_World->CommitEntity(BottomWall);

		m_World->SetProperty(BottomWall, "Name", "BottomWall");
	}

	{
		auto topWall = m_World->CreateEntity();
		std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(topWall);
		transform->Position = glm::vec3(0.f, 6.f, -10.f);
		transform->Scale = glm::vec3(20.f, 0.5f, 1.f);

		auto wall = m_World->AddComponent<Components::Wall>(topWall);

		//std::shared_ptr<Components::Sprite> sprite = m_World->AddComponent<Components::Sprite>(topWall);
		//sprite->SpriteFile = "Textures/Core/ErrorTexture.png";

		std::shared_ptr<Components::RectangleShape> rectangleShape = m_World->AddComponent<Components::RectangleShape>(topWall);
		rectangleShape->Dimensions = glm::vec2(20.f, 0.5f);

		std::shared_ptr<Components::Physics> physics = m_World->AddComponent<Components::Physics>(topWall);
		physics->CollisionType = CollisionType::Type::Static;
		physics->Category = CollisionLayer::Wall;
		physics->Mask = static_cast<CollisionLayer::Type>(CollisionLayer::Ball | CollisionLayer::Brick);

		m_World->CommitEntity(topWall);
	}

	{
		auto leftWall = m_World->CreateEntity();
		std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(
			leftWall);
		transform->Position = glm::vec3(-4.f, 1.f, -10.f);
		transform->Scale = glm::vec3(0.5f, 20.f, 1.f);

		auto wall = m_World->AddComponent<Components::Wall>(leftWall);

		//std::shared_ptr<Components::Sprite> sprite = m_World->AddComponent<Components::Sprite>(leftWall);
		//sprite->SpriteFile = "Textures/Core/ErrorTexture.png";

		std::shared_ptr<Components::RectangleShape> rectangleShape = m_World->AddComponent<Components::RectangleShape>(leftWall);
		rectangleShape->Dimensions = glm::vec2(0.5f, 20.f);

		std::shared_ptr<Components::Physics> physics = m_World->AddComponent<Components::Physics>(leftWall);
		physics->CollisionType = CollisionType::Type::Static;
		physics->Category = CollisionLayer::Wall;
		physics->Mask = static_cast<CollisionLayer::Type>(CollisionLayer::Ball | CollisionLayer::Brick);

		m_World->CommitEntity(leftWall);
	}

	{
		auto rightWall = m_World->CreateEntity();
		std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(
			rightWall);
		transform->Position = glm::vec3(4.f, 1.f, -10.f);
		transform->Scale = glm::vec3(0.5f, 20.f, 1.f);

		auto wall = m_World->AddComponent<Components::Wall>(rightWall);

		//std::shared_ptr<Components::Sprite> sprite = m_World->AddComponent<Components::Sprite>(rightWall);
		//sprite->SpriteFile = "Textures/Core/ErrorTexture.png";

		std::shared_ptr<Components::RectangleShape> rectangleShape = m_World->AddComponent<Components::RectangleShape>(rightWall);
		rectangleShape->Dimensions = glm::vec2(0.5f, 20.f);

		std::shared_ptr<Components::Physics> physics = m_World->AddComponent<Components::Physics>(rightWall);
		physics->CollisionType = CollisionType::Type::Static;
		physics->Category = CollisionLayer::Wall;
		physics->Mask = static_cast<CollisionLayer::Type>(CollisionLayer::Ball | CollisionLayer::Brick);

		m_World->CommitEntity(rightWall);
	}

    m_BrickTemplate = m_World->CreateEntity();
    std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(m_BrickTemplate);
    auto model = m_World->AddComponent<Components::Model>(m_BrickTemplate);
    std::shared_ptr<Components::Brick> cBrick = m_World->AddComponent<Components::Brick>(m_BrickTemplate);
    std::shared_ptr<Components::RectangleShape> cRec = m_World->AddComponent<Components::RectangleShape>(m_BrickTemplate);
    cRec->Dimensions = glm::vec2(0.9f, 0.35f);
    std::shared_ptr<Components::Physics> cPhys = m_World->AddComponent<Components::Physics>(m_BrickTemplate);
    std::shared_ptr<Components::Template> cTemplate = m_World->AddComponent<Components::Template>(m_BrickTemplate);

	auto travels = m_World->AddComponent<Components::Travels>(m_BrickTemplate);
	travels->CurrentlyTraveling = false;

    cPhys->CollisionType = CollisionType::Type::Static;
    cPhys->GravityScale = 0.f;
    cPhys->Category = CollisionLayer::Type::Brick;
    cPhys->Mask = static_cast<CollisionLayer::Type>(CollisionLayer::Type::Ball | CollisionLayer::Type::Projectile | CollisionLayer::Type::Wall | CollisionLayer::LifeBuoy);

    model->ModelFile = "Models/Brick/WhiteBrick.obj";
	transform->Position = glm::vec3(50, 50, -10);
	//sound
	auto collisionSound = m_World->AddComponent<Components::CollisionSound>(m_BrickTemplate);
	collisionSound->FilePath = "Sounds/Brick/shortbrickbreak.wav";
	m_World->CommitEntity(m_BrickTemplate);

    return;
}

void dd::Systems::LevelSystem::Update(double dt)
{
    if (!m_Initialized) {
        GetNextLevel();
        CreateLevel(0);
		for (int i = 0; i < m_KrakenBricks.size(); i++) {
			m_KrakenBricks[i] = false;
		}
        m_Initialized = true;
    }
}

void dd::Systems::LevelSystem::UpdateEntity(double dt, EntityID entity, EntityID parent)
{
    if (IsPaused()) {
        return;
    }
    auto templateCheck = m_World->GetComponent<Components::Template>(entity);
    if (templateCheck != nullptr){ return; }

    // Check the background.
    auto background = m_World->GetComponent<Components::Background>(entity);
	if (background != nullptr) {
		auto transform = m_World->GetComponent<Components::Transform>(entity);
		if (transform->Position.y <= -34.6) { //Halfpipe Value.
			transform->Position.y = 34.6;
			Events::Move e;
			e.Entity = entity;
			e.GoalPosition = glm::vec3(transform->Position.x, 34.6 + background->distanceLeftToCorrectTravelPosition, transform->Position.z);
			e.Speed = 6.0f;
			e.Queue = false;
			EventBroker->Publish(e);
		}
	}

    auto brick = m_World->GetComponent<Components::Brick>(entity);
    if (brick != nullptr) {
        auto transform = m_World->GetComponent<Components::Transform>(entity);
        //Removes bricks that falls out of the stage.
		if (transform->Position.y < -10) {
			if (brick->Type == StandardBrick) {
			} else if (brick->Type == MultiBallBrick) {
                Events::MultiBall e;
                e.padTransform = transform;
                EventBroker->Publish(e);
			} else if (brick->Type == LifebuoyBrick) {
				Events::Lifebuoy e;
				e.Transform = transform;
				EventBroker->Publish(e);
			} else if (brick->Type == StickyBrick) {
				Events::StickyPad e;
				e.Times = 3;
				EventBroker->Publish(e);
			} else if (brick->Type == InkBlasterBrick) {
				Events::InkBlaster e;
				e.Shots = 5;
				e.Speed = 7;
				EventBroker->Publish(e);
			} else if(brick->Type == KrakenAttackBrick) {
				Events::KrakenAttack e;
				e.ChargeUpdate = 0;
				e.KrakenStrength = 0.1;
				e.PlayerStrength = 0.05;
				EventBroker->Publish(e);
			}
			m_LooseBricks--;
			m_World->RemoveEntity(entity);
			return;
        }
    }

    auto ball = m_World->GetComponent<Components::Ball>(entity);

    if (NumberOfBricks() <= 0 && m_LooseBricks <= 0 && !Restarting()) {
        if (MultiBalls() <= 0 && PowerUps() <= 0) {
            Events::StageCleared ec;
            ec.ClearedStage = m_CurrentLevel;
            ec.StageCluster = m_CurrentCluster;
			ec.StagesInCluster = m_StagesInCluster;
            EventBroker->Publish(ec);
        } else {
            if (ball != nullptr && MultiBalls() > 0) {
                Events::MultiBallLost e;
                EventBroker->Publish(e);
                m_World->RemoveEntity(entity);
            } else {
                auto powerUp = m_World->GetComponent<Components::PowerUp>(entity);
                if (powerUp != nullptr) {
                    SetPowerUps(PowerUps() - 1);
                    m_World->RemoveEntity(entity);
                } else {
                    if (brick != nullptr) {
                        m_LooseBricks--;
                        m_World->RemoveEntity(entity);
                    }
                }
            }
        }
    }
}

bool dd::Systems::LevelSystem::OnPause(const dd::Events::Pause &event)
{
    /*if (event.Type != "LevelSystem" && event.Type != "All") {
        return false;
    }*/

    m_Pause = true;

    return true;
}

bool dd::Systems::LevelSystem::OnResume(const dd::Events::Resume &event)
{
	/*if (event.Type != "LevelSystem" && event.Type != "All") {
	return false;
	}*/
	m_Pause = false;
	return true;
}

void dd::Systems::LevelSystem::CreateBasicLevel(int rows, int lines, glm::vec2 spacesBetweenBricks, float spaceToEdge)
{
    SetNumberOfBricks(rows * lines);
    //std::cout << "You're only doing this once, right?" << std::endl;
    int num = Lines();
    for (int i = 0; i < rows; i++) {
        num--;
        for (int j = 0; j < Lines(); j++) {
            CreateBrick(i, j, spacesBetweenBricks, spaceToEdge, 0, num, 1, glm::vec4(0, 0, 0, 0));
        }
        if (num == 1)
            num = Lines();
    }

    SetNumberOfBricks(rows * lines);
    SetRestarting(false);
    return;
}

void dd::Systems::LevelSystem::CreateLevel(int aboveBasicLevel)
{
    std::cout << "Loading level: " << m_CurrentLevel << std::endl;
    SetNumberOfBricks(Rows() * Lines());
    int num = Lines();
    int getter = 0;

    for (int i = 0; i < Rows(); i++) {
        num--;
        for (int j = 0; j < Lines(); j++) {
            CreateBrick(i, j, SpaceBetweenBricks(), SpaceToEdge(), aboveBasicLevel, num, m_Bricks[getter], m_Colors[getter]);
            getter++;
        }
        if (num == 1) {
            num = Lines();
        }
    }
    m_LooseBricks = NumberOfBricks();
    SetRestarting(false);
}

EntityID dd::Systems::LevelSystem::CreateBrick(int row, int line, glm::vec2 spacesBetweenBricks, float spaceToEdge, int aboveLevel, int num, int typeInt, glm::vec4 colorVec)
{
    if (typeInt == EmptyBrickSpace) {
        SetNumberOfBricks(NumberOfBricks() - 1);
        return NULL;
    }
    auto brick = m_World->CloneEntity(m_BrickTemplate);
    m_World->RemoveComponent<Components::Template>(brick);
    auto transform = m_World->GetComponent<Components::Transform>(brick);
    auto cBrick = m_World->GetComponent<Components::Brick>(brick);
	auto model = m_World->GetComponent<Components::Model>(brick);

	float x = line * spacesBetweenBricks.x;
	float y = row * spacesBetweenBricks.y;
	transform->Position = glm::vec3(x - 3, 5 - spaceToEdge - y + aboveLevel, -10.f);

    if (typeInt == StandardBrick) {
		cBrick->Type = StandardBrick;
		auto type = m_World->AddComponent<Components::StandardBrick>(brick);
		model->Color = colorVec;
		SetBrokenModel(brick);
    } else if (typeInt == MultiBallBrick) {
        cBrick->Type = MultiBallBrick;
		auto type = m_World->AddComponent<Components::MultiBallBrick>(brick);
        model->ModelFile = "Models/Brick/IceBrick.obj";
	} else if (typeInt == LifebuoyBrick) {
		cBrick->Type = LifebuoyBrick;
		auto type = m_World->AddComponent<Components::LifebuoyBrick>(brick);
		model->ModelFile = "Models/Brick/LifeBuoyBrick.obj";
	} else if (typeInt == StickyBrick) {
		cBrick->Type = StickyBrick;
		auto type = m_World->AddComponent<Components::StickyBrick>(brick);
		model->ModelFile = "Models/Brick/StickyBrick.obj";
	} else if (typeInt == InkBlasterBrick) {
		cBrick->Type = InkBlasterBrick; 
		auto type = m_World->AddComponent<Components::InkBlasterBrick>(brick);
		//model->ModelFile = "Models/Brick/InkBrick.obj";
		model->Color = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
	} else if (typeInt == KrakenAttackBrick) {
		cBrick->Type = KrakenAttackBrick;
		auto type = m_World->AddComponent<Components::KrakenAttackBrick>(brick);
		model->Color = glm::vec4(0.f, 0.5f, 1.0f, 1.0f);
	} else if (typeInt == Kraken) {
		Events::KrakenAppear e;
		e.Position = transform->Position;
		e.Health = 10;
		EventBroker->Publish(e);
		m_World->RemoveEntity(brick);
		return brick;
	}

    cBrick->Score = 10 * num;
    return brick;
}

bool dd::Systems::LevelSystem::OnBrickGenerating(const dd::Events::BrickGenerating &event)
{
	m_BrickGenerating = true;
	m_BrickGeneratingEvent = event;
	return true;
}

bool dd::Systems::LevelSystem::BrickGenerating(const dd::Events::BrickGenerating &event)
{
	GetBrickSet(event.Set);

	int rows = 6;
	int lines = 7;
	int getter = 0;
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < lines; j++) {
			if (!m_KrakenBricks[getter]) {
				auto ent = CreateBrick(i, j, SpaceBetweenBricks(), SpaceToEdge(), -3.2, 1, m_BrickSet[getter], m_ColorSet[getter]);
				if (ent != NULL) {
					auto brick = m_World->GetComponent<Components::Brick>(ent);
					brick->Number = getter;
					m_KrakenBricks[getter] = true;
					auto transform = m_World->GetComponent<Components::Transform>(ent);
					Events::Move e;
					e.GoalPosition = transform->Position;
					if (j < 4) {
						transform->Position = event.Origin1;
					} else {
						transform->Position = event.Origin2;
					}
					e.Entity = ent;
					e.Speed = 6;
					e.Queue = false;
					EventBroker->Publish(e);
					m_LooseBricks++;
					SetNumberOfBricks(NumberOfBricks() + 1);
				}
			}
			getter++;
		}
	}
	return true;
}

void dd::Systems::LevelSystem::GetBrickSet(int set) // These are sets the Kraken gets!
{
	std::array<int, 42> level;
	std::array<glm::vec4, 42> color;
	// Level array indicates type of brick.
	// 0 is empty space.
	// 1 is standard brick.
	// 2 is multiball brick.
	// 3 is lifebuoy brick.
	// 4 is sticky brick.
	// 5 is ink blaster brick.
	// 6 is kraken attack brick.

	// color array determines the color of standard bricks.
	// w is white.
	// r is red.
	// g is green.
	// b is blue.
	// y is yellow.
	// c is cyan.
	// m is magenta.
	// d is dark.
	// Feel free to make your own.
	glm::vec4 w = glm::vec4(1, 1, 1, 1);
	glm::vec4 r = glm::vec4(1, 0, 0, 1);
	glm::vec4 g = glm::vec4(0, 1, 0, 1);
	glm::vec4 b = glm::vec4(0, 0, 1, 1);
	glm::vec4 y = glm::vec4(1, 1, 0, 1);
	glm::vec4 c = glm::vec4(0, 1, 1, 1);
	glm::vec4 m = glm::vec4(1, 0, 1, 1);
	glm::vec4 d = glm::vec4(0, 0, 0, 1);

	if (set == 1) {
		level =
		{1, 0, 0, 0, 0, 0, 1,
		 1, 0, 0, 0, 0, 0, 1,
		 1, 0, 0, 0, 0, 0, 1,
		 1, 1, 1, 1, 1, 1, 2,
		 1, 1, 1, 1, 1, 1, 1,
		 0, 0, 0, 0, 0, 0, 0};
		color =
		{r, r, r, r, r, r, r,
		 r, r, r, r, r, r, r,
		 r, r, r, r, r, r, r,
		 r, r, r, r, r, r, r,
		 r, r, r, r, r, r, r, 
		 r, r, r, r, r, r, r};
	}
	else if (set == 2) {
		level =
		{0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0,
		 3, 0, 1, 0, 1, 0, 1,
		 0, 1, 0, 1, 0, 1, 0,
		 1, 0, 1, 0, 1, 0, 1,
		 0, 0, 0, 0, 0, 0, 0};
		color =
		{y, y, y, y, y, y, y,
		 y, y, y, y, y, y, y,
		 y, y, y, y, y, y, y,
		 y, y, y, y, y, y, y,
		 y, y, y, y, y, y, y, 
		 y, y, y, y, y, y, y};
	}
	else if (set == 3) {
		level =
		{0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0,
		 1, 0, 1, 0, 1, 0, 1,
		 0, 1, 1, 1, 1, 1, 0,
		 0, 0, 4, 1, 1, 0, 0,
		 0, 0, 0, 1, 0, 0, 0};
		color =
		{b, b, b, b, b, b, b,
		 b, b, b, b, b, b, b,
		 b, b, b, b, b, b, b,
		 b, b, b, b, b, b, b,
		 b, b, b, b, b, b, b, 
		 b, b, b, b, b, b, b};
	}
	else if (set == 4) {
		level =
		{0, 0, 0, 0, 0, 0, 0,
		 1, 1, 1, 1, 1, 3, 1,
		 0, 0, 0, 0, 0, 0, 0,
		 1, 1, 1, 1, 1, 1, 1,
		 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0};
		color =
		{m, m, m, m, m, m, m,
		 m, m, m, m, m, m, m,
		 m, m, m, m, m, m, m,
		 m, m, m, m, m, m, m,
		 m, m, m, m, m, m, m, 
		 m, m, m, m, m, m, m};
	}
	else if (set == 5) {
		level =
		{0, 0, 0, 0, 0, 0, 0,
		 0, 1, 1, 0, 1, 1, 0,
		 0, 1, 2, 0, 1, 1, 0,
		 0, 1, 1, 0, 1, 1, 0,
		 0, 1, 1, 0, 1, 1, 0,
		 0, 0, 0, 0, 0, 0, 0};
		color =
		{g, g, g, g, g, g, g,
		 g, g, g, g, g, g, g,
		 g, g, g, g, g, g, g,
		 g, g, g, g, g, g, g,
		 g, g, g, g, g, g, g, 
		 g, g, g, g, g, g, g};
	}
	else if (set == 6) {
		level =
		{0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0,
		 0, 0, 0, 0, 0, 0, 0,
		 1, 1, 1, 1, 1, 1, 1,
		 1, 1, 1, 1, 1, 1, 1,
		 0, 0, 0, 0, 0, 0, 0};
		color =
		{r, r, r, r, r, r, r, 
		 r, r, r, r, r, r, r,
		 r, r, r, r, r, r, r,
		 r, r, r, r, r, r, r,
		 r, r, r, r, r, r, r, 
		 r, r, r, r, r, r, r};
	}

	std::copy(std::begin(level), std::end(level), std::begin(m_BrickSet));
	std::copy(std::begin(color), std::end(color), std::begin(m_ColorSet));
}


void dd::Systems::LevelSystem::OnEntityRemoved(EntityID entity)
{

}

bool dd::Systems::LevelSystem::OnContact(const dd::Events::Contact &event)
{
	auto templateCheck = m_World->GetComponent<Components::Template>(event.Entity1);
	if (templateCheck != nullptr){ return false; }
	templateCheck = m_World->GetComponent<Components::Template>(event.Entity2);
	if (templateCheck != nullptr){ return false; }

    EntityID entityBrick;
    EntityID entityBall;
	EntityID entityShot = 0;
    auto ball = m_World->GetComponent<Components::Ball>(event.Entity2);
    auto brick = m_World->GetComponent<Components::Brick>(event.Entity1);
    if (brick != nullptr) {
        entityBrick = event.Entity1;
    } else {
        brick = m_World->GetComponent<Components::Brick>(event.Entity2);
        if (brick != nullptr) {
            entityBrick = event.Entity2;
        } else {
            return false;
        }
    }
    if (ball != nullptr) {
        entityBall = event.Entity2;
    } else {
        ball = m_World->GetComponent<Components::Ball>(event.Entity1);
        if (ball != nullptr) {
            entityBall = event.Entity1;
        } else {
			auto shot = m_World->GetComponent<Components::Projectile>(event.Entity1);
			if (shot != nullptr) {
				entityShot = event.Entity1;
			} else {
				shot = m_World->GetComponent<Components::Projectile>(event.Entity2);
				if (shot != nullptr) {
					entityShot = event.Entity2;
				} else {
					return false;
				}
			}
        }
    }

    brick = m_World->GetComponent<Components::Brick>(entityBrick);
    if (brick == nullptr) {
        return false;
    }

	if (entityShot != 0) {
		// For when a brick gets shot.
		brick->Removed = true;
		m_World->RemoveEntity(entityShot);
		BrickHit(entityShot, entityBrick, 1);
		//ep.Radius = 0.05;

		return true;
	}

    if (!Restarting() && !brick->Removed) {
        brick->Removed = true;
        
		// Hit brick with ball code.
		ball->Combo += 1;
		Events::ComboEvent ec;
		ec.Combo = ball->Combo;
		ec.Ball = entityBall;
		EventBroker->Publish(ec);

		BrickHit(entityBall, entityBrick, ball->Combo);
    }

    return true;
}

void dd::Systems::LevelSystem::BrickHit(EntityID entityHitter, EntityID entityBrick, int combo)
{
	auto brick = m_World->GetComponent<Components::Brick>(entityBrick);
	auto physicsComponent = m_World->GetComponent<Components::Physics>(entityBrick);
		//physicsComponent->CollisionType = CollisionType::Type::Dynamic;
        //physicsComponent->GravityScale = 1.f;
        //physicsComponent->Mask = static_cast<CollisionLayer::Type>(CollisionLayer::Water | CollisionLayer::Wall);

	auto transformComponentBrick = m_World->GetComponent<Components::Transform>(entityBrick);
	auto transformComponentHitter = m_World->GetComponent<Components::Transform>(entityHitter);
	auto brickModel = m_World->GetComponent<Components::Model>(entityBrick);

	if (brick->Number < 50) {
		m_KrakenBricks[brick->Number] = false;
	}

	// Check if the brick is any kind of special.
	auto multi = m_World->GetComponent<Components::MultiBallBrick>(entityBrick);
	if (multi != nullptr) {
		Events::MultiBall e;
		e.padTransform = transformComponentBrick;
		EventBroker->Publish(e);
	}
	else {
		auto buoy = m_World->GetComponent<Components::LifebuoyBrick>(entityBrick);
		if (buoy != nullptr) {
			Events::Lifebuoy e;
			e.Transform = transformComponentBrick;
			EventBroker->Publish(e);
		}
		else {
			auto sticky = m_World->GetComponent<Components::StickyBrick>(entityBrick);
			if (sticky != nullptr) {
				Events::StickyPad e;
				e.Transform = transformComponentBrick;
				e.Times = 3;
				EventBroker->Publish(e);
			}
			else {
				auto ink = m_World->GetComponent<Components::InkBlasterBrick>(entityBrick);
				if (ink != nullptr) {
					Events::InkBlaster e;
					e.Transform = transformComponentBrick;
					e.Shots = 5;
					e.Speed = 7;
					EventBroker->Publish(e);
				}
				else {
					auto kraken = m_World->GetComponent<Components::KrakenAttackBrick>(entityBrick);
					if (kraken != nullptr) {
						Events::KrakenAttack e;
						e.ChargeUpdate = 0;
						e.KrakenStrength = 0.1;
						e.PlayerStrength = 0.05;
						EventBroker->Publish(e);
					}
					else {

					}
				}
			}
		}
	}
	
	auto brickChildren = m_World->GetEntityChildren(entityBrick);
	for (auto b : brickChildren)
	{
		auto cTransform = m_World->GetComponent<Components::Transform>(b);
		cTransform->Position = cTransform->Position + transformComponentBrick->Position;
		glm::vec2 ballToBrick = 4.f * glm::normalize(glm::vec2(cTransform->Position.x - transformComponentHitter->Position.x, cTransform->Position.y - transformComponentHitter->Position.y));
		cTransform->Velocity = glm::vec3(ballToBrick.x, ballToBrick.y, 0.f);

		std::uniform_real_distribution<float> dist(-1.5f, 1.5f);
		int impulseStrength = dist(m_RandomGenerator);

		Events::SetImpulse i;
		i.Entity = b;
		i.Impulse = glm::vec2(impulseStrength, 0.f);
		i.Point = glm::vec2(cTransform->Position.x, cTransform->Position.y + 0.5f);
		EventBroker->Publish(i);

		auto cModel = m_World->GetComponent<Components::Model>(b);
		cModel->Color = brickModel->Color;
		cModel->Color *= 0.5f;

		m_World->RemoveComponent<Components::Template>(b);
		m_World->SetEntityParent(b, 0);
		m_World->CommitEntity(b);

		//Particle trail
		Events::CreateParticleSequence trail;
		trail.parent = b;
		trail.AlphaValues.push_back(1.f);
		trail.AlphaValues.push_back(0.f);
		trail.ScaleValues.push_back(glm::vec3(0.08f));
		trail.ScaleValues.push_back(glm::vec3(0.f));
		trail.RadiusDistribution = 1;
		trail.EmitterLifeTime = 2.f;
		trail.ParticleLifeTime = 1.f;
		trail.ParticlesPerTick = 1;
		trail.SpawnRate = 0.1f;
		trail.Speed = 10.f;
		trail.EmittingAngle = glm::half_pi<float>();
		trail.SpriteFile = "Textures/Particles/FadeBall.png";
		trail.Color = brickModel->Color;
		//p.Spread = ...
		EventBroker->Publish(trail);

		//ParticlePoof
		Events::CreateParticleSequence poof;
		poof.EmitterLifeTime = 4;
		poof.EmittingAngle = glm::half_pi<float>();
		poof.Spread = 0.5f;
		poof.NumberOfTicks = 1;
		poof.ParticleLifeTime = 1.5f;
		poof.ParticlesPerTick = 1;
		poof.Position = cTransform->Position;
		poof.ScaleValues.clear();
		poof.ScaleValues.push_back(glm::vec3(0.5f));
		poof.ScaleValues.push_back(glm::vec3(2.f, 2.f, 0.2f));
		poof.SpriteFile = "Textures/Particles/Cloud_Particle.png";
		poof.Color = brickModel->Color;
		poof.AlphaValues.clear();
		poof.AlphaValues.push_back(1.f);
		poof.AlphaValues.push_back(0.f);
		poof.Speed = 10;
		EventBroker->Publish(poof);
	}


	SetNumberOfBricks(NumberOfBricks() - 1);
	Events::ScoreEvent es;
	es.Score = brick->Score * combo;
	EventBroker->Publish(es);

	m_LooseBricks--;
	m_World->RemoveEntity(entityBrick);
}

bool dd::Systems::LevelSystem::OnScoreEvent(const dd::Events::ScoreEvent &event)
{
    SetScore(Score() += event.Score);

    return true;
}

bool dd::Systems::LevelSystem::OnMultiBall(const dd::Events::MultiBall &event)
{
    SetMultiBalls(MultiBalls()+2);
    return true;
}

bool dd::Systems::LevelSystem::OnMultiBallLost(const dd::Events::MultiBallLost &event)
{
    SetMultiBalls(MultiBalls()-1);
    if (MultiBalls() < 0) {
        SetMultiBalls(0);
    }
    return true;
}

bool dd::Systems::LevelSystem::OnCreatePowerUp(const dd::Events::CreatePowerUp &event)
{
    SetPowerUps(PowerUps() + 1);
    auto powerUp = m_World->CreateEntity();
    std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(powerUp);
    auto model = m_World->AddComponent<Components::Model>(powerUp);
    std::shared_ptr<Components::CircleShape> cRec = m_World->AddComponent<Components::CircleShape>(powerUp);
    std::shared_ptr<Components::Physics> cPhys = m_World->AddComponent<Components::Physics>(powerUp);
    cPhys->CollisionType = CollisionType::Type::Dynamic;
    cPhys->Category = CollisionLayer::Type::PowerUp;
    cPhys->Mask = CollisionLayer::Type::Pad;

    std::shared_ptr<Components::PowerUp> cPow = m_World->AddComponent<Components::PowerUp>(powerUp);

    transform->Position = event.Position;
    transform->Scale = glm::vec3(0.2, 0.2, 0.2);
    transform->Velocity = glm::vec3(0, -4, 0);

    model->ModelFile = "Models/Test/Ball/Ballopus.obj";

    m_World->CommitEntity(powerUp);

    return true;
}

bool dd::Systems::LevelSystem::OnPowerUpTaken(const dd::Events::PowerUpTaken &event)
{
    SetPowerUps(PowerUps() - 1);
    return true;
}

bool dd::Systems::LevelSystem::OnKrakenDefeated(const dd::Events::KrakenDefeated &event)
{
	m_BrickGenerating = false;
	SetNumberOfBricks(NumberOfBricks() - 1);
	m_LooseBricks--;

	std::cout << m_LooseBricks << std::endl;

	return true;
}

bool dd::Systems::LevelSystem::OnStageCleared(const dd::Events::StageCleared &event)
{
    if (!m_Cleared) {
        m_Cleared = true;
        SetRestarting(true);
        Events::ResetBall e;
        EventBroker->Publish(e);
        Events::ScoreEvent es;
        es.Score = 500;
        EventBroker->Publish(es);
        m_CurrentLevel++;
        if (m_CurrentLevel <= event.StagesInCluster) {
            GetNextLevel();
            CreateLevel(12); //NextLevelDistance
		} else {
			// Win
			Events::ClusterClear e;
			e.ClusterCleared = m_CurrentCluster;
			EventBroker->Publish(e);

			Events::Pause p;
			p.Type = "All";
			EventBroker->Publish(p);
		}
    }
    return true;
}

bool dd::Systems::LevelSystem::OnArrivedAtNewStage(const dd::Events::ArrivedAtNewStage &event)
{
	m_Cleared = false;
	return true;
}

void dd::Systems::LevelSystem::GetNextLevel()
{
    std::array<int, 42> level;
	std::array<glm::vec4, 42> color;
    // Level array indicates type of brick.
	// 0 is empty space.
    // 1 is standard brick.
    // 2 is multiball brick.
	// 3 is lifebuoy brick.
	// 4 is sticky brick.
	// 5 is ink blaster brick.
	// 6 is kraken attack brick.

	// 100 is KRAKEN!

	// color array determines the color of standard bricks.
	// w is white.
	// r is red.
	// g is green.
	// b is blue.
	// y is yellow.
	// c is cyan.
	// m is magenta.
	// d is dark.
	// Feel free to make your own.
	glm::vec4 w = glm::vec4(1, 1, 1, 1);
	glm::vec4 r = glm::vec4(1, 0, 0, 1);
	glm::vec4 g = glm::vec4(0, 1, 0, 1);
	glm::vec4 b = glm::vec4(0, 0, 1, 1);
	glm::vec4 y = glm::vec4(1, 1, 0, 1);
	glm::vec4 c = glm::vec4(0, 1, 1, 1);
	glm::vec4 m = glm::vec4(1, 0, 1, 1);
	glm::vec4 d = glm::vec4(0, 0, 0, 1);

	//glm::vec4 p4 = glm::vec4(asd.f / 255.f, asd.f / 255.f, sad.f / 255.f, 1.f);
	glm::vec4 br = glm::vec4(143.f / 255.f, 98.f / 255.f, 0.f / 255.f, 1.f);

	glm::vec4 lb1 = glm::vec4(115.f / 255.f, 232.f / 255.f, 236.f / 255.f, 1.f);
	glm::vec4 lb2 = glm::vec4(107.f / 255.f, 214.f / 255.f, 218.f / 255.f, 1.f);
	glm::vec4 lb3 = glm::vec4(91.f / 255.f, 183.f / 255.f, 186.f / 255.f, 1.f);
	glm::vec4 lb4 = glm::vec4(70.f / 255.f, 153.f / 255.f, 156.f / 255.f, 1.f);

	glm::vec4 p1 = glm::vec4(255.f / 255.f, 184.f / 255.f, 254.f / 255.f, 1.f);
	glm::vec4 p2 = glm::vec4(255.f / 255.f, 81.f / 255.f, 253.f / 255.f, 1.f);
	glm::vec4 p3 = glm::vec4(189.f / 255.f, 0.f / 255.f, 187.f / 255.f, 1.f);
	glm::vec4 p4 = glm::vec4(125.f / 255.f, 0.f / 255.f, 147.f / 255.f, 1.f);


    if (m_CurrentCluster == 0) {
        if (m_CurrentLevel == 1) {
            level =
                    {0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 1};
			color = 
					{w, w, w, w, w, w, w,
					 w, w, w, w, w, w, w,
					 w, w, w, w, w, w, w,
					 w, w, w, w, w, w, w,
					 w, w, w, w, w, w, w,
					 w, w, w, w, w, w, w};
        } else if (m_CurrentLevel == 2) {
            level =
                    {0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 100, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0};
			color = 
					{w, w, w, w, w, w, w,
					 w, w, w, w, w, w, w,
					 w, w, w, w, w, w, w,
					 w, w, w, w, w, w, w,
					 w, w, w, w, w, w, w,
					 w, w, w, w, w, w, w};
        } else if (m_CurrentLevel == 3) {
            level =
                    {0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     1, 0, 0, 0, 0, 0, 0};
			color = 
					{w, w, w, w, w, w, w,
					 w, w, w, w, w, w, w,
					 w, w, w, w, w, w, w,
					 w, w, w, w, w, w, w,
					 w, w, w, w, w, w, w,
					 w, w, w, w, w, w, w};
        } else if (m_CurrentLevel == 4) {
            level =
                    {0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 1};
			color = 
					{w, w, w, w, w, w, w,
					 w, w, w, w, w, w, w,
					 w, w, w, w, w, w, w,
					 w, w, w, w, w, w, w,
					 w, w, w, w, w, w, w,
					 w, w, w, w, w, w, w};
        } else if (m_CurrentLevel == 5) {
            level =
                    {0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 1, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0};
			color = 
					{w, w, w, w, w, w, w,
					 w, w, w, w, w, w, w,
					 w, w, w, w, w, w, w,
					 w, w, w, w, w, w, w,
					 w, w, w, w, w, w, w,
					 w, w, w, w, w, w, w};
		} else if (m_CurrentLevel == 6) {
					level =
					{0, 0, 0, 1, 0, 0, 0,
					 0, 0, 0, 0, 0, 0, 0,
					 0, 0, 0, 0, 0, 0, 0,
					 0, 0, 0, 0, 0, 0, 0,
					 0, 0, 0, 0, 0, 0, 0,
					 0, 0, 0, 0, 0, 0, 0};
					color =
					{w, w, w, w, w, w, w,
					 w, w, w, w, w, w, w,
					 w, w, w, w, w, w, w,
					 w, w, w, w, w, w, w,
					 w, w, w, w, w, w, w,
					 w, w, w, w, w, w, w};
		}
    } else if (m_CurrentCluster == 1) {
        if (m_CurrentLevel == 1) {
            level =
                    {0, 0, 0, 0, 0, 0, 0,
                     1, 1, 1, 1, 1, 1, 1,
                     1, 0, 1, 2, 1, 0, 1,
                     0, 1, 0, 1, 0, 1, 0,
                     0, 0, 0, 0, 0, 0, 6,
                     0, 0, 0, 0, 0, 0, 0};
			color = 
					{w, w, w, w, w, w, w,
					 lb4, lb4, lb4, lb4, lb4, lb4, lb4,
					 lb3, lb3, lb3, lb3, lb3, lb3, lb3,
					 lb2, lb2, lb2, lb2, lb2, lb2, lb2,
					 lb1, lb1, lb1, lb1, lb1, lb1, lb1,
					 w, w, w, w, w, w, w};
        } else if (m_CurrentLevel == 2) {
            level =
                    {0, 0, 1, 0, 1, 0, 0,
                     0, 1, 0, 1, 0, 1, 0,
                     0, 0, 1, 1, 1, 0, 0,
                     0, 0, 0, 3, 0, 0, 0,
                     1, 1, 1, 0, 1, 1, 1,
                     1, 0, 0, 0, 0, 0, 1};
			color = 
					{w, w, r, w, r, w, w,
					 w, w, w, r, w, w, w,
					 w, w, r, w, r, w, w,
					 w, w, w, w, w, w, w,
					 w, r, w, w, w, r, w,
					 r, w, w, w, w, w, r};
        } else if (m_CurrentLevel == 3) {
            level =
                    {0, 0, 0, 1, 0, 0, 0,
                     0, 0, 1, 1, 1, 0, 0,
                     0, 1, 1, 1, 1, 1, 0,
                     1, 1, 1, 4, 1, 1, 1,
                     0, 0, 0, 1, 0, 0, 0,
                     0, 0, 1, 1, 1, 0, 0};
			color = 
					{g, g, g, g, g, g, g,
					 g, g, g, g, g, g, g,
					 g, g, g, g, g, g, g,
					 g, g, g, w, g, g, g,
					 w, w, w, br, w, w, w,
					 w, w, br, br, br, w, w};
		} else if (m_CurrentLevel == 4) {
            level =
                    {0, 0, 1, 0, 1, 0, 0,
                     0, 1, 1, 1, 1, 1, 0,
                     1, 1, 1, 1, 1, 1, 1,
                     0, 1, 1, 1, 1, 1, 0,
                     0, 0, 1, 5, 1, 0, 0,
                     0, 0, 0, 1, 0, 0, 0};
			color = 
					{w, w, p1, w, p1, w, w,
					 w, p2, p3, p2, p3, p2, w,
					 p1, p2, p3, p4, p3, p2, p1,
					 w, p1, p2, p3, p2, p1, w,
					 w, w, p1, w, p1, w, w,
					 w, w, w, p1, w, w, w};
        } else if (m_CurrentLevel == 5) {
            level =
                    {0, 1, 0, 0, 0, 1, 0,
                     1, 4, 1, 1, 1, 3, 1,
                     0, 1, 0, 1, 0, 1, 0,
                     0, 1, 0, 1, 0, 1, 0,
                     1, 5, 1, 6, 1, 2, 1,
                     0, 1, 0, 0, 0, 1, 0};
			color = 
					{w, p3, w, w, w, r, w,
					 p2, w, p1, g, w, w, w,
					 w, p4, w, g, w, r, w,
					 w, br, w, g, w, lb1, w,
					 y, w, y, d, b, w, b,
					 w, br, w, w, w, lb1, w};
		}
		else if (m_CurrentLevel == 6) {
					level =
					{0, 0, 0, 0, 0, 0, 0,
					 0, 0, 0, 100, 0, 0, 0,
					 0, 0, 0, 0, 0, 0, 0,
					 0, 0, 0, 0, 0, 0, 0,
					 0, 0, 0, 0, 0, 0, 0,
					 0, 0, 0, 0, 0, 0, 0};
					color =
					{w, w, w, w, w, w, w,
					 w, w, w, w, w, w, w,
					 w, w, w, w, w, w, w,
					 w, w, w, w, w, w, w,
					 w, w, w, w, w, w, w,
					 w, w, w, w, w, w, w};
		}
    } else if (m_CurrentCluster == 3) {

    }

	std::copy(std::begin(level), std::end(level), std::begin(m_Bricks));
    std::copy(std::begin(color), std::end(color), std::begin(m_Colors));
}

bool dd::Systems::LevelSystem::OnHitPad(const dd::Events::HitPad &event)
{
    m_Cleared = false;
	if (m_BrickGenerating) {
		m_BrickGenerating = false;
		BrickGenerating(m_BrickGeneratingEvent);
	}
    return true;
}

void dd::Systems::LevelSystem::SetBrokenModel(EntityID entity)
{
	std::uniform_int_distribution<int> dist(0, 2);
	int index = dist(m_RandomGenerator);

	int parts = 0;

	switch (index)
	{
	case 0:
		CreateBrokenModelPart(entity, "Models/Brick/BrickBreak/1/WhiteBrickBreak1.obj", glm::vec3(-0.29912f, 0.0082f, 0.f), glm::vec2(0.281f, 0.351f), 3);
		CreateBrokenModelPart(entity, "Models/Brick/BrickBreak/1/WhiteBrickBreak2.obj", glm::vec3(0.07597f, -0.02899f, 0.f), glm::vec2(0.559f, 0.351f), 3);
		CreateBrokenModelPart(entity, "Models/Brick/BrickBreak/1/WhiteBrickBreak3.obj", glm::vec3(0.3227f, 0.05632, 0.f), glm::vec2(0.25f, 0.25f), 3);
		break;

	case 1:
		CreateBrokenModelPart(entity, "Models/Brick/BrickBreak/2/WhiteBrickBreak1.obj", glm::vec3(-0.3236f, 0.0159f, 0.f), glm::vec2(0.237f, 0.327f), 4);
		CreateBrokenModelPart(entity, "Models/Brick/BrickBreak/2/WhiteBrickBreak2.obj", glm::vec3(0.07645f, 0.03131f, 0.f), glm::vec2(0.594f, 0.211f), 4);
		CreateBrokenModelPart(entity, "Models/Brick/BrickBreak/2/WhiteBrickBreak3.obj", glm::vec3(0.09468f, -0.11792f, 0.f), glm::vec2(0.692f, 0.112f), 4);
		CreateBrokenModelPart(entity, "Models/Brick/BrickBreak/2/WhiteBrickBreak4.obj", glm::vec3(0.33957f, 0.13079f, 0.f), glm::vec2(0.208f, 0.085f), 4);
		break;

	case 2:
		CreateBrokenModelPart(entity, "Models/Brick/BrickBreak/3/WhiteBrickBreak1.obj", glm::vec3(-0.34882f, 0.0564f, 0.f), glm::vec2(0.175f, 0.228f), 4);
		CreateBrokenModelPart(entity, "Models/Brick/BrickBreak/3/WhiteBrickBreak2.obj", glm::vec3(0.0678f, 0.07679f, 0.f), glm::vec2(0.699f, 0.183f), 4);
		CreateBrokenModelPart(entity, "Models/Brick/BrickBreak/3/WhiteBrickBreak3.obj", glm::vec3(-0.11862f, -0.10342, 0.f), glm::vec2(0.563f, 0.164f), 4);
		CreateBrokenModelPart(entity, "Models/Brick/BrickBreak/3/WhiteBrickBreak4.obj", glm::vec3(0.31479f, -0.06351f, 0.f), glm::vec2(0.261f, 0.21f), 4);
		break;

	default:
		LOG_INFO("Something went wrong with the random generator in SetBrokenModel");
		break;
	}

	return;
}

void dd::Systems::LevelSystem::CreateBrokenModelPart(EntityID Parent, std::string ModelPath, glm::vec3 RelativePosition, glm::vec2 Hitbox, int NumberOfParts)
{
	auto ent = m_World->CreateEntity(Parent);
	auto cTemplate = m_World->AddComponent<Components::Template>(ent);
	auto cTransform = m_World->AddComponent<Components::Transform>(ent);
	auto cBrickPart = m_World->AddComponent<Components::BrickPart>(ent);
	cBrickPart->NumberOfPartsInBrick = NumberOfParts;

	cTransform->Position = RelativePosition;

	auto cModel = m_World->AddComponent<Components::Model>(ent);
	cModel->ModelFile = ModelPath;

	auto cRec = m_World->AddComponent<Components::RectangleShape>(ent);
	cRec->Dimensions = Hitbox;

	auto cPhys = m_World->AddComponent<Components::Physics>(ent);
	cPhys->CollisionType = CollisionType::Type::Dynamic;
	cPhys->GravityScale = 1.f;
	cPhys->Density = 30;
	cPhys->Category = CollisionLayer::Type::Brick;
	cPhys->Mask = static_cast<CollisionLayer::Type>(CollisionLayer::Type::Wall);
}
