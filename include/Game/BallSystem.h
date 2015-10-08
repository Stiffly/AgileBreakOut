#ifndef DAYDREAM_BALLSYSTEM_H
#define DAYDREAM_BALLSYSTEM_H

#include "Core/System.h"
#include "Core/World.h"
#include "Core/EventBroker.h"
#include "Core/CTransform.h"
#include "Core/CTemplate.h"
#include "Physics/CPhysics.h"
#include "Physics/CCircleShape.h"
#include "Physics/CRectangleShape.h"
#include "Physics/EContact.h"
#include "Rendering/CModel.h"
#include "Rendering/CPointLight.h"
#include "Game/CBall.h"
#include "Game/CPowerUp.h"
#include "Game/CLife.h"
#include "Game/CBrick.h"
#include "Game/ELifeLost.h"
#include "Game/EComboEvent.h"
#include "Game/EResetBall.h"
#include "Game/EMultiBall.h"
#include "Game/EMultiBallLost.h"
#include "Game/EStickyPad.h"
#include "Game/EInkBlaster.h"
#include "Game/EGameOver.h"
#include "Game/EPause.h"
#include "Game/EHitPad.h"
#include "Game/EHitLag.h"
#include "Game/EActionButton.h"

namespace dd
{

namespace Systems
{

class BallSystem : public System
{
public:
    BallSystem(World *world, std::shared_ptr<dd::EventBroker> eventBroker)
            : System(world, eventBroker) { }

    ~BallSystem();



    void RegisterComponents(ComponentFactory *cf) override;

    void Initialize() override;

    // Called once per system every tick
    void Update(double dt) override;

    // Called once for every entity in the world every tick
    void UpdateEntity(double dt, EntityID entity, EntityID parent) override;

    // Called when components are committed to an entity
    void OnEntityCommit(EntityID entity) override;

    // Called when an entity is removed
    void OnEntityRemoved(EntityID entity) override;

    EntityID CreateBall();
    void CreateLife(int);
    EntityID Ball() { return m_Ball; };
    void SetBall(const EntityID& ball) { m_Ball = ball; }

    float& XMovementMultiplier() { return m_XMovementMultiplier; }
    void SetXMovementMultiplier(const float& xMovementMultiplier) { m_XMovementMultiplier = xMovementMultiplier; }
    float& EdgeX() { return m_EdgeX; }
    void SetEdgeX(const float& edgeX) { m_EdgeX = edgeX; }
    float& EdgeY() { return m_EdgeY; }
    void SetEdgeY(const float& edgeY) { m_EdgeY = edgeY; }
    int& MultiBalls() { return m_MultiBalls; }
    void SetMultiBalls(const int& multiBalls) { m_MultiBalls = multiBalls; }
    int& Lives() { return m_Lives; }
    void SetLives(const int& lives) { m_Lives = lives; }
    int& PastLives() { return m_PastLives; }
    void SetPastLives(const int& pastLives) { m_PastLives = pastLives; }
    bool ReplaceBall() const { return m_ReplaceBall; }
    void SetReplaceBall(const bool& replaceBall) { m_ReplaceBall = replaceBall; }
    bool IsPaused() const { return m_Pause; }
    void SetPause(const bool& pause) { m_Pause = pause; }

private:
    float m_XMovementMultiplier = 2.f;
    float m_EdgeX = 3.2f;
    float m_EdgeY = 5.2f;
    int m_MultiBalls = 0;
    int m_Lives = 3;
    int m_PastLives = 3;
    bool m_ReplaceBall = false;
    bool m_Pause = false;
    bool m_Waiting = true;
	bool m_Sticky = false;
	bool m_InkBlaster = false;
	int m_StickyCounter = 5;

    glm::vec3 m_SavedSpeed;
    bool m_InitializePause = false;

    EntityID m_Ball;

    std::unordered_map<EntityID, std::list<glm::vec2>> m_Contacts;
    void ResolveContacts();

    dd::EventRelay<BallSystem, dd::Events::LifeLost> m_ELifeLost;
    dd::EventRelay<BallSystem, dd::Events::MultiBallLost> m_EMultiBallLost;
    dd::EventRelay<BallSystem, dd::Events::ResetBall> m_EResetBall;
    dd::EventRelay<BallSystem, dd::Events::MultiBall> m_EMultiBall;
	dd::EventRelay<BallSystem, dd::Events::StickyPad> m_EStickyPad;
	dd::EventRelay<BallSystem, dd::Events::InkBlaster> m_EInkBlaster;
    dd::EventRelay<BallSystem, dd::Events::Pause> m_EPause;
    dd::EventRelay<BallSystem, dd::Events::Contact> m_Contact;
    dd::EventRelay<BallSystem, dd::Events::ActionButton> m_EActionButton;

    bool Contact(const Events::Contact &event);
    bool OnLifeLost(const dd::Events::LifeLost &event);
    bool OnMultiBallLost(const dd::Events::MultiBallLost &event);
    bool OnResetBall(const dd::Events::ResetBall &event);
    bool OnMultiBall(const dd::Events::MultiBall &event);
	bool OnStickyPad(const dd::Events::StickyPad &event);
	bool OnInkBlaster(const dd::Events::InkBlaster &event);
    bool OnPause(const dd::Events::Pause &event);
    bool OnActionButton(const dd::Events::ActionButton &event);
};

}

}

#endif
