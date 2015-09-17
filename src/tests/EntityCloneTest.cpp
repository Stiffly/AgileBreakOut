#include <boost/test/unit_test.hpp>
#include "Core/EventBroker.h"
#include "Core/World.h"
#include "Core/CTemplate.h"
using namespace dd;

struct TestComponent : public Component
{
	int Int = 5;
	float Float = 1.33333f;
	double Double = 1.33333;
	std::string String = "Hello World";
};

BOOST_AUTO_TEST_CASE(EntityCloneTest)
{
	// Setup world
	std::shared_ptr<EventBroker> eventBroker = std::make_shared<EventBroker>();
	World world(eventBroker);
	world.ComponentFactory.Register<Components::Template>();
	world.ComponentFactory.Register<TestComponent>();

	// Create our original entity
	EntityID entOriginal = world.CreateEntity();
	world.AddComponent<Components::Template>(entOriginal);
	world.AddComponent<TestComponent>(entOriginal);

	// Clone it
	EntityID entClone = world.CloneEntity(entOriginal);

	// Check
	// EntityID of clone should not be same as original (obviously)
	BOOST_CHECK(entOriginal != entClone);
	// Template component SHOULD be in clone
	auto templateComponentClone = world.GetComponent<Components::Template>(entClone);
	BOOST_REQUIRE(templateComponentClone != nullptr);
	// Values should be the same across the clone
	auto testComponentOriginal = world.GetComponent<TestComponent>(entOriginal);
	BOOST_REQUIRE(testComponentOriginal != nullptr);
	auto testComponentClone = world.GetComponent<TestComponent>(entClone);
	BOOST_REQUIRE(testComponentClone != nullptr);
	BOOST_CHECK(testComponentClone->Int == testComponentOriginal->Int);
	BOOST_CHECK(testComponentClone->Float == testComponentOriginal->Float);
	BOOST_CHECK(testComponentClone->Double == testComponentOriginal->Double);
	BOOST_CHECK(testComponentClone->String == testComponentOriginal->String);
}