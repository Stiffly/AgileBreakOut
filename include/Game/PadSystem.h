//
// Created by Adniklastrator on 2015-09-10.
//

#ifndef DAYDREAM_PADSYSTEM_H
#define DAYDREAM_PADSYSTEM_H

#include "Core/InputController.h"
#include "Core/System.h"
#include "Core/Component.h"
#include "Core/CTransform.h"
#include "Core/CTemplate.h"
#include "Core/EventBroker.h"
#include "Core/EKeyDown.h"
#include "Core/EKeyUp.h"

#include "Input/EBindKey.h"

#include "Physics/EContact.h"
#include "Physics/CRectangleShape.h"
#include "Physics/CWaterVolume.h"
#include "Physics/CPhysics.h"
#include "Physics/CCircleShape.h"
#include "Physics/ESetImpulse.h"

#include "Rendering/CSprite.h"
#include "Rendering/CModel.h"

#include "Transform/EMove.h"
#include "Transform/ERotate.h"

#include "Game/CBall.h"
#include "Game/CPad.h"
#include "Game/CPowerUp.h"
#include "Game/CStickyAim.h"
#include "Game/EResetBall.h"
#include "Game/EResetAll.h"
#include "Game/ERaiseWater.h"
#include "Game/ERaiseWaterWall.h"
#include "Game/EMultiBall.h"
#include "Game/ELifebuoy.h"
#include "Game/EStickyPad.h"
#include "Game/EStickyAttachedToPad.h"
#include "Game/EInkBlaster.h"
#include "Game/EKrakenAttack.h"
#include "Game/EPowerUpTaken.h"
#include "Game/EStageCleared.h"
#include "Game/EPause.h"
#include "Game/EResume.h"
#include "Game/EHitLag.h"
#include "Game/EScreenShake.h"
#include "Game/EActionButton.h"
#include "Rendering/CAnimation.h"

#include "Game/EBrickGenerating.h"


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

    EntityID& Entity() { return m_Entity; }
    void SetEntity(const EntityID& ent) { m_Entity = ent; }
    glm::vec3 Acceleration() const { return m_Acceleration; }
    void SetAcceleration(const glm::vec3& acceleration) { m_Acceleration = acceleration; }

    bool Left() const { return m_Left; }
    void SetLeft(const bool& left) { m_Left = left; }
    bool Right() const { return m_Right; }
    void SetRight(const bool& right) { m_Right = right; }
    float Edge() const { return m_Edge; }
    void SetEdge(const float& edge) { m_Edge = edge; }

    Components::Transform* Transform() const { return m_Transform; }
    void SetTransform(Components::Transform* transform) { m_Transform = transform; }
    Components::Pad* Pad() const { return m_Pad; }
    void SetPad(Components::Pad* pad) { m_Pad = pad; }

    bool IsPaused() const { return m_Pause; }
    void SetPause(const bool& pause) { m_Pause = pause; }

private:
    EntityID m_Entity = 0;
    glm::vec3 m_Acceleration = glm::vec3(0.f, 0.f, 0.f);
    bool m_Left = false;
    bool m_Right = false;
    bool m_ReplaceBall = false;
    bool m_MultiBall = false;
	bool m_KrakenAttack = false;
	bool m_BallStuck = false;
	double m_KrakenCharge = 0;
	double m_KrakenStrength = 0;
	double m_PlayerStrength = 0;
    float m_Edge = 2.8f;
	EntityID m_KrakenArm;
    Components::Transform* m_Transform = nullptr;
    Components::Pad* m_Pad = nullptr;
	std::shared_ptr<Components::Transform> m_StickTransform;
	std::shared_ptr<Components::StickyAim> m_StickyAim;

	bool m_ResetBall = false;
    bool m_Pause = false;

    dd::EventRelay<PadSystem, dd::Events::KeyDown> m_EKeyDown;
    dd::EventRelay<PadSystem, dd::Events::KeyUp> m_EKeyUp;
    dd::EventRelay<PadSystem, dd::Events::Contact> m_EContact;
    dd::EventRelay<PadSystem, dd::Events::Contact> m_EContactPowerUp;
    dd::EventRelay<PadSystem, dd::Events::StageCleared> m_EStageCleared;
	dd::EventRelay<PadSystem, dd::Events::ResetBall> m_EResetBall;
	dd::EventRelay<PadSystem, dd::Events::ResetAll> m_EResetAll;
	dd::EventRelay<PadSystem, dd::Events::RaiseWater> m_ERaiseWater;
	dd::EventRelay<PadSystem, dd::Events::RaiseWaterWall> m_ERaiseWaterWall;
    dd::EventRelay<PadSystem, dd::Events::Pause> m_EPause;
	dd::EventRelay<PadSystem, dd::Events::Resume> m_EResume;
	dd::EventRelay<PadSystem, dd::Events::KrakenAttack> m_EKrakenAttack;
	dd::EventRelay<PadSystem, dd::Events::StickyPad> m_EStickyPad;
	dd::EventRelay<PadSystem, dd::Events::StickyAttachedToPad> m_EStickyAttachedToPad;
	dd::EventRelay<PadSystem, dd::Events::ActionButton> m_EActionButton;

    bool OnKeyDown(const dd::Events::KeyDown &event);
    bool OnKeyUp(const dd::Events::KeyUp &event);
    bool OnContact(const dd::Events::Contact &event);
    bool OnContactPowerUp(const dd::Events::Contact &event);
    bool OnStageCleared(const dd::Events::StageCleared &event);
	bool OnResetBall(const dd::Events::ResetBall &event);
	bool OnResetAll(const dd::Events::ResetAll &event);
	bool OnRaiseWater(const dd::Events::RaiseWater &event);
	bool OnRaiseWaterWall(const dd::Events::RaiseWaterWall &event);
	bool RaisePad(float amount, float speed);
    bool OnPause(const dd::Events::Pause &event);
	bool OnResume(const dd::Events::Resume &event);
	bool OnKrakenAttack(const dd::Events::KrakenAttack &event);
	bool OnStickyPad(const dd::Events::StickyPad &event);
	bool OnStickyAttachedToPad(const dd::Events::StickyAttachedToPad &event);
	bool OnActionButton(const dd::Events::ActionButton &event);

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
