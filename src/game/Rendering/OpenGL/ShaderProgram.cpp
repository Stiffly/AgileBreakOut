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

#include "PrecompiledHeader.h"
#include "Rendering/ShaderProgram.h"

void dd::Shader::Compile()
{
	LOG_INFO("Compiling shader \"%s\"", m_FileName.c_str());

	std::string shaderFile;
	std::ifstream in(m_FileName, std::ios::in);
	if (!in)
	{
		LOG_ERROR("Error: Failed to open shader file \"%s\"", m_FileName.c_str());
		return;
	}
	in.seekg(0, std::ios::end);
	shaderFile.resize((int)in.tellg());
	in.seekg(0, std::ios::beg);
	in.read(&shaderFile[0], shaderFile.size());
	in.close();

	const GLchar* shaderFileC = shaderFile.c_str();
	const GLint length = shaderFile.length();
	glShaderSource(m_ShaderHandle, 1, &shaderFileC, &length);
	if(GLERROR("glShaderSource"))
		return;

	glCompileShader(m_ShaderHandle);

	GLint compileStatus;
	glGetShaderiv(m_ShaderHandle, GL_COMPILE_STATUS, &compileStatus);
	if(compileStatus != GL_TRUE)
	{
		LOG_ERROR("Shader compilation failed");
		GLsizei infoLogLength;
		glGetShaderiv(m_ShaderHandle, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* infolog = new GLchar[infoLogLength];
		glGetShaderInfoLog(m_ShaderHandle, infoLogLength, &infoLogLength, infolog);
		LOG_ERROR(infolog);
		delete[] infolog;
	}

	if(GLERROR("glCompileShader"))
		return;
}

dd::Shader::Shader(GLenum shaderType, std::string resourceName)
		: m_ShaderType(shaderType)
		, m_FileName(resourceName)
{
	m_ShaderHandle = glCreateShader(shaderType);
	if (GLERROR("glCreateShader"))
		return;

	Compile();
}

dd::Shader::~Shader()
{
	if (m_ShaderHandle != 0)
	{
		glDeleteShader(m_ShaderHandle);
	}
}

GLenum dd::Shader::GetType() const
{
	return m_ShaderType;
}

std::string dd::Shader::GetFileName() const
{
	return m_FileName;
}

GLuint dd::Shader::GetHandle() const
{
	return m_ShaderHandle;
}

dd::ShaderProgram::ShaderProgram(std::string resourceName)
{
	auto path = boost::filesystem::path(resourceName);

	if (!boost::filesystem::is_directory(path))
	{
		LOG_ERROR("Failed to load shader program: \"%s\" is not a directory", resourceName.c_str());
		return;
	}

	for (auto it = boost::filesystem::directory_iterator(path); it != boost::filesystem::directory_iterator(); it++)
	{
		std::string filename = it->path().filename().string();
		std::string filepath = it->path().string();
		if (filename == "Vertex.glsl")
		{
			m_Shaders.push_back(ResourceManager::Load<VertexShader>(filepath, this));
		}
		else if (filename == "Fragment.glsl")
		{
			m_Shaders.push_back(ResourceManager::Load<FragmentShader>(filepath, this));
		}
		else if (filename == "Geometry.glsl")
		{
			m_Shaders.push_back(ResourceManager::Load<GeometryShader>(filepath, this));
		}
	}
}

dd::ShaderProgram::~ShaderProgram()
{
	if (m_ShaderProgramHandle != 0)
	{
		glDeleteProgram(m_ShaderProgramHandle);
	}
}

GLuint dd::ShaderProgram::Link()
{
	if (m_Shaders.size() == 0)
	{
		LOG_ERROR("Failed to link shader program: No shaders bound");
		return 0;
	}

	if (m_ShaderProgramHandle == 0)
	{
		m_ShaderProgramHandle = glCreateProgram();
	}

	LOG_INFO("Linking shader program");

	for (auto &shader : m_Shaders)
	{
		glAttachShader(m_ShaderProgramHandle, shader->GetHandle());
	}
	glLinkProgram(m_ShaderProgramHandle);
	if (GLERROR("glLinkProgram"))
		return 0;

	return m_ShaderProgramHandle;
}

GLuint dd::ShaderProgram::GetHandle()
{
	return m_ShaderProgramHandle;
}

void dd::ShaderProgram::Bind()
{
	if (m_ShaderProgramHandle == 0)
		return;

	glUseProgram(m_ShaderProgramHandle);
}

void dd::ShaderProgram::Unbind()
{
	glActiveShaderProgram(0, 0);
}

void dd::ShaderProgram::BindFragDataLocation(int colorNumber, std::string name)
{
	if (m_ShaderProgramHandle == 0)
		return;

	glBindFragDataLocation(m_ShaderProgramHandle, colorNumber, name.c_str());
}

void dd::ShaderProgram::OnChildReloaded(dd::Resource* child) {
	LOG_INFO("Re-linking shader program");
	glLinkProgram(m_ShaderProgramHandle);
	if (GLERROR("glLinkProgram"))
		return;
}
