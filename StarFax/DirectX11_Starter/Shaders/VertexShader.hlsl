
// The constant buffer that holds our "per model" data
// - Each object you draw with this shader will probably have
//   slightly different data (at least for the world matrix)
cbuffer perModel : register( b0 )
{
	matrix world;
	matrix view;
	matrix projection;
};

// Defines what kind of data to expect as input
// - This should match our input layout!
struct VertexShaderInput
{ 
	float3 position		: POSITION;
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD;
};

// Defines the output data of our vertex shader
// - At a minimum, you'll need an SV_POSITION
// - Should match the pixel shader's input
struct VertexToPixel
{
	float4 position		: SV_POSITION;	// System Value Position - Has specific meaning to the pipeline!
	float3 normal		: NORMAL;
	float2 UV			: TEXCOORD0;
	float4 lightViewPos	:TEXCOORD1;
};

// The entry point for our vertex shader
VertexToPixel main( VertexShaderInput input )
{
	// Set up output
	VertexToPixel output;

	// Calculate output position
	matrix worldViewProj = mul(mul(world, view), projection);
	output.position = mul(float4(input.position, 1.0f), worldViewProj);

	output.normal = mul(input.normal, (float3x3)world);

	output.UV = input.uv;

	output.lightViewPos = mul(world, input.position);
	output.lightViewPos = mul(worldViewProj, output.lightViewPos);

	return output;
}