
// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
	float4 position		: SV_POSITION;	// System Value Position - Has specific meaning to the pipeline!
	float3 normal		: NORMAL;
	float2 UV			: TEXCOORD;
};

struct DirectionalLight
{
	float4 AmbientColor;
	float4 DiffuseColor;
	float3 Direction;
};

cbuffer perModel : register(b0)
{
	DirectionalLight light;
};

Texture2D diffuseTexture : register(t0);

SamplerState basicSampler : register(s0);

float4 calcLight(float3 normal, DirectionalLight light)
{

	float3 tolight = normalize(-light.Direction);

	normal = normalize(normal);

	float NdotL = dot(normal, tolight);

	NdotL = saturate(NdotL);

	return (light.DiffuseColor * NdotL) + light.AmbientColor;

}

// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{

	float4 surfaceColor = diffuseTexture.Sample(basicSampler, input.UV);

	

	return surfaceColor * calcLight(input.normal, light);

}

