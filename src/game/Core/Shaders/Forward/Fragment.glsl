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

layout (binding=0) uniform sampler2D DiffuseTexture;
layout (binding=1) uniform sampler2D NormalMap;
layout (binding=2) uniform sampler2D SpecularMap;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform vec3 LightPosition;
uniform float LightRadius;
uniform vec3 LightSpecular;
uniform vec3 LightDiffuse;

in VertexData
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
} Input;

out vec4 frag_Diffuse;

vec4 phong(vec3 position, vec3 normal, vec3 specular, float specularExponent)
{
	// Diffuse
	vec3 lightPos = vec3(V * vec4(LightPosition, 1.0));
	vec3 distanceToLight = lightPos - position;
	vec3 directionToLight = normalize(distanceToLight);
	float dotProd = dot(directionToLight, normal);
	dotProd = max(dotProd, 0.0);
	vec3 Idiffuse = LightDiffuse * dotProd;

	// Specular
	//vec3 reflection = reflect(-directionToLight, normal);
	vec3 surfaceToViewer = normalize(-position);
	vec3 halfWay = normalize(surfaceToViewer + directionToLight);
	float dotSpecular = max(dot(halfWay, normal), 0.0);
	float specularFactor = pow(dotSpecular, specularExponent);
	vec3 Ispecular = specular * LightSpecular * specularFactor;

	//Attenuation
	float dist = distance(lightPos, position);
	//float attenuation = 1.0 - pow(dist / LightRadius, 2);
	float attenuation = pow(max(0.0f, 1.0 - (dist / LightRadius)), 2);

	return vec4((Idiffuse + Ispecular) * attenuation, 1.0);
}

void main()
{
	vec4 normalTexel = texture(NormalMap, Input.TextureCoord);
	mat3 TBN = mat3(Input.Tangent, Input.BiTangent, Input.Normal);
	vec3 normal = normalize(vec4(TBN * normalTexel.xyz, 0.0).xyz);

	vec4 specularTexel = texture(SpecularMap, Input.TextureCoord);

	vec4 diffuseTexel = texture(DiffuseTexture, Input.TextureCoord);
	vec4 La = vec4(0.3, 0.3, 0.3, 1.0);
	vec4 light = phong(Input.Position, normalize(normal), specularTexel.rgb, specularTexel.a);
	//frag_Diffuse = light;
	frag_Diffuse = diffuseTexel;
}