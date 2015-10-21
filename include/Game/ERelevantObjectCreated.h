//
// Created by Administrator on 2015-10-20.
//

#ifndef DAYDREAM_ERELEVANTOBJECTCREATED_H
#define DAYDREAM_ERELEVANTOBJECTCREATED_H

#include "Core/EventBroker.h"
#include "Core/Entity.h"

namespace dd
{

namespace Events
{

struct RelevantObjectCreated : Event
{
	std::string ObjectName;
	EntityID ObjectID;
};

}

}

#endif //DAYDREAM_ERELEVANTOBJECTCREATED_H
