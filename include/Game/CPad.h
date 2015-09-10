//
// Created by Administrator on 2015-09-10.
//

#ifndef DAYDREAM_CPAD_H
#define DAYDREAM_CPAD_H

#include "Core/Component.h"
#include "Core/EKeyDown.h"
#include "Core/EventBroker.h"

namespace dd
{

namespace Components
{

struct Pad : Component
{
    //std::shared_ptr<dd::EventBroker> eventBroker;
    //EVENT_SUBSCRIBE_MEMBER(m_EKeyDown, CPad::OnKeyDown);
    int Lives;
    int Points;
};

}

}

#endif //DAYDREAM_CPAD_H
