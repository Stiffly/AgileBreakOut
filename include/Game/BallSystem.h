#ifndef DAYDREAM_BALLSYSTEM_H
#define DAYDREAM_BALLSYSTEM_H

#include "Core/System.h"
#include "Core/World.h"
#include "Core/EventBroker.h"
#include "Core/CTransform.h"
#include "Physics/EContact.h"
#include "Game/CBall.h"
#include "Game/CPowerUp.h"

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

private:

};

}
}

#endif
