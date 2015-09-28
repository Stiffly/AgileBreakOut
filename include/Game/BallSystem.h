#ifndef DAYDREAM_BALLSYSTEM_H
#define DAYDREAM_BALLSYSTEM_H

#include "Core/System.h"
#include "Core/World.h"
#include "Core/EventBroker.h"
#include "Core/CTransform.h"
#include "Physics/CPhysics.h"
#include "Physics/CCircleShape.h"
#include "Physics/EContact.h"
#include "Rendering/CModel.h"
#include "Rendering/CPointLight.h"
#include "Game/CBall.h"
#include "Game/CPowerUp.h"
#include "Game/ELifeLost.h"
#include "Game/EResetBall.h"
#include "Game/EMultiBall.h"
#include "Game/EMultiBallLost.h"
#include "Game/EGameOver.h"

namespace dd {

namespace Systems {

class BallSystem : public System {
public:
    BallSystem(World *world, std::shared_ptr<dd::EventBroker> eventBroker)
            : System(world, eventBroker) { }

    ~BallSystem();

    EventRelay<BallSystem, Events::Contact> m_Contact;
    bool Contact(const Events::Contact &event);

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
    EntityID Ball() { return m_Ball; };
    void SetBall(const EntityID& ball) { m_Ball = ball; }

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
    bool MultiBall() const { return m_MultiBall; }
    void SetMultiBall(const bool& multiBall) { m_MultiBall = multiBall; }

private:
    float m_EdgeX = 3.2f;
    float m_EdgeY = 5.2f;
    int m_MultiBalls = 0;
    int m_Lives = 3;
    int m_PastLives = 3;
    bool m_ReplaceBall = false;
    bool m_MultiBall = false;

    EntityID m_Ball;

    dd::EventRelay<BallSystem, dd::Events::LifeLost> m_ELifeLost;
    dd::EventRelay<BallSystem, dd::Events::MultiBallLost> m_EMultiBallLost;
    dd::EventRelay<BallSystem, dd::Events::ResetBall> m_EResetBall;
    dd::EventRelay<BallSystem, dd::Events::MultiBall> m_EMultiBall;

    bool OnLifeLost(const dd::Events::LifeLost &event);
    bool OnMultiBallLost(const dd::Events::MultiBallLost &event);
    bool OnResetBall(const dd::Events::ResetBall &event);
    bool OnMultiBall(const dd::Events::MultiBall &event);
};

}
}

#endif
