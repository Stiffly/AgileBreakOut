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

#ifndef Components_BlendMap_h__
#define Components_BlendMap_h__

#include "Core/Component.h"
#include "Core/Entity.h"

namespace dd
{

namespace Components
{

struct BlendMap : Component
{
	BlendMap() 
		: TextureRed("Textures/ErrorTextureRed.png")
		, TextureRedNormal("Textures/NeutralNormalMap.png")
		, TextureRedSpecular("Textures/NeutralSpecularMap.png")
		, TextureGreen("Textures/ErrorTextureGreen.png")
		, TextureGreenNormal("Textures/NeutralNormalMap.png")
		, TextureGreenSpecular("Textures/NeutralSpecularMap.png")
		, TextureBlue("Textures/ErrorTextureBlue.png")
		, TextureBlueNormal("Textures/NeutralNormalMap.png")
		, TextureBlueSpecular("Textures/NeutralSpecularMap.png")
		, TextureRepeats(100.f) { }

	std::string TextureRed;
	std::string TextureRedNormal;
	std::string TextureRedSpecular;
	std::string	TextureGreen;
	std::string	TextureGreenNormal;
	std::string	TextureGreenSpecular;
	std::string	TextureBlue;
	std::string	TextureBlueNormal;
	std::string	TextureBlueSpecular;
	float TextureRepeats;
};

}
}

#endif // !Components_BlendMap_h__