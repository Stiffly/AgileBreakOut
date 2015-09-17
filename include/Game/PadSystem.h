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
#include "Rendering/CModel.h"
#include "Game/CBall.h"
#include "Game/EResetBall.h"
#include "Game/CPad.h"
#include "Game/EMultiBall.h"


namespace dd
{

namespace Systems
{

class PadSystem : public System
{
public:
    PadSystem(World* world, std::shared_ptr<dd::EventBroker> eventBroker)
    : System(world, eventBroker) { }

    void Initialize() override;
    void Update(double dt) override;
    void UpdateEntity(double dt, EntityID entity, EntityID parent) override;

    EntityID CreateBall();

    EntityID& Entity() { return m_Entity; }
    void SetEntity(const EntityID& ent) { m_Entity = ent; }
    glm::vec3 Acceleration() const { return m_Acceleration; }
    void SetAcceleration(const glm::vec3& acceleration) { m_Acceleration = acceleration; }

    bool Left() const { return m_Left; }
    void SetLeft(const bool& left) { m_Left = left; }
    bool Right() const { return m_Right; }
    void SetRight(const bool& right) { m_Right = right; }
    bool ReplaceBall() const { return m_ReplaceBall; }
    void SetReplaceBall(const bool& replaceBall) { m_ReplaceBall = replaceBall; }
    bool MultiBall() const { return m_MultiBall; }
    void SetMultiBall(const bool& multiBall) { m_MultiBall = multiBall; }

    Components::Transform* Transform() const { return m_Transform; }
    void SetTransform(Components::Transform* transform) { m_Transform = transform; }
    Components::Pad* Pad() const { return m_Pad; }
    void SetPad(Components::Pad* pad) { m_Pad = pad; }

private:
    EntityID m_Entity = 0;
    glm::vec3 m_Acceleration = glm::vec3(0.f, 0.f, 0.f);
    bool m_Left = false, m_Right = false, m_ReplaceBall = false, m_MultiBall = false;
    Components::Transform* m_Transform;
    Components::Pad* m_Pad;

    dd::EventRelay<PadSystem, dd::Events::KeyDown> m_EKeyDown;
    dd::EventRelay<PadSystem, dd::Events::KeyUp> m_EKeyUp;
    dd::EventRelay<PadSystem, dd::Events::Contact> m_EContact;
    dd::EventRelay<PadSystem, dd::Events::ResetBall> m_EResetBall;
    dd::EventRelay<PadSystem, dd::Events::MultiBall> m_EMultiBall;

    bool OnKeyDown(const dd::Events::KeyDown &event);
    bool OnKeyUp(const dd::Events::KeyUp &event);

    bool OnContact(const dd::Events::Contact &event);
    bool OnResetBall(const dd::Events::ResetBall &event);
    bool OnMultiBall(const dd::Events::MultiBall &event);

    class PadSteeringInputController;
    std::array<std::shared_ptr<PadSteeringInputController>, 4> m_PadInputControllers;
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
