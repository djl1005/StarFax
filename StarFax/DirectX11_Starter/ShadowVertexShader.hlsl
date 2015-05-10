cbuffer perModel : register( b0 ) 
{
	matrix world;
	matrix lightViewProj;
};

struct VertexShaderInput
{
	float3 position		: POSITION;
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD;
};

struct VertexShaderOutput
{
	float4 position		: SV_POSITION;
};

VertexShaderOutput main(VertexShaderInput input)
{
	VertexShaderOutput output;
	
	output.position = mul(world, input.position);
	output.position = mul(lightViewProj, output.position);

	return output;
}