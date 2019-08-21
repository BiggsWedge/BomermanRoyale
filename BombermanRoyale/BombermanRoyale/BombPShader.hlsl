/****************************************************
*	Filename:			BombPShader.hlsl
*	Date:				08/16/2019
*	Mod. Date:			08/16/2019
*	Mod. Initials:		R.E.
*	Author:				Robert Ellis
*	Purpose:			Pixel shader for altering color of bomb
						based on the timer inside each CBomb object.
****************************************************/

Texture2D _diffuse : register(t0);
Texture2D _normal : register(t1);
Texture2D _emissive : register(t2);
Texture2D _specular : register(t3);

SamplerState _samState : register(s0);


cbuffer bombpbuffer
{
	float4 time;
};

struct inPixel
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD0;
};



float3 lightDir = { -1.0f, -1.0f, -1.0f };
float4 lightColor = { 1.0f, 1.0f, 1.0f, 1.0f };


float4 main(in inPixel _inPix) : SV_TARGET
{
	float4 outColor = (float4) 0;
	float4 diff = (float4) 0;
	float3 norm = normalize(_inPix.normal);
	float4 red = float4(0.8f, -0.5f, -0.5f, 1.0f);
	float ratio = time.x / 3.0f;
	red *= ratio;
	diff = red;

	diff += _diffuse.Sample(_samState, _inPix.texCoord);

	return diff;
	// return saturate(outColor * diff);
	 //saturate(outColor * diff);

}