#include <boost/test/unit_test.hpp>

#include "Core/Component.h"
using namespace dd;

struct TestComponent : public Component
{
	int Int = 5;
	float Float = 1.33333f;
	double Double = 1.33333;
	std::string String = "Hello World";
};

BOOST_AUTO_TEST_CASE(component_copy)
{
	auto componentFactory = new Factory<Component>();
	componentFactory->Register<TestComponent>();

	auto component1 = static_cast<TestComponent*>(componentFactory->Create<TestComponent>());
	auto component2 = static_cast<TestComponent*>(componentFactory->Copy<TestComponent>(component1));
	BOOST_CHECK(component1->Int == component2->Int);
	BOOST_CHECK_CLOSE(component1->Float, component2->Float, 0.00001f);
	BOOST_CHECK_CLOSE(component1->Double, component2->Double, 0.00001f);
	BOOST_CHECK(component1->String == component2->String);

	delete component2;
	delete component1;
	delete componentFactory;
}