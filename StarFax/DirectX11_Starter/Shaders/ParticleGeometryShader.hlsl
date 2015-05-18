
cbuffer externalData : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
};

struct VStoGS
{
	int type : TEXCOORD0;
	float3 position	    : POSITION;
	float4 color        : COLOR;
	float size : TEXCOORD1;
};


struct GStoPS
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 UV : TEXCOORD;
};


[maxvertexcount(4)]
void main(point VStoGS input[1], inout TriangleStream<GStoPS> outStream)
{
	// Don't draw root particle itself
	if (input[0].type == 0)
		return;

	GStoPS output;

	// Offsets for smaller triangles
	float4 offsets[4];
	offsets[0] = float4(-0.1f, +0.1f, -0.1f, 0);
	offsets[1] = float4(+0.1f, +0.1f, -0.1f, 0);
	offsets[2] = float4(-0.1f, -0.1f, -0.1f, 0);
	offsets[3] = float4(+0.1f, -0.1f, -0.1f, 0);

	float2 UVs[4];
	UVs[0] = float2(0, 1);
	UVs[1] = float2(1, 1);
	UVs[2] = float2(0, 0);
	UVs[3] = float2(1, 0);



	// Calculate world view proj matrix
	matrix wvp = mul(mul(world, view), projection);

	// For each vert, spit out a small triangle
	[unroll]
	for (int o = 0; o < 4; o++)
	{
		// Create a single vertex and add to the stream
		output.position = mul(float4(input[0].position, 1.0f), wvp);

		// Depth stuff
		float depthChange = output.position.z / output.position.w;

		// Adjust based on depth
		output.position.xy += offsets[o].xy * depthChange * input[0].size;
		output.color = input[0].color;

		output.UV = UVs[o];

		// Done
		outStream.Append(output);
	}

}