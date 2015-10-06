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

#ifndef Texture_h__
#define Texture_h__

#include <string>
#include <unordered_map>
#include <cstdio>

#include "Core/ResourceManager.h"
#include "Rendering/PNG.h"

namespace dd
{

class Texture : public Resource
{
	friend class ResourceManager;

private:
	Texture(std::string path);

public:
	~Texture();

	void Bind(GLenum textureUnit = GL_TEXTURE0);

	operator GLuint() const { return m_Texture; }

	unsigned int Width = 0;
	unsigned int Height = 0;

private:
	GLuint m_Texture = 0;
};


}

#endif // Texture_h__
