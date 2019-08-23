/****************************************************
*	Filename:			SkyPixel.hlsl
*	Mod. Date:			08/15/2019
*	Mod. Initials:		D.R.
*	Author:				Drew Byh-Reeves
*	Purpose:			This is a pixel shader
*                       created for a Skybox.
****************************************************/

Texture2D Skybox : register(t0);
SamplerState samLine : register(s0);

cbuffer PconstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Norm : NORMAL;
    float3 Tex : TEXCOORD;
};

struct OutputVertex
{
    float4 xyzw : SV_POSITION;
    float3 Norm : NORMAL;
    float3 Tex : TEXCOORD;
};

float4 main(OutputVertex inputPixel) : SV_TARGET
{
    float4 color = Skybox.Sample(samLine, inputPixel.Tex);
    return color;
}