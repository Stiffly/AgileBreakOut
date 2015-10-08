#ifndef GAME_GUI_ELOADINGFRAMECOMPLETE_H__
#define GAME_GUI_ELOADINGFRAMECOMPLETE_H__

#include "Core/EventBroker.h"

namespace dd
{

namespace GUI { class LoadingFrame; }

namespace Events
{

struct LoadingFrameComplete : Event
{
	std::string FrameName;
	GUI::LoadingFrame* Frame = nullptr;
};

}
}

#endif