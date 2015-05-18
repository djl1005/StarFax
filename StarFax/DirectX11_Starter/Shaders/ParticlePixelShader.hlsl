
// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float4 color		: COLOR;
	float2 UV			: TEXCOORD;
};

Texture2D diffuseTexture : register(t0);

SamplerState basicSampler : register(s0);
// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
	// Just return the input color
	// - Note that this color (like all values that pass through the rasterizer)
	//   is interpolated for each pixel between the corresponding 
	//   vertices of the triangle
	float4 color = input.color;
	float4 surfaceColor = diffuseTexture.Sample(basicSampler, input.UV);
	return (color * surfaceColor);
}