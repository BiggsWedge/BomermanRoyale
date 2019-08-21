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
    float3 Tex : TEXCOORD;
};


struct OutputVertex
{
    float4 xyzw : SV_POSITION;
    float3 Norm : NORMAL;
    float3 Tex : TEXCOORD;
};

OutputVertex main(VS_INPUT input)
{
    OutputVertex output = (OutputVertex) 0;
	//Fancy Stuff
    output.xyzw = mul(float4(input.xyz, 1), World);
    output.xyzw = mul(output.xyzw, View);
    output.xyzw = mul(output.xyzw, Projection);
    output.Tex = input.Tex;
    output.Tex = input.xyz;

    return output;
}