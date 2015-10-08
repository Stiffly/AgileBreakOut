//
// Created by Adniklastrator on 2015-09-09.
//


#include "PrecompiledHeader.h"
#include "Game/LevelSystem.h"


void dd::Systems::LevelSystem::Initialize()
{
    EVENT_SUBSCRIBE_MEMBER(m_EContact, &LevelSystem::OnContact);
    EVENT_SUBSCRIBE_MEMBER(m_EScoreEvent, &LevelSystem::OnScoreEvent);
    EVENT_SUBSCRIBE_MEMBER(m_EMultiBall, &LevelSystem::OnMultiBall);
    EVENT_SUBSCRIBE_MEMBER(m_ECreatePowerUp, &LevelSystem::OnCreatePowerUp);
    EVENT_SUBSCRIBE_MEMBER(m_EMultiBallLost, &LevelSystem::OnMultiBallLost);
    EVENT_SUBSCRIBE_MEMBER(m_EPowerUpTaken, &LevelSystem::OnPowerUpTaken);
    EVENT_SUBSCRIBE_MEMBER(m_EStageCleared, &LevelSystem::OnStageCleared);
    EVENT_SUBSCRIBE_MEMBER(m_EPause, &LevelSystem::OnPause);
    EVENT_SUBSCRIBE_MEMBER(m_EHitPad, &LevelSystem::OnHitPad);

    m_BrickTemplate = m_World->CreateEntity();
    std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(m_BrickTemplate);
    auto model = m_World->AddComponent<Components::Model>(m_BrickTemplate);
    std::shared_ptr<Components::Brick> cBrick = m_World->AddComponent<Components::Brick>(m_BrickTemplate);
    std::shared_ptr<Components::RectangleShape> cRec = m_World->AddComponent<Components::RectangleShape>(m_BrickTemplate);
    cRec->Dimensions = glm::vec2(0.9f, 0.35f);
    std::shared_ptr<Components::Physics> cPhys = m_World->AddComponent<Components::Physics>(m_BrickTemplate);
    std::shared_ptr<Components::Template> cTemplate = m_World->AddComponent<Components::Template>(m_BrickTemplate);
    cPhys->CollisionType = CollisionType::Type::Static;
    cPhys->GravityScale = 0.f;
    cPhys->Category = CollisionLayer::Type::Brick;
    cPhys->Mask = static_cast<CollisionLayer::Type>(CollisionLayer::Type::Ball | CollisionLayer::Type::Projectile | CollisionLayer::Type::Wall | CollisionLayer::LifeBuoy);
    transform->Sticky = false;

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
    auto model = m_World->GetComponent<Components::Model>(entity);
    if (model != nullptr) {
		if (model->ModelFile == "Models/Test/halfpipe/Halfpipe.obj") {
            auto transform = m_World->GetComponent<Components::Transform>(entity);
            if (transform->Position.y <= -34.6) {
                transform->Position.y = 34.6;
            }
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
				EventBroker->Publish(e);
			} else if (brick->Type == InkBlasterBrick) {
				Events::InkBlaster e;
				EventBroker->Publish(e);
			}
            m_LooseBricks--;
            m_World->RemoveEntity(entity);
        }
    }

    auto ball = m_World->GetComponent<Components::Ball>(entity);

    if (NumberOfBricks() <= 0 && m_LooseBricks <= 0 && !Restarting()) {
        if (MultiBalls() <= 0 && PowerUps() <= 0) {
            Events::StageCleared ec;
            ec.ClearedStage = m_CurrentLevel;
            ec.StageCluster = m_CurrentCluster;
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
    if (event.Type != "LevelSystem" && event.Type != "All") {
        return false;
    }

    if (IsPaused()) {
        SetPause(false);
    } else {
        SetPause(true);
    }
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
            CreateBrick(i, j, spacesBetweenBricks, spaceToEdge, 0, num, 1);
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
            CreateBrick(i, j, SpaceBetweenBricks(), SpaceToEdge(), aboveBasicLevel, num, m_Bricks[getter]);
            getter++;
        }
        if (num == 1) {
            num = Lines();
        }
    }
    m_LooseBricks = NumberOfBricks();
    SetRestarting(false);
}

void dd::Systems::LevelSystem::CreateBrick(int row, int line, glm::vec2 spacesBetweenBricks, float spaceToEdge, int aboveLevel, int num, int typeInt)
{
    if (typeInt == EmptyBrickSpace) {
        SetNumberOfBricks(NumberOfBricks() - 1);
        return;
    }
    auto brick = m_World->CloneEntity(m_BrickTemplate);
    m_World->RemoveComponent<Components::Template>(brick);
    auto transform = m_World->GetComponent<Components::Transform>(brick);
    auto cBrick = m_World->GetComponent<Components::Brick>(brick);
    if (typeInt == StandardBrick) {
    } else if (typeInt == MultiBallBrick) {
        cBrick->Type = MultiBallBrick;
        auto model = m_World->GetComponent<Components::Model>(brick);
        model->ModelFile = "Models/Brick/IceBrick.obj";
		
	} else if (typeInt == LifebuoyBrick) {
		cBrick->Type = LifebuoyBrick;
		auto model = m_World->GetComponent<Components::Model>(brick);
		model->ModelFile = "Models/Brick/LifeBuoyBrick.obj";
	} else if (typeInt == StickyBrick) {
		cBrick->Type = StickyBrick;
		auto model = m_World->GetComponent<Components::Model>(brick);
		model->ModelFile = "Models/Brick/StickyBrick.obj";
	} else if (typeInt == InkBlasterBrick) {
		cBrick->Type = InkBlasterBrick;
		auto model = m_World->GetComponent<Components::Model>(brick);
		model->Color = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
	}
    float x = line * spacesBetweenBricks.x;
    float y = row * spacesBetweenBricks.y;
    transform->Position = glm::vec3(x - 3, 5 - spaceToEdge - y + aboveLevel, -10.f);
    cBrick->Score = 10 * num;
    return;
}

void dd::Systems::LevelSystem::OnEntityRemoved(EntityID entity)
{

}

bool dd::Systems::LevelSystem::OnContact(const dd::Events::Contact &event)
{
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

		auto physicsComponent = m_World->GetComponent<Components::Physics>(entityBrick);
		physicsComponent->CollisionType = CollisionType::Type::Dynamic;
		physicsComponent->GravityScale = 1.f;
		physicsComponent->Mask = static_cast<CollisionLayer::Type>(CollisionLayer::Water | CollisionLayer::Wall);

		auto transformComponentBrick = m_World->GetComponent<Components::Transform>(entityBrick);
		auto transformComponentShot = m_World->GetComponent<Components::Transform>(entityShot);
		auto brickModel = m_World->GetComponent<Components::Model>(entityBrick);

		Events::SetImpulse e;
		e.Entity = entityBrick;

		glm::vec2 point = glm::vec2(transformComponentBrick->Position.x + ((transformComponentShot->Position.x - transformComponentBrick->Position.x) / 4),
			transformComponentBrick->Position.y + ((transformComponentShot->Position.y - transformComponentBrick->Position.y) / 4));

		e.Impulse = glm::normalize(point)*5.f;
		e.Point = point;
		EventBroker->Publish(e);


		SetNumberOfBricks(NumberOfBricks() - 1);
		Events::ScoreEvent es;
		es.Score = brick->Score;
		EventBroker->Publish(es);

		m_World->RemoveEntity(entityShot);

		Events::CreateParticleSequence ep;
		ep.parent = entityBrick;
		ep.EmitterLifeTime = 1.f;
		ep.ParticleLifeTime = 1.5f;
		ep.ParticlesPerTick = 1;
		ep.SpawnRate = 0.2;
		ep.EmittingAngle = glm::half_pi<float>();
		ep.Spread = 1.5f;
		ep.Position = transformComponentBrick->Position;
		ep.Radius = 0.05;
		ep.SpriteFile = "Textures/Particles/FadeBall.png";
		ep.Color = brickModel->Color + glm::vec4(1);
		ep.Speed = 50;
		EventBroker->Publish(ep);

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

        auto ballTransform = m_World->GetComponent<Components::Transform>(entityBall);

        auto physicsComponent = m_World->GetComponent<Components::Physics>(entityBrick);
		physicsComponent->CollisionType = CollisionType::Type::Dynamic;
        physicsComponent->GravityScale = 1.f;
        physicsComponent->Mask = static_cast<CollisionLayer::Type>(CollisionLayer::Water | CollisionLayer::Wall);

        auto transformComponentBrick = m_World->GetComponent<Components::Transform>(entityBrick);
        auto transformComponentBall = m_World->GetComponent<Components::Transform>(entityBall);
		auto brickModel = m_World->GetComponent<Components::Model>(entityBrick);

        Events::SetImpulse e;
        e.Entity = entityBrick;

        glm::vec2 point = glm::vec2(transformComponentBrick->Position.x + ((transformComponentBall->Position.x - transformComponentBrick->Position.x )/4),
                                    transformComponentBrick->Position.y  + ((transformComponentBall->Position.y - transformComponentBrick->Position.y)/4));

        e.Impulse = glm::normalize(point)*5.f;///2.f;
        e.Point = point;
        EventBroker->Publish(e);
        //TODO: Bricks dont collide with walls D=


        SetNumberOfBricks(NumberOfBricks() - 1);
        Events::ScoreEvent es;
        es.Score = brick->Score * ball->Combo;
        EventBroker->Publish(es);

		Events::CreateParticleSequence ep;
		ep.parent = entityBrick;
		ep.EmitterLifeTime = 1.f;
		ep.ParticleLifeTime = 1.5f;
		ep.ParticlesPerTick = 1;
		ep.SpawnRate = 0.2;
		ep.EmittingAngle = glm::half_pi<float>();
		ep.Spread = 1.5f;
		ep.Position = transformComponentBrick->Position;
		ep.Radius = 0.05;
		ep.SpriteFile = "Textures/Particles/FadeBall.png";
		ep.Color = brickModel->Color + glm::vec4(1);
		ep.Speed = 50;
		EventBroker->Publish(ep);

        //std::cout << "Combo: " << ball->Combo << std::endl;
        //std::cout << NumberOfBricks() << std::endl;
        //std::cout << "Brick Score: " << brick->Score  << std::endl;
        //std::cout << "Score: " << Score() << std::endl;
    }

    return true;
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
    transform->Sticky = false;

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
        if (m_CurrentLevel < 6) {
            GetNextLevel();
            CreateLevel(12);
        }
    }
    return true;
}

void dd::Systems::LevelSystem::GetNextLevel()
{
    std::array<int, 42> level;
    // 0 is empty space.
    // 1 is standard brick.
    // 2 is multiball brick.
	// 3 is lifebuoy brick.
	// 4 is sticky brick.
	// 5 is ink blaster brick.
    if (m_CurrentCluster == 0) {
        if (m_CurrentLevel == 1) {
            level =
                    {0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 1, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0};
        } else if (m_CurrentLevel == 2) {
            level =
                    {1, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0};
        } else if (m_CurrentLevel == 3) {
            level =
                    {0, 0, 0, 0, 0, 0, 1,
                     0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0};
        } else if (m_CurrentLevel == 4) {
            level =
                    {0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     1, 0, 0, 0, 0, 0, 0};
        } else if (m_CurrentLevel == 5) {
            level =
                    {0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 1};
        }
    } else if (m_CurrentCluster == 1) {
        if (m_CurrentLevel == 1) {
            level =
                    {0, 0, 0, 0, 0, 0, 0,
                     1, 1, 1, 1, 1, 1, 1,
                     1, 0, 1, 2, 1, 0, 1,
                     0, 1, 0, 1, 0, 1, 0,
                     1, 0, 1, 0, 1, 0, 1,
                     0, 0, 0, 0, 0, 0, 0};
        } else if (m_CurrentLevel == 2) {
            level =
                    {0, 0, 0, 0, 0, 0, 0,
                     0, 1, 0, 1, 0, 1, 0,
                     0, 0, 1, 1, 1, 0, 0,
                     0, 0, 0, 3, 0, 0, 0,
                     1, 1, 1, 0, 1, 1, 1,
                     1, 0, 0, 0, 0, 0, 1};
        } else if (m_CurrentLevel == 3) {
            level =
                    {0, 0, 0, 1, 0, 0, 0,
                     0, 0, 1, 1, 1, 0, 0,
                     0, 1, 1, 1, 1, 1, 0,
                     1, 1, 1, 4, 1, 1, 1,
                     0, 0, 0, 1, 0, 0, 0,
                     0, 0, 1, 1, 1, 0, 0};
        } else if (m_CurrentLevel == 4) {
            level =
                    {0, 0, 1, 0, 1, 0, 0,
                     0, 1, 1, 1, 1, 1, 0,
                     1, 1, 1, 1, 1, 1, 1,
                     0, 1, 1, 1, 1, 1, 0,
                     0, 0, 1, 5, 1, 0, 0,
                     0, 0, 0, 1, 0, 0, 0};
        } else if (m_CurrentLevel == 5) {
            level =
                    {0, 1, 0, 0, 0, 1, 0,
                     1, 4, 1, 1, 1, 3, 1,
                     0, 1, 0, 1, 0, 1, 0,
                     0, 1, 0, 1, 0, 1, 0,
                     1, 5, 1, 1, 1, 2 , 1,
                     0, 1, 0, 0, 0, 1, 0};
        }
    } else if (m_CurrentCluster == 3) {

    }

    std::copy(std::begin(level), std::end(level), std::begin(m_Bricks));
}

bool dd::Systems::LevelSystem::OnHitPad(const dd::Events::HitPad &event)
{
    m_Cleared = false;
    return true;
}
