cbuffer ConstantBuffer
{
	float4x4 MVP;
	float4x4 ModelMatrix;
	float4x4 ViewMatrix;
	float4x4 ProjectionMatrix;
	float4x4 InverseTransposeViewModel;
	float4 Color;
	float4 LightPositions[10];
	float4 LightColors[10];
	float LightRadii[10];
	uint NumLights;
}

Texture2D Texture;
SamplerState samplerState;

struct VOut
{
    float4 PixelPosition : SV_POSITION;
	float4 Position : POSITION;
	float4 Normal : NORMAL;
	float4 Tangent : TANGENT0;
	float4 BiTangent : TANGENT1;
	float2 TextureCoord : TEXCOORD;
	float4 DiffuseColor : COLOR0;
	float4 SpecularColor : COLOR1;
	float4 BoneIndices1 : BLENDINDICES0;
	float4 BoneIndices2 : BLENDINDICES1;
	float4 BoneWeights1 : BLENDWEIGHT0;
	float4 BoneWeights2 : BLENDWEIGHT1;
};

float4 phong(float3 lightPos, float lightRadius, float3 lightColor, float3 position, float3 normal, float3 specular, float specularExponent)
{
	float3 LightSpecular = float3(1.0f, 1.0f, 1.0f) * 0.5f;

	// Diffuse
	lightPos = mul(ViewMatrix, float4(lightPos, 1.0f)).xyz;
	float3 distanceToLight = lightPos - position;
	float3 directionToLight = normalize(distanceToLight);
	float dotProd = dot(directionToLight, normal);
	float3 Idiffuse = lightColor * dotProd;

	// Specular
	float3 surfaceToViewer = normalize(-position);
	float3 halfWay = normalize(surfaceToViewer + directionToLight);
	float dotSpecular = max(dot(halfWay, normal), 0.0f);
	float specularFactor = pow(dotSpecular, specularExponent);
	float3 Ispecular = specular * LightSpecular * specularFactor;

	// Attenuation
	float dist = distance(lightPos, position);
	float attenuation = pow(max(0.0f, 1.0f - (dist / lightRadius)), 2.0f);

	return float4((Idiffuse + Ispecular) * attenuation, 1.0f);
}

float4 main(VOut input) : SV_TARGET
{
	//float4 ambientLight = float4(1.0f, 1.0f, 1.0f, 1.0f) * 0.1f;
	//float4 lightColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	//float3 lightToPixel = normalize(LightPosition - mul(input.Position, transpose(ModelMatrix)));
	//float diffuseBrightness = saturate(dot(lightToPixel, normalize(mul(input.Normal, transpose(ModelMatrix)))));

	float4 diffuseTexture = Texture.Sample(samplerState, input.TextureCoord) * Color;
	float4 lights = float4(1.f, 1.f, 1.f, 1.f);

	for (uint i = 0; i < NumLights; i++) {
		float4 light = phong(LightPositions[i], LightRadii[i], LightColors[i].rgb, input.Position, normalize(input.Normal.xyz), float3(1.0f, 1.0f, 1.0f), 1.0f);
		lights = lights + light;
	}

	//return float4(1.f, 0.f, 0.f, 1.f);
	return float4(diffuseTexture.rgb * 0.8f, diffuseTexture.a) * lights;
}