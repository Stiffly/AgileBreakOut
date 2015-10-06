#include "PrecompiledHeader.h"
#include <boost/test/unit_test.hpp>
#include "Core/World.h"
#include "Core/EventBroker.h"
#include "Physics/PhysicsSystem.h"
#include "Transform/TransformSystem.h"

#include "Physics/CPhysics.h"
#include "Physics/CRectangleShape.h"




using namespace dd;

BOOST_AUTO_TEST_CASE(RemovePhysicsSystemTest)
{
    // Setup world
    std::shared_ptr<EventBroker> eventBroker = std::make_shared<EventBroker>();
    World world(eventBroker);

    world.SystemFactory.Register<Systems::TransformSystem>(
            [this]() { return new Systems::TransformSystem(&world, eventBroker); });
    world.AddSystem<Systems::TransformSystem>();

    world.SystemFactory.Register<Systems::PhysicsSystem>(
            [this]() { return new Systems::PhysicsSystem(&world, eventBroker); });
    world.AddSystem<Systems::PhysicsSystem>();

    world.ComponentFactory.Register<Components::RectangleShape>();
    world.ComponentFactory.Register<Components::Physics>();

    //Create entity
    EntityID ent = world.CreateEntity();

    auto transform = world.AddComponent<Components::Transform>(ent);
    transform->Position = glm::vec3(20.f, 0.f, -10.f);
    transform->Velocity = glm::vec3(1.f, 3.f, 0.f);
    transform->Scale = glm::vec3(2.f, 5.f, 1.f);

    auto physics = world.AddComponent<Components::Physics>(ent);
    physics->Static = false;
    physics->Calculate = true;
    physics->Category = CollisionLayer::Type::Ball;
    physics->Mask = CollisionLayer::Type::Brick | CollisionLayer::Type::Pad | CollisionLayer::Wall;
    physics->GravityScale = 0.f;

    auto rectangle = world.AddComponent<Components::RectangleShape>(ent);


}