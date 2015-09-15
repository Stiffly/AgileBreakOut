//
// Created by Adniklastrator on 2015-09-10.
//

#ifndef DAYDREAM_PADSYSTEM_H
#define DAYDREAM_PADSYSTEM_H

#include "Core/InputController.h"
#include "Core/System.h"
#include "Core/Component.h"
#include "Core/CTransform.h"
#include "Core/EventBroker.h"
#include "Core/EKeyDown.h"
#include "Core/EKeyUp.h"
#include "Input/EBindKey.h"
#include "Physics/EContact.h"
#include "Physics/CBoxShape.h"
#include "Physics/CPhysics.h"
#include "Physics/CCircleShape.h"
#include "Physics/ESetImpulse.h"
#include "Rendering/CSprite.h"
#include "Game/CBall.h"
#include "Game/EBallFellOffStage.h"




namespace dd
{

namespace Systems
{

class PadSystem : public System
{
public:
    PadSystem(World* world, std::shared_ptr<dd::EventBroker> eventBroker)
    : System(world, eventBroker)
    { }

    void Initialize() override;
    void Update(double dt) override;
    void UpdateEntity(double dt, EntityID entity, EntityID parent) override;

private:
    dd::EventRelay<PadSystem, dd::Events::KeyDown> m_EKeyDown;
    dd::EventRelay<PadSystem, dd::Events::KeyUp> m_EKeyUp;
    dd::EventRelay<PadSystem, dd::Events::Contact> m_EContact;
    dd::EventRelay<PadSystem, dd::Events::BallFellOffStage> m_EBall;

    bool OnKeyDown(const dd::Events::KeyDown &event);
    bool OnKeyUp(const dd::Events::KeyUp &event);


    bool OnContact(const dd::Events::Contact &event);
    bool BallFellOffStage(const dd::Events::BallFellOffStage &event);

    EntityID removeThisObject = NULL;

    float slowdownModifier = 5.f;
    float accelerationSpeed = 80.f;
    float maxSpeed = 40.f;



    class PadSteeringInputController;
    std::array<std::shared_ptr<PadSteeringInputController>, 4> m_PadInputControllers;

    EntityID ent = 0;
    Components::Transform* transform;
    glm::vec3 acceleration = glm::vec3(0.f, 0.f, 0.f);
    bool left = false, right = false, replaceBall = false;
};

class PadSystem::PadSteeringInputController : InputController<PadSystem>
{
public:
    PadSteeringInputController(std::shared_ptr<dd::EventBroker> eventBroker)
    : InputController(eventBroker)
            {

            }
protected:
    virtual bool OnCommand(const Events::InputCommand &event);
};

}

}


#endif //DAYDREAM_PADSYSTEM_H
