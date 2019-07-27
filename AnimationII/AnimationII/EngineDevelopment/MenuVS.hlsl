cbuffer MVP_t : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
};

cbuffer joints : register(b1)
{
	int numJoints;
	matrix joints[30];
}

struct VS_IN
{
	float3 pos : POSITION;
	float3 norm : NORMAL;
	float2 tex : TEXCOORD;
	int4 joints : JOINTS;
	float4 weights : WEIGHTS;
};


struct VS_OUT
{
	float4 pos : SV_POSITION;
	float3 norm : NORMAL;
	float2 tex : TEXCOORD1;
	float4 wPos : WORLD;
	float4 eyePos : EYE;

};

VS_OUT main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.pos = mul(float4(input.pos.xyz, 1), world);
	output.wPos = output.pos;
	output.pos = mul(output.pos, view);
	output.pos = mul(output.pos, projection);
	//output.pos = float4(output.pos.xyz, 1);

	output.tex = float2(input.tex.x, 1.0f - input.tex.y);
	output.eyePos.x = -dot(view[3].xyz, view[0].xyz);
	output.eyePos.y = -dot(view[3].xyz, view[1].xyz);
	output.eyePos.z = -dot(view[3].xyz, view[2].xyz);
	output.eyePos.w = 1.0f;

	return output;
}