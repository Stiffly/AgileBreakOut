#include <boost/test/unit_test.hpp>
#include "PrecompiledHeader.h"
#include "Core/EventBroker.h"
#include "GUI/Frame.h"

using namespace dd;

BOOST_AUTO_TEST_CASE(FrameTest)
{
	EventBroker eb;

	GUI::Frame baseFrame(&eb);
	baseFrame.X = 100;
	baseFrame.Y = 100;

	// Create a frame with a relative position to baseFrame
	GUI::Frame f1(&baseFrame, "f1");
	f1.X = 50;
	f1.Y = 50;
	f1.Width = 50;
	f1.Height = 50;

	BOOST_CHECK(f1.Left() == 150); BOOST_CHECK(baseFrame.X + f1.X == 150);
	BOOST_CHECK(f1.Top() == 150); BOOST_CHECK(baseFrame.Y + f1.Y == 150);
	BOOST_CHECK(f1.Right() == 200); BOOST_CHECK(baseFrame.X + f1.X + f1.Width == 200);
	BOOST_CHECK(f1.Bottom() == 200); BOOST_CHECK(baseFrame.Y + f1.Y + f1.Height == 200);

	// Create a frame to the right of f1
	GUI::Frame f2(&baseFrame, "f2");
	f2.SetLeft(f1.Right());
	f2.SetTop(f1.Bottom());
	f2.Width = 50;
	f2.Height = 50;

	BOOST_CHECK(f2.X == 100); BOOST_CHECK(f2.X == f1.X + f1.Width);
	BOOST_CHECK(f2.Left() == 200); BOOST_CHECK(f2.Left() == f1.Right());
	BOOST_CHECK(f2.Y == 100); BOOST_CHECK(f2.Y == f1.Y + f1.Height);
	BOOST_CHECK(f2.Top() == 200); BOOST_CHECK(f2.Top() == f1.Bottom());
	BOOST_CHECK(f2.Right() == 250); BOOST_CHECK(f2.Right() == f1.Right() + f2.Width);
	BOOST_CHECK(f2.Bottom() == 250); BOOST_CHECK(f2.Bottom() == f1.Bottom() + f2.Height);
}