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
#include "Core/Texture.h"

dd::Texture::Texture(std::string path)
{
	PNG image(path);

	if (image.Width == 0 && image.Height == 0 || image.Format == Image::ImageFormat::Unknown) {
		image = PNG("Textures/Core/ErrorTexture.png");
		if (image.Width == 0 && image.Height == 0 || image.Format == Image::ImageFormat::Unknown) {
			LOG_ERROR("Couldn't even load the error texture. This is a dark day indeed.");
			return;
		}
	}

	this->Width = image.Width;
	this->Height = image.Height;

	GLint format;
	switch (image.Format) {
	case Image::ImageFormat::RGB:
		format = GL_RGB;
		break;
	case Image::ImageFormat::RGBA:
		format = GL_RGBA;
		break;
	}

	// Construct the OpenGL texture
	glGenTextures(1, &m_Texture);
	glBindTexture(GL_TEXTURE_2D, m_Texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, format, image.Width, image.Height, 0, format, GL_UNSIGNED_BYTE, image.Data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	GLERROR("Texture load");
}

dd::Texture::~Texture()
{
	glDeleteTextures(1, &m_Texture);
}

void dd::Texture::Bind(GLenum textureUnit /* = GL_TEXTURE0 */)
{
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, m_Texture);
}