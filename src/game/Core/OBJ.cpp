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
#include "Core/OBJ.h"

bool dd::OBJ::LoadFromFile(std::string filename)
{
	m_Path = boost::filesystem::path(filename);

	// http://paulbourke.net/dataformats/obj/
	std::ifstream file(m_Path.string());
	if (!file.is_open())
	{
		LOG_ERROR("Failed to open .obj \"%s\": %s", m_Path.string().c_str(), strerror(errno));
		return false;
	}

	//LOG_INFO("Parsing .obj \"%s\"", m_Path.string().c_str());

	std::string line;
	while (std::getline(file, line))
	{
		if (line.length() == 0)
			continue;

		std::stringstream ss(line);

		std::string prefix;
		ss >> prefix;

		// Ignore comments
		if (prefix == "#")
			continue;

		// Material files
		if (prefix == "mtllib")
		{
			std::string materialFilename;
			ss >> materialFilename;
			m_MaterialPath = m_Path.branch_path() / materialFilename;
			ParseMaterial();
			continue;
		}

		// Material statement
		if (prefix == "usemtl")
		{
			std::string material;
			ss >> material;
			m_CurrentMaterial = &Materials[material];
			continue;
		}

		// Vertices
		if (prefix == "v")
		{
			float x, y, z;
			ss >> x >> y >> z;
			Vertices.push_back(std::make_tuple(x, y, z));
			continue;
		}

		// Normals
		if (prefix == "vn")
		{
			float x, y, z;
			ss >> x >> y >> z;
			Normals.push_back(std::make_tuple(x, y, z));
		}

		// Texture coordinates
		if (prefix == "vt")
		{
			float u, v, w;
			ss >> u >> v >> w;
			TextureCoords.push_back(std::make_tuple(u, v, w));
		}

		// Face definitions
		if (prefix == "f")
		{
			Face face;
			face.Material = m_CurrentMaterial;

			std::string faceDefString;
			while (ss >> faceDefString)
			{
				std::stringstream ss2(faceDefString);
				FaceDefinition faceDef = { 0, 0, 0 };

				ss2 >> faceDef.VertexIndex;
				ss2.ignore(); // Ignore first delimiter
				if (!ss2)
					continue;

				if (ss2.peek() == '/')
				{
					ss2.ignore();
					ss2 >> faceDef.NormalIndex;
				}
				else
				{
					ss2 >> faceDef.TextureCoordIndex;
					ss2.ignore();
					ss2 >> faceDef.NormalIndex;
				}
				face.Definitions.push_back(faceDef);
			}
			Faces.push_back(face);
		}
	}

	return true;
}

void dd::OBJ::ParseMaterial()
{
	// http://paulbourke.net/dataformats/mtl/
	std::ifstream file(m_MaterialPath.string());
	if (!file.is_open())
	{
		LOG_ERROR("Failed to open .mtl \"%s\"", m_MaterialPath.string().c_str());
		return;
	}

	//LOG_INFO("Parsing .mtl \"%s\"", m_MaterialPath.string().c_str());

	std::string currentMaterialName;
	MaterialInfo* currentMaterial = nullptr;

	unsigned int currentLine = 0;
	std::string line;
	while (std::getline(file, line))
	{
		currentLine++;
		if (line.length() == 0)
			continue;

		std::stringstream ss(line);

		std::string prefix;
		ss >> prefix;

		// Create a new material definition
		if (prefix == "newmtl")
		{
			MaterialInfo mat;
			ss >> currentMaterialName;
			//LOG_INFO("Parsing material %s", currentMaterialName.c_str());
			Materials[currentMaterialName] = mat;
			currentMaterial = &Materials[currentMaterialName];
			continue;
		}

		if (!currentMaterial)
			continue;

		// Ambient color
		if (prefix == "Ka")
		{
			float r, g, b;
			ss >> r >> g >> b;
			currentMaterial->AmbientColor = std::make_tuple(r, g, b);
			continue;
		}
		// Diffuse color
		if (prefix == "Kd")
		{
			float r, g, b;
			ss >> r >> g >> b;
			currentMaterial->DiffuseColor = std::make_tuple(r, g, b);
			continue;
		}
		// Specular color
		if (prefix == "Ks")
		{
			float r, g, b;
			ss >> r >> g >> b;
			currentMaterial->SpecularColor = std::make_tuple(r, g, b);
			continue;
		}
		// Transmission filter
		if (prefix == "Tf")
		{
			std::stringstream ss2;
			ss2 << ss.str();

			std::string command;
			ss2 >> command;
			if (command == "xyz")
			{
				// TODO: "The "Ks xyz" statement specifies the specular reflectivity using CIEXYZ values."
			}
			else if (command == "spectral")
			{
				// TODO: "The "Tf spectral" statement specifies the transmission filter using a spectral curve."
			}
			else
			{
				float r, g, b;
				ss >> r;
				// G and B are optional
				if (!(ss >> g >> b))
				{
					g = r;
					b = r;
				}
				currentMaterial->TransmissionFilter = std::make_tuple(r, g, b);
			}
			continue;
		}
		// Optical density
		if (prefix == "Ni")
		{
			ss >> currentMaterial->OpticalDensity;
			continue;
		}
		// Alpha
		if (prefix == "d" || prefix == "Tr")
		{
			ss >> currentMaterial->Alpha;
			continue;
		}
		// Shininess
		if (prefix == "Ns")
		{
			ss >> currentMaterial->Shininess;
			continue;
		}
		// Illumination model
		if (prefix == "illum")
		{
			int illum = 0;
			ss >> illum;
			currentMaterial->IlluminationModel = illum;
			continue;
		}
		// Diffuse texture
		if (prefix == "map_Kd")
		{
			MaterialInfo::ColorMap colorMap;

			std::string fileName;
			std::string arg;
			while (ss >> arg)
			{
				ParseColorMap(currentLine, ss, prefix, arg, colorMap);
			}

			// HACK: Should we really have to specify the full path here?
			colorMap.FileName = (m_MaterialPath.branch_path() / colorMap.FileName).string();
			currentMaterial->DiffuseTexture = colorMap;
			continue;
		}
		// Specular map
		if (prefix == "map_Ks")
		{
			MaterialInfo::ColorMap colorMap;

			std::string fileName;
			std::string arg;
			while (ss >> arg)
			{
				ParseColorMap(currentLine, ss, prefix, arg, colorMap);
			}

			// HACK: Should we really have to specify the full path here?
			colorMap.FileName = (m_MaterialPath.branch_path() / colorMap.FileName).string();
			currentMaterial->SpecularMap = colorMap;
			continue;
		}
		// Normal map (bump map)
		if (prefix == "bump" || prefix == "map_Bump" )
		{
			MaterialInfo::BumpMap bumpMap;

			std::string fileName;
			std::string arg;
			while (ss >> arg)
			{
				ParseBumpMap(currentLine, ss, prefix, arg, bumpMap);
			}

			// HACK: Should we really have to specify the full path here?
			bumpMap.FileName = (m_MaterialPath.branch_path() / bumpMap.FileName).string();
			currentMaterial->NormalMap = bumpMap;
			continue;
		}
	}
}

void dd::OBJ::ParseTextureMap(unsigned int line, std::stringstream &ss, std::string prefix, std::string arg, MaterialInfo::TextureMap &textureMap)
{
	if (arg == "-blendu")
	{
		std::string val;
		ss >> val;
		if (val == "off")
			textureMap.blendu = false;
		else if (val == "on")
			textureMap.blendu = true;
		else
			LOG_ERROR("Unrecognized MTL value \"%s\" to argument \"%s %s\" on line %i", val.c_str(), prefix.c_str(), arg.c_str(), line);
	}
	else if (arg == "-blendv")
	{
		std::string val;
		ss >> val;
		if (val == "off")
			textureMap.blendv = false;
		else if (val == "on")
			textureMap.blendv = true;
		else
			LOG_ERROR("Unrecognized MTL value \"%s\" to argument \"%s %s\" on line %i", val.c_str(), prefix.c_str(), arg.c_str(), line);
	}
	else if (arg == "-clamp")
	{
		std::string val;
		ss >> val;
		if (val == "off")
			textureMap.clamp = false;
		else if (val == "on")
			textureMap.clamp = true;
		else
			LOG_ERROR("Unrecognized MTL value \"%s\" to argument \"%s %s\" on line %i", val.c_str(), prefix.c_str(), arg.c_str(), line);
	}
	else if (arg == "-o")
	{
		float u, v, w;
		if (ss >> u >> v >> w)
		{
			textureMap.o = std::make_tuple(u, v, w);
		}
		else
		{
			LOG_ERROR("Unrecognized MTL value to argument \"%s %s\" on line %i", prefix.c_str(), arg.c_str(), line);
		}
	}
	else if (arg == "-s")
	{
		float u, v, w;
		if (ss >> u >> v >> w)
		{
			textureMap.s = std::make_tuple(u, v, w);
		}
		else
		{
			LOG_ERROR("Unrecognized MTL value to argument \"%s %s\" on line %i", prefix.c_str(), arg.c_str(), line);
		}
	}
	// Assume unrecognized options is part of the file name
	else
	{
		if (!textureMap.FileName.empty())
			textureMap.FileName += " ";
		textureMap.FileName += arg;
	}
}

void dd::OBJ::ParseColorMap(unsigned int line, std::stringstream &ss, std::string prefix, std::string arg, MaterialInfo::ColorMap &colorMap)
{
	if (arg == "-cc")
	{
		if (prefix != "map_Kd" || prefix != "map_Ks")
		{
			LOG_ERROR("Invalid MTL argument \"%s\" to option \"%s\" on line %i", arg.c_str(), prefix.c_str(), line);
			return;
		}

		std::string val;
		if (ss >> val)
		{
			if (val == "off")
				colorMap.cc = false;
			else if (val == "on")
				colorMap.cc = true;
			else
				LOG_ERROR("Unrecognized MTL value \"%s\" to argument \"%s %s\" on line %i", val.c_str(), prefix.c_str(), arg.c_str(), line);
		}
		else
		{
			LOG_ERROR("Unrecognized MTL value to argument \"%s %s\" on line %i", prefix.c_str(), arg.c_str(), line);
		}
	}
	else
	{
		ParseTextureMap(line, ss, prefix, arg, colorMap);
	}
}

void dd::OBJ::ParseBumpMap(unsigned int line, std::stringstream &ss, std::string prefix, std::string arg, MaterialInfo::BumpMap &bumpMap)
{
	if (arg == "-bm")
	{
		if (prefix != "bump")
		{
			LOG_ERROR("Invalid MTL argument \"%s\" to option \"%s\" on line %i", arg.c_str(), prefix.c_str(), line);
			return;
		}

		float val;
		if (ss >> val)
			bumpMap.bm = val;
		else
			LOG_ERROR("Unrecognized MTL value to argument \"%s %s\" on line %i", prefix.c_str(), arg.c_str(), line);
	}
	else
	{
		ParseTextureMap(line, ss, prefix, arg, bumpMap);
	}
}