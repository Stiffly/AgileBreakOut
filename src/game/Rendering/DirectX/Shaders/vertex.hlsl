cbuffer ConstantBuffer
{
	float4x4 MVP;
	float4x4 ModelMatrix;
	float4x4 ViewMatrix;
	float4x4 ProjectionMatrix;
	float4x4 InverseTransposeViewModel;
	float3 LightPosition;
}

struct VIn
{
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

VOut main(VIn input)
{
	/*float4 ambientLight = float4(1.0f, 1.0f, 1.0f, 1.0f) * 0.1f;
	float4 lightPos = float4(0.0f, 0.0f, 8.0f, 0.0f);
	float4 lightColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	float diffuseBrightness = saturate(dot(normal, lightPos));*/

    VOut output;

    output.PixelPosition = mul(MVP, input.Position);
	//TODO: Make sure that boneTransform works here.
	//output.Position = (V * M * boneTransform * vec4(Position, 1.0)).xyz;
	//output.Normal = (inverse(transpose(V * M)) * boneTransform * vec4(Normal, 0.0)).xyz;
	output.Tangent = input.Tangent;
	output.BiTangent = input.BiTangent;
	output.TextureCoord = input.TextureCoord;
	output.DiffuseColor = input.DiffuseColor;
	output.SpecularColor = input.SpecularColor;
	output.BoneIndices1 = input.BoneIndices1;
	output.BoneIndices2 = input.BoneIndices2;
	output.BoneWeights1 = input.BoneWeights1;
	output.BoneWeights2 = input.BoneWeights2;

    return output;
}