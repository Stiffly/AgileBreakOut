//
// Created by Adam on 2015-09-09.
//

#ifndef DAYDREAM_SOUND_H
#define DAYDREAM_SOUND_H

#include "../../deps/include/AL/al.h"
#include "../../deps/include/AL/alc.h"

#include "Core/EKeyDown.h"
#include "Core/EventBroker.h"
#include "Core/System.h"
#include "Core/World.h"


namespace dd
{
namespace Systems
{
class Sound : public System {
public:
    Sound(World *world, std::shared_ptr<dd::EventBroker> eventBroker)
            : System(world, eventBroker) { EVENT_SUBSCRIBE_MEMBER(m_EKeyDown, &Sound::OnKeyDown); }


    void Init();

private:
    dd::EventRelay<Sound, dd::Events::KeyDown> m_EKeyDown;

    bool OnKeyDown(const dd::Events::KeyDown &event);
    void HelloWorld();

};
}
}

#endif //DAYDREAM_SOUND_H
