/****************************************************
*	Filename:			BasicPixel.hlsl
*	Date:				07/16/2019
*	Mod. Date:			07/16/2019
*	Mod. Initials:		D.S.
*	Author:				Dominic Sondgeroth
*	Purpose:			This is a basic pixel shader
*                       created for testing.
****************************************************/

Texture2D _diffuse : register(t0);
Texture2D _normal : register(t1);
Texture2D _emissive : register(t2);
Texture2D _specular : register(t3);

SamplerState _samState : register(s0);


struct inPixel
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD0;
};

cbuffer textureFlags
{
    bool texFlags[4];
};


float4 main(in inPixel _inPix) : SV_TARGET
{
    float4 outColor = (float4) 0;
    if (texFlags[0])
        outColor += _diffuse.Sample(_samState, _inPix.texCoord);

    return outColor;
}


