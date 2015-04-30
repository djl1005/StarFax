// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
	float4 position		: SV_POSITION;	// System Value Position - Has specific meaning to the pipeline!
	float3 normal		: NORMAL;
	float2 UV			: TEXCOORD;
};


cbuffer perModel : register(b0)
{
	float4 color; 
};

Texture2D diffuseTexture : register(t0);

SamplerState basicSampler : register(s0);



// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{

	float4 surfaceColor = diffuseTexture.Sample(basicSampler, input.UV) * color;

	return surfaceColor;

}