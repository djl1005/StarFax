
// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
	float4 position		: SV_POSITION;	// System Value Position - Has specific meaning to the pipeline!
	float3 normal		: NORMAL;
	float2 UV			: TEXCOORD0;
	float4 lightViewPos	: TEXCOORD1;
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
Texture2D depthTexture : register(t1);

SamplerState basicSampler : register(s0);

float4 calcLight(float3 normal, DirectionalLight light)
{

	float3 tolight = normalize(-light.Direction);

		normal = normalize(normal);

	float NdotL = dot(normal, tolight);

	NdotL = saturate(NdotL);

	return (light.DiffuseColor * NdotL);

	return (light.DiffuseColor * NdotL) + light.AmbientColor;

}

// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
	float bias = 0.005f;
	float4 color = light.AmbientColor;
	float4 lightColor = calcLight(input.normal, light);
	float2 projectTexCoord;
	projectTexCoord.x = input.lightViewPos.x / input.lightViewPos.w / 2.0f + 0.5f;
	projectTexCoord.y = -input.lightViewPos.y / input.lightViewPos.w / 2.0f + 0.5f;

	if (saturate(projectTexCoord.x) == projectTexCoord.x
		&& saturate(projectTexCoord.y) == projectTexCoord.y)
	{
		float depthValue = depthTexture.Sample(basicSampler, projectTexCoord).r;
		float lightDepthValue = input.lightViewPos.z / input.lightViewPos.w;
		lightDepthValue = lightDepthValue - bias;

		if ((lightDepthValue <= depthValue))
		{
			color = lightColor;
			//color.w = 1;
		}
	}
	else
	{
		color = lightColor;
	}


	float4 surfaceColor = diffuseTexture.Sample(basicSampler, input.UV);

		//without shadows
		//return surfaceColor * calcLight(input.normal, light);

		//with shadows
		return surfaceColor * color;
	//return color;
}

