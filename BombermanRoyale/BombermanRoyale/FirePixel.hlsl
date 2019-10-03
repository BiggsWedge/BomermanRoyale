/****************************************************
*	Filename:			FirePixel.hlsl
*	Mod. Date:			10/1/2019
*	Mod. Initials:		R.E.
*	Author:				Robert Ellis
*	Purpose:			Fire Sprite texture shader.
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
	int texFlags[8];
};

float3 lightDir = { -1.0f, -1.0f, -1.0f };
float4 lightColor = { 1.0f, 1.0f, 1.0f, 1.0f };


float4 main(in inPixel _inPix) : SV_TARGET
{
	float4 outColor = (float4) 0;
	float4 diff = (float4) 0;
	float3 norm = normalize(_inPix.normal);

	if (texFlags[0] == 1)
		diff += _diffuse.Sample(_samState, _inPix.texCoord);
	else
		diff = float4(1.0f, 1.0f, 1.0f, 1.0f);

	//if (diff.x == 0.0f && diff.y == 0.0f && diff.z == 0.0f)
	//	diff = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float lightRatio = saturate(dot(normalize(lightDir), norm));
	outColor += lightRatio * lightColor;


	return diff;
	// return saturate(outColor * diff);
	 //saturate(outColor * diff);

}