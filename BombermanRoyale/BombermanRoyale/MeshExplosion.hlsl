/****************************************************
*	Filename:			MeshExplosion.hlsl
*	Mod. Date:			10/07/2019
*	Mod. Initials:		D.R.
*	Author:				Drew Byh-Reeves
*	Purpose:			Splits up mesh to imitate an explosion
****************************************************/

cbuffer SVconstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
    float time;
};

struct GSInput
{
    float4 pos : SV_POSITION;
    float3 norm : NORMAL;
    float2 tex : TEXCOORD1;

};

[maxvertexcount(20)]
void main(triangle GSInput input[3] : SV_POSITION, inout TriangleStream<GSInput> TriStream)
{
    GSInput output;

    
    // face normal
    
    float3 faceEdgeA = input[1].pos - input[0].pos;
    float3 faceEdgeB = input[2].pos - input[0].pos;
    float3 faceNormal = normalize(cross(faceEdgeA, faceEdgeB));
    float3 ExplodeAmt = faceNormal * time; // cbuffer stuff * Explode


    
    //  face center
    
    float3 centerPos = (input[0].pos.xyz + input[1].pos.xyz + input[2].pos.xyz) / 3.0;
    float2 centerTex = (input[0].tex + input[1].tex + input[2].tex) / 3.0;
    centerPos += faceNormal * time; // cbuffer stuff* Explode

    for (uint i = 0; i < 3; i++)
    {
        output.pos = input[i].pos + float4(ExplodeAmt, 0);
        output.pos = mul(output.pos, View);
        output.pos = mul(output.pos, Projection);
        output.norm = input[i].norm;
        output.tex = input[i].tex;
        TriStream.Append(output);
        
        int iNext = (i + 1) % 3;
        output.pos = input[iNext].pos + float4(ExplodeAmt, 0);
        output.pos = mul(output.pos, View);
        output.pos = mul(output.pos, Projection);
        output.norm = input[iNext].norm;
        output.tex = input[iNext].tex;
        TriStream.Append(output);
        
        output.pos = float4(centerPos, 1) + float4(ExplodeAmt, 0);
        output.pos = mul(output.pos, View);
        output.pos = mul(output.pos, Projection);
        output.norm = faceNormal;
        output.tex = centerTex;
        TriStream.Append(output);
        
        TriStream.RestartStrip();
    }

    for (int a = 2; a >= 0; a--)
    {
        output.pos = input[a].pos + float4(ExplodeAmt, 0);
        output.pos = mul(output.pos, View);
        output.pos = mul(output.pos, Projection);
        output.norm = -input[a].norm;
        output.tex = input[a].tex;
        TriStream.Append(output);
    }
    TriStream.RestartStrip();
}