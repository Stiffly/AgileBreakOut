#ifndef COMPONENTS_CANIMATION_H__
#define COMPONENTS_CANIMATION_H__

#include "Core/Component.h"

namespace dd
{
namespace Components
{
	
struct Animation : Component
{
	std::string Name;
	double Time = 0.0;
	double Speed = 0.0;
	bool NoRootMotion = true;
};

}
}
#endif