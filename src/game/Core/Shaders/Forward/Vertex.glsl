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

#version 440

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 Bones[100];

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec3 Tangent;
layout (location = 3) in vec3 BiTangent;
layout (location = 4) in vec2 TextureCoord;
layout (location = 5) in vec4 DiffuseColor;
layout (location = 6) in vec4 SpecularColor;
layout (location = 7) in vec4 BoneIndices1;
layout (location = 8) in vec4 BoneIndices2;
layout (location = 9) in vec4 BoneWeights1;
layout (location = 10) in vec4 BoneWeights2;

out VertexData
{
	vec3 Position;
	vec3 Normal;
	vec3 Tangent;
	vec3 BiTangent;
	vec2 TextureCoord;
	vec4 DiffuseColor;
	vec4 SpecularColor;
	vec4 BoneIndices1;
	vec4 BoneIndices2;
	vec4 BoneWeights1;
	vec4 BoneWeights2;
} Output;

void main()
{
	mat4 boneTransform = mat4(1);
	if (length(BoneWeights1 + BoneWeights2) > 0) {
		boneTransform = BoneWeights1[0] * Bones[int(BoneIndices1[0])]
					  + BoneWeights1[1] * Bones[int(BoneIndices1[1])]
					  + BoneWeights1[2] * Bones[int(BoneIndices1[2])]
					  + BoneWeights1[3] * Bones[int(BoneIndices1[3])]
					  + BoneWeights2[0] * Bones[int(BoneIndices2[0])]
					  + BoneWeights2[1] * Bones[int(BoneIndices2[1])]
					  + BoneWeights2[2] * Bones[int(BoneIndices2[2])]
					  + BoneWeights2[3] * Bones[int(BoneIndices2[3])];
	}

	//gl_Position = MVP * boneTransform * vec4(Position, 1.0);
	gl_Position = MVP * vec4(Position, 1.0);

	Output.Position = (V * M * boneTransform * vec4(Position, 1.0)).xyz;
	Output.Normal = (inverse(transpose(V * M)) * boneTransform * vec4(Normal, 0.0)).xyz;
	Output.Tangent = Tangent;
	Output.BiTangent = BiTangent;
	Output.TextureCoord = TextureCoord;
	Output.DiffuseColor = DiffuseColor;
	Output.SpecularColor = SpecularColor;
	Output.BoneIndices1 = BoneIndices1;
	Output.BoneIndices2 = BoneIndices2;
	Output.BoneWeights1 = BoneWeights1;
	Output.BoneWeights2 = BoneWeights2;
}