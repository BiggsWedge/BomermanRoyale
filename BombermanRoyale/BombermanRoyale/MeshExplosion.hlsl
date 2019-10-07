cbuffer SVconstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
    float4 vLightDir[2];
    float4 vLightColor[2];
    float4 vOutputColor;
    float time;
};

struct GSInput
{
	float4 pos : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD1;
};

[maxvertexcount(20)]
void main(triangle GSInput input[3] : SV_POSITION, inout TriangleStream<GSInput> TriStream)
{
    GSInput output;

    //
    // Calculate the face normal
    //
    float3 faceEdgeA = input[1].pos - input[0].pos;
    float3 faceEdgeB = input[2].pos - input[0].pos;
    float3 faceNormal = normalize(cross(faceEdgeA, faceEdgeB));
    float3 ExplodeAmt = faceNormal * time; // cbuffer stuff * Explode


    //
    // Calculate the face center
    //
    float3 centerPos = (input[0].pos.xyz + input[1].pos.xyz + input[2].pos.xyz) / 3.0;
    float2 centerTex = (input[0].uv + input[1].uv + input[2].uv) / 3.0;
    centerPos += faceNormal * time; // cbuffer stuff* Explode

    for (uint i = 0; i < 3; i++)
    {
        output.pos = input[i].pos + float4(ExplodeAmt, 0);
        output.pos = mul(output.pos, View);
        output.pos = mul(output.pos, Projection);
        output.normal = input[i].normal;
        output.uv = input[i].uv;
        TriStream.Append(output);
        
        int iNext = (i + 1) % 3;
        output.pos = input[iNext].pos + float4(ExplodeAmt, 0);
        output.pos = mul(output.pos, View);
        output.pos = mul(output.pos, Projection);
        output.normal = input[iNext].normal;
        output.uv = input[iNext].uv;
        TriStream.Append(output);
        
        output.pos = float4(centerPos, 1) + float4(ExplodeAmt, 0);
        output.pos = mul(output.pos, View);
        output.pos = mul(output.pos, Projection);
        output.normal = faceNormal;
        output.uv = centerTex;
        TriStream.Append(output);
        
        TriStream.RestartStrip();
    }

    for (int a = 2; a >= 0; a--)
    {
        output.pos = input[a].pos + float4(ExplodeAmt, 0);
        output.pos = mul(output.pos, View);
        output.pos = mul(output.pos, Projection);
        output.normal = -input[a].normal;
        output.uv = input[a].uv;
        TriStream.Append(output);
    }
    TriStream.RestartStrip();
}