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

#ifndef Model_h__
#define Model_h__

#include "Rendering/RawModel.h"

namespace dd
{

class Model : public RawModel
{
	friend class ResourceManager;

private:
	Model(std::string fileName);

public:
	~Model();

	GLuint VAO;
	GLuint ElementBuffer;

private:
	GLuint VertexBuffer;
	GLuint DiffuseVertexColorBuffer;
	GLuint SpecularVertexColorBuffer;
	GLuint NormalBuffer;
	GLuint TangentNormalsBuffer;
	GLuint BiTangentNormalsBuffer;
	GLuint TextureCoordBuffer;
};

}

#endif // Model_h__