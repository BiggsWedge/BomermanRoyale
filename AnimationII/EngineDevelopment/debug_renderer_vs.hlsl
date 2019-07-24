cbuffer MVP_t : register(b0)
{
	matrix modeling;
	matrix view;
	matrix projection;
};

struct VSIn
{
	float3 position : POSITION;
	float4 color  : COLOR;
};

struct VSOut
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};

VSOut main(VSIn input)
{
	VSOut output;

	float4 vert_pos = float4(input.position, 1.0f);
	float4 vert_color = input.color;

	output.pos = mul(vert_pos, modeling);
	output.pos = mul(output.pos, view);
	output.pos = mul(output.pos, projection);


	output.color = vert_color;

	return output;
}
