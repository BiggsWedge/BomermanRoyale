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


struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD;
};


float4 main(PS_INPUT inputPixel) : SV_TARGET
{
    float4 color = Skybox.Sample(samLine, inputPixel.Tex);
    return color;
}