cbuffer ConstantBuffer
{
	float4x4 MVP;
	float4x4 ModelMatrix;
	float4x4 ViewMatrix;
	float4x4 ProjectionMatrix;
	float4x4 InverseTransposeViewModel;
	float3 LightPosition;
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
	float4 ambientLight = float4(1.0f, 1.0f, 1.0f, 1.0f) * 0.1f;
	////float4 lightPos = float4(0.0f, -1.0f, 2.0f, 0.0f);
	float4 lightColor = float4(1.0f, 1.0f, 1.0f, 1.0f);


	float3 lightToPixel = normalize(LightPosition - mul(input.Position, transpose(ModelMatrix)));

	//float3 lightModelPos = mul(ModelMatrix, LightPosition);
	float diffuseBrightness = saturate(dot(lightToPixel, normalize(mul(input.Normal, transpose(ModelMatrix)))));

	//input.texcoord.y = 1 - input.texcoord.y;
	//return float4(input.normal.xy, 0.0f, 0.0f);
	return Texture.Sample(samplerState, input.TextureCoord);
	//return float4(input.color.xy, 0.0f, 0.0f);
	//return (ambientLight + lightColor * diffuseBrightness) * Texture.Sample(samplerState, input.texcoord);
	//return float4(0.0f, 0.0f, 1.0f, 1.0f);
}