//
// Created by Adniklastrator on 2015-09-10.
//

#ifndef DAYDREAM_PADSYSTEM_H
#define DAYDREAM_PADSYSTEM_H

#include "Core/InputController.h"
#include "Core/System.h"
#include "Core/EventBroker.h"
#include "Core/EKeyDown.h"
#include "Input/EBindKey.h"

namespace dd
{

namespace Systems
{

class PadSystem : public System
{
public:
    PadSystem(World* world, std::shared_ptr<dd::EventBroker> eventBroker)
    : System(world, eventBroker)
    { }

    void Initialize() override ;
private:
    dd::EventRelay<PadSystem, dd::Events::KeyDown> m_EKeyDown;

    class PadSteeringInputController;
    std::array<std::shared_ptr<PadSteeringInputController>, 4> m_PadInputControllers;

    bool OnKeyDown(const dd::Events::KeyDown &event);
};

class PadSystem::PadSteeringInputController : InputController<PadSystem>
{
public:
    PadSteeringInputController(std::shared_ptr<dd::EventBroker> eventBroker)
    : InputController(eventBroker)
            {

            }
protected:
    virtual bool OnCommand(const Events::InputCommand &event);
};

}

}


#endif //DAYDREAM_PADSYSTEM_H
