/*
	This file is part of Daydream Engine.
	Copyright 2014 Adam Byléhn, Tobias Dahl, Simon Holmberg, Viktor Ljung
	
	Daydream Engine is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	
	Daydream Engine is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.
	
	You should have received a copy of the GNU Lesser General Public License
	along with Daydream Engine.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef Components_DamageModel_h__
#define Components_DamageModel_h__

#include <string>

#include "Core/Component.h"

namespace dd
{

namespace Components
{

struct DamageModel : Component
{
	DamageModel() 
		: Color(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
		, ShadowCaster(true)
		, Transparent(false)
	{ }
	std::string ModelFile;
	glm::vec4 Color;
	bool ShadowCaster;
	bool Transparent;
};

}
}

#endif // !Components_DamageModel_h__