/****************************************************
*	Filename:			ExplosionVertex.hlsl
*	Mod. Date:			09/26/2019
*	Mod. Initials:		R.E.
*	Author:				Robert Ellis
*	Purpose:			Sin wave math to alter shape of explosion objects.
****************************************************/
cbuffer wvp
{
	matrix world;
	matrix view;
	matrix projection;
	float time;
	float3 padding;
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

outVertex main(in inVertex _inVert)
{
	outVertex outVert = (outVertex)0;
	float ratio = 0.0f;
	ratio = cos(time * 2.0f)*80.0f;
	float freqx = _inVert.position.x % 4;
	float freqz = _inVert.position.z % 4;
	outVert.position = float4(_inVert.position, 1.0f);
	if (padding.x > padding.z)
	{
		outVert.position.z += ratio / freqx;
	}
	else
	{
		outVert.position.x += ratio / freqz;
	}
	//outVert.position = float4(_inVert.position, 0.0f);
	outVert.position = mul(outVert.position, world);
	outVert.position = mul(outVert.position, view);
	outVert.position = mul(outVert.position, projection);

	outVert.normal = normalize(mul(float4(_inVert.normal.xyz, 1.0f), world));
	outVert.texCoord = _inVert.texCoord;
	return outVert;
}