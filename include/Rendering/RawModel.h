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

#ifndef RawModel_h__
#define RawModel_h__

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <memory>
#include <cstdlib>
#include <stack>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <boost/filesystem/path.hpp>

#include "Core/ResourceManager.h"
#include "Rendering/Texture.h"
#include "Rendering/Skeleton.h"

namespace dd
{

class RawModel : public Resource
{
	friend class ResourceManager;

protected:
	RawModel(std::string fileName);

public:
	~RawModel();

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec3 Tangent;
		glm::vec3 BiTangent;
		glm::vec2 TextureCoords;
		glm::vec4 DiffuseVertexColor;
		glm::vec4 SpecularVertexColor;
		glm::vec4 BoneIndices1;
		glm::vec4 BoneIndices2;
		glm::vec4 BoneWeights1;
		glm::vec4 BoneWeights2;
	};

	struct MaterialGroup
	{
		float Shininess;
		std::shared_ptr<dd::Texture> Texture;
		std::shared_ptr<dd::Texture> NormalMap;
		std::shared_ptr<dd::Texture> SpecularMap;
		unsigned int StartIndex;
		unsigned int EndIndex;
	};

	std::vector<MaterialGroup> TextureGroups;

	std::vector<Vertex> m_Vertices;
	std::vector<unsigned int> m_Indices;
	Skeleton* m_Skeleton = nullptr;
	glm::mat4 m_Matrix;

private:
	std::vector<glm::ivec2> BoneIndices;
	std::vector<glm::vec2> BoneWeights;
	std::vector<glm::vec3> Normals;
	std::vector<glm::vec4> DiffuseVertexColor;
	std::vector<glm::vec4> SpecularVertexColor;
	std::vector<glm::vec3> TangentNormals;
	std::vector<glm::vec3> BiTangentNormals;
	std::vector<glm::vec2> TextureCoords;

	void CreateSkeleton(std::vector<std::tuple<std::string, glm::mat4>> &boneInfo, std::map<std::string, int> &boneNameMapping, aiNode* node, int parentID);
};

}

#endif