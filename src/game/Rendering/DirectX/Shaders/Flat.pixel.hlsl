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

float4 main(VOut input) : SV_TARGET
{
	float4 diffuseTexture = Texture.Sample(samplerState, input.TextureCoord) * Color;
	return diffuseTexture;
}
