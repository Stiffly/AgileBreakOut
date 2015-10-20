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
#include "Rendering/Texture.h"

dd::Texture::Texture(std::string path)
{
	std::unique_ptr<Image> image = std::make_unique<PNG>(path);

	if (image->Width == 0 && image->Height == 0 || image->Format == Image::ImageFormat::Unknown) {
		image = std::make_unique<PNG>("Textures/Core/ErrorTexture.png");
		if (image->Width == 0 && image->Height == 0 || image->Format == Image::ImageFormat::Unknown) {
			LOG_ERROR("Couldn't even load the error texture. This is a dark day indeed.");
			return;
		}
	}

	this->Width = image->Width;
	this->Height = image->Height;

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = this->Width;
	desc.Height = this->Height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = image->Data;
	data.SysMemPitch = image->Width * 4;
	data.SysMemSlicePitch = image->Width * image->Height * 4;

	HRESULT result;
	result = StaticSystem::Get<Renderer>()->m_Device->CreateTexture2D(&desc, &data, &m_Texture);
	if (result != S_OK) {
		LOG_ERROR("DirectX failed to create texture \"%s\"", path.c_str());
	}
	result = StaticSystem::Get<Renderer>()->m_Device->CreateShaderResourceView(m_Texture, nullptr, &m_ShaderResourceView);
	if (result != S_OK) {
		LOG_ERROR("DirectX failed to create shader resource view for texture \"%s\"", path.c_str());
	}
}

dd::Texture::~Texture()
{
}