//
// Created by Adniklastrator on 2015-09-10.
//

#include "PrecompiledHeader.h"
#include "Game/PadSystem.h"
#include "Core/World.h"
#include <iostream>
#include <Game/CPad.h>
#include <Rendering/CPointLight.h>


void dd::Systems::PadSystem::Initialize()
{
    Events::BindKey r;
    r.KeyCode = GLFW_KEY_RIGHT;
    r.Command = "right";
    EventBroker->Publish(r);

    Events::BindKey l;
    l.KeyCode = GLFW_KEY_LEFT;
    l.Command = "left";
    EventBroker->Publish(l);

    EVENT_SUBSCRIBE_MEMBER(m_EKeyDown, &PadSystem::OnKeyDown);
    EVENT_SUBSCRIBE_MEMBER(m_EKeyUp, &PadSystem::OnKeyUp);
    EVENT_SUBSCRIBE_MEMBER(m_EContact, &PadSystem::OnContact);
    EVENT_SUBSCRIBE_MEMBER(m_EContactPowerUp, &PadSystem::OnContactPowerUp);
    EVENT_SUBSCRIBE_MEMBER(m_EStageCleared, &PadSystem::OnStageCleared);
	EVENT_SUBSCRIBE_MEMBER(m_EResetBall, &PadSystem::OnResetBall);
	EVENT_SUBSCRIBE_MEMBER(m_EResetAll, &PadSystem::OnResetAll);
	EVENT_SUBSCRIBE_MEMBER(m_ERaiseWater, &PadSystem::OnRaiseWater);
	EVENT_SUBSCRIBE_MEMBER(m_ERaiseWaterWall, &PadSystem::OnRaiseWaterWall);
    EVENT_SUBSCRIBE_MEMBER(m_EPause, &PadSystem::OnPause);
	EVENT_SUBSCRIBE_MEMBER(m_EResume, &PadSystem::OnResume);
	EVENT_SUBSCRIBE_MEMBER(m_EKrakenAttack, &PadSystem::OnKrakenAttack);
	EVENT_SUBSCRIBE_MEMBER(m_EKrakenAttackEnd, &PadSystem::OnKrakenAttackEnd);
	EVENT_SUBSCRIBE_MEMBER(m_EStickyPad, &PadSystem::OnStickyPad);
	EVENT_SUBSCRIBE_MEMBER(m_EStickyAttachedToPad, &PadSystem::OnStickyAttachedToPad);
	EVENT_SUBSCRIBE_MEMBER(m_EActionButton, &PadSystem::OnActionButton);
    //EVENT_SUBSCRIBE_MEMBER(m_EBindKey, &PadSystem::OnBindKey);

    {
        auto ent = m_World->CreateEntity();
        m_World->SetProperty(ent, "Name", "Pad");
        auto ctransform = m_World->AddComponent<Components::Transform>(ent);
        ctransform->Position = glm::vec3(0.f, m_PadHeight, -10.f);
        auto rectangleShape = m_World->AddComponent<Components::RectangleShape>(ent);
        rectangleShape->Dimensions = glm::vec2(1.f, 0.1f);
        auto physics = m_World->AddComponent<Components::Physics>(ent);
        physics->CollisionType = CollisionType::Type::Kinematic;
        physics->Category = CollisionLayer::Type::Pad;
		physics->Mask = static_cast<CollisionLayer::Type>(CollisionLayer::Ball | CollisionLayer::PowerUp | CollisionLayer::LifeBuoy);
        physics->Calculate = true;
        auto cModel = m_World->AddComponent<Components::Model>(ent);
        cModel->ModelFile = "Models/Ship/Ship.obj";

        auto pad = m_World->AddComponent<Components::Pad>(ent);
		pad->SlowdownModifier = 10.f;
        m_World->CommitEntity(ent);

        m_Edge = 3.2 - (ctransform->Scale.x / 2);
		
		m_PadEntity = ent;
		m_PadTransform = ctransform.get();
		m_PadComponent = pad.get();
    }

	//Stick
	{
		auto ent = m_World->CreateEntity();
		m_World->SetProperty(ent, "Name", "Stick");
		std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(ent);
		transform->Position = glm::vec3(30.f, 0.f, -10.f);
		transform->Scale = glm::vec3(0.1f, 25.f, 0.1f);
		std::shared_ptr<Components::Sprite> sprite = m_World->AddComponent<Components::Sprite>(ent);
		sprite->SpriteFile = "Models/Brick/White.png";
		sprite->Color = glm::vec4(0.f, 0.5f, 0.f, 0.5f);
		std::shared_ptr<Components::StickyAim> sticky = m_World->AddComponent<Components::StickyAim>(ent);
		m_World->CommitEntity(ent);

		m_StickTransform = transform;
		m_StickyAim = sticky;

			/*{
		auto entChild = m_World->CreateEntity(ent);
		std::shared_ptr<Components::Transform> transform = m_World->AddComponent<Components::Transform>(entChild);
		transform->Position = glm::vec3(0.f, 12.5f, 0.f);

		transform->Scale = glm::vec3(0.1f, 25.f, 0.1f);
		std::shared_ptr<Components::Sprite> sprite = m_World->AddComponent<Components::Sprite>(entChild);
		sprite->SpriteFile = "Models/Brick/White.png";
		sprite->Color = glm::vec4(0.f, 0.5f, 0.f, 0.5f);
		}*/
	}

	// Kraken arm
	{
		m_KrakenArm = m_World->CreateEntity(m_PadEntity);
		auto transform = m_World->AddComponent<Components::Transform>(m_KrakenArm);
		transform->Position = glm::vec3(1.f, -3.0f, 0.f);
		auto model = m_World->AddComponent<Components::Model>(m_KrakenArm);
		model->ModelFile = "Models/Kraken/Arm.dae";
		auto animation = m_World->AddComponent<Components::Animation>(m_KrakenArm);
		animation->Speed = 0.0f;
		animation->Loop = false;
		m_World->CommitEntity(m_KrakenArm);
	}

	m_ResetBall = true;
}

void dd::Systems::PadSystem::UpdateEntity(double dt, EntityID entity, EntityID parent)
{
	if (m_Pause) {
		return;
	}

    auto templateCheck = m_World->GetComponent<Components::Template>(entity);
    if (templateCheck != nullptr){ return; }

	auto ball = m_World->GetComponent<Components::Ball>(entity);
	if (ball != nullptr) {
		if (ball->Waiting || ball->Sticky) {
			if (m_ResetBall && ball->Waiting) {
				ball->StickyPlacement = glm::vec3(0.f, 0.f, 0.f);
				ball->StickyPlacement.y += 0.25f;
				m_ResetBall = false;
			}
			auto ballTransform = m_World->GetComponent<Components::Transform>(entity);
			ballTransform->Position = m_PadTransform->Position;
			ballTransform->Position += ball->StickyPlacement;
			if (ball->Loaded) {
				ballTransform->Orientation = glm::quat();
			} else {
				glm::vec2 dir = glm::normalize(glm::vec2(ball->SavedSpeed.x, ball->SavedSpeed.y));
				glm::vec2 up = glm::vec2(0.f, 1.f);
				float angle = glm::acos(glm::dot<float>(dir, up)) * glm::sign(dir.x);
				ballTransform->Orientation = glm::rotate(glm::quat(), angle, glm::vec3(0.f, 0.f, -1.f));
				if (ball->Sticky) {
					m_StickTransform->Orientation = glm::rotate(glm::quat(), angle, glm::vec3(0.f, 0.f, 1.f));
					m_StickTransform->Position = ballTransform->Position;
				}
			}
			
		}
		if (!m_StickyAim->Aiming) {
			m_StickTransform->Position = glm::vec3(30.f, 0.f, 0.f);
		}
	}
}

void dd::Systems::PadSystem::Update(double dt)
{
    if (IsPaused()) {
        return;
    }

	if (m_KrakenAttack) {
		m_KrakenCharge -= m_KrakenStrength * dt;
		if (m_KrakenCharge < 0) {
			m_KrakenCharge = 0;
		}
		Events::KrakenAttack e;
		e.ChargeUpdate = m_KrakenCharge;
		EventBroker->Publish(e);
		return;
	}

	auto transform = m_PadTransform;
	auto pad = m_PadComponent;
	auto acceleration = m_PadAcceleration;

	if (glm::abs(transform->Position.y - m_PadHeight) > m_PadRiseSpeed) {
		if (transform->Position.y < m_PadHeight) {
			transform->Position.y += m_PadRiseSpeed * (float)dt;
		} else if (transform->Position.y > m_PadHeight) {
			transform->Position.y -= m_PadRiseSpeed * 5 * (float)dt;
		}
	}
    if (transform->Velocity.x < -pad->MaxSpeed) {
        transform->Velocity.x = -pad->MaxSpeed;
    }
    else if (transform->Velocity.x > pad->MaxSpeed) {
        transform->Velocity.x = pad->MaxSpeed;
    }
    transform->Position += transform->Velocity * (float)dt;
    if (transform->Position.x > m_Edge) {
        transform->Position.x = m_Edge;
    } else if (transform->Position.x < -m_Edge) {
        transform->Position.x = -m_Edge;
    }
    transform->Velocity += acceleration * (float)dt;
    if (glm::abs(transform->Velocity.x) > 1 || (!m_Left && !m_Right)) {
        transform->Velocity -= transform->Velocity * pad->SlowdownModifier * (float) dt;
    }

    if (m_Left) {
        acceleration.x = -pad->AccelerationSpeed;
    } else if (m_Right) {
        acceleration.x = pad->AccelerationSpeed;
    } else {
        acceleration.x = 0.f;
    }

    m_PadTransform = transform;
    m_PadComponent = pad;
    m_PadAcceleration = acceleration;

    return;
}

bool dd::Systems::PadSystem::OnPause(const dd::Events::Pause &event)
{
    /*if (event.Type != "PadSystem" && event.Type != "All") {
        return false;
    }*/

	m_Pause = true;

    return true;
}

bool dd::Systems::PadSystem::OnResume(const dd::Events::Resume &event)
{
	/*if (event.Type != "PadSystem" && event.Type != "All") {
	return false;
	}*/
	m_Pause = false;
	return true;
}

bool dd::Systems::PadSystem::OnKeyDown(const dd::Events::KeyDown &event) {
    int val = event.KeyCode;
    if (val == GLFW_KEY_UP) {
        //std::cout << "Up!" << std::endl;
    } else if (val == GLFW_KEY_DOWN) {
        //std::cout << "Down!" << std::endl;
    } else if (val == GLFW_KEY_LEFT) {
        //std::cout << "Left!" << std::endl;
        //acceleration.x = -0.01f;
        m_Left = true;
    } else if (val == GLFW_KEY_RIGHT) {
        //std::cout << "Right!" << std::endl;
        //acceleration.x = 0.01f;
        m_Right = true;
    } else if (val == GLFW_KEY_R) {
        Events::ResetBall e;
        EventBroker->Publish(e);
	} else if (val == GLFW_KEY_W) {
		Events::RaiseWater e;
		e.Amount = 0.2;
		e.Speed = 0.2;
		EventBroker->Publish(e);
	} else if (val == GLFW_KEY_Q) {
		Events::RaiseWaterWall e;
		e.Amount = 0.2;
		e.Speed = 0.2;
		EventBroker->Publish(e);
	} else if (val == GLFW_KEY_A) {
		Events::ResetAll e;
		EventBroker->Publish(e);
	} else if (val == GLFW_KEY_M) {
        Events::MultiBall e;
        e.padTransform = m_PadTransform;
        EventBroker->Publish(e);
    } else if (val == GLFW_KEY_P) {
		if (IsPaused()) {
			Events::Resume e;
			e.Type = "All";
			EventBroker->Publish(e);
		} else {
			Events::Pause e;
			e.Type = "All";
			EventBroker->Publish(e);
		}
    } else if (val == GLFW_KEY_H) {
        Events::HitLag e;
        e.Time = 0.2;
        e.Type = "All";
        EventBroker->Publish(e);
    } else if (val == GLFW_KEY_S) {
		Events::StageCleared e;
		EventBroker->Publish(e);
	} else if (val == GLFW_KEY_E) {
		Events::ScreenShake e;
		e.Intensity = 20;
		e.Time = 3;
		e.TimeTakenToCoolDown = 10;
		EventBroker->Publish(e);
	} else if (val == GLFW_KEY_G) {
		Events::BrickGenerating e;
		e.Origin1 = glm::vec3(-5, 7, -10);
		e.Origin2 = glm::vec3(5, 7, -10);
		e.Set = 1;
		e.SetCluster = 1;
		EventBroker->Publish(e);
	} else if (val == GLFW_KEY_C) {
		Events::BreakAllBricks e;
		EventBroker->Publish(e);
	} else if (val == GLFW_KEY_L) {
		Events::Lifebuoy e;
		e.Transform = m_PadTransform;
		EventBroker->Publish(e);
	} else if (val == GLFW_KEY_Y) {
		Events::StickyPad e;
		e.Times = 3;
		EventBroker->Publish(e);
	} else if (val == GLFW_KEY_I) {
		Events::InkBlaster e;
		e.Shots = 5;
		e.Speed = 7;
		EventBroker->Publish(e);
	} else if (val == GLFW_KEY_K) {
		Events::KrakenAttack e;
		e.ChargeUpdate = 0;
		e.KrakenStrength = 0.1;
		e.PlayerStrength = 0.05;
		EventBroker->Publish(e);
	} else if (val == GLFW_KEY_Q) {
		Events::Move e;
		e.Entity = m_PadEntity;
		e.GoalPosition = glm::vec3(0, 0, -10); 
		e.Speed = 5;
		e.Queue = false;
		EventBroker->Publish(e);
	} else if (val == GLFW_KEY_SPACE) {
        Events::ActionButton e;
		e.Position = m_PadTransform->Position;
        EventBroker->Publish(e);
    } else if (val == GLFW_KEY_D) {
        return false;
    }
    return true;
}

bool dd::Systems::PadSystem::OnKeyUp(const dd::Events::KeyUp &event)
{
    int val = event.KeyCode;
    if (val == GLFW_KEY_UP) {

    } else if (val == GLFW_KEY_DOWN) {

    } else if (val == GLFW_KEY_LEFT) {
        m_Left = false;
    } else if (val == GLFW_KEY_RIGHT) {
        m_Right = false;
    }
    return true;
}

bool dd::Systems::PadSystem::OnContact(const dd::Events::Contact &event)
{
    /*
    EntityID entityBall = event.Entity2;
    auto ball = m_World->GetComponent<Components::Ball>(entityBall);
    if (ball == NULL) {
        return false;
    }
    EntityID entityPad = event.Entity1;
    auto pad = m_World->GetComponent<Components::Pad>(entityPad);
    if (pad == NULL) {
        return false;
    }
    auto transformBall = m_World->GetComponent<Components::Transform>(entityBall);
    auto transformPad = m_World->GetComponent<Components::Transform>(entityPad);

    float movementMultiplier = 0.5f;

    //float movementX = (event.ContactPoint.x - transformPad->Position.x) * movementMultiplier;
    //float movementY = glm::cos((abs(movementX) / (3.2f * movementMultiplier)) * 3.14159265359f / 2) * 2.f;
    if (whatX > 0) {
        movementX = movementMultiplier * whatX;
        //std::cout << "Right!" << std::endl;
    } else {
        movementX = movementMultiplier * whatX;
        //std::cout << "Left!" << std::endl;
    }

   // std::cout << movementX << " " << movementY << std::endl;

    //float movementX = (event.ContactPoint.x - transformPad->Position.x) * movementMultiplier;
    float movementY = glm::cos((abs(movementX) / ((1.6f) * movementMultiplier)) * 3.14159265359f / 2)+ 0.2;
    //std::cout << movementX << " " << movementY << std::endl;
    float len = glm::length<float>(transformBall->Velocity);
    //auto pointlight = m_World->AddComponent<Components::PointLight>(ent);


    transformBall->Velocity += glm::vec3(transformPad->Velocity.x, 0, 0);
    transformBall->Velocity = glm::normalize(transformBall->Velocity) * len;


    //transform->Velocity = glm::vec3(movementX, movementY, 0.f);
     */
	return false;
}

bool dd::Systems::PadSystem::OnContactPowerUp(const dd::Events::Contact &event)
{
    EntityID entityPower = 0;
    EntityID entityPad = 0;
    auto powerUp = m_World->GetComponent<Components::PowerUp>(event.Entity1);
    auto pad = m_World->GetComponent<Components::Pad>(event.Entity2);
    if (powerUp != nullptr) {
        entityPower = event.Entity1;
    } else {
        powerUp = m_World->GetComponent<Components::PowerUp>(event.Entity2);
        if (powerUp != nullptr) {
            entityPower = event.Entity2;
        }
    }
    if (pad != nullptr) {
        entityPad = event.Entity2;
    } else {
        pad = m_World->GetComponent<Components::Pad>(event.Entity1);
        if (pad != nullptr) {
            entityPad = event.Entity1;
        }
    }

    if (entityPower == 0 || entityPad == 0) {
        return false;
    }

    pad = m_World->GetComponent<Components::Pad>(entityPad);
    if (pad == nullptr) {
        return false;
    }

    m_World->RemoveEntity(entityPower);
    Events::PowerUpTaken ep;
    ep.Name = "Something";
    EventBroker->Publish(ep);

    Events::MultiBall e;
    auto transform = m_World->GetComponent<Components::Transform>(entityPad);
    e.padTransform = transform;
    EventBroker->Publish(e);
    return true;
}

bool dd::Systems::PadSystem::OnStageCleared(const dd::Events::StageCleared &event)
{
    //auto entity = CreateBall();
	m_StickyAim->Aiming = false;
	m_PadHeight = m_PadOriginalHeight;
    return true;
}

bool dd::Systems::PadSystem::OnResetBall(const dd::Events::ResetBall &event)
{
	m_ResetBall = true;
	return true;
}

bool dd::Systems::PadSystem::OnResetAll(const dd::Events::ResetAll &event)
{

	return true;
}

bool dd::Systems::PadSystem::OnRaiseWater(const dd::Events::RaiseWater &event)
{
	RaisePad(event.Amount, event.Speed);

	return true;
}

bool dd::Systems::PadSystem::OnRaiseWaterWall(const dd::Events::RaiseWaterWall &event)
{
	RaisePad(event.Amount, event.Speed);

	return true;
}

bool dd::Systems::PadSystem::RaisePad(float amount, float speed)
{
	m_PadHeight += amount;
	m_PadRiseSpeed = speed;
	return true;
}

bool dd::Systems::PadSystem::OnKrakenAttack(const dd::Events::KrakenAttack &event)
{
	if (!m_KrakenAttack) {
		m_KrakenStrength = event.KrakenStrength;
		m_PlayerStrength = event.PlayerStrength;
		m_KrakenCharge = event.ChargeUpdate;
		m_KrakenAttack = true;
		auto transform = m_PadTransform;
		auto acceleration = m_PadAcceleration;

		transform->Velocity = glm::vec3(0, 0, 0);
		acceleration = glm::vec3(0, 0, 0);
		if (m_KrakenArmHitbox == 0) {
			auto animation = m_World->GetComponent<Components::Animation>(m_KrakenArm);
			animation->Time = 0.0;
			animation->Speed = 1.0;

			m_KrakenArmHitbox = m_World->CreateEntity();
			auto hb_transform = m_World->AddComponent<Components::Transform>(m_KrakenArmHitbox);
			hb_transform->Position = transform->Position + glm::vec3(-1.f, -5.0f, -10.f);
			//hb_transform->Position = glm::vec3(-2.f, -8.0f, -10.f);
			hb_transform->Scale = glm::vec3(2.5f, 2.5f, 1.f);
			hb_transform->Velocity = glm::vec3(0.f, 10.f, 0.f);
			//auto hb_sprite = m_World->AddComponent<Components::Sprite>(m_KrakenArmHitbox);
			//hb_sprite->SpriteFile = "Textures/Core/ErrorTexture.png";
			auto hb_physics = m_World->AddComponent<Components::Physics>(m_KrakenArmHitbox);
			hb_physics->Calculate = true;
			hb_physics->CollisionType = CollisionType::Type::Static;
			hb_physics->Category = CollisionLayer::Type::Other;
			hb_physics->Mask = CollisionLayer::Type::Water;
			hb_physics->GravityScale = 0.f;
			auto hb_HitBox = m_World->AddComponent<Components::CircleShape>(m_KrakenArmHitbox);
			hb_HitBox->Radius = 1.25f;
			m_World->CommitEntity(m_KrakenArmHitbox);
		}

		m_PadTransform = transform;
		m_PadAcceleration = acceleration;
	} else if (m_KrakenCharge >= 1) {
		m_KrakenAttack = false;
		Events::KrakenAttackEnd e;
		EventBroker->Publish(e);
	}
	return true;
}

bool dd::Systems::PadSystem::OnKrakenAttackEnd(const dd::Events::KrakenAttackEnd &event)
{
	m_KrakenAttack = false;
	m_KrakenCharge = 0;
	m_KrakenStrength = 0;
	m_PlayerStrength = 0;

		//m_World->RemoveEntity(m_KrakenArm);

	auto animation = m_World->GetComponent<Components::Animation>(m_KrakenArm);
	animation->Time = 0.0;
	animation->Speed = -1.5;
	m_World->RemoveEntity(m_KrakenArmHitbox);
	//m_KrakenArm = NULL;
	m_KrakenArmHitbox = 0;
		
	return true;
}

bool dd::Systems::PadSystem::OnStickyPad(const dd::Events::StickyPad &event)
{
	return true;
}

bool dd::Systems::PadSystem::OnStickyAttachedToPad(const dd::Events::StickyAttachedToPad &event)
{
	m_StickyAim->Aiming = true;
	return true;
}

bool dd::Systems::PadSystem::OnActionButton(const dd::Events::ActionButton &event)
{
	if (m_KrakenAttack) {
		m_KrakenCharge += m_PlayerStrength;

		Events::KrakenAttack e;
		e.ChargeUpdate = m_KrakenCharge;
		e.KrakenStrength = 0;
		e.PlayerStrength = 0;
		EventBroker->Publish(e);
	}
	m_StickyAim->Aiming = false;
	return true;
}

bool dd::Systems::PadSystem::PadSteeringInputController::OnCommand(const Events::InputCommand &event)
{
    std::string command = event.Command;

    std::cout << "Command!" << std::endl;

    if (command == "right") {
        std::cout << "Right!" << std::endl;
    } else if (command == "left") {
        std::cout << "Left!" << std::endl;
    }

    return true;
}