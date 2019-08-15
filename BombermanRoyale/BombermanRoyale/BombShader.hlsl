cbuffer wvp
{
	matrix world;
	matrix view;
	matrix projection;
	float4 time;
};

struct inVertex
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
	uint4 joints : JOINTS;
	float4 weights : WEIGHTS;
};

struct outVertex
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD0;
};

outVertex main(in inVertex input)
{
	outVertex output;
	int t = time;
	float ratio = time - t;
	ratio = (1.5f - 0.5f) * ratio + 0.5f;
	if (ratio <= 1.0f)
		input.position.xyz = input.position.xyz * ratio;
	if (ratio >= 1.0f)
		input.position.xyz = input.position.xyz / ratio;
	float4 world_position = mul(float4(input.position.xyz, 1.0f), world);
	output.position = mul(world_position, view);
	output.position = mul(output.position, projection);

	output.normal = mul(float4(input.normal.xyz, 0.0f), world);
	output.texCoord = input.texCoord.xy;
	return output;
	/*PS_INPUT output = (PS_INPUT)0;
	output.position = mul( input.position, World );
	output.position = mul( output.position, View );
	output.position = mul( output.position, Projection );
	output.normal = mul( float4( input.normal, 1 ), World ).xyz;
	output.Tex = input.Tex;
	return output;*/
}

