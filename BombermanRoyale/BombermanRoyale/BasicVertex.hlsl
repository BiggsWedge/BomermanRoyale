/****************************************************
*	Filename:			BasicVertex.hlsl
*	Date:				07/16/2019
*	Mod. Date:			07/16/2019
*	Mod. Initials:		D.S.
*	Author:				Dominic Sondgeroth
*	Purpose:			This is a basic vertex shader
*                       created for testing.
****************************************************/
cbuffer wvp
{
    matrix world;
    matrix view;
    matrix projection;
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
    outVertex outVert = (outVertex) 0;
	//outVert.position = float4(_inVert.position, 0.0f);
    outVert.position = mul(float4(_inVert.position, 1.0f), world);
    outVert.position = mul(outVert.position, view);
    outVert.position = mul(outVert.position, projection);

    outVert.normal = normalize(mul(float4(_inVert.normal, 1.0f), world));
    outVert.texCoord = _inVert.texCoord;
    return outVert;
}