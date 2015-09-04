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

#ifndef ShaderProgram_h__
#define ShaderProgram_h__

#include <memory>
#include <string>
#include <fstream>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>

#include "ResourceManager.h"

namespace dd
{

class Shader
{
public:
	Shader(GLenum shaderType, std::string resourceName);
	virtual ~Shader();

	GLenum GetType() const;
	std::string GetFileName() const;
	GLuint GetHandle() const;

protected:
	GLenum m_ShaderType;
	std::string m_FileName;
	GLuint m_ShaderHandle;

	void Compile();
};

template <int SHADERTYPE>
class ShaderType : public Shader, public Resource
{
	friend class ResourceManager;

public:
	ShaderType(std::string resourceName)
		: Shader(SHADERTYPE, resourceName)
	{ }

private:
	virtual void Reload() override;
};

template <int SHADERTYPE>
void ShaderType<SHADERTYPE>::Reload()
{
	Compile();

	Resource::Reload();
}

class VertexShader : public ShaderType<GL_VERTEX_SHADER>
{
public:
	VertexShader(std::string resourceName)
		: ShaderType(resourceName)
	{ }
};

class FragmentShader : public ShaderType<GL_FRAGMENT_SHADER>
{
public:
	FragmentShader(std::string resourceName)
		: ShaderType(resourceName)
	{ }
};

class GeometryShader : public ShaderType<GL_GEOMETRY_SHADER>
{
public:
	GeometryShader(std::string resourceName)
		: ShaderType(resourceName)
	{ }
};

class ShaderProgram : public Resource
{
	friend class ResourceManager;

protected:
	ShaderProgram(std::string resourceName);
	~ShaderProgram();

public:
	void BindFragDataLocation(int colorNumber, std::string name);
	GLuint Link();
	GLuint GetHandle();
	operator GLuint() const { return m_ShaderProgramHandle; }
	void Bind();
	void Unbind();

	virtual void OnChildReloaded(Resource *child) override;

private:
	GLuint m_ShaderProgramHandle = 0;
	std::vector<Shader*> m_Shaders;
};

}

#endif // ShaderProgram_h__