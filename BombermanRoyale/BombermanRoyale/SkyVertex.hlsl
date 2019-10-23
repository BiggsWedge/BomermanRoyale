/****************************************************
*	Filename:			SkyVertex.hlsl
*	Mod. Date:			08/15/2019
*	Mod. Initials:		D.R.
*	Author:				Drew Byh-Reeves
*	Purpose:			This is a vertex shader
*                       created for a Skybox.
****************************************************/
cbuffer VconstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
};


struct VS_INPUT
{
    float3 xyz : POSITION;
    float3 Norm : NORMAL;
    float2 Tex : TEXCOORD;
    uint4 joints : JOINTS;
    float4 weights : WEIGHTS;
};


struct OutputVertex
{
    float4 xyzw : SV_POSITION;
    float2 Tex : TEXCOORD;
};

OutputVertex main(VS_INPUT input)
{
    OutputVertex output = (OutputVertex) 0;
	//Fancy Stuff
    output.xyzw = mul(float4(input.xyz, 1.0f), World);
    output.xyzw = mul(output.xyzw, View);
    output.xyzw = mul(output.xyzw, Projection);
    output.Tex = input.Tex;

    return output;
}